// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_DefaultGraphicsWorld
#define GT_DefaultGraphicsWorld

#include "GraphicsWorld.hpp"
#include "GraphicsAPI.hpp"

namespace GT
{
    class DefaultGraphicsWorld : public GraphicsWorld
    {
    public:

        /// Constructor.
        DefaultGraphicsWorld(GUIContext &gui, GraphicsAPI &graphicsAPI);

        /// Destructor.
        virtual ~DefaultGraphicsWorld();


        ////////////////////////////////////////////
        // Virtual Methods

        /// @copydoc GraphicsWorld::Startup()
        virtual bool Startup();

        /// @copydoc GraphicsWorld::Shutdown()
        virtual void Shutdown();



        ////////////////////
        // Resources

        /// @copydoc GraphicsWorld::CreateTextureResource()
        virtual HGraphicsResource CreateTextureResource(const GraphicsTextureResourceDesc &textureDesc);


        /// @copydoc GraphicsWorld::CreateMaterialResource()
        virtual HGraphicsResource CreateMaterialResource(const GraphicsMaterialResourceDesc &materialDesc);

        /// @copydoc GraphicsWorld::SetMaterialResourceInputVariable()
        virtual void SetMaterialResourceInputVariable(HGraphicsResource hMaterialResource, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::CreateMeshResource()
        virtual HGraphicsResource CreateMeshResource(const GraphicsMeshResourceDesc &meshDesc);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterial()
        virtual void SetMeshResourceMaterial(HGraphicsResource hMeshResource, unsigned int materialSlot, HGraphicsResource hMaterialResource);

        /// @copydoc GraphicsWorld::SetMeshResourceMaterialInputVariable()
        virtual void SetMeshResourceMaterialInputVariable(HGraphicsResource hMeshResource, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::DeleteResource()
        virtual void DeleteResource(HGraphicsResource hResource);



        ////////////////////
        // Objects

        /// @copydoc GraphicsWorld::SetObjectTransform()
        virtual void SetObjectTransform(HGraphicsObject hObject, const vec4 &position, const quat &rotation, const vec4 &scale);

        /// @copydoc GraphicsWorld::SetObjectPosition()
        virtual void SetObjectPosition(HGraphicsObject hObject, const vec4 &position);

        /// @copydoc GraphicsWorld::SetObjectRotation()
        virtual void SetObjectRotation(HGraphicsObject hObject, const quat &rotation);

        /// @copydoc GraphicsWorld::SetObjectScale()
        virtual void SetObjectScale(HGraphicsObject hObject, const vec4 &scale);


        /// @copydoc GraphicsWorld::CreateMeshObject()
        virtual HGraphicsObject CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position = vec4(0, 0, 0, 1), const quat &rotation = quat::identity, const vec4 &scale = vec4(1, 1, 1, 1));

        /// @copydoc GraphicsWorld::SetMeshObjectMaterial()
        virtual void SetMeshObjectMaterial(HGraphicsObject hMeshObject, unsigned int materialSlot, HGraphicsResource hMaterialResource);

        /// @copydoc GraphicsWorld::SetMeshObjectMaterialInputVariable()
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x);
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y);
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z);
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, float x, float y, float z, float w);
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x);
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y);
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z);
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, int x, int y, int z, int w);
        virtual void SetMeshObjectMaterialInputVariable(HGraphicsObject hMeshObject, unsigned int materialSlot, const char* variableName, HGraphicsResource hTexture);


        /// @copydoc GraphicsWorld::DeleteObject()
        virtual void DeleteObject(HGraphicsObject);



        ////////////////////
        // Render Targets

#if defined(GT_PLATFORM_WINDOWS)
        /// @copydoc GraphicsWorld::CreateRenderTargetFromWindow()
        virtual HGraphicsRenderTarget CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags);

        /// @copydoc GraphicsWorld::GetRenderTargetByWindow()
        virtual HGraphicsRenderTarget GetRenderTargetByWindow(HWND hWnd) const;
