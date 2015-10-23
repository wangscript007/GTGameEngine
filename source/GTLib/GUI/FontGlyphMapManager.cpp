// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/GUI/FontGlyphMapManager.hpp>
#include <GTLib/GUI/Server.hpp>

namespace GTGUI
{
    FontGlyphMapManager::FontGlyphMapManager(Server &server)
        : m_server(server), m_glyphMaps()
    {
    }

    FontGlyphMapManager::~FontGlyphMapManager()
    {
    }


    GTLib::GlyphMapHandle FontGlyphMapManager::CreateGlyphMap(unsigned int width, unsigned int height)
    {
        auto imageManager = m_server.GetImageManager();
        if (imageManager != nullptr)
        {
            // TODO: When we add support for an initial data pointer of null, remove this "initialData" variable.
            
            // The initial image data.
            size_t dataSize = width * height;
            auto initialData = malloc(dataSize);
            
            GTLib::GlyphMapHandle newImage = static_cast<GTLib::GlyphMapHandle>(imageManager->CreateImage(width, height, GTGUI::ImageFormat_A8, initialData));
            this->m_glyphMaps.PushBack(newImage);
            
            
            free(initialData);
            return newImage;
        }
        
        return 0;
    }

    void FontGlyphMapManager::DeleteGlyphMap(GTLib::GlyphMapHandle glyphMapHandle)
    {
        auto imageManager = m_server.GetImageManager();
        if (imageManager != nullptr)
        {
            m_glyphMaps.RemoveFirstOccuranceOf(glyphMapHandle);
            imageManager->DeleteImage(static_cast<ImageHandle>(glyphMapHandle));
        }
    }

    void FontGlyphMapManager::SetGlyphMapData(GTLib::GlyphMapHandle glyphMapHandle, int xPos, int yPos, unsigned int width, unsigned int height, void* data)
    {
        auto imageManager = m_server.GetImageManager();
        if (imageManager != nullptr)
        {
            imageManager->SetImageSubData(static_cast<ImageHandle>(glyphMapHandle), xPos, yPos, width, height, data);
        }
    }

    void* FontGlyphMapManager::MapGlyphMapData(GTLib::GlyphMapHandle)
    {
        return nullptr;
    }
    void FontGlyphMapManager::UnmapGlyphMapData(GTLib::GlyphMapHandle)
    {
    }
    
    void FontGlyphMapManager::GetGlyphMapDimensions(GTLib::GlyphMapHandle glyphMapHandle, unsigned int &widthOut, unsigned int &heightOut) const
    {
        auto imageManager = m_server.GetImageManager();
        if (imageManager != nullptr)
        {
            imageManager->GetImageDimensions(static_cast<ImageHandle>(glyphMapHandle), widthOut, heightOut);
        }
    }
}
