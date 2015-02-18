// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDevice_OpenGL21_hpp_
#define __GT_GE_GPURenderingDevice_OpenGL21_hpp_

#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>

#if defined(GT_GE_BUILD_OPENGL21)
#include <GTGameEngine/GPURenderingDevice_Gen1.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/Vector.hpp>

namespace GT
{
    namespace GE
    {
        /// OpenGL 2.1 implementation of the rendering device.
        class GPURenderingDevice_OpenGL21 : public GPURenderingDevice_Gen1
        {
        public:

            /// Constructor.
            GPURenderingDevice_OpenGL21(const GPURenderingDeviceInfo &info);

            /// Destructor.
            virtual ~GPURenderingDevice_OpenGL21();


            /// GPURenderingDevice::Startup().
            ResultCode Startup();

            /// GPURenderingDevice::Shutdown().
            void Shutdown();


            /// GPURenderingDevice::GetRenderingAPI().
            RenderingAPI GetRenderingAPI() const;



            /// GPURenderingDevice::SetSwapInterval().
            void SetSwapInterval(int swapInterval);


            ////////////////////////////////////////////
            // Drawing

            /// GPURenderingDevice::ClearColor().
            void ClearColor(float r, float g, float b, float a);

            /// GPURenderingDevice::Draw().
            void Draw(unsigned int indexCount, unsigned int startIndexLocation);



            /////////////////////////////////////////////
            // State

            /// GPURenderingDevice::SetPrimitiveTopology().
            void SetPrimitiveTopology(PrimitiveTopology topology);

            /// GPURenderingDevice::SetCurrentVertexBuffer().
            void SetCurrentVertexBuffer(GPUBuffer* buffer);

            /// GPURenderingDevice::SetCurrentIndexBuffer().
            void SetCurrentIndexBuffer(GPUBuffer* buffer);

            /// GPURenderingDevice::SetCurrentConstantBuffer().
            void SetCurrentConstantBuffer(GPUBuffer* buffer, unsigned int slot);



            ////////////////////////////////////////////
            // Shaders

            /// GPURenderingDevice::CompileShader().
            ResultCode CompileShader(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut);

            /// GPURenderingDevice::IsShaderTargetSupported().
            bool IsShaderTargetSupported(GPUShaderTarget target) const;



            ////////////////////////////////////////////
            // Buffers

            /// GPURenderingDevice::CreateBuffer().
            ResultCode CreateBuffer(GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags, size_t sizeInBytes, const void* data, GPUBuffer* &bufferOut);

            /// GPURenderingDevice::DeleteBuffer().
            void DeleteBuffer(GPUBuffer* buffer);

            /// GPURenderingDevice::MapBuffer().
            ResultCode MapBuffer(GPUBuffer* buffer, GPUBufferMapType mapType, void* &dataOut);

            /// GPURenderingDevice::UnmapBuffer().
            void UnmapBuffer(GPUBuffer* buffer);

            /// GPURenderingDevice::SetBufferData().
            ResultCode SetBufferData(GPUBuffer* buffer, size_t offsetInBytes, size_t sizeInBytes, const void* data);



            ///////////////////////////////////////////
            // Framebuffers

            /// GPURenderingDevice::CreateFramebuffer().
            ResultCode CreateFramebuffer(Framebuffer* &framebuffer);



            //////////////////////////////////////////
            // Platform-Specific Methods

#if defined(GT_PLATFORM_WINDOWS)
            /// GPURenderingDevice::SwapBuffers().
            void SwapBuffers();

            /// GPURenderingDevice::InitWindowFramebuffer().
            ResultCode InitWindowFramebuffer(HWND hWnd, bool includeDepthStencil);

            /// GPURenderingDevice::UninitWindowFramebuffer().
            void UninitWindowFramebuffer(HWND hWnd);

            /// GPURenderingDevice::SetCurrentWindow().
            ResultCode SetCurrentWindow(HWND hWnd);
#endif

#if defined(GT_PLATFORM_LINUX)
#endif


        private:

            /// Helper for compiling a GLSL shader.
            ResultCode CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut);
            ResultCode CompileShader_GLSL(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &messagesOut, GLuint &objectGLOut);

            /// Helper for compiler an ARB shader program.
            ResultCode CompileShader_ARB(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, GT::BasicBuffer &byteCodeOut, GT::BasicBuffer &messagesOut);



        private:

            /// The OpenGL context object.
            OpenGLContext m_gl;


#if defined(GT_PLATFORM_WINDOWS)
            /// Structure containing the elements that make up the framebuffer for a given window.
            struct HWNDFramebuffer
            {
                /// A pointer to the swap-chain object for the framebuffer.
                HDC m_hDC;
            };

            /// A list of HWNDs mapped to their framebuffer object.
            GTLib::Map<HWND, HWNDFramebuffer> m_windowFramebuffers;


            /// A handle to the current window where rendering operations will be output to.
            HWND m_currentHWND;

            /// A handle to the DC of the current window.
            HDC m_currentDC;
#endif

            /// The list of supported shader targets.
            GTLib::Vector<GPUShaderTarget> m_supportedShaderTargets;


            ////////////////////////////////////////////////////
            // State

            /// Boolean state flags.
            uint32_t m_stateFlags;

            /// The current primitive topology for use by the OpenGL API. 
            GLenum m_currentTopologyGL;


            /// A pointer to the current vertex buffer.
            GPUBuffer* m_currentVertexBuffer;

            /// A pointer to the current index buffer.
            GPUBuffer* m_currentIndexBuffer;


            
            ///////////////////////////////////////////
            // State Flags

            static const uint32_t StageFlag_IsWindowFramebufferCurrent = (1 << 0);     //< Is the current window's framebuffer the current render target?

        public:

            //////////////////////////////////////////////////////
            // Error Codes

            static const ResultCode FailedToCreateShaderObject      = (1 << 31) | 0x000000A0;
            static const ResultCode FailedToCompileShader           = (1 << 31) | 0x000000A1;

            // Win32 Errors
#if defined(GT_PLATFORM_WINDOWS)
#endif

            // Linux/X11 Errors
#if defined(GT_PLATFORM_LINUX)
#endif


        private:    // No copying.
            GPURenderingDevice_OpenGL21(const GPURenderingDevice_OpenGL21 &);
            GPURenderingDevice_OpenGL21 & operator=(const GPURenderingDevice_OpenGL21 &);
        };
    }
}

#endif

#endif
