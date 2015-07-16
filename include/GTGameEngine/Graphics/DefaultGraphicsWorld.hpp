// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_DefaultGraphicsWorld_hpp_
#define __GT_DefaultGraphicsWorld_hpp_

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
        virtual HGraphicsResource CreateTextureResource(unsigned int width, unsigned int height, unsigned int depth, TextureFormat format, const void* pData);

        /// @copydoc GraphicsWorld::CreateMaterialResource()
        virtual HGraphicsResource CreateMaterialResource();

        /// @copydoc GraphicsWorld::CreateMeshResource()
        virtual HGraphicsResource CreateMeshResource();

        /// @copydoc GraphicsWorld::DeleteResource()
        virtual void DeleteResource(HGraphicsResource hResource);



        ////////////////////
        // Objects

        /// @copydoc GraphicsWorld::CreateMeshObject()
        virtual HGraphicsObject CreateMeshObject(HGraphicsResource hMeshResource, const vec4 &position = vec4(0, 0, 0, 1), const quat &rotation = quat::identity, const vec4 &scale = vec4(1, 1, 1, 1));

        /// @copydoc GraphicsWorld::DeleteObject()
        virtual void DeleteObject(HGraphicsObject);



        ////////////////////
        // Render Targets

#if defined(GT_PLATFORM_WINDOWS)
        /// @copydoc GraphicsWorld::CreateRenderTargetFromWindow()
        virtual HGraphicsRenderTarget CreateRenderTargetFromWindow(HWND hWnd, uint32_t flags);
#endif

        /// @copydoc GraphicsWorld::CreateRenderTargetFromTexture()
        virtual HGraphicsRenderTarget CreateRenderTargetFromTexture(HGraphicsResource hTextureResource, uint32_t flags);

        /// @copydoc GraphicsWorld::DeleteRenderTarget()
        virtual void DeleteRenderTarget(HGraphicsRenderTarget hRT);


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
    

    private:

        /// A pointer to the graphics world that all rendering commands will be routed to.
        GraphicsWorld* m_pGraphicsWorldImpl;
    };
}

#endif
