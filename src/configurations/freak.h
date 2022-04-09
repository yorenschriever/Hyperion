#pragma once

#include "outputs/dmxOutput.h"
#include "outputs/apcminiOutput.h"
#include "outputs/neopixelOutput.h"
#include "outputs/pwmOutput.h"
#include "outputs/udpOutput.h"
#include "outputs/rotaryOutput.h"
#include "outputs/spiOutput.h"
#include "metaoutputs/temperatureControlledOutput.h"
#include "inputs/udpInput.h"
#include "inputs/apcminiInput.h"
#include "inputs/dmxInput.h"
#include "inputs/patternInput.h"
#include "inputs/layeredPatternInput.h"
#include "inputs/layeredApcminiInput.h"
#include "patterns/rgbPatterns.h"
#include "patterns/ledstripPatterns.h"
#include "patterns/movingheadPatterns.h"
#include "patterns/monochromePatterns.h"
#include "patterns/monochromeLayeredPatterns.h"
#include "patterns/mappedPatterns.h"
#include "patterns/helpers/params.h"
#include "patterns/helpers/pixelMap.h"
#include "luts/colourCorrectionLut.h"
#include "luts/gammaLut.h"
#include "luts/incandescentLut.h"
#include "luts/colourCorrectionLut.h"
#include "colours.h"
#include "pipe.h"
#include "configurationStruct.h"

//I picked colour correction values that Fastled uses for neopixels "TypicalLEDStrip"
//http://fastled.io/docs/3.1/group___color_enums.html
//note the different order, fastled uses RGB, luts are in ouput order (GRB)
LUT* PixelLut = new ColourCorrectionLUT(1.5, 255,255, 255, 240); 
LUT *IncandescentLut = new IncandescentLUT(2.5, 4096, 200);
LUT *IncandescentLut8 = new IncandescentLUT(2.5, 255, 24);
LUT *GammaLut12 = new GammaLUT(2.5, 4096);
LUT *GammaLut8 = new GammaLUT(2.5,255);

