#pragma once
#include "pattern.h"
#include "helpers/pixelMap.h"
#include "helpers/fade.h"
#include "helpers/interval.h"
#include "helpers/timeline.h"
#include <vector>
#include <math.h>

namespace Ledster
{
    int petals[6][45] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 410, 409, 408, 407, 406, 405, 404, 403, 402, 401, 400, 399, 398, 397, 396, 395, 394, 393, 392, 391, 390, 389, 388, 387, 386, 385, 384, 383, 382, 381, 380, 379, 378, 377, 376},
        {9, 10, 32, 33, 59, 60, 90, 91, 125, 126, 445, 444, 443, 442, 441, 440, 439, 438, 437, 436, 435, 434, 433, 432, 431, 430, 429, 428, 427, 426, 425, 424, 423, 422, 421, 420, 419, 418, 417, 416, 415, 414, 413, 412, 411},
        {126, 162, 163, 195, 196, 224, 225, 249, 250, 270, 480, 479, 478, 477, 476, 475, 474, 473, 472, 471, 470, 469, 468, 467, 466, 465, 464, 463, 462, 461, 460, 459, 458, 457, 456, 455, 454, 453, 452, 451, 450, 449, 448, 447, 446},
        {270, 269, 268, 267, 266, 265, 264, 263, 262, 261, 305, 304, 303, 302, 301, 300, 299, 298, 297, 296, 295, 294, 293, 292, 291, 290, 289, 288, 287, 286, 285, 284, 283, 282, 281, 280, 279, 278, 277, 276, 275, 274, 273, 272, 271},
        {261, 260, 238, 237, 211, 210, 180, 179, 145, 144, 340, 339, 338, 337, 336, 335, 334, 333, 332, 331, 330, 329, 328, 327, 326, 325, 324, 323, 322, 321, 320, 319, 318, 317, 316, 315, 314, 313, 312, 311, 310, 309, 308, 307, 306},
        {144, 108, 107, 75, 74, 46, 45, 21, 20, 0, 375, 374, 373, 372, 371, 370, 369, 368, 367, 366, 365, 364, 363, 362, 361, 360, 359, 358, 357, 356, 355, 354, 353, 352, 351, 350, 349, 348, 347, 346, 345, 344, 343, 342, 341},
    };

    std::vector<uint16_t> hexagons[10] = {
        {135},
        {116, 117, 134, 154, 153, 136},
        {100, 99, 98, 118, 133, 155, 170, 171, 172, 152, 137, 115},
        {81, 82, 83, 84, 97, 119, 132, 156, 169, 189, 188, 187, 186, 173, 151, 138, 114, 101},
        {69, 68, 67, 66, 65, 85, 96, 120, 131, 157, 168, 190, 201, 202, 203, 204, 205, 185, 174, 150, 139, 113, 102, 80},
        {50, 51, 52, 53, 54, 55, 64, 86, 95, 121, 130, 158, 167, 191, 200, 220, 219, 218, 217, 216, 215, 206, 184, 175, 149, 140, 112, 103, 79, 70},
        {42, 41, 40, 39, 38, 37, 36, 56, 63, 87, 94, 122, 129, 159, 166, 192, 199, 221, 228, 229, 230, 231, 232, 233, 234, 214, 207, 183, 176, 148, 141, 111, 104, 78, 71, 49},
        {23, 24, 25, 26, 27, 28, 29, 30, 35, 57, 62, 88, 93, 123, 128, 160, 165, 193, 198, 222, 227, 247, 246, 245, 244, 243, 242, 241, 240, 235, 213, 208, 182, 177, 147, 142, 110, 105, 77, 72, 48, 43},
        {19, 18, 17, 16, 15, 14, 13, 12, 11, 31, 34, 58, 61, 89, 92, 124, 127, 161, 164, 194, 197, 223, 226, 248, 251, 252, 253, 254, 255, 256, 257, 258, 259, 239, 236, 212, 209, 181, 178, 146, 143, 109, 106, 76, 73, 47, 44, 22},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 32, 33, 59, 60, 90, 91, 125, 126, 162, 163, 195, 196, 224, 225, 249, 250, 270, 269, 268, 267, 266, 265, 264, 263, 262, 261, 260, 238, 237, 211, 210, 180, 179, 145, 144, 108, 107, 75, 74, 46, 45, 21, 20}};

    std::vector<uint16_t> snake = {
        135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125, 91, 90, 60, 59, 33, 32, 10, 9, 410, 409, 408, 407, 406, 405, 404, 403, 402, 401, 400, 399, 398, 397, 396, 395, 394, 393, 392, 391, 390, 389, 388, 387, 386, 385, 384, 383, 382, 381, 380, 379, 378, 377, 376, 0, 20, 21, 45, 46, 74, 75, 107, 108, 144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 154, 170, 189, 201, 220, 228, 247, 251, 270, 250, 249, 225, 224, 196, 195, 163, 162, 126, 445, 444, 443, 442, 441, 440, 439, 438, 437, 436, 435, 434, 433, 432, 431, 430, 429, 428, 427, 426, 425, 424, 423, 422, 421, 420, 419, 418, 417, 416, 415, 414, 413, 412, 411, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 19, 23, 42, 50, 69, 81, 100, 116, 135, 153, 172, 186, 205, 215, 234, 240, 259, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 480, 479, 478, 477, 476, 475, 474, 473, 472, 471, 470, 469, 468, 467, 466, 465, 464, 463, 462, 461, 460, 459, 458, 457, 456, 455, 454, 453, 452, 451, 450, 449, 448, 447, 446, 126, 125, 91, 90, 60, 59, 33, 32, 10, 9, 11, 30, 36, 55, 65, 84, 98, 117, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 179, 180, 210, 211, 237, 238, 260, 261, 305, 304, 303, 302, 301, 300, 299, 298, 297, 296, 295, 294, 293, 292, 291, 290, 289, 288, 287, 286, 285, 284, 283, 282, 281, 280, 279, 278, 277, 276, 275, 274, 273, 272, 271, 270, 250, 249, 225, 224, 196, 195, 163, 162, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 116, 100, 81, 69, 50, 42, 23, 19, 0, 20, 21, 45, 46, 74, 75, 107, 108, 144, 340, 339, 338, 337, 336, 335, 334, 333, 332, 331, 330, 329, 328, 327, 326, 325, 324, 323, 322, 321, 320, 319, 318, 317, 316, 315, 314, 313, 312, 311, 310, 309, 308, 307, 306, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 251, 247, 228, 220, 201, 189, 170, 154, 135, 117, 98, 84, 65, 55, 36, 30, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 375, 374, 373, 372, 371, 370, 369, 368, 367, 366, 365, 364, 363, 362, 361, 360, 359, 358, 357, 356, 355, 354, 353, 352, 351, 350, 349, 348, 347, 346, 345, 344, 343, 342, 341, 144, 145, 179, 180, 210, 211, 237, 238, 260, 261, 259, 240, 234, 215, 205, 186, 172, 153, 135};

    uint16_t ribben[36][10] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        {9, 10, 32, 33, 59, 60, 90, 91, 125, 126},
        {126, 162, 163, 195, 196, 224, 225, 249, 250, 270},
        {270, 269, 268, 267, 266, 265, 264, 263, 262, 261},
        {261, 260, 238, 237, 211, 210, 180, 179, 145, 144},
        {144, 108, 107, 75, 74, 46, 45, 21, 20, 0},
        {135, 134, 133, 132, 131, 130, 129, 128, 127, 126},
        {135, 117, 98, 84, 65, 55, 36, 30, 11, 9},
        {135, 116, 100, 81, 69, 50, 42, 23, 19, 0},
        {135, 136, 137, 138, 139, 140, 141, 142, 143, 144},
        {135, 153, 172, 186, 205, 215, 234, 240, 259, 261},
        {135, 154, 170, 189, 201, 220, 228, 247, 251, 270},
        {270, 271, 272, 273, 274, 275, 276, 277, 278, 279},
        {279, 280, 281, 282, 283, 284, 285, 286, 287, 288},
        {288, 289, 290, 291, 292, 293, 294, 295, 296, 297},
        {297, 298, 299, 300, 301, 302, 303, 304, 305, 261},
        {261, 306, 307, 308, 309, 310, 311, 312, 313, 314},
        {314, 315, 316, 317, 318, 319, 320, 321, 322, 323},
        {323, 324, 325, 326, 327, 328, 329, 330, 331, 332},
        {332, 333, 334, 335, 336, 337, 338, 339, 340, 144},
        {144, 341, 342, 343, 344, 345, 346, 347, 348, 349},
        {349, 350, 351, 352, 353, 354, 355, 356, 357, 358},
        {358, 359, 360, 361, 362, 363, 364, 365, 366, 367},
        {367, 368, 369, 370, 371, 372, 373, 374, 375, 0},
        {0, 376, 377, 378, 379, 380, 381, 382, 383, 384},
        {384, 385, 386, 387, 388, 389, 390, 391, 392, 393},
        {393, 394, 395, 396, 397, 398, 399, 400, 401, 402},
        {402, 403, 404, 405, 406, 407, 408, 409, 410, 9},
        {9, 411, 412, 413, 414, 415, 416, 417, 418, 419},
        {419, 420, 421, 422, 423, 424, 425, 426, 427, 428},
        {428, 429, 430, 431, 432, 433, 434, 435, 436, 437},
        {437, 438, 439, 440, 441, 442, 443, 444, 445, 126},
        {126, 446, 447, 448, 449, 450, 451, 452, 453, 454},
        {454, 455, 456, 457, 458, 459, 460, 461, 462, 463},
        {463, 464, 465, 466, 467, 468, 469, 470, 471, 472},
        {472, 473, 474, 475, 476, 477, 478, 479, 480, 270}};

    class RadarPattern : public LayeredPattern<RGBA>
    {
        PixelMap map;
        LFO<SawDown> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        std::vector<float> scaledAngles;
        FadeDown fade = FadeDown(2400, WaitAtEnd);

    public:
        RadarPattern(PixelMap map)
        {
            this->map = map;
            this->lfo = LFO<SawDown>(5000);
            this->lfo.setSkew(0.25);
            this->lfo.setPulseWidth(0.7);
            std::transform(map.begin(), map.end(), std::back_inserter(scaledAngles), [](PixelPosition pos) -> float
                           { return (atan2(pos.y, pos.x) + PI) / (2 * PI); });
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            lfo.setPeriod(Params::getVelocity(10000, 1000));
            lfo.setSkew(Params::getIntensity(0.33, 1));

            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                RGBA colour = Params::getPrimaryColour();
                float lfoVal = lfo.getValue(scaledAngles[index]);
                RGBA dimmedColour = colour * transition.getValue() * lfoVal;
                pixels[index] += dimmedColour;

                if (index == 200 && lfoVal > 0.5) // TODO timeline.happened
                {
                    fade.reset();
                }
            }

            RGBA dot = RGBA(255, 255, 255, 255) * transition.getValue() * fade.getValue();
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
            FadeDown(1400, WaitAtEnd)};
        TempoWatcher watcher = TempoWatcher();
        int pos = 0;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            for(int i=0;i<6;i++) 
                fade[i].duration = Params::getIntensity(3000, 100);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                pos = (pos + 1) % 6;
                fade[pos].reset();
            }

            for (int i = 0; i < 6; i++)
            {
                RGBA col = Params::getPrimaryColour() * fade[i].getValue() * transition.getValue();
                for (int j = 0; j < 45; j++)
                    pixels[petals[i][j]] += col;
            }
        }
    };

    class PetalGlitchPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        Timeline timeline = Timeline(1000);
        int petal = 0;

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            //i made a timelineline longer than 60, and reset it here manually, 
            //so it resets exactly during this frame instead of freely cycling at its own pace
            timeline.FrameStart();
            if (timeline.Happened(60))
            {
                timeline.reset();
                petal = (petal + 1 + stableRandom(4)) % 6;
            }

            RGBA col = Params::getHighlightColour() * transition.getValue();
            for (int j = 0; j < 45; j++)
                pixels[petals[petal][j]] += col;
        }
    };

    class ConcentricChaserPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo = LFO<SawDown>(2000);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            //lfo.setPeriod(Params::getVelocity(10000, 1000));

            if (!transition.Calculate(active))
                return;

            for (int hex = 0; hex < 10; hex++)
            {
                int size = hexagons[hex].size();
                for (int i = 0; i < size; i++)
                {
                    float phase = (float)i / size;
                    if (hex % 2 == 0)
                        phase += 0.5;
                    if (phase > 1)
                        phase -= 1;

                    if (hex % 2 == 0)
                        phase = 1.0 - phase;

                    pixels[hexagons[hex][i]] += Params::getSecondaryColour() * lfo.getValue(phase, 1500 + hex*250) * transition.getValue();
                }
            }
        }
    };

    class HexBeatPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade = FadeDown(200, WaitAtEnd);
        TempoWatcher watcher = TempoWatcher();

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered()){
                fade.reset();
            }
            fade.duration = Params::getIntensity(300,50);

            int dist = Params::getVelocity(100,15);
            for (int hex = 0; hex < 10; hex++)
            {
                RGBA colour = Params::getHighlightColour() * fade.getValue(hex * dist) * transition.getValue();
                for (int i = 0; i < hexagons[hex].size(); i++)
                {
                    pixels[hexagons[hex][i]] += colour;
                }
            }
        }
    };

    class SnakePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDownShort> lfo = LFO<SawDownShort>(2000);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            lfo.setPeriod(Params::getVelocity(10000, 500));
            lfo.setSkew(Params::getIntensity());
            lfo.setPulseWidth(1);
            int variant = Params::getVariant() * 7 + 1;

            // lfo.setSkew(0.5);
            // lfo.setPulseWidth(1);
            // int variant = 5;

            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < snake.size(); i++)
            {
                pixels[snake[i]] += Params::getSecondaryColour() * lfo.getValue((float)i / snake.size() * variant) * transition.getValue();
            }
        }
    };

    class RadialFadePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        std::vector<float> normalizedRadii;
        TempoWatcher watcher = TempoWatcher();

    public:
        RadialFadePattern(PixelMap map)
        {
            this->map = map;
            std::transform(map.begin(), map.end(), std::back_inserter(normalizedRadii), [](PixelPosition pos) -> float
                           { return sqrt(pos.y * pos.y + pos.x * pos.x); });
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            fade.duration = Params::getIntensity(500,120);
            int velocity = Params::getVelocity(500,50);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
            }

            for (int i = 0; i < normalizedRadii.size(); i++)
            {
                pixels[i] += Params::getPrimaryColour() * fade.getValue(normalizedRadii[i] * velocity) * transition.getValue();
            }
        }
    };

    template <class T>
    class RibbenClivePattern : public LayeredPattern<RGBA>
    {
        const int numSegments = 36;
        const int segmentSize = 10;
        int averagePeriod;
        float precision;
        LFO<T> lfo = LFO<T>();
        Permute perm = Permute(36);

    public:
        RibbenClivePattern(int averagePeriod = 10000, float precision = 1, float pulsewidth = 0.025)
        {
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            this->lfo.setPulseWidth(pulsewidth);
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            for (int ribbe = 0; ribbe < numSegments; ribbe++)
            {
                int interval = averagePeriod + perm.at[ribbe] * (averagePeriod * precision) / numSegments;
                RGBA col = Params::getPrimaryColour() * lfo.getValue(0, interval);
                for (int j = 0; j < segmentSize; j++)
                {
                    pixels[ribben[ribbe][j]] += col;
                }
            }
        }
    };

    class AdidasPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SoftSquare> lfo = LFO<SoftSquare>(2000);
        const std::vector<uint16_t> trails[4] = {
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 32, 33, 59, 60, 90, 91, 125, 126, 162, 163, 195, 196, 224, 225, 249, 250, 270, 269, 268, 267, 266, 265, 264, 263, 262, 261, 260, 238, 237, 211, 210, 180, 179, 145, 144, 108, 107, 75, 74, 46, 45, 21, 20},
            {19, 18, 17, 16, 15, 14, 13, 12, 11, 31, 34, 58, 61, 89, 92, 124, 127, 161, 164, 194, 197, 223, 226, 248, 251, 252, 253, 254, 255, 256, 257, 258, 259, 239, 236, 212, 209, 181, 178, 146, 143, 109, 106, 76, 73, 47, 44, 22},
            {5, 14, 28, 37, 55, 64, 86, 95, 121, 130, 159, 165, 194, 196, 224, 223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 181, 177, 148, 140, 112, 103, 79, 70, 50, 41, 25, 16, 4},
            {15, 27, 38, 54, 65, 85, 96, 120, 131, 158, 166, 193, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 182, 176, 149, 139, 113, 102, 80, 69, 51, 40, 26}};

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPulseWidth(0.05);
            lfo.setPeriod(Params::getVelocity(4000,500));
            int amount = Params::getIntensity(1,6);

            for (int trailnr = 0; trailnr < 4; trailnr++)
            {
                auto trail = trails[trailnr];
                auto col = trailnr >= 2 ? Params::getPrimaryColour() : Params::getSecondaryColour();
                for (int i = 0; i < trail.size(); i++)
                {
                    float phase = float(i) / trail.size() + (trailnr >= 2 ? 0.1 : 0);
                    if (trailnr >= 2)
                        phase = 1.0 - phase;
                    pixels[trail[i]] +=
                        col *
                        lfo.getValue(phase * amount) *
                        transition.getValue();
                }
            }
        }
    };

    class ChevronsPattern : public LayeredPattern<RGBA>
    {
        PixelMap map;
        LFO<SawDown> lfo;
        LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade1 = FadeDown(1400, WaitAtEnd);

    public:
        ChevronsPattern(PixelMap map)
        {
            this->map = map;
            this->lfo = LFO<SawDown>(1000);
            this->lfoColour = LFO<Square>(1000);
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            float amount = Params::getIntensity(0.25,4);
            lfo.setPeriod(Params::getVelocity(2000,500));
            lfoColour.setPeriod(Params::getVariant(2000,500));

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                float phase = (0.5 * abs(map[index].y) + map[index].x) * amount;
                auto col = lfoColour.getValue(phase) ? Params::getSecondaryColour() : Params::getPrimaryColour();
                pixels[index] += col * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class SnowflakePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            auto col = Params::getPrimaryColour() * transition.getValue();
            for (auto petal : petals)
            {
                for (int j = 0; j < 45; j++)
                {
                    float dist = 1 - abs((float)j - 9 + 5 - 45. / 2) / 10;
                    pixels[petal[j]] += col * dist;
                }
            }

            auto hex = hexagons[9];
            for (int j = 0; j < hex.size(); j++)
            {
                float dist = abs(float(j % (hex.size() / 6)) - (hex.size() / 12) - 0.5) / 4;
                pixels[hex[j]] += col * dist;
            }

            hex = hexagons[3];
            for (int j = 0; j < hex.size(); j++)
            {
                float dist = abs(float(j % (hex.size() / 6)) - (hex.size() / 12) - 0.5) / 2;
                pixels[hex[j]] += col * dist;
            }
        }
    };

    class PetalRotatePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo = LFO<SawDown>(600);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(Params::getVelocity(1000,400));
            float phase = Params::getVariant(45,45*2);

            auto col = Params::getSecondaryColour() * transition.getValue();
            for (auto petal : petals)
            {
                for (int j = 0; j < 45; j++)
                {
                    pixels[petal[j]] += col * lfo.getValue((float)j / phase);
                }
            }
        }
    };

    class MandalaPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        const uint16_t notfilled[72] = {24, 25, 28, 29, 35, 37, 39, 41, 43, 48, 49, 52, 53, 56, 57, 78, 79, 82, 83, 86, 87, 93, 95, 97, 99, 101, 103, 105, 110, 111, 114, 115, 118, 119, 122, 123, 147, 148, 151, 152, 155, 156, 159, 160, 165, 167, 169, 171, 173, 175, 177, 183, 184, 187, 188, 191, 192, 213, 214, 217, 218, 221, 222, 227, 229, 231, 233, 235, 241, 242, 245, 246};

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            int notfilledIndex = 0;
            for (int i = 0; i < 271; i++)
            {
                if (notfilled[notfilledIndex] == i)
                    notfilledIndex++;
                else
                    pixels[i] += Params::getHighlightColour() * transition.getValue();
            }

            // auto hex = hexagons[9];
            // for (int j = 0; j < hex.size(); j++)
            // {
            //     pixels[hex[j]] += Params::getHighlightColour() * transition.getValue();
            // }
        }
    };

    class RadialGlitterFadePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        std::vector<float> normalizedRadii;
        //Timeline timeline = Timeline(1000);
        TempoWatcher watcher = TempoWatcher();
        Permute perm;

    public:
        RadialGlitterFadePattern(PixelMap map)
        {
            this->map = map;
            std::transform(map.begin(), map.end(), std::back_inserter(normalizedRadii), [](PixelPosition pos) -> float
                           { return sqrt(pos.y * pos.y + pos.x * pos.x); });
            this->perm = Permute(map.size());
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = Params::getIntensity(500,100);

            //timeline.FrameStart();
            //if (timeline.Happened(0))
            if (watcher.Triggered())
            {
                fade.reset();
                perm.permute();
            }

            float velocity = Params::getVelocity(600,100);
            float trail = Params::getIntensity(1,3);

            for (int i = 0; i < normalizedRadii.size(); i++)
            {
                fade.duration = perm.at[i] * trail; // + 100;
                pixels[i] += Params::getSecondaryColour() * fade.getValue(normalizedRadii[i] * velocity);
            }
        }
    };

    class PixelGlitchPattern : public LayeredPattern<RGBA>
    {
        Timeline timeline = Timeline(50);
        Permute perm = Permute(0);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();
            perm.setSize(width);

            if (timeline.Happened(0))
                perm.permute();

            uint8_t val = timeline.GetTimelinePosition() < 25 ? 255 * transition.getValue() : 0;

            for (int index = 0; index < width / 2; index++)
                pixels[perm.at[index]] += Params::getSecondaryColour() * val;
        }
    };

    class SquareGlitchPattern : public LayeredPattern<RGBA>
    {
        //Timeline timeline = Timeline(50);
        Permute perm = Permute(256);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        PixelMap map;

    public:
        SquareGlitchPattern(PixelMap map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            //timeline.FrameStart();

            //if (timeline.Happened(0))
                perm.permute();

            int threshold = Params::getIntensity(width * 0.1, width * 0.5);
            int numSquares = Params::getVariant(2,9);

            for (int index = 0; index < width; index++)
            {
                int xquantized = (map[index].x + 1) * numSquares;
                int yquantized = (map[index].y + 1) * numSquares;
                int square = xquantized + yquantized * numSquares;
                if (perm.at[square] > threshold / numSquares )
                    continue;
                pixels[index] += Params::getHighlightColour() * transition.getValue();
            }
        }
    };

    template <class T>
    class ClivePattern : public LayeredPattern<RGBA>
    {
        int numSegments;
        int averagePeriod;
        float precision;
        LFO<T> lfo;
        Permute perm;

    public:
        ClivePattern(int numSegments = 10, int averagePeriod = 1000, float precision = 1, float pulsewidth = 1)
        {
            this->numSegments = std::max(numSegments, 1);
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            this->perm = Permute(numSegments);
            this->lfo.setPulseWidth(pulsewidth);
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
            {
                int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
                int interval = averagePeriod + permutedQuantized * (averagePeriod * precision) / width;
                pixels[index] += Params::getSecondaryColour() * lfo.getValue(0, interval);
            }
        }
    };

    class KonamiFadePattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap map;
        FadeDown fade[8] = {
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd)};
        const int interval = 500;
        Timeline timeline = Timeline(8 * interval);
        int8_t directionsx[8] = {-1, -1, 1, 1, 0, 0, 0, 0};
        int8_t directionsy[8] = {0, 0, 0, 0, 1, -1, 1, -1};

    public:
        KonamiFadePattern(PixelMap map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();
            for (int i = 0; i < 8; i++)
            {
                if (timeline.Happened(i * interval))
                {
                    fade[i].reset();
                }
                fade[i].duration = Params::getIntensity(500,100);
            }

            for (int i = 0; i < std::min(width, (int)map.size()); i++)
            {
                float cumulativeFadeValue = 0;
                for (int j = 0; j < 8; j++)
                {
                    cumulativeFadeValue += fade[j].getValue((((float)map[i].x * directionsy[j] + map[i].y * directionsx[j]) + 1.0) * 300);
                }

                pixels[i] += Params::getHighlightColour() * cumulativeFadeValue * transition.getValue();
            }
        }
    };

    class RadialRainbowPattern : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap map;
        std::vector<float> normalizedRadii;
        LFO<SawUp> lfo = LFO<SawUp>(1000);

    public:
        RadialRainbowPattern(PixelMap map)
        {
            this->map = map;
            std::transform(map.begin(), map.end(), std::back_inserter(normalizedRadii), [](PixelPosition pos) -> float
                           { return sqrt(pos.y * pos.y + pos.x * pos.x); });
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(Params::getVelocity(4000,500));
            float scale = Params::getIntensity(0.5,3);
            if (Params::getVariant() < 0.5)
              scale *= -1;

            for (int i = 0; i < normalizedRadii.size(); i++)
            {
                pixels[i] += ((RGBA)Hue((normalizedRadii[i]*scale + lfo.getValue()) * 255)) * transition.getValue();
            }
        }
    };


    class StrobePattern : public LayeredPattern<RGBA>
    {
        // Timeline timeline = Timeline(100);

        // inline void Calculate(RGBA *pixels, int width, bool active) override
        // {
        //     if (!active) return;

        //     timeline.SetDuration(Params::getVelocity(400,100));

        //     timeline.FrameStart();
        //     RGBA color = timeline.GetTimelinePosition() < 40 ? Params::getHighlightColour() : RGBA(0,0,0,255);

        //     for (int index = 0; index < width; index++)
        //         pixels[index] = color;
        // }

        int framecounter = 1;

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active) return;

            framecounter--;

            RGBA color = RGBA(0,0,0,255);
            if (framecounter <= 1)
                color = Params::getPrimaryColour();

            if (framecounter == 0)
                framecounter = 5; //Params::getVelocity(40,4);

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class FlashesPattern : public LayeredPattern<RGBA>
    {
        FadeDown fade = FadeDown(2400, WaitAtEnd);
        TempoWatcher watcher = TempoWatcher();

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active) return;
            fade.duration = Params::getVelocity(1500,100);

            if (watcher.Triggered())
                fade.reset();

            RGBA color;
            float val = fade.getValue();
            if (val >= 0.5)
                color = Params::getSecondaryColour() + RGBA(255,255,255,255) * ((val - 0.5)*2);
            else 
                color = Params::getSecondaryColour() * ((val - 0.5)*2); 

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class RibbenFlashPattern : public LayeredPattern<RGBA>
    {
        const int numSegments = 36;
        const int segmentSize = 10;
        Permute perm = Permute(36);
        TempoWatcher watcher = TempoWatcher();
        FadeDown fade = FadeDown(2400, WaitAtEnd);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active)
                return;

            if (watcher.Triggered()) {
                perm.permute();
                fade.reset();
            }

            fade.duration = Params::getVelocity(2500,100);

            int numVisible = Params::getIntensity(1,numSegments);

            for (int ribbe = 0; ribbe < numVisible; ribbe++)
            {
                RGBA col = Params::getHighlightColour() * fade.getValue();
                for (int j = 0; j < segmentSize; j++)
                {
                    pixels[ribben[perm.at[ribbe]][j]] += col;
                }
            }
        }
    };

    class DoorPattern : public LayeredPattern<RGBA>
    {
        PixelMap map;
        Transition transition = Transition(
            1000, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        DoorPattern(PixelMap map, int period = 1000)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            for (int index = 0; index < 271; index++)
            {
                auto col = 2*abs(map[index].y) < (transition.getValue()) ? RGBA(0,0,0,255) : RGBA(0,0,0,0);
                pixels[index] += col ;
            }

        }
    };

    class RadialFadePattern2 : public LayeredPattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        std::vector<float> normalizedRadii;
        Timeline interval = Timeline(500);

    public:
        RadialFadePattern2(PixelMap map)
        {
            this->map = map;
            std::transform(map.begin(), map.end(), std::back_inserter(normalizedRadii), [](PixelPosition pos) -> float
                           { return sqrt(pos.y * pos.y + pos.x * pos.x); });
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            fade.duration = Params::getIntensity(500,120);
            int velocity = Params::getVelocity(500,50);
            interval.SetDuration(Params::getVariant(500,2000));

            if (!transition.Calculate(active))
                return;

            interval.FrameStart();
            if (interval.Happened(0))
            {
                fade.reset();
            }

            for (int i = 0; i < normalizedRadii.size(); i++)
            {
                pixels[i] += Params::getPrimaryColour() * fade.getValue((1.-normalizedRadii[i]) * velocity) * transition.getValue();
            }
        }
    };


} // namespace