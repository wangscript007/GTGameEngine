// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/FontServer.hpp>
#include "FontEngine_FCFT.hpp"

#include "FontEngine_FCFT.hpp"
#include "FontEngine_Win32.hpp"

#include <GTLib/Config.hpp>
#include <GTLib/System.hpp>
#include <cassert>

namespace GTLib
{
    FontServer::FontServer(GlyphMapManager &glyphMapManager)
        : m_fontEngine(nullptr), m_glyphMapManager(&glyphMapManager), eventHandlers()
    {
    #if defined(GT_PLATFORM_LINUX)
        m_fontEngine = new FontEngine_FCFT(m_glyphMapManager);
    #elif defined(GT_PLATFORM_WINDOWS)
        m_fontEngine = new FontEngine_Win32(m_glyphMapManager);
    #endif
    }

    FontServer::~FontServer()
    {
        delete m_fontEngine;
    }
    
    
    Font* FontServer::CreateFont(const FontInfo &fontInfo)
    {
        assert(m_fontEngine != nullptr);
        {
            auto handle = m_fontEngine->CreateFont(fontInfo);
            if (handle != 0)
            {
                return new Font(*this, handle);
            }
        }
        
        return nullptr;
    }
    
    void FontServer::DeleteFont(Font* font)
    {
        assert(m_fontEngine != nullptr);
        {
            if (font != nullptr)
            {
                m_fontEngine->DeleteFont(font->GetFontHandle());
                delete font;
            }
        }
    }
    

    void FontServer::AttachEventHandler(FontEventHandler* eventHandler)
    {
        // Only attach if it isn't already attached...
        if (this->eventHandlers.Find(eventHandler) == nullptr)
        {
            this->eventHandlers.Append(eventHandler);
        }
    }

    void FontServer::DetachEventHandler(FontEventHandler* eventHandler)
    {
        this->eventHandlers.Remove(this->eventHandlers.Find(eventHandler));
    }
}
