// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Color.hpp>
#include <GTLib/Strings/Trim.hpp>
#include <GTLib/Parse.hpp>
#include <cstdio>

namespace GT
{
    Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
    Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
    Color Color::TransparentBlack(0.0f, 0.0f, 0.0f, 0.0f);
    Color Color::TransparentWhite(1.0f, 1.0f, 1.0f, 0.0f);
    Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
    Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
    Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);
    Color Color::Alpha(0.0f, 0.0f, 0.0f, 1.0f);

    Color Color::DefaultBackground(0.16f, 0.164f, 0.20f, 1.0f);


    Color::Color(const char *value, ptrdiff_t valueSize)
        : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
    {
        TryParse(*this, value, valueSize);
    }

    bool Color::TryParse(Color &colour, const char *value, ptrdiff_t valueSize)
    {
        value     = GTLib::Strings::TrimStart(value, valueSize);
        valueSize = GTLib::Strings::TrimEnd(value, valueSize) - value;

        if (valueSize > 0)
        {
            if (*value == '#')
            {
                // Looking at a HTML colour.
                ++value;
                --valueSize;

                uint32_t r = 0;
                uint32_t g = 0;
                uint32_t b = 0;
                uint32_t a = 255;

                char rStr[3] = { '\0' };
                char gStr[3] = { '\0' };
                char bStr[3] = { '\0' };

                // valueSize must be 3 or 6. Anything else, and we can't convert.
                if (valueSize == 6)
                {
                    rStr[0] = value[0]; rStr[1] = value[1];
                    gStr[0] = value[2]; gStr[1] = value[3];
                    bStr[0] = value[4]; bStr[1] = value[5];
                }
                else if (valueSize == 3)
                {
                    rStr[0] = value[0]; rStr[1] = value[0];
                    gStr[0] = value[1]; gStr[1] = value[1];
                    bStr[0] = value[2]; bStr[1] = value[2];
                }
                else
                {
                    return false;
                }

                r = GTLib::Parse<uint32_t>(rStr, 16);
                g = GTLib::Parse<uint32_t>(gStr, 16);
                b = GTLib::Parse<uint32_t>(bStr, 16);

                colour.r = r / 255.0f;
                colour.g = g / 255.0f;
                colour.b = b / 255.0f;
                colour.a = a / 255.0f;

                return true;
            }
        }

        return false;
    }

    void Color::ToString(char *dest, size_t destSize, const Color &colour)
    {
        char r[3];
        snprintf(r, 3, "%.2x", static_cast<uint32_t>(colour.r * 255.0f));

        char g[3];
        snprintf(g, 3, "%.2x", static_cast<uint32_t>(colour.g * 255.0f));

        char b[3];
        snprintf(b, 3, "%.2x", static_cast<uint32_t>(colour.b * 255.0f));

        snprintf(dest, destSize, "#%s%s%s", r, g, b);
    }
}