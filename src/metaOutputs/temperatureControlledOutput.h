#pragma once

#include "outputs/Output.h"
#include <Arduino.h>
#include "debug.h"
#include "colours.h"

struct TemperatureModelParameters
{
    float heatupCoefficient;
    float cooldownCoefficient;
    float maxTemperature;
    float ambientTemperature;
    float maxtemperatureDescent; 
    float equilibriumDutyCycleComplement;
    float correctionThresholdTemperature;
};

class TemperatureModel
{

public:
    float Temperature;
    float slowTemperature;
    float pwmScale;
    float previousRequestedIntensity;
    TemperatureModelParameters *Parameters;

    TemperatureModel(TemperatureModelParameters *params, float initialTemperature)
    {
        Temperature = initialTemperature;
        slowTemperature = initialTemperature;
        previousRequestedIntensity = 0; //float, range 0-1
        pwmScale = 0;
        Parameters = params;
    }

    //requestedIntensity is on the scale 0-1 (full of, full on)
    //dt is in ms
    float getCorrectionFactor(float requestedIntensity, float dtms)
    {
        float dt = dtms/1000;
        float oldtemp = Temperature;

        //update the temperature in the past dt interval
        this->Temperature = calcTemp(Temperature, pwmScale * previousRequestedIntensity, dt);

        //keep a exponentially filtered temperature average to slow down the corrections
        //exponential 0.5 filter to dampen out oscillations when max temp is reached
        slowTemperature = calcSlowTemp(slowTemperature, Temperature, oldtemp) * 0.5 + 0.5 * slowTemperature;

        //with the updated temp, calculate the new correction factor
        pwmScale = calcPwmScale(slowTemperature);

        previousRequestedIntensity = requestedIntensity;

        return pwmScale;
    }

private:
    float calcTemp(float oldtemp, float scaledpwm, float dt)
    {
        float dT_heat = Parameters->heatupCoefficient * scaledpwm * dt;
        float dT_cool = -1. * Parameters->cooldownCoefficient * (oldtemp - Parameters->ambientTemperature) * dt;
        return oldtemp + dT_heat + dT_cool;
    }

    inline float calcSlowTemp(float slowtemp, float temp, float oldtemp)
    {
        //if the temperature is rising, keep the slowtemp at a fixed value, until the actual temp pick it up
        //if it is cooling, cool down, but at a slow rate (maxtempdescent)
        //applying a slower rate here makes the effect more consistent over time: the lamp might have cooled down already,
        //but its likely that the lightpattern is repeating, and it will soon heatup again. This slow 
        //decline makes the pattern look more consistent.
        if (temp < oldtemp)
            slowtemp *= Parameters->maxtemperatureDescent;

        //if the actual temp becomes higer than the slow temp, that is more important to avoid overheating, so
        //pickup that value immideately
        return fmax(slowtemp, temp);
    }

    float calcPwmScale(float temp)
    {
        float temprange = fminf(fmaxf((temp - Parameters->correctionThresholdTemperature) / (Parameters->maxTemperature - Parameters->correctionThresholdTemperature), 0), 1);
        return 1.0 - Parameters->equilibriumDutyCycleComplement * temprange;
    }
};

