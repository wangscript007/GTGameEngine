// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#ifndef __GTEngine_GUIImageManager_hpp_
#define __GTEngine_GUIImageManager_hpp_

#include <GTLib/GUI/ImageManager.hpp>
#include "Rendering/Texture2D.hpp"

namespace GTEngine
{
    class GUIImageManager : public GTGUI::ImageManager
    {
    public:

        /// Constructor.
        GUIImageManager();
        
        /// Destructor.
        ~GUIImageManager();
        
        
        ///////////////////////////////////////////
        // Create
        
        /// GTGUI::ImageManager::CreateImage().
        GTGUI::ImageHandle CreateImage(unsigned int width, unsigned int height, GTGUI::ImageFormat format, void* data);
        GTGUI::ImageHandle CreateImage(unsigned int width, unsigned int height, GTGUI::ImageFormat format, const void* data);
        
        /// GTGUI::ImageManager::LoadImage().
        GTGUI::ImageHandle LoadImage(const char* filePath);
        
        
        ///////////////////////////////////////////
        // Delete
        
        /// GTGUI::ImageManager::DeleteImage().
        void DeleteImage(GTGUI::ImageHandle imageToDelete);
        
        
        ///////////////////////////////////////////
        // Update
        
        /// GTGUI::ImageManager::SetImageSubData().
        void SetImageSubData(GTGUI::ImageHandle image, unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, void* data);
        
        
        ///////////////////////////////////////////
        // Properties
        
        /// GTGUI::ImageManager::GetImageDimensions().
        void GetImageDimensions(GTGUI::ImageHandle image, unsigned int &widthOut, unsigned int &heightOut) const;
        
        /// GTGUI::ImageManager::GetImageFormat().
        GTGUI::ImageFormat GetImageFormat(GTGUI::ImageHandle image) const;



    private:
    };
}

#endif
