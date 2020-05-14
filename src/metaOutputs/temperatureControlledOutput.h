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

        SetLength(1);
    }

    //index and size are in bytes
    void SetData(uint8_t *data, int size, int index)
    {
        if (size != sizeof(T_COLOUR))
            return; //currently only works if setdata is called pixel by pixels

        //Todo: also scale existing data if no new input comes in
        //create a task for this?
        T_COLOUR colour = *(data);

        float dt = 0.1; //TODO

        //make it monochromatic, in case it wasn't already
        Monochrome monochrome = static_cast<Monochrome>(colour);

        float requestedIntensity = float(monochrome.L) / 255;
        float correctedIntensity = models[i].getCorrectedIntensity(requestedIntensity, dt);

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
        //showgroup will be called regularly,even if there is no new input.
        //I use this to perform additional temperature correction
        if (dt > 100)
        {
            for
                *(int i = 0; i < length; i++)
                {
                    //todo remember the orinal colour so we can
                    SetData()
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

        temp
    }

private:
    Output *childOutput;
    int length;
    TemperatureModel *models;

    TemperatureModelParameters parameters;
};

struct TemperatureModelParameters
{
    float heatupCoefficient;
    float cooldownCoefficient;
    float maxTemperature;
    float ambientTemperature;
    float maxtemperatureDescent; // = 0.999
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

        //if the actual temp becomes higer that out slow temp, that is more important to avoid overheating, so
        //pickup that value immideately
        return fmax(slowtemp, temp);
    }

    float calcPwmScale(float temp)
    {
        float temprange = fminf(fmaxf((temp - tempthresh) / (tempmax - tempthresh), 0), 1);
        return 1.0 - equilibriumdutycomplement * temprange;
    }
}