// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Colour
#define GT_Colour

namespace GT
{
    class ColorF
    {
    public:
    
        ColorF()
            : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
        {
        }
        
        ColorF(float r, float g, float b, float a = 1.0f)
            : r(r), g(g), b(b), a(a)
        {
        }

        ColorF(const char *value, ptrdiff_t valueSizeInBytes = -1);


        inline       float* GetPointer()       { return &this->r; }
        inline const float* GetPointer() const { return &this->r; }

        inline       float & operator[] (int index)       { return (&this->r)[index]; }
        inline const float & operator[] (int index) const { return (&this->r)[index]; }

        inline bool operator==(const ColorF &other) const { return this->r == other.r && this->g == other.g && this->b == other.b && this->a == other.a; }
        inline bool operator!=(const ColorF &other) const { return !(*this == other); }
        
    public:
    
        /// The red, green, blue and alpha colour components.
        float r, g, b, a;


    public:
    
        static ColorF Black;
        static ColorF White;
        static ColorF TransparentBlack;
        static ColorF TransparentWhite;
        static ColorF Red;
        static ColorF Green;
        static ColorF Blue;
        static ColorF Alpha;
        
        static ColorF DefaultBackground;
        
        
    public:
    
        /// Tries to convert a string to a colour, returning true if successful.
        static bool TryParse(ColorF &colour, const char *value, ptrdiff_t valueSizeInBytes = -1);

        /// Converts a colour to a string. The returned string will be in the format #xxxxxx
        static void ToString(char *dest, size_t destSize, const ColorF &colour);
    };

    typedef ColorF Color;
}


#endif
