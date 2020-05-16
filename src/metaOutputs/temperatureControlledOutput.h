#pragma once

#include "outputs/Output.h"
#include <Arduino.h>
#include "debug.h"
#include "colours.h"

class TemperatureModel;

//this class models the heating up/cooling down of the lights, and estimates the temperature of the lamp.
//If the temperature reaches above a threshold, that output will be scaled down.
//This is a feedforward model only, and there is not feedback, so the temperature can be a bit off.
//You have to measure heating/cooling coefficients of the lamp to get a usable model.
template <class T_COLOUR>
class TemperatureControlledOutput : public Output
{
public:
    TemperatureControlledOutput(Output *childOutput, float heatupCoefficient, float cooldownCoefficient, float maxTemperature, float ambientTemperature = 22.4)
    {
        this->childOutput = childOutput;

        float equilibriumduty = cooldownCoefficient * (maxTemperature - ambientTemperature) / heatupCoefficient;
        float equilibriumdutycomplement = 1.0 - equilibriumduty;

        parameters.heatupCoefficient = heatupCoefficient;       //how fast the lamp will heat up if it is fully on
        parameters.cooldownCoefficient = cooldownCoefficient;   //how fast the lamp will cool down if it is off
        parameters.maxTemperature = maxTemperature;             //the maximum temperature we allow
        parameters.ambientTemperature = ambientTemperature;     //the temperature of the room
        parameters.maxtemperatureDescent = 0.999;               //this makes sure the doesnt cool down too fast, if the model work work corrently we wouldnt need this
        parameters.equilibriumDutyCycleComplement = equilibriumdutycomplement;  //1-the equilibriumdutycycle. The equilibrium duty cycle is pwm duty cycle at which the heatup and cooldown are equal, and the lamp would exentuallt exactly reach the maximum temperature.
        parameters.correctionThresholdTemperature = ambientTemperature + (maxTemperature-ambientTemperature)*0.75; //The temperature where the model start scaling down. If the temperature is below this value the requested duty cycle doesn't have to be corrected.

        SetLength(1);
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        //store the requested colour, so we can use it to further dim the values
        //if no input comes in, but the lamp is so bright it will overheat.
        memcpy(this->pixelData + index, data, copylength);

        if (size != sizeof(T_COLOUR))
            return; //currently only works if setdata is called pixel by pixels

        //calculate the time between the last update and the new update.
        float dt = 0.1; //TODO

        //the model needs the requested intensity as a value between 0 and 1.
        //to do this, first read the colour from the data argument, 
        //then make it monochromatic, in case it wasn't already, get the
        //brightness value and device by 255.
        T_COLOUR colour = *(data);
        Monochrome monochrome = static_cast<Monochrome>(colour);
        float requestedIntensity = float(monochrome.L) / 255;

        //let the model calculate the correction value
        float correctedIntensity = models[i].getCorrectedIntensity(requestedIntensity, dt);

        //dim the colour and send it to the child Output
        colour.dim(correctedIntensity * 255);
        childOutput->SetData(&colour, sizeof(T_COLOUR), index);
    }

    boolean Ready()
    {
        return childOutput->Ready();
    }

    void Show()
    {
        childOutput->Show();
    }

    void ShowGroup()
    {
        //showgroup will be called regularly, even if there is no new input.
        //I use this to perform additional temperature correction in case no
        //new data comes in, and setdata therefore cannot perform the correction.
        float dt = todo
        
        if (dt > 100)
        {
            for (int i = 0; i < length; i++)
            {
                float correctedIntensity = models[i].getCorrectedIntensity(model[i].previousRequestedIntensity, dt);
                T_COLOUR dimmed = pixelData[i];
                dimmed.dim(correctedIntensity * 255);
                childOutput->SetData(&dimmed, sizeof(T_COLOUR), i);
            }
        }
        childOutput->ShowGroup();
    }

    void Begin() override
    {
        childOutput->Begin();
    }

    void Clear()
    {
        childOutput->Clear();
    }

    void SetLength(int len) override
    {
        this->length = len / sizeof(T_COLOUR);

        pixelData = realloc(pixelData,len);
        models = realloc(models,length * sizeof(TemperatureModel));
        for (inti=0;i<length;i++)
            models[i] = TemperatureModel(&parameters,parameters->maxTemperature)
    }

private:
    Output *childOutput;
    int length;
    TemperatureModel *models = NULL;
    T_COLOUR *pixelData = NULL;

    TemperatureModelParameters parameters;
};

struct TemperatureModelParameters
{
    float heatupCoefficient;
    float cooldownCoefficient;
    float maxTemperature;
    float ambientTemperature;
    float maxtemperatureDescent; // = 0.999
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
    //dt is in ms? seconds?
    float getCorrectedIntensity(float requestedIntensity, float dt)
    {
        float oldtemp = Temperature;

        //update the temperature in the past dt interval
        Temperature = calcTemp(Temperature, pwmScale * previousRequestedIntensity, dt);

        //keep a exponentially filtered temperature average to slow down the corrections
        //exponential 0.5 filter to dampen out oscillations when max temp is reached
        slowTemperature = calcSlowTemp(slowTemperature, Temperature, oldtemp) * 0.5 + 0.5 * slowTemperature;

        //with the updated temp, calculate the new correction factor
        pwmScale = calcPwmScale(slowTemperature);

        previousRequestedIntensity = requestedIntensity;

        return pwmScale * requestedIntensity;
    }

private:
    float calcTemp(float oldtemp, float scaledpwm, float dt)
    {
        return oldtemp + (Parameters->heatupCoefficient * scaledpwm - Parameters->cooldownCoefficient * (oldtemp - Parameters->ambientTemperature)) * dt;
    }

    inline float calcSlowTemp(float slowtemp, float temp, float oldtemp)
    {
        //if the temperature is rising, keep the slowtemp at a fixed value, intil the actual temp pick it up
        //if it is cooling, cool down, but at a slow rate (maxtempdescent)
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
}