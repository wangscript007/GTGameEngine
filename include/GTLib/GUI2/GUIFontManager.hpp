// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_FontManager_hpp_
#define __GT_GUI_FontManager_hpp_

#include "GUIGlyphMapManager.hpp"
#include "../FontServer.hpp"
#include "../Map.hpp"

namespace GT
{
    /// Class for managing loaded fonts.
    ///
    /// When the GUI server wants a font, it will "acquire" it from the font cache. When the font is no longer
    /// needed, it will be "unacquired".
    ///
    /// Each loaded font has a reference counter. When the last reference has been unacquired, the font will be
    /// deleted completely.
    class GUIFontManager
    {
    public:

        /// Constructor.
        GUIFontManager(GUIImageManager &imageManager);

        /// Destructor.
        ~GUIFontManager();


        /// Acquires the closest matching font based on the given font information.
        ///
        /// @param fontInfo [in] A FontInfo structure containing information about the font.
        ///
        /// @return A pointer to the font if one is found; false otherwise.
        ///
        /// @remarks
        ///     So long as there is at least 1 font installed, this should never really return null.
        ///     @par
        ///     The font should be unacquired with UnacquireFont().
        GTLib::Font* AcquireFont(const GTLib::FontInfo &fontInfo);
        
        /// Unacquires a font that was previously acquired with AcquireFont().
        ///
        /// @param font [in] A pointer to the font to unacquire.
        void UnacquireFont(const GTLib::Font* font);
        

        /// Encodes a font-family string.
        ///
        /// @param family [in] The font family to encode.
        ///
        /// @return A 32-bit token that can be passed to DecodeFontFamily() to retrieve the decoded font family string.
        uint32_t EncodeFontFamily(const char* family);

        /// Decodes a font-family string from the given token.
        ///
        /// @param family [in] The font family token that was previously returned by EncodeFontFamily().
        ///
        /// @return The decoded font family string, or null if the string could not be decoded.
        const char* DecodeFontFamily(uint32_t family);

        
    private:
        
        /// Structure representing a font that's already loaded in the cache.
        struct LoadedFont
        {
            /// Constructor.
            LoadedFont(GTLib::Font* fontIn, GTLib::FontInfo fontInfoIn)
                : font(fontIn), fontInfo(fontInfoIn), referenceCount(1)
            {
            }
            
            /// Copy constructor.
            LoadedFont(const LoadedFont &other)
                : font(other.font), fontInfo(other.fontInfo), referenceCount(other.referenceCount)
            {
            }
            
            /// Assignment operator.
            LoadedFont & operator=(const LoadedFont &other)
            {
                if (this != &other)
                {
                    this->font           = other.font;
                    this->fontInfo       = other.fontInfo;
                    this->referenceCount = other.referenceCount;
                }
                
                return *this;
            }
            
            
            /// A pointer to the main font object.
            GTLib::Font* font;
            
            /// The FontInfo structure that was used to create the font. This is used in determining identical fonts.
            GTLib::FontInfo fontInfo;
            
            /// The reference count. When this hits zero, it marks the end of the font.
            int referenceCount;
        };
    
    
    private:
        
        /// Retrieves a pointer to the already-loaded font that has the same font info.
        ///
        /// @param fontInfo [in] The FontInfo structure of the font being retrieved.
        ///
        /// @return A pointer to the already-loaded font, or null if it isn't already loaded.
        ///
        /// @remarks
        ///     This does not create a new font - it simply looks at the already loaded fonts.
        ///     @par
        ///     This will increment the reference counter if non-null is returned.
        GTLib::Font* AcquireLoadedFont(const GTLib::FontInfo &fontInfo);
        
        /// Retrieves a pointer to a LoadedFont structure that is mapped to the givn GTLib::Font.
        ///
        /// @param font     [in]  A pointer to the GTLib::Font whose corresponding LoadedFont object is being retrieved.
        /// @param indexOut [out] A reference to the variable that will receive the index of the font info.
        ///
        /// @return A pointer to the LoadedFont object that corresponds to the given font.
        LoadedFont* FindLoadedFont(const GTLib::Font* font, size_t &indexOut);
    
    


    private:

        /// The object for managing glyph maps.
        GUIGlyphMapManager m_glyphMapManager;

        /// The font server. The font server depends on the glyph map manager, so it must be declared and initialized afterwards.
        GTLib::FontServer m_fontServer;

        /// The list of loaded fonts.
        GTLib::Vector<LoadedFont> m_loadedFonts;

        /// A map for mapping a font-family hash to it's actual string value.
        GTLib::Map<uint32_t, GTLib::String> m_fontFamilies;
    };
}

#endif