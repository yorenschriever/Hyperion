#pragma once
#include "pattern.h"
#include "helpers/pixelMap.h"
#include "helpers/fade.h"
#include <vector>
#include <math.h>

namespace Ledster
{
    int petals[6][45] = {
        {261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305},
        {306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 144, 145, 179, 180, 210, 211, 237, 238, 260, 261},
        {341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 0, 20, 21, 45, 46, 74, 75, 107, 108, 144},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410},
        {411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 9, 10, 32, 33, 59, 60, 90, 91, 125, 126},
        {446, 447, 448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 126, 162, 163, 195, 196, 224, 225, 249, 250, 270} 
    };

    class RadarPattern : public LayeredPattern<RGBA>
    {
        PixelMap map;
        LFO<SawDown> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        std::vector<float> scaledAngles;
        FadeDown fade1 = FadeDown(1400, WaitAtEnd);

    public:
        RadarPattern(PixelMap map, int period = 5000)
        {
            this->map = map;
            this->lfo = LFO<SawDown>(period);
            this->lfo.setSkew(0.25);
            std::transform(map.begin(), map.end(), std::back_inserter(scaledAngles), [](PixelPosition pos) -> float
                           { return (atan2(pos.y, pos.x) + PI) / (2 * PI); });
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                RGBA colour = Params::getSecondaryColour();
                float lfoVal = lfo.getValue(scaledAngles[index]);
                RGBA dimmedColour = colour * transition.getValue() * lfoVal;
                pixels[index] += dimmedColour;

                if (index == 200 && lfoVal > 0.5)//TODO timeline.happened
                {
                    fade1.reset();
                }
            }

            RGBA dot = RGBA(255, 255, 255, 255) * transition.getValue() * fade1.getValue();
            pixels[200] += dot;
            pixels[199] += dot;
            pixels[201] += dot;

            pixels[190] += dot;
            pixels[191] += dot;

            pixels[220] += dot;
            pixels[221] += dot;
        }
    };


    class PetalChasePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[6] = {
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd)
        };
        Timeline timeline = Timeline(500);
        int pos = 0;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();
            if (timeline.Happened(0)){
                pos = (pos+1)%6;
                fade[pos].reset();
            }

            for(int i=0; i < 6; i++) {
                RGBA col = Params::getSecondaryColour() * fade[i].getValue() * transition.getValue();
                for(int j=0; j < 45; j++)
                    pixels[petals[i][j]] += col;
            }
        }
    };

} // namespace Mapped