#endif

        /// @copydoc GraphicsWorld::CreateRenderTargetFromTexture()
        virtual HGraphicsRenderTarget CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags);

        /// @copydoc GraphicsWorld::DeleteRenderTarget()
        virtual void DeleteRenderTarget(HGraphicsRenderTarget hRT);


        /// @copydoc GraphicsWorld::SetRenderTargetViewport()
        virtual void SetRenderTargetViewport(HGraphicsRenderTarget hRT, int x, int y, unsigned int width, unsigned height);

        /// @copydoc GraphicsWorld::GetRenderTargetViewport()
        virtual void GetRenderTargetViewport(HGraphicsRenderTarget hRT, int &xOut, int &yOut, unsigned int &widthOut, unsigned int &heightOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetClearColor()
        virtual void SetRenderTargetClearColor(HGraphicsRenderTarget hRT, const GTLib::Colour &color);

        /// @copydoc GraphicsWorld::EnableRenderTargetColorClearing()
        virtual void EnableRenderTargetColorClearing(HGraphicsRenderTarget hRT);

        /// @copydoc GraphicsWorld::DisableRenderTargetColorClearing()
        virtual void DisableRenderTargetColorClearing(HGraphicsRenderTarget hRT);


        /// @copydoc GraphicsWorld::SetRenderTargetPriority()
        virtual void SetRenderTargetPriority(HGraphicsRenderTarget hRT, int priority);

        /// @copydoc GraphicsWorld::GetRenderTargetPriority()
        virtual int GetRenderTargetPriority(HGraphicsRenderTarget hRT) const;


        /// @copydoc GraphicsWorld::EnableRenderTarget()
        virtual void EnableRenderTarget(HGraphicsRenderTarget hRT);

        /// @copydoc GraphicsWorld::DisableRenderTarget()
        virtual void DisableRenderTarget(HGraphicsRenderTarget hRT);

        /// @copydoc GraphicsWorld::IsRenderTargetEnabled()
        virtual bool IsRenderTargetEnabled(HGraphicsRenderTarget hRT) const;


        /// @copydoc GraphicsWorld::SetRenderTargetProjectionAndView()
        virtual void SetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, const mat4 &projection, const mat4 &view);

        /// @copydoc GraphicsWorld::GetRenderTargetProjectionAndView()
        virtual void GetRenderTargetProjectionAndView(HGraphicsRenderTarget hRT, mat4 &projectionOut, mat4 &viewOut) const;


        /// @copydoc GraphicsWorld::SetRenderTargetGUISurface()
        virtual void SetRenderTargetGUISurface(HGraphicsRenderTarget hRT, HGUISurface hSurface);

        /// @copydoc GraphicsWorld::GetRenderTargetGUISurface()
        virtual HGUISurface GetRenderTargetGUISurface(HGraphicsRenderTarget hRT) const;

        /// @copydoc GraphicsWorld::SetRenderTargetSurfaceSizeToRenderTarget()
        virtual void SetRenderTargetSurfaceSizeToRenderTarget(HGraphicsRenderTarget hRT);



        ////////////////////
        // Rendering

        /// @copydoc GraphicsWorld::IsCommandBuffersSupported()
        virtual bool IsCommandBuffersSupported() const;

        /// @copydoc GraphicsWorld::BuildCommandBuffers()
        virtual void BuildCommandBuffers();

        /// @copydoc GraphicsWorld::ExecuteCommandBuffers()
        virtual void ExecuteCommandBuffers();

        /// @copydoc GraphicsWorld::ExecuteRenderingCommands()
        virtual void ExecuteRenderingCommands();


        // GUI

        /// @copydoc GUIRenderer::BeginPaintSurface()
        virtual void GUI_BeginPaintSurface(GUIContext &gui, HGUISurface hSurface, void* pInputData);

        /// @copydoc GUIRenderer::EndPaintSurface()
        virtual void GUI_EndPaintSurface(GUIContext &gui);

        /// @copydoc GUIRenderer::Clear()
        virtual void GUI_Clear(GUIContext &gui);

        /// @copydoc GUIRenderer::Clear()
        virtual void GUI_Clear(GUIContext &gui, const GTLib::Rect<int> &rect);

        /// @copydoc GUIRenderer::DrawRectangle()
        virtual void GUI_DrawRectangle(GUIContext &gui, GTLib::Rect<int> rect, GTLib::Colour colour);

        /// @copydoc GUIRenderer::SetClippingRect()
        virtual void GUI_SetClippingRect(GUIContext &gui, GTLib::Rect<int> rect);

        /// @copydoc GUIRenderer::CanDrawText()
        virtual bool GUI_CanDrawText(GUIContext &gui, HGUIFont hFont);

        /// @copydoc GUIRenderer::DrawText()
        virtual void GUI_DrawText(GUIContext &gui, const GUITextRunDesc &textRunDesc);

        /// @copydoc GUIRenderer::DrawRawImage()
        virtual void GUI_DrawRawImage(GUIContext &gui, int xPos, int yPos, unsigned int width, unsigned int height, const void* pImageData, bool isTransparent);


    private:

        /// A pointer to the graphics world that all rendering commands will be routed to.
        GraphicsWorld* m_pGraphicsWorldImpl;
    };
}

#endif