PixelMap map0 = {
    {x : -0.9208333333333334, y : -0.17958333333333337}, 
    {x : -0.8975, y : -0.18052083333333335}, 
    {x : -0.8715624999999999, y : -0.17989583333333337}, 
    {x : -0.8444791666666667, y : -0.18072916666666666}, 
    {x : -0.8147916666666667, y : -0.18156250000000002}, 
    {x : -0.7875, y : -0.18156250000000002}, 
    {x : -0.7576041666666666, y : -0.1832291666666667}, 
    {x : -0.7273958333333334, y : -0.18291666666666667}, 
    {x : -0.6980208333333333, y : -0.18354166666666666}, 
    {x : -0.6686458333333334, y : -0.18270833333333336}, 
    {x : -0.6682291666666667, y : -0.14885416666666668}, 
    {x : -0.6675, y : -0.11447916666666669}, 
    {x : -0.666875, y : -0.08020833333333334}, 
    {x : -0.6946875, y : -0.07989583333333335}, 
    {x : -0.7185937499999999, y : -0.08046875}, 
    {x : -0.71828125, y : -0.114140625}, 
    {x : -0.7189583333333334, y : -0.14625}, 
    {x : -0.7608333333333334, y : -0.14656249999999998}, 
    {x : -0.8011458333333333, y : -0.1440625}, 
    {x : -0.8004166666666667, y : -0.10927083333333334}, 
    {x : -0.8002343749999999, y : -0.076875}, 
    {x : -0.8003125000000001, y : -0.045703125}, 
    {x : -0.778671875, y : -0.045546875}, 
    {x : -0.778671875, y : -0.07132812499999999}, 
    {x : -0.749140625, y : -0.0703125}, 
    {x : -0.748828125, y : -0.04421875}, 
    {x : -0.7486458333333333, y : -0.010833333333333358}, 
    {x : -0.7738541666666667, y : -0.0036458333333333572}, 
    {x : -0.7861458333333333, y : 0.020624999999999998}, 
    {x : -0.7886458333333334, y : 0.045625}, 
    {x : -0.7702083333333334, y : 0.06625}, 
    {x : -0.7489583333333334, y : 0.08166666666666664}, 
    {x : -0.75875, y : 0.104375}, 
    {x : -0.7869791666666667, y : 0.1103125}, 
    {x : -0.8119791666666668, y : 0.09510416666666664}, 
    {x : -0.8307291666666667, y : 0.076875}, 
    {x : -0.8469791666666667, y : 0.10260416666666665}, 
    {x : -0.874375, y : 0.11145833333333328}, 
    {x : -0.9008333333333333, y : 0.09833333333333329}, 
    {x : -0.906875, y : 0.06916666666666664}, 
    {x : -0.8759375, y : 0.06041666666666664}, 
    {x : -0.8691666666666666, y : 0.03291666666666664}, 
    {x : -0.8729166666666668, y : 0.0059375}, 
    {x : -0.9012499999999999, y : -0.010312499999999999}, 
    {x : -0.8766666666666667, y : -0.026875}, 
    {x : -0.8716666666666667, y : -0.054479166666666676}, 
    {x : -0.8721875000000001, y : -0.0865625}, 
    {x : -0.8721875000000001, y : -0.11854166666666668}, 
    {x : -0.8745312500000001, y : -0.143125}, 
    {x : -0.9153125, y : -0.14614583333333334}, 
    {x : -0.5265625, y : -0.1840625}, 
    {x : -0.4875, y : -0.18541666666666667}, 
    {x : -0.44270833333333337, y : -0.18687499999999999}, 
    {x : -0.3905208333333333, y : -0.18760416666666668}, 
    {x : -0.3451041666666667, y : -0.18812500000000001}, 
    {x : -0.3045833333333333, y : -0.18239583333333337}, 
    {x : -0.2747916666666667, y : -0.159375}, 
    {x : -0.25583333333333336, y : -0.1290625}, 
    {x : -0.2577083333333333, y : -0.09375}, 
    {x : -0.2764375, y : -0.0663125}, 
    {x : -0.200625, y : 0.10645833333333328}, 
    {x : -0.2743125, y : -0.025312499999999998}, 
    {x : -0.2525624999999999, y : -0.001062499999999993}, 
    {x : -0.24549999999999997, y : 0.026750000000000006}, 
    {x : -0.243828125, y : 0.056406250000000005}, 
    {x : -0.218203125, y : 0.051875000000000004}, 
    {x : -0.22125, y : 0.08791666666666664}, 
    {x : -0.25145833333333334, y : 0.10552083333333329}, 
    {x : -0.29, y : 0.1015625}, 
    {x : -0.315625, y : 0.075}, 
    {x : -0.3173125, y : 0.037437500000000026}, 
    {x : -0.3265625, y : -0.0019374999999999787}, 
    {x : -0.3561875, y : -0.022624999999999992}, 
    {x : -0.37262499999999993, y : -0.05387499999999999}, 
    {x : -0.3375, y : -0.06524999999999999}, 
    {x : -0.3253125, y : -0.103125}, 
    {x : -0.33677083333333335, y : -0.14156249999999998}, 
    {x : -0.37270833333333336, y : -0.15416666666666667}, 
    {x : -0.4102083333333334, y : -0.15375}, 
    {x : -0.4101041666666667, y : -0.1234375}, 
    {x : -0.40875000000000006, y : -0.09}, 
    {x : -0.40881249999999997, y : -0.0576875}, 
    {x : -0.39168749999999997, y : -0.0196875}, 
    {x : -0.40881249999999997, y : 0.010562500000000022}, 
    {x : -0.40703125, y : 0.046328125}, 
    {x : -0.374, y : 0.06387500000000003}, 
    {x : -0.3823958333333334, y : 0.10020833333333327}, 
    {x : -0.4225, y : 0.09885416666666665}, 
    {x : -0.4491666666666667, y : 0.071875}, 
    {x : -0.4686458333333333, y : 0.100625}, 
    {x : -0.5075000000000001, y : 0.0925}, 
    {x : -0.5217968749999999, y : 0.058046875}, 
    {x : -0.493671875, y : 0.052968749999999995}, 
    {x : -0.486796875, y : 0.010859375000000001}, 
    {x : -0.511625, y : -0.02043749999999999}, 
    {x : -0.48637499999999995, y : -0.051937499999999984}, 
    {x : -0.48604166666666665, y : -0.08989583333333334}, 
    {x : -0.4854166666666667, y : -0.12562500000000001}, 
    {x : -0.494765625, y : -0.153046875}, 
    {x : -0.5271874999999999, y : -0.15697916666666667}, 
    {x : -0.11072916666666668, y : -0.17708333333333334}, 
    {x : -0.06218749999999999, y : -0.1782291666666667}, 
    {x : -0.0072916666666667145, y : -0.17781249999999998}, 
    {x : 0.041145833333333284, y : -0.1784375}, 
    {x : 0.0834375, y : -0.179375}, 
    {x : 0.12895833333333329, y : -0.17979166666666668}, 
    {x : 0.1315625, y : -0.14760416666666668}, 
    {x : 0.13072916666666665, y : -0.118125}, 
    {x : 0.12875, y : -0.0878125}, 
    {x : 0.09135416666666665, y : -0.08916666666666669}, 
    {x : 0.08958333333333328, y : -0.1203125}, 
    {x : 0.0865625, y : -0.14703125}, 
    {x : 0.044921875, y : -0.146796875}, 
    {x : 0.005104166666666643, y : -0.146875}, 
    {x : 0.0036458333333332857, y : -0.1134375}, 
    {x : 0.0034375000000000005, y : -0.078125}, 
    {x : 0.001875, y : -0.04645833333333336}, 
    {x : 0.027734375, y : -0.04421875}, 
    {x : 0.028046875, y : -0.0803125}, 
    {x : 0.05671875, y : -0.0796875}, 
    {x : 0.05645833333333329, y : -0.0440625}, 
    {x : 0.05125, y : -0.008958333333333356}, 
    {x : 0.009270833333333287, y : -0.004270833333333357}, 
    {x : 0.0027083333333332857, y : 0.029270833333333322}, 
    {x : 0.011979166666666643, y : 0.061875}, 
    {x : 0.03927083333333329, y : 0.09052083333333331}, 
    {x : 0.07484375, y : 0.093515625}, 
    {x : 0.10197916666666665, y : 0.07625}, 
    {x : 0.11197916666666666, y : 0.04510416666666665}, 
    {x : 0.09364583333333329, y : 0.01614583333333332}, 
    {x : 0.12437499999999999, y : 0.002916666666666643}, 
    {x : 0.1470833333333333, y : 0.03552083333333332}, 
    {x : 0.14322916666666666, y : 0.07572916666666665}, 
    {x : 0.14229166666666665, y : 0.11541666666666664}, 
    {x : 0.11010416666666664, y : 0.09875}, 
    {x : 0.0759375, y : 0.11585937500000001}, 
    {x : 0.03041666666666664, y : 0.11875}, 
    {x : -0.007083333333333356, y : 0.10124999999999999}, 
    {x : -0.03354166666666671, y : 0.08208333333333331}, 
    {x : -0.056874999999999995, y : 0.11166666666666665}, 
    {x : -0.093125, y : 0.10625}, 
    {x : -0.10572916666666668, y : 0.06802083333333332}, 
    {x : -0.07510416666666672, y : 0.06708333333333331}, 
    {x : -0.06343750000000001, y : 0.03541666666666664}, 
    {x : -0.07083333333333336, y : 0.0003125}, 
    {x : -0.06854166666666671, y : -0.031666666666666676}, 
    {x : -0.068125, y : -0.06708333333333334}, 
    {x : -0.0690625, y : -0.10885416666666667}, 
    {x : -0.0707291666666667, y : -0.150625}, 
    {x : -0.11145833333333335, y : -0.15166666666666667}, 
    {x : 0.36343749999999997, y : -0.18562499999999998}, 
    {x : 0.4030208333333333, y : -0.1877083333333333}, 
    {x : 0.44437499999999996, y : -0.18833333333333332}, 
    {x : 0.48666666666666664, y : -0.18822916666666667}, 
    {x : 0.5014583333333332, y : -0.15458333333333335}, 
    {x : 0.5118750000000001, y : -0.12052083333333334}, 
    {x : 0.5220833333333333, y : -0.0909375}, 
    {x : 0.5311458333333332, y : -0.06364583333333336}, 
    {x : 0.537109375, y : -0.03625}, 
    {x : 0.5599999999999999, y : -0.026953125}, 
    {x : 0.5640625, y : -0.004479166666666678}, 
    {x : 0.5496875, y : 0.015729166666666645}, 
    {x : 0.5535416666666666, y : 0.04375}, 
    {x : 0.5613541666666666, y : 0.068125}, 
    {x : 0.5559375, y : 0.09625}, 
    {x : 0.5257291666666666, y : 0.1}, 
    {x : 0.5001041666666667, y : 0.0834375}, 
    {x : 0.46604166666666663, y : 0.100625}, 
    {x : 0.4486458333333333, y : 0.0709375}, 
    {x : 0.4646875, y : 0.04208333333333332}, 
    {x : 0.4607291666666667, y : 0.011354166666666643}, 
    {x : 0.44749999999999995, y : -0.010546875}, 
    {x : 0.42010416666666667, y : -0.010312499999999999}, 
    {x : 0.42132812499999994, y : -0.04359375}, 
    {x : 0.4420833333333333, y : -0.05}, 
    {x : 0.4328125, y : -0.0834375}, 
    {x : 0.42145833333333327, y : -0.11322916666666667}, 
    {x : 0.40802083333333333, y : -0.1426041666666667}, 
    {x : 0.39734375, y : -0.11968749999999999}, 
    {x : 0.387890625, y : -0.093125}, 
    {x : 0.3798750000000001, y : -0.0679375}, 
    {x : 0.37374999999999997, y : -0.047421875}, 
    {x : 0.399375, y : -0.043515625}, 
    {x : 0.39453125, y : -0.009609375}, 
    {x : 0.36843750000000003, y : -0.006875000000000001}, 
    {x : 0.36989583333333326, y : 0.02708333333333332}, 
    {x : 0.3778125, y : 0.061250000000000006}, 
    {x : 0.37416666666666665, y : 0.09583333333333328}, 
    {x : 0.33833333333333326, y : 0.09822916666666665}, 
    {x : 0.3132291666666666, y : 0.07989583333333332}, 
    {x : 0.28760416666666666, y : 0.0996875}, 
    {x : 0.2605208333333333, y : 0.08791666666666664}, 
    {x : 0.26104166666666667, y : 0.05666666666666664}, 
    {x : 0.2872916666666666, y : 0.033958333333333326}, 
    {x : 0.2934375, y : 0.0002083333333333215}, 
    {x : 0.2584375, y : -0.015104166666666679}, 
    {x : 0.3036458333333333, y : -0.031979166666666684}, 
    {x : 0.3185416666666666, y : -0.06760416666666667}, 
    {x : 0.33322916666666663, y : -0.10833333333333335}, 
    {x : 0.3489583333333333, y : -0.14833333333333334}, 
    {x : 0.6677083333333333, y : -0.18072916666666666}, 
    {x : 0.721875, y : -0.18041666666666667}, 
    {x : 0.7758333333333333, y : -0.18145833333333336}, 
    {x : 0.8240624999999999, y : -0.18156250000000002}, 
    {x : 0.8190625, y : -0.14833333333333334}, 
    {x : 0.7894791666666666, y : -0.14729166666666668}, 
    {x : 0.7886458333333333, y : -0.10854166666666668}, 
    {x : 0.7875, y : -0.06802083333333334}, 
    {x : 0.7878125, y : -0.02302083333333336}, 
    {x : 0.8136458333333334, y : -0.050833333333333355}, 
    {x : 0.8365625, y : -0.08114583333333333}, 
    {x : 0.8582291666666666, y : -0.11322916666666667}, 
    {x : 0.87375, y : -0.14677083333333335}, 
    {x : 0.8463541666666666, y : -0.1482291666666667}, 
    {x : 0.8563541666666665, y : -0.18125}, 
    {x : 0.8966666666666667, y : -0.18197916666666666}, 
    {x : 0.9391666666666666, y : -0.18177083333333335}, 
    {x : 0.9460416666666667, y : -0.14989583333333334}, 
    {x : 0.9194791666666667, y : -0.14166666666666666}, 
    {x : 0.9015625, y : -0.11541666666666668}, 
    {x : 0.8834375, y : -0.088125}, 
    {x : 0.8641666666666665, y : -0.058020833333333355}, 
    {x : 0.8966666666666667, y : -0.0365625}, 
    {x : 0.9282291666666665, y : -0.018645833333333355}, 
    {x : 0.9345833333333333, y : 0.015}, 
    {x : 0.9461458333333332, y : 0.04625}, 
    {x : 0.9715624999999999, y : 0.07125000000000001}, 
    {x : 0.9580208333333333, y : 0.10322916666666664}, 
    {x : 0.921875, y : 0.08416666666666664}, 
    {x : 0.8802083333333333, y : 0.09729166666666664}, 
    {x : 0.868125, y : 0.06854166666666664}, 
    {x : 0.8643750000000001, y : 0.026145833333333323}, 
    {x : 0.8296875, y : 0.007708333333333321}, 
    {x : 0.7971874999999999, y : 0.038645833333333324}, 
    {x : 0.8182291666666666, y : 0.06770833333333333}, 
    {x : 0.8132291666666667, y : 0.10187500000000001}, 
    {x : 0.7753125, y : 0.1061458333333333}, 
    {x : 0.7479166666666666, y : 0.084375}, 
    {x : 0.7195833333333332, y : 0.10666666666666665}, 
    {x : 0.6827083333333334, y : 0.09583333333333328}, 
    {x : 0.6728125, y : 0.06572916666666664}, 
    {x : 0.7079166666666666, y : 0.04989583333333332}, 
    {x : 0.7067708333333333, y : 0.0140625}, 
    {x : 0.6796875, y : -0.013020833333333356}, 
    {x : 0.7108333333333333, y : -0.03572916666666668}, 
    {x : 0.7098958333333333, y : -0.073125}, 
    {x : 0.7072916666666667, y : -0.108125}, 
    {x : 0.7015625, y : -0.14625}, 
    {x : 0.6673958333333333, y : -0.1482291666666667}, 
    {x : 0.668051948051948, y : -0.16050086605080832}, 
};


