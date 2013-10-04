// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#include <GTEngine/GUIImageManager.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTImage/Loader.hpp>

namespace GTEngine
{
    GUIImageManager::GUIImageManager()
    {
    }
    
    GUIImageManager::~GUIImageManager()
    {
    }
    
    
    ///////////////////////////////////////////
    // Create
    
    GTGUI::ImageHandle GUIImageManager::CreateImage(unsigned int width, unsigned int height, GTGUI::ImageFormat format, void* data)
    {
        return this->CreateImage(width, height, format, const_cast<const void*>(data));
    }
    
    GTGUI::ImageHandle GUIImageManager::CreateImage(unsigned int width, unsigned int height, GTGUI::ImageFormat format, const void* data)
    {
        // First, translate the format to a GTImage format. If it's not a supported format, we need to return 0.
        auto translatedFormat = GTImage::ImageFormat_None;
        if (format == GTGUI::ImageFormat_RGB8)
        {
            translatedFormat = GTImage::ImageFormat_RGB8;
        }
        else if (format == GTGUI::ImageFormat_RGBA8)
        {
            translatedFormat = GTImage::ImageFormat_RGBA8;
        }
        else
        {
            return 0;
        }
        
        auto texture = Renderer::CreateTexture2D();
        assert(texture != nullptr);
        {
            // We don't care about local content with GUI images, but we do need to keep track of the size so we can do an accurate size
            // retrieval. To do this, we just pass null to SetData(), and then make sure the local data is deleted with DeleteLocalData().
            texture->SetData(width, height, translatedFormat, nullptr);     // <-- Important we pass null here.
            texture->DeleteLocalData();
            
            Renderer::SetTexture2DData(texture, 0, width, height, translatedFormat, data, Renderer::HasFlippedTextures());
            Renderer::SetTexture2DFilter(texture, TextureFilter_Nearest, TextureFilter_Nearest);
        }
        
        return reinterpret_cast<GTGUI::ImageHandle>(texture);
    }
    
    GTGUI::ImageHandle GUIImageManager::LoadImage(const char* filePath)
    {
        auto loader = GTImage::Loader::Create(filePath);
        if (loader != nullptr)
        {
            GTImage::ImageFileInfo info;
            loader->GetImageInfo(info);
            
            if (info.format == GTImage::ImageFormat_RGB8 || info.format == GTImage::ImageFormat_RGBA8)      // <-- Only supporting RGB8 and RGBA8 formats.
            {
                GTImage::Mipmap mipmap;
                if (loader->LoadMipmap(0, mipmap))      // <-- '0' means the base mipmap.
                {
                    return this->CreateImage(mipmap.width, mipmap.height, (info.format == GTImage::ImageFormat_RGB8) ? GTGUI::ImageFormat_RGB8 : GTGUI::ImageFormat_RGBA8, mipmap.data);
                }
                else
                {
                    // Error loading the mipmap. Fall through and return null.
                }
            }
            else
            {
                // Not a supported format. Fall through and return null.
            }
        }
        
        return 0;
    }
    
    
    ///////////////////////////////////////////
    // Delete

    void GUIImageManager::DeleteImage(GTGUI::ImageHandle imageToDelete)
    {
        Renderer::DeleteTexture2D(reinterpret_cast<Texture2D*>(imageToDelete));
    }


    ///////////////////////////////////////////
    // Update

    void GUIImageManager::SetImageSubData(GTGUI::ImageHandle image, unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, void* data)
    {
        auto texture = reinterpret_cast<Texture2D*>(image);
        assert(texture != nullptr);
        {
            Renderer::SetTexture2DSubData(texture, 0, xPos, yPos, width, height, texture->GetFormat(), data, Renderer::HasFlippedTextures());
        }
    }


    ///////////////////////////////////////////
    // Properties

    void GUIImageManager::GetImageDimensions(GTGUI::ImageHandle image, unsigned int &widthOut, unsigned int &heightOut) const
    {
        auto texture = reinterpret_cast<Texture2D*>(image);
        assert(texture != nullptr);
        {
            widthOut  = texture->GetWidth();
            heightOut = texture->GetHeight();
        }
    }

    GTGUI::ImageFormat GUIImageManager::GetImageFormat(GTGUI::ImageHandle image) const
    {
        auto texture = reinterpret_cast<Texture2D*>(image);
        assert(texture != nullptr);
        {
            auto format = texture->GetFormat();
            if (format == GTImage::ImageFormat_RGB8)
            {
                return GTGUI::ImageFormat_RGB8;
            }
            else if (format == GTImage::ImageFormat_RGBA8)
            {
                return GTGUI::ImageFormat_RGBA8;
            }
        }
        
        return GTGUI::ImageFormat_Unknown;
    }
}