//this class models the heating up/cooling down of the lights, and estimates the temperature of the lamp.
//If the temperature reaches above a threshold, that output will be scaled down.
//This is a feedforward model only, and there is not feedback, so the temperature can be a bit off.
//You have to measure heating/cooling coefficients of the lamp to get a usable model.
//This class is memory intensive (22 bytes/lamp), so be careful when adding a lot of lamps
template <class T_COLOUR>
class TemperatureControlledOutput : public Output
{
public:
    TemperatureControlledOutput(Output *childOutput, float heatupCoefficient, float cooldownCoefficient, float maxTemperature, float ambientTemperature = 22.4)
    {
        this->childOutput = childOutput;

        float equilibriumduty = cooldownCoefficient * (maxTemperature - ambientTemperature) / heatupCoefficient;
        float equilibriumdutycomplement = 1.0 - equilibriumduty;

        parameters.heatupCoefficient = heatupCoefficient;                                                              //how fast the lamp will heat up if it is fully on
        parameters.cooldownCoefficient = cooldownCoefficient;                                                          //how fast the lamp will cool down if it is off
        parameters.maxTemperature = maxTemperature;                                                                    //the maximum temperature we allow
        parameters.ambientTemperature = ambientTemperature;                                                            //the temperature of the room
        parameters.maxtemperatureDescent = 0.999;                                                                      //this makes sure the doesnt cool down too fast, if the model work work corrently we wouldnt need this
        parameters.equilibriumDutyCycleComplement = equilibriumdutycomplement;                                         //1-the equilibriumdutycycle. The equilibrium duty cycle is pwm duty cycle at which the heatup and cooldown are equal, and the lamp would exentuallt exactly reach the maximum temperature.
        parameters.correctionThresholdTemperature = ambientTemperature + (maxTemperature - ambientTemperature) * 0.75; //The temperature where the model start scaling down. If the temperature is below this value the requested duty cycle doesn't have to be corrected.

        SetLength(1*sizeof(T_COLOUR));
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        //store the requested colour, so we can use it to further dim the values
        //if no input comes in, but the lamp is so bright it will overheat.
        memcpy((uint8_t*)this->pixelData + index, data, size);

        if (size != sizeof(T_COLOUR))
            return; //currently only works if setdata is called pixel by pixels

        //back-calculate which pixel this was
        int modelIndex = index / sizeof(T_COLOUR);

        //calculate the time between the last update and the new update.
        int dt = millis() - lastFrame;

        //the model needs the requested intensity as a value between 0 and 1.
        //to do this, first read the colour from the data argument,
        //then make it monochromatic, in case it wasn't already, get the
        //brightness value and divide by 255.
        T_COLOUR colour;
        memcpy(&colour,data,sizeof(T_COLOUR));
        Monochrome monochrome = static_cast<Monochrome>(colour);
        float requestedIntensity = float(monochrome.L) / 255;

        //let the model calculate the correction factor
        float correctionFactor = models[modelIndex].getCorrectionFactor(requestedIntensity, dt);

        //dim the colour and send it to the child Output
        colour.dim(correctionFactor * 255);
        childOutput->SetData((uint8_t*) &colour, sizeof(T_COLOUR), index);
    }

    boolean Ready()
    {
        if (millis() - lastFrame < 10)
            return false; //limit the framerate to minimize the effect of temporal quantization (because we measure time in ms)
        return childOutput->Ready();
    }

    void Show()
    {
        lastFrame = millis();
        childOutput->Show();
    }

    void ShowGroup()
    {
        //showgroup will be called regularly, even if there is no new input.
        //I use this to perform additional temperature correction in case no
        //new data comes in, and setdata therefore cannot perform the correction.
        int dt = millis() - lastFrame;

        if (dt > 100)
        {
            for (int i = 0; i < length; i++)
            {
                float correctionFactor = models[i].getCorrectionFactor(models[i].previousRequestedIntensity, dt);
                T_COLOUR dimmed = pixelData[i];
                dimmed.dim(correctionFactor * 255);
                childOutput->SetData((uint8_t*)  &dimmed, sizeof(T_COLOUR), i);
            }
            Show();
        }
        childOutput->ShowGroup();
    }

    void Begin() override
    {
        childOutput->Begin();
        lastFrame = millis();
    }

    void Clear()
    {
        childOutput->Clear();
    }

    void SetLength(int len) override
    {
        while (!childOutput->Ready()) delay(1); 

        if (this->length != len / sizeof(T_COLOUR))
        {
            Debug.printf("set len %d, %d, old=%d\n", len, len / sizeof(T_COLOUR), this->length);
            this->length = len / sizeof(T_COLOUR);
            
            pixelData = (T_COLOUR*) realloc(pixelData, len);
            memset((void*)pixelData,0,len);
            models = (TemperatureModel*) realloc(models, length * sizeof(TemperatureModel));
            
            //i initialize all models to the maximum temperature. this is safest
            for (int i = 0; i < length; i++)
                models[i] = TemperatureModel(&parameters, parameters.maxTemperature);

        }
        childOutput->SetLength(len);
    }

private:
    Output *childOutput;
    int length;
    TemperatureModel *models = NULL;
    T_COLOUR *pixelData = NULL;
    unsigned long lastFrame = 0;

    TemperatureModelParameters parameters;
};