void updateParams()
{
    const int apcColumn = 0;
    if (APCMini::getStatus(apcColumn,0)){
        //Orange
        Params::primaryColour = RGBA(255, 100, 0, 255);
        Params::secondaryColour = RGBA(255, 100, 0, 255);
        Params::highlightColour = RGBA(255, 0, 0, 255);
    }

    if (APCMini::getStatus(apcColumn,1)){
        //Red
        Params::primaryColour = RGB(255,0,0);
        Params::secondaryColour = RGB(255,0,0);
        Params::highlightColour = RGB(255,75,0);
    }

    if (APCMini::getStatus(apcColumn,2)){
        //Yellow
        Params::primaryColour = RGB(255,255,0);
        Params::secondaryColour = RGB(255,100,0);
        Params::highlightColour = RGB(255,0,10);
    }

    if (APCMini::getStatus(apcColumn,3)){
        //Hot
        Params::primaryColour = RGB(255,0,0);
        Params::secondaryColour = RGB(255,150,0);
        Params::highlightColour = RGB(255,0,200);
    }

    if (APCMini::getStatus(apcColumn,4)){
        //Cool
        Params::primaryColour = RGB(0,100,255);
        Params::secondaryColour = RGB(0,0,200);
        Params::highlightColour = RGB(150,150,150);
    }

    if (APCMini::getStatus(apcColumn,5)){
        //Green blue
        Params::primaryColour = RGB(0,0,255);
        Params::secondaryColour = RGB(0,255,0);
        Params::highlightColour = RGB(0,127,127);
    }

    if (APCMini::getStatus(apcColumn,6)){
        //blue/orange
        Params::primaryColour = RGB(255,0,0);
        Params::secondaryColour = RGB(255,100,0);
        Params::highlightColour = RGB(255,127,0);
    }

    if (APCMini::getStatus(apcColumn,7)){
        //green red
        Params::primaryColour = RGB(255,0,255);
        Params::secondaryColour = RGB(0,0,255);
        Params::highlightColour = RGB(255,0,0);
    }
}

void LoadConfiguration()
{
    Params::primaryColour = RGBA(255, 100, 0, 255);
    Params::secondaryColour = RGBA(100, 100, 75, 255);
    Params::highlightColour = RGBA(255, 0, 0, 255);
    Params::velocity = 0.3;

    Configuration.hostname = "hyperion";

    Configuration.pwmFrequency = 1000;

    Configuration.tapMidiNote = 98;
    Configuration.tapStopMidiNote = 89;
    Configuration.tapAlignMidiNote = 88;
    Configuration.tapBarAlignMidiNote = 87;
    
    Configuration.paramCalculator = updateParams;

    Configuration.pipes = {

        ///////////////////////
        // BPM PULSE INDICATOR
        ///////////////////////
        new Pipe(
            new PatternInput<RGB>(1, new BPMIndicatorPattern()),
            new RotaryOutput()),

        ///////////////////////
        // PAR & FAIRYLIGHTS
        ///////////////////////

        //PAR
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                1,
                1,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
                    new Layered::SinPattern(),
                    new Layered::EmptyPattern(),
                    new Layered::EmptyPattern(),
                    new Layered::EmptyPattern(),

                    new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
                    new Layered::EmptyPattern(),
                    new Layered::EmptyPattern()
                }),
            new PWMOutput(12),
            Pipe::transfer<Monochrome, Monochrome12>,
            IncandescentLut),

        //FAIRY
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                1,
                1,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
                    new Layered::SinPattern(4,3.1415),
                    new Layered::BeatAllFadePattern(1000),
                    new Layered::BeatAllFadePattern(150),
                    new Layered::SlowStrobePattern(),

                    new Layered::BlinderPattern(Transition::none, Transition::none,255,100,1500),
                    new Layered::SlowStrobePattern(),
                    new Layered::FastStrobePattern(),
                }),
            new PWMOutput(11),
            Pipe::transfer<Monochrome, Monochrome12>,
            GammaLut12),

        ///////////////////////
        // VERGIET
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                18,
                2,
                new LayeredPattern<Monochrome> *[8] {
                    new Layered::BlinderPattern(Transition::fromCenter, Transition::fromSides,150,1000,1500),
                    new Layered::GlowPattern(),
                    new Layered::SinPattern(),
                    new Layered::BeatMultiFadePattern(500),
                    new Layered::LFOPattern<SawDown>(2, 1000),
                    
                    new Layered::BlinderPattern(Transition::fromLeft,Transition::fromRight),
                    new Layered::GlitchPattern(),
                    new Layered::BeatAllFadePattern(600),
                }),
            new DMXOutput(1),
            Pipe::transfer<Monochrome, Monochrome>,
            IncandescentLut8),
            
        ///////////////////////
        // FREAK LETTERS
        ///////////////////////

        new Pipe( 
            new LayeredApcminiInput<RGBA>(
                250, //width of the pattern, in pixels
                3,   //button column on the apc to listen to (0-7)
                new LayeredPattern<RGBA> *[8] {
                //the patterns to attach to the buttons
                    new LedStrip::ClivePattern(50),
                    new Mapped::HorizontalWavePattern<SinFast>(map0,1000),
                    new LedStrip::LFOPattern<SawDown>(10,2000),
                    new LedStrip::ClivePattern(5,2500),
                    new LedStrip::BeatSingleFadePattern(5),

                    new LedStrip::BlinderPattern(),
                    new LedStrip::SlowStrobePattern(),
                    new Mapped::ConcentricPulsePattern<SawDown>(map0)
                }),
            new SpiOutput(1,0,1000000),
            Pipe::transfer<RGBA, RGB>,
            PixelLut
        ),

        ///////////////////////
        // MOVING HEAD
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<MovingHead>(
                3,
                4,
                new LayeredPattern<MovingHead> *[8] {
                    //
                    //new MovingheadPatterns::TestPattern(),
                    new MovingheadPatterns::WallPattern(),
                    new MovingheadPatterns::DJPattern(),
                    new MovingheadPatterns::SidesPattern(),
                    new MovingheadPatterns::Flash360Pattern(),
                    new MovingheadPatterns::GlitchPattern(),


                    new MovingheadPatterns::FrontPattern(),
                    new MovingheadPatterns::UVPattern(),
                    new MovingheadPatterns::GlitchPattern(),
                }),
            new DMXOutput(30),
            Pipe::transfer<MovingHead,Miniwash7>
        ),

        ///////////////////////
        // KIMONOS
        ///////////////////////

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                10, //width of the pattern, in pixels
                5,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::SinPattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatMultiFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::GlitchPattern(),
                new Layered::SlowStrobePattern()
                }),
            new UDPOutput("192.168.1.183", 9601, 100),
            Pipe::transfer<Monochrome, RGB>,
            GammaLut8
        ),

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                10, //width of the pattern, in pixels
                5,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::SinPattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatMultiFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::GlitchPattern(),
                new Layered::SlowStrobePattern()
                }),
            new UDPOutput("192.168.1.184", 9601, 100),
            Pipe::transfer<Monochrome, RGB>,
            GammaLut8
        ),

        ///////////////////////
        // LASERS
        ///////////////////////
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                5, //width of the pattern, in pixels
                6,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::SinPattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatMultiFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::GlitchPattern(),
                new Layered::SlowStrobePattern()
                }),
            new UDPOutput("lasers.local", 9619, 100),
            Pipe::transfer<Monochrome, Monochrome12>
        ),

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                10, //width of the pattern, in pixels
                6,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::SinPattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatMultiFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::GlitchPattern(),
                new Layered::SlowStrobePattern()
                }),
            new UDPOutput("192.168.1.175", 9601, 100),
            Pipe::transfer<Monochrome, RGB>,
            GammaLut8
        ),

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                10, //width of the pattern, in pixels
                6,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::SinPattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatMultiFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::GlitchPattern(),
                new Layered::SlowStrobePattern()
                }),
            new UDPOutput("192.168.1.176", 9601, 100),
            Pipe::transfer<Monochrome, RGB>,
            GammaLut8
        ),

                new Pipe(
            new LayeredApcminiInput<Monochrome>(
                10, //width of the pattern, in pixels
                6,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::SinPattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatMultiFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::GlitchPattern(),
                new Layered::SlowStrobePattern()
                }),
            new UDPOutput("192.168.1.177", 9601, 100),
            Pipe::transfer<Monochrome, RGB>,
            GammaLut8
        ),

        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                10, //width of the pattern, in pixels
                6,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::SinPattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatMultiFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BlinderPattern(Transition::fromLeft,Transition::fromLeft,255),
                new Layered::GlitchPattern(),
                new Layered::SlowStrobePattern()
                }),
            new UDPOutput("192.168.1.178", 9601, 100),
            Pipe::transfer<Monochrome, RGB>,
            GammaLut8
        ),

        ///////////////////////
        // STROBES
        ///////////////////////
        new Pipe(
            new LayeredApcminiInput<Monochrome>(
                10, //width of the pattern, in pixels
                7,  //button column on the apc to listen to (0-7)
                //the patterns to attach to the buttons
                new LayeredPattern<Monochrome> *[8] {
                new Layered::BlinderPattern(Transition::fromCenter,Transition::fromCenter,80),
                //new Layered::BeatSingleFadePattern(),
                new Layered::BeatSingleFadePattern(),
                new Layered::BeatShakePattern(),
                new Layered::BeatAllFadePattern(),
                new Layered::SlowStrobePattern(),

                new Layered::BlinderPattern(Transition::fromCenter,Transition::fromCenter),
                new Layered::GlitchPattern(),
                new Layered::FastStrobePattern()
                }),
            new UDPOutput("strobes.local", 9619, 100),
            Pipe::transfer<Monochrome, Monochrome12>
        ),

    };

}

