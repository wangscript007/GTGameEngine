// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Rendering/Renderer.hpp>
#include <GTGE/GTEngine.hpp>
#include <GTGE/Core/ImageUtils.hpp>


#include <gtgl/gtgl.h>

#if defined(_WIN32)
#include <WinGDI.h>
#else
#include <GTGE/Core/Windowing/X11/X11.hpp>
#endif


#include "../../RCCache.hpp"
#include "../../RCQueue.hpp"
#include "../RendererCaps.hpp"
#include "../Debugging_OpenGL.hpp"
#include "../TypeConversion.hpp"
#include "State_OpenGL21.hpp"
#include "ServerState_OpenGL21.hpp"
#include "VertexArray_OpenGL21.hpp"
#include "Texture2D_OpenGL21.hpp"
#include "TextureCube_OpenGL21.hpp"
#include "Shader_OpenGL21.hpp"
#include "Framebuffer_OpenGL21.hpp"



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a general explanation on how the caching system works.
//
// Conceptually, the idea is that each individual call has it's own cachable version. In reality this is too inefficient because there exists
// overhead when performing the actual caching. Instead, calls are batched based on their general type. For example, many state changes don't
// affect other state changes. Thus, state changes can often be performed in any order. If two state changes occur straight after each other,
// they can be batched into a single call instead of being separated into separate calls.
//
// Here are the different categories of calls.
//      Global State (glClearColor, glEnable/glDisable, bindings, etc)
//      Texture State
//      Buffer State
//      Shader State
//      Framebuffer State
//      Drawing (glClear, glDrawElements, etc)
//
// You will see how many categories don't actually affect each other. For example, texture bindings will never be affected by basic state
// changes. What we do is keep track of a pointer to a cached call that can be used for a call that falls into that same category. When that
// pointer is null, we just create a new cached call. It will be set to null when another call is performed where it can not longer be used
// with batching.
//
// The state of the renderer is only relevant for the construction of the cached calls. When calls are actually getting executed, they won't
// ever care about state. Thus, we use a State object that will contain the state at the time in which a cached call is created. This state
// object is used in determine when a particular operation actually needs to be performed. For example, if the current texture is the same as
// the one getting bound, it'll just skip over the call.
//
// The aforementioned state object will also contain pointers to the different cached calls where an individual call can be cached. These
// pointers will be cleared to null when the call caches are swapped. When the pointers a null, it means the call can not be batched and a
// new call will need to be created and cached.




namespace GT
{
    /////////////////////////////////////////////////////////////
    // Direct Calls

    /// Keeps track of whether or not the renderer is initialised.
    static bool IsInitialised = false;

    /// The OpenGL context.
    static GTGLcontext OpenGLContext = nullptr;

    /// The hardware capabilities. This is initialised at startup.
    static RendererCaps_OpenGL RendererCaps;

    /// The current state.
    static State_OpenGL21 State;


    /// The two call caches. The back call back is identified with BackCallCacheIndex.
    //static RCQueue CallCaches[2];

    /// The two call caches for creating resources.
    //static RCQueue ResourceCreationCallCaches[2];

    /// The two call caches for deleting resources.
    //static RCQueue ResourceDeletionCallCaches[2];

    /// The index identifying the back call cache.
    //static unsigned int BackCallCacheIndex = 0;


    /// The mutex for synching applicable resource creation operations.
    static easyutil_mutex ResourceCreationLock = NULL;

    /// The mutex for synching applicable resource deletion operations.
    static easyutil_mutex ResourceDeletionLock = NULL;


    /// The caches for individual commands. There are two of each - one for the back and one for the front.
#if 0
    static struct _RCCaches
    {
        // State RCs.
        RCCache<RCSetGlobalState>         RCSetGlobalStateCache;
        RCCache<RCSetVertexArrayState>    RCSetVertexArrayStateCache;
        RCCache<RCSetTextureState>        RCSetTextureStateCache;
        RCCache<RCSetShaderState>         RCSetShaderStateCache;
        RCCache<RCSetFramebufferStateEXT> RCSetFramebufferStateCacheEXT;
        RCCache<RCSetFramebufferStateARB> RCSetFramebufferStateCacheARB;

        // Drawing RCs.
        RCCache<RCClear>               RCClearCache;
        RCCache<RCDraw>                RCDrawCache;

        // Create and Delete RCs.
        RCCache<RCCreateVertexArray>    RCCreateVertexArrayCache;
        RCCache<RCDeleteVertexArray>    RCDeleteVertexArrayCache;
        RCCache<RCCreateTexture>        RCCreateTextureCache;
        RCCache<RCDeleteTexture>        RCDeleteTextureCache;
        RCCache<RCCreateShader>         RCCreateShaderCache;
        RCCache<RCDeleteShader>         RCDeleteShaderCache;
        RCCache<RCCreateFramebufferEXT> RCCreateFramebufferCacheEXT;
        RCCache<RCCreateFramebufferARB> RCCreateFramebufferCacheARB;
        RCCache<RCDeleteFramebufferEXT> RCDeleteFramebufferCacheEXT;
        RCCache<RCDeleteFramebufferARB> RCDeleteFramebufferCacheARB;


        _RCCaches()
            : RCSetGlobalStateCache(), RCSetVertexArrayStateCache(), RCSetTextureStateCache(), RCSetShaderStateCache(), RCSetFramebufferStateCacheEXT(), RCSetFramebufferStateCacheARB(),
              RCClearCache(), RCDrawCache(),
              RCCreateVertexArrayCache(), RCDeleteVertexArrayCache(),
              RCCreateTextureCache(),     RCDeleteTextureCache(),
              RCCreateShaderCache(),      RCDeleteShaderCache(),
              RCCreateFramebufferCacheEXT(), RCCreateFramebufferCacheARB(), RCDeleteFramebufferCacheEXT(), RCDeleteFramebufferCacheARB()
        {
        }

        void Clear(bool clearResources = false)
        {
            this->RCSetGlobalStateCache.Reset();
            this->RCSetVertexArrayStateCache.Reset();
            this->RCSetTextureStateCache.Reset();
            this->RCSetShaderStateCache.Reset();
            this->RCSetFramebufferStateCacheEXT.Reset();
            this->RCSetFramebufferStateCacheARB.Reset();


            this->RCClearCache.Reset();
            this->RCDrawCache.Reset();


            if (clearResources)
            {
                easyutil_lock_mutex(ResourceCreationLock);
                {
                    this->RCCreateVertexArrayCache.Reset();
                    this->RCCreateTextureCache.Reset();
                    this->RCCreateShaderCache.Reset();
                    this->RCCreateFramebufferCacheEXT.Reset();
                    this->RCCreateFramebufferCacheARB.Reset();
                }
                easyutil_unlock_mutex(ResourceCreationLock);


                easyutil_lock_mutex(ResourceDeletionLock);
                {
                    this->RCDeleteVertexArrayCache.Reset();
                    this->RCDeleteTextureCache.Reset();
                    this->RCDeleteShaderCache.Reset();
                    this->RCDeleteFramebufferCacheEXT.Reset();
                    this->RCDeleteFramebufferCacheARB.Reset();
                }
                easyutil_unlock_mutex(ResourceDeletionLock);
            }
        }

    }RCCaches[2];
#endif


    void SetVertexAttribState(const VertexFormat &format, const float* vertices)
    {
        // Set the vertex state.
        uint32_t  newVertexAttribEnableBits = 0x0;
        uint32_t &oldVertexAttribEnableBits = ServerState_EnabledVertexAttributes;
        size_t    formatArraySize           = format.GetAttributeCount();

        GLsizei formatSizeInBytes = static_cast<GLsizei>(format.GetSizeInBytes());
        int offset = 0;

        for (size_t i = 0; i < formatArraySize; i += 2)
        {
            GLuint   attribIndex = static_cast<GLuint>(format[i]);
            GLint    attribSize  = static_cast<GLint>( format[i + 1]);
            uint32_t bit         = static_cast<uint32_t>(1 << attribIndex);

            newVertexAttribEnableBits |= bit;

            if (!(oldVertexAttribEnableBits & bit))
            {
                glEnableVertexAttribArray(attribIndex);
            }
            else
            {
                // We clear the bit from the old bitfield because after this loop we're going to check if any bits are remaining. If so,
                // those attributes need to be disabled. By clearing, we can just check if the bitfield is 0, in which case nothing else
                // needs disabling.
                oldVertexAttribEnableBits &= ~bit;
            }

            glVertexAttribPointer(attribIndex, attribSize, GL_FLOAT, GL_FALSE, formatSizeInBytes, vertices + offset);

            // The offset must be set AFTER glVertexAttribPointer().
            offset += attribSize;
        }

        // If any attributes need to be disable, we need to do that now. This is where our enabled bitfields come in handy.
        while (oldVertexAttribEnableBits != 0)
        {
            GLuint attribIndex  = static_cast<GLuint>(NextBitIndex(oldVertexAttribEnableBits));

            glDisableVertexAttribArray(attribIndex);

            // The bit needs to be cleared in preperation for the next iteration.
            oldVertexAttribEnableBits &= ~(1 << attribIndex);
        }

        // Now we need to set the renderer's enabled bits to our new bits.
        ServerState_EnabledVertexAttributes = newVertexAttribEnableBits;
    }

    GLuint BindOpenGL21Texture(GLenum target, GLuint texture)
    {
        GLuint previousTextureObject = 0;
        if (target == GL_TEXTURE_1D)
        {
            previousTextureObject = ServerState_GL_TEXTURE_BINDING_1D;
        }
        else if (target == GL_TEXTURE_2D)
        {
            previousTextureObject = ServerState_GL_TEXTURE_BINDING_2D;
        }
        else if (target == GL_TEXTURE_3D)
        {
            previousTextureObject = ServerState_GL_TEXTURE_BINDING_3D;
        }
        else if (target == GL_TEXTURE_CUBE_MAP || (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z))
        {
            previousTextureObject = ServerState_GL_TEXTURE_BINDING_CUBE;
        }


        if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        }
        else
        {
            glBindTexture(target, texture);
        }


        return previousTextureObject;
    }


    GLuint CreateOpenGL21Shader(GLenum type, const String &sourceIn)
    {
        auto source       = sourceIn.c_str();
        auto sourceLength = static_cast<GLint>(sourceIn.GetLengthInTs());

        auto shaderObject = glCreateShader(type);
        glShaderSource(shaderObject, 1, &source, &sourceLength);
        glCompileShader(shaderObject);


        // Here we need to log any details.
        GLint compileStatus;
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileStatus);
        
        GLint logLength;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 1)
        {
            auto log = static_cast<char*>(malloc(static_cast<size_t>(logLength)));
            glGetShaderInfoLog(shaderObject, logLength, nullptr, log);

            // Whether or not we output the error message depends on the message. Some drivers output messages like "No errors." when there are no
            // errors or warnings.

            const char* successStringIntel = Strings::FindFirst(log, "No errors.");
            
            const char* successStringAMD   = nullptr;
            if (type == GL_VERTEX_SHADER)
            {
                successStringAMD = Strings::FindFirst(log, "Vertex shader was successfully compiled");
            }
            else if (type == GL_FRAGMENT_SHADER)
            {
                successStringAMD = Strings::FindFirst(log, "Fragment shader was successfully compiled");
            }

            // If we can't find one of the success strings, we'll need to log the output.
            if (!(successStringIntel == log || successStringAMD == log))
            {
                String title;
                if (type == GL_VERTEX_SHADER)
                {
                    title = "Vertex Shader Info Log";
                }
                else if (type == GL_FRAGMENT_SHADER)
                {
                    title = "Fragment Shader Info Log";
                }
                else
                {
                    title = "Geometry Shader Info Log";
                }

                g_EngineContext->Logf("--- %s ---\n%s%s", title.c_str(), log, source);


                // We only delete the shader object if we failed the compilation.
                if (compileStatus == GL_FALSE)
                {
                    glDeleteShader(shaderObject);
                    shaderObject = 0;
                }
            }

            free(log);
        }

        return shaderObject;
    }

    GLuint LinkOpenGL21Program(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader)
    {
        // We'll need a program object.
        GLuint program = glCreateProgram();


        // We need to have concretely defined vertex attributes for OpenGL 2.0 GLSL since we don't really have much control of vertex attributes
        // from inside the shader code. Thus, we're going to have to use hard coded attributes names. Later on we might make this configurable
        // via the shader library or a config file.
        glBindAttribLocation(program, 0, "VertexInput_Position");
        glBindAttribLocation(program, 1, "VertexInput_TexCoord");
        glBindAttribLocation(program, 2, "VertexInput_Normal");
        glBindAttribLocation(program, 3, "VertexInput_Tangent");
        glBindAttribLocation(program, 4, "VertexInput_Bitangent");
        glBindAttribLocation(program, 5, "VertexInput_Colour");


        // Finally we reattach the shaders, link the program and check for errors.
        if (vertexShader   != 0) glAttachShader(program, vertexShader);
        if (fragmentShader != 0) glAttachShader(program, fragmentShader);
        if (geometryShader != 0) glAttachShader(program, geometryShader);

        glLinkProgram(program);


        // Check for link errors.
        GLint linkStatus;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus == GL_FALSE)
        {
            GLint logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            auto log = static_cast<char*>(malloc(static_cast<size_t>(logLength)));
            glGetProgramInfoLog(program, logLength, nullptr, log);

            g_EngineContext->Logf("--- Program Link Status ---\n%s", log);

            free(log);

            glDeleteProgram(program);
            program = 0;
        }


        return program;
    }


    void SetOpenGL21TextureUniform(ShaderState_OpenGL21* programState, GLint location, ShaderState_OpenGL21::TextureParameter &value)
    {
        if (location != -1)
        {
            bool setUniform = true;
            bool setBinding = true;

            // We need to check if the shader state has the texture already set.
            auto iExistingParameter = programState->currentTextureUniforms.Find(location);
            if (iExistingParameter != nullptr)
            {
                // The parameter already exists. The uniform does not need to be set, but the texture binding might.
                setUniform = false;

                if (iExistingParameter->value.textureState == value.textureState)
                {
                    setBinding = false;
                }
                else
                {
                    iExistingParameter->value.textureState->shaders.RemoveFirstOccuranceOf(programState);
                    iExistingParameter->value.textureState = value.textureState;
                    iExistingParameter->value.textureState->shaders.PushBack(programState);

                    value.textureUnit = iExistingParameter->value.textureUnit;
                    value.location    = iExistingParameter->value.location;
                }
            }
            else
            {
                // The parameter has not been set before. We need to set the uniform and bind.
                value.textureUnit = programState->FindAvailableTextureUnit();
                value.location    = location;
                value.textureState->shaders.PushBack(programState);

                programState->currentTextureUniforms.Add(value.location, value);
            }


            // If this program state is the current one, we'll need to bind the texture straight away.
            if (ServerState_GL_CURRENT_PROGRAM == programState->programObject && setBinding)
            {
                glActiveTexture(GL_TEXTURE0 + value.textureUnit);
                glBindTexture(value.textureTarget, value.textureState->objectGL);

                // State needs to be set.
                if (value.textureTarget == GL_TEXTURE_1D)
                {
                    ServerState_GL_TEXTURE_BINDING_1D = value.textureState->objectGL;
                }
                else if (value.textureTarget == GL_TEXTURE_2D)
                {
                    ServerState_GL_TEXTURE_BINDING_2D = value.textureState->objectGL;
                }
                else if (value.textureTarget == GL_TEXTURE_3D)
                {
                    ServerState_GL_TEXTURE_BINDING_3D = value.textureState->objectGL;
                }
                else if (value.textureTarget == GL_TEXTURE_CUBE_MAP)
                {
                    ServerState_GL_TEXTURE_BINDING_CUBE = value.textureState->objectGL;
                }
            }

            if (setUniform)
            {
                glUniform1i(value.location, value.textureUnit);
            }
        }
    }



    bool Renderer::Startup()
    {
        assert(!IsInitialised);
        {
            ResourceCreationLock = easyutil_create_mutex();
            ResourceDeletionLock = easyutil_create_mutex();

            size_t contextAttribs[] =
            {
                GTGL_CONTEXT_MAJOR_VERSION,      2,
                GTGL_CONTEXT_MINOR_VERSION,      1,
                GTGL_CONTEXT_FORWARD_COMPATIBLE, GL_TRUE,

            #if !defined(NDEBUG)
                GTGL_CONTEXT_DEBUG,              GL_FALSE,
            #endif

                0, 0
            };

            OpenGLContext = gtglStartup(contextAttribs);
            if (OpenGLContext != nullptr)
            {
                // We'll log a few capabilities.
                unsigned int versionMajor;
                unsigned int versionMinor;
                gtglGetOpenGLVersion(&versionMajor, &versionMinor);
                g_EngineContext->Logf("OpenGL Version:  %d.%d", versionMajor, versionMinor);
                g_EngineContext->Logf("OpenGL Renderer: %s",    glGetString(GL_RENDERER));
                g_EngineContext->Logf("OpenGL Vendor:   %s",    glGetString(GL_VENDOR));

                g_EngineContext->Logf("OpenGL Extensions:");
                g_EngineContext->Logf("    ARB_vertex_program:                 %s", GTGL_ARB_vertex_program                 ? "yes" : "no");
                g_EngineContext->Logf("    ARB_fragment_program:               %s", GTGL_ARB_fragment_program               ? "yes" : "no");
                g_EngineContext->Logf("    ARB_texture_rg:                     %s", GTGL_ARB_texture_rg                     ? "yes" : "no");
                g_EngineContext->Logf("    ARB_texture_float:                  %s", GTGL_ARB_texture_float                  ? "yes" : "no");
                g_EngineContext->Logf("    ARB_framebuffer_object:             %s", GTGL_ARB_framebuffer_object             ? "yes" : "no");
                g_EngineContext->Logf("    ARB_cl_event:                       %s", GTGL_ARB_cl_event                       ? "yes" : "no");
                g_EngineContext->Logf("    ARB_timer_query:                    %s", GTGL_ARB_timer_query                    ? "yes" : "no");
                g_EngineContext->Logf("    ARB_vertex_buffer_object:           %s", GTGL_ARB_vertex_buffer_object           ? "yes" : "no");
                g_EngineContext->Logf("    ARB_get_program_binary:             %s", GTGL_ARB_get_program_binary             ? "yes" : "no");
                g_EngineContext->Logf("    ARB_separate_shader_objects:        %s", GTGL_ARB_separate_shader_objects        ? "yes" : "no");
                g_EngineContext->Logf("    ARB_ES2_compatibility:              %s", GTGL_ARB_ES2_compatibility              ? "yes" : "no");
                g_EngineContext->Logf("    EXT_framebuffer_object:             %s", GTGL_EXT_framebuffer_object             ? "yes" : "no");
                g_EngineContext->Logf("    EXT_packed_depth_stencil:           %s", GTGL_EXT_packed_depth_stencil           ? "yes" : "no");
                g_EngineContext->Logf("    EXT_texture_compression_s3tc:       %s", GTGL_EXT_texture_compression_s3tc       ? "yes" : "no");
                g_EngineContext->Logf("    EXT_texture_filter_anisotropic:     %s", GTGL_EXT_texture_filter_anisotropic     ? "yes" : "no");
                g_EngineContext->Logf("    EXT_timer_query:                    %s", GTGL_EXT_timer_query                    ? "yes" : "no");
                g_EngineContext->Logf("    NV_bindless_texture:                %s", GTGL_NV_bindless_texture                ? "yes" : "no");
                g_EngineContext->Logf("    NV_shader_buffer_load:              %s", GTGL_NV_shader_buffer_load              ? "yes" : "no");
                g_EngineContext->Logf("    NV_vertex_buffer_unified_memory:    %s", GTGL_NV_vertex_buffer_unified_memory    ? "yes" : "no");
                g_EngineContext->Logf("    NV_gpu_program4:                    %s", GTGL_NV_gpu_program4                    ? "yes" : "no");
                g_EngineContext->Logf("    NV_gpu_program5:                    %s", GTGL_NV_gpu_program5                    ? "yes" : "no");


            #if !defined(NDEBUG)
                gtglEnableSynchronousDebugOutput();
                gtglSetDebugMessageCallback(OpenGLDebugMessageHandler);
            #endif


                // Uncomment this line to test the EXT_framebuffer_object path.
                //GTGL_ARB_framebuffer_object = GL_FALSE;


                // Here we'll grab the renderer's capabilities.
                glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS,   &RendererCaps.MaxColourAttachments);
                glGetIntegerv(GL_MAX_DRAW_BUFFERS,        &RendererCaps.MaxDrawBuffers);
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &RendererCaps.MaxTextureUnits);
                RendererCaps.SupportsMixedSizedBufferAttachments = GTGL_ARB_framebuffer_object;


                // Now we'll set some defaults.
                glDepthFunc(GL_LEQUAL);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
                //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


                // We're going to initialise the X11 sub-system from here.
            #ifdef GTGL_GLX
                X11::Initialize(gtglGetDisplay());
            #endif

                IsInitialised = true;
            }
            else
            {
                g_EngineContext->LogErrorf("Error creating OpenGL context. %s", gtglGetLastError()->message);
            }
        }

        return IsInitialised;
    }

    void Renderer::Shutdown()
    {
        assert(IsInitialised);
        {
            // Objects created by the renderer need to be deleted. If this doesn't happen, Intel drivers will sometimes have a whinge after shutdown.
            //ResourceDeletionCallCaches[ BackCallCacheIndex].Execute();
            //ResourceDeletionCallCaches[!BackCallCacheIndex].Execute();

            // Both caches should be cleared.
            //CallCaches[0].Clear();
            //CallCaches[1].Clear();

            //ResourceCreationCallCaches[0].Clear();
            //ResourceCreationCallCaches[1].Clear();
            //ResourceDeletionCallCaches[0].Clear();
            //ResourceDeletionCallCaches[1].Clear();


            // The client-side state needs to be shutdown.
            State.Shutdown();


            // GTGL needs to be shutdown.
            gtglShutdown();
            OpenGLContext = nullptr;


            // Mutexes need to be deleted.
            easyutil_delete_mutex(ResourceCreationLock);
            ResourceCreationLock = NULL;

            easyutil_delete_mutex(ResourceDeletionLock);
            ResourceDeletionLock = NULL;


            // Finally, we need to mark the renderer as uninitialised.
            IsInitialised = false;
        }
    }


    Window* Renderer::CreateWindow()
    {
        assert(IsInitialised);
        {
            // When creating a window, we use GTLib to create the main window. Then we do platform specific stuff to get it working with the GTGL context.
        #ifdef _WIN32
            Window *window = new Window();
            const InternalWindowObjects &iwo = window->GetInternalObjects();

            ::SetPixelFormat(iwo.hDC, gtglGetPixelFormat(), gtglGetPFD());

            // We now need to make the window current.
            gtglSetCurrentDC(iwo.hDC);
        #else
            InternalWindowObjects iwo;
            iwo.vi       = gtglGetVisualInfo();
            iwo.colormap = gtglGetColormap();

            Window *window = new Window(iwo);

            // The window needs to be made current...
            gtglSetCurrentWindow(window->GetInternalObjects().window);
        #endif

            return window;
        }
    }

    void Renderer::SetCurrentWindow(Window *window)
    {
    #ifdef _WIN32
        if (window != nullptr)
        {
            gtglSetCurrentDC(window->GetInternalObjects().hDC);
        }
        else
        {
            gtglSetCurrentDC(0);
        }
    #else   // X
        if (window != nullptr)
        {
            gtglSetCurrentWindow(window->GetInternalObjects().window);
        }
        else
        {
            gtglSetCurrentWindow(0);
        }
    #endif
    }

    void Renderer::SwapCallCaches()
    {
        // 1) Swap the back call cache index.
        //BackCallCacheIndex = !BackCallCacheIndex;

        // 2) Clear the new back cache in preparation for filling by another thread. Note how we don't clear the resource creation and deletion
        //    call caches here. The reason is because they are cleared in ExecuteCallCache().
        //CallCaches[BackCallCacheIndex].Clear();

        easyutil_lock_mutex(ResourceCreationLock);
        //ResourceCreationCallCaches[BackCallCacheIndex].Clear();
        easyutil_unlock_mutex(ResourceCreationLock);

        easyutil_lock_mutex(ResourceDeletionLock);
        //ResourceDeletionCallCaches[BackCallCacheIndex].Clear();
        easyutil_unlock_mutex(ResourceDeletionLock);



        // 3) Clear the sub-caches.
        //RCCaches[BackCallCacheIndex].Clear();


        // 4) Cleanup deleted objects.
        State.ClearDeletedOpenGLObjects();


        // 5) Reset all of the "current" commands in order to force new ones in preparation for the new frame.
        //State.currentRCSetGlobalState      = nullptr;
        //State.currentRCSetVertexArrayState = nullptr;
        //State.currentRCSetTextureState     = nullptr;
        //State.currentRCSetShaderState      = nullptr;
        //State.currentRCSetFramebufferState = nullptr;
        //State.currentRCClear               = nullptr;
        //State.currentRCDraw                = nullptr;
    }

    void Renderer::ExecuteCallCache()
    {
        // 1) Create resources. We want to lock and clear this all at the same time.
        easyutil_lock_mutex(ResourceCreationLock);
        //ResourceCreationCallCaches[!BackCallCacheIndex].Execute();
        easyutil_unlock_mutex(ResourceCreationLock);

        // 2) Normal calls.
        //CallCaches[!BackCallCacheIndex].Execute();

        // 3) Delete resources. We want to lock, execute and clear this all at the same time.
        easyutil_lock_mutex(ResourceDeletionLock);
        //ResourceDeletionCallCaches[!BackCallCacheIndex].Execute();
        easyutil_unlock_mutex(ResourceDeletionLock);
    }

    void Renderer::SwapBuffers()
    {
        if (State.swapIntervalNeedsUpdate)
        {
            gtglSwapInterval(State.swapInterval);
            State.swapIntervalNeedsUpdate = false;
        }

        gtglSwapBuffers();
    }





    /////////////////////////////////////////////////////////////
    // Cached Calls

#if 0
    #define UPDATE_CURRENT_RC(renderCommandName) \
        if (State.current##renderCommandName == nullptr) \
        { \
            State.current##renderCommandName = &RCCaches[BackCallCacheIndex].renderCommandName##Cache.Acquire(); \
            CallCaches[BackCallCacheIndex].Append(*State.current##renderCommandName); \
        }
#endif


    ///////////////////////////
    // Simple State Changes

    void Renderer::SetViewport(int x, int y, unsigned int width, unsigned int height)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetViewport(x, y, width, height);
        }

        State.currentRCClear = nullptr;
#endif

        glViewport(x, y, width, height);
    }

    void Renderer::SetScissor(int x, int y, unsigned int width, unsigned int height)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetScissor(x, y, width, height);
        }

        State.currentRCClear = nullptr;
#endif

        glScissor(x, y, width, height);
    }


    void Renderer::SetClearColour(float r, float g, float b, float a)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetClearColour(r, g, b, a);
        }

        State.currentRCClear = nullptr;
#endif

        glClearColor(r, g, b, a);
    }

    void Renderer::SetClearDepth(float depth)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetClearDepth(depth);
        }

        State.currentRCClear = nullptr;
#endif

        glClearDepth(depth);
    }

    void Renderer::SetClearStencil(int stencil)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetClearStencil(stencil);
        }

        State.currentRCClear = nullptr;
#endif

        glClearStencil(stencil);
    }


    void Renderer::SetCurrentShader(Shader* programToMakeCurrent)
    {
        if (programToMakeCurrent != nullptr)
        {
            auto programStateToMakeCurrent = static_cast<Shader_OpenGL21*>(programToMakeCurrent)->GetOpenGLState();
            if (programStateToMakeCurrent != State.currentProgramState)
            {
#if 0
                UPDATE_CURRENT_RC(RCSetGlobalState);
                assert(State.currentRCSetGlobalState != nullptr);
                {
                    State.currentRCSetGlobalState->SetCurrentShader(programStateToMakeCurrent);
                    State.currentProgramState = programStateToMakeCurrent;
                }
#endif

                // 1) Bind the program.
                glUseProgram(programStateToMakeCurrent->programObject);

                // 2) Bind textures.
                auto &textures = programStateToMakeCurrent->currentTextureUniforms;
                for (size_t i = 0; i < textures.count; ++i)
                {
                    auto &texture = textures.buffer[i]->value;

                    glActiveTexture(GL_TEXTURE0 + texture.textureUnit);
                    glBindTexture(texture.textureTarget, texture.textureState->objectGL);

                    // State needs to be set.
                    if (texture.textureTarget == GL_TEXTURE_1D)
                    {
                        ServerState_GL_TEXTURE_BINDING_1D = texture.textureState->objectGL;
                    }
                    else if (texture.textureTarget == GL_TEXTURE_2D)
                    {
                        ServerState_GL_TEXTURE_BINDING_2D = texture.textureState->objectGL;
                    }
                    else if (texture.textureTarget == GL_TEXTURE_3D)
                    {
                        ServerState_GL_TEXTURE_BINDING_3D = texture.textureState->objectGL;
                    }
                    else if (texture.textureTarget == GL_TEXTURE_CUBE_MAP)
                    {
                        ServerState_GL_TEXTURE_BINDING_CUBE = texture.textureState->objectGL;
                    }
                }

                // 3) Set state.
                ServerState_GL_CURRENT_PROGRAM = programStateToMakeCurrent->programObject;
            }
        }
    }

    void Renderer::SetCurrentFramebuffer(Framebuffer* framebufferToMakeCurrent)
    {
        FramebufferState_OpenGL21* framebufferState = nullptr;
        if (framebufferToMakeCurrent != nullptr)
        {
            framebufferState = static_cast<Framebuffer_OpenGL21*>(framebufferToMakeCurrent)->GetOpenGLState();
        }

        if (framebufferState != State.currentFramebufferState)
        {
#if 0
            UPDATE_CURRENT_RC(RCSetGlobalState);
            assert(State.currentRCSetGlobalState != nullptr);
            {
                if (GTGL_ARB_framebuffer_object)
                {
                    // Primary (ARB_framebuffer_object).
                    State.currentRCSetGlobalState->SetCurrentFramebufferARB(framebufferState);
                }
                else
                {
                    // Secondary (EXT_framebuffer_object).
                    State.currentRCSetGlobalState->SetCurrentFramebufferEXT(framebufferState);
                }
            }
#endif

            if (GTGL_ARB_framebuffer_object)
            {
                // Primary (ARB_framebuffer_object).
                if (framebufferState != nullptr)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, framebufferState->framebufferObject);
                    ServerState_GL_FRAMEBUFFER_BINDING = framebufferState->framebufferObject;
                }
                else
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    ServerState_GL_FRAMEBUFFER_BINDING = 0;
                }
            }
            else
            {
                // Secondary (EXT_framebuffer_object).
                if (framebufferState != nullptr)
                {
                    glBindFramebufferEXT(GL_FRAMEBUFFER, framebufferState->framebufferObject);
                    ServerState_GL_FRAMEBUFFER_BINDING = framebufferState->framebufferObject;
                }
                else
                {
                    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
                    ServerState_GL_FRAMEBUFFER_BINDING = 0;
                }
            }

            State.currentFramebufferState = framebufferState;
        }
    }



    void Renderer::EnableScissorTest()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_SCISSOR_TEST);
        }

        State.currentRCClear = nullptr;
#endif

        glEnable(GL_SCISSOR_TEST);
    }

    void Renderer::DisableScissorTest()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_SCISSOR_TEST);
        }

        State.currentRCClear = nullptr;
#endif

        glDisable(GL_SCISSOR_TEST);
    }


    void Renderer::EnableBlending()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_BLEND);
        }
#endif

        glEnable(GL_BLEND);
    }

    void Renderer::DisableBlending()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_BLEND);
        }
#endif

        glDisable(GL_BLEND);
    }

    void Renderer::SetBlendFunction(BlendFunc sfactor, BlendFunc dfactor)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetBlendFunction(ToOpenGLBlendFunc(sfactor), ToOpenGLBlendFunc(dfactor));
        }
#endif

        glBlendFunc(ToOpenGLBlendFunc(sfactor), ToOpenGLBlendFunc(dfactor));
    }

    void Renderer::SetBlendEquation(BlendEquation equation)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetBlendEquation(ToOpenGLBlendEquation(equation));
        }
#endif

        glBlendEquation(ToOpenGLBlendEquation(equation));
    }

    void Renderer::SetBlendColour(float r, float g, float b, float a)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetBlendColour(r, g, b, a);
        }
#endif

        glBlendColor(r, g, b, a);
    }


    void Renderer::EnableAlphaTest()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_ALPHA_TEST);
        }
#endif

        glEnable(GL_ALPHA_TEST);
    }

    void Renderer::DisableAlphaTest()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_ALPHA_TEST);
        }
#endif

        glDisable(GL_ALPHA_TEST);
    }

    void Renderer::SetAlphaTestFunction(RendererFunction function, float ref)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetAlphaTestFunction(ToOpenGLFunc(function), static_cast<GLclampf>(ref));
        }
#endif

        glAlphaFunc(ToOpenGLFunc(function), static_cast<GLclampf>(ref));
    }


    void Renderer::EnableColourWrites()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->EnableColourWrites();
        }

        State.currentRCClear = nullptr;
#endif

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    void Renderer::DisableColourWrites()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->DisableColourWrites();
        }

        State.currentRCClear = nullptr;
#endif

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }

    void Renderer::EnableDepthWrites()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->EnableDepthWrites();
        }

        State.currentRCClear = nullptr;
#endif

        glDepthMask(GL_TRUE);
    }

    void Renderer::DisableDepthWrites()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->DisableDepthWrites();
        }

        State.currentRCClear = nullptr;
#endif

        glDepthMask(GL_FALSE);
    }


    void Renderer::EnableDepthTest()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_DEPTH_TEST);
        }
#endif

        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::DisableDepthTest()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_DEPTH_TEST);
        }
#endif

        glDisable(GL_DEPTH_TEST);
    }

    void Renderer::SetDepthFunction(RendererFunction function)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetDepthFunction(ToOpenGLFunc(function));
        }
#endif

        glDepthFunc(ToOpenGLFunc(function));
    }



    void Renderer::EnableStencilTest()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(GL_STENCIL_TEST);
        }
#endif

        glEnable(GL_STENCIL_TEST);
    }

    void Renderer::DisableStencilTest()
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(GL_STENCIL_TEST);
        }
#endif

        glDisable(GL_STENCIL_TEST);
    }

    void Renderer::SetStencilMaskSeparate(bool frontFace, bool backFace, unsigned int mask)
    {
        GLenum face;
        if (frontFace && backFace)
        {
            face = GL_FRONT_AND_BACK;
        }
        else
        {
            if (frontFace)
            {
                face = GL_FRONT;
            }
            else
            {
                face = GL_BACK;
            }
        }

#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetStencilMask(face, static_cast<GLuint>(mask));
        }
#endif

        glStencilMaskSeparate(face, mask);
    }

    void Renderer::SetStencilFuncSeparate(bool frontFace, bool backFace, RendererFunction func, int ref, unsigned int mask)
    {
        GLenum face;
        if (frontFace && backFace)
        {
            face = GL_FRONT_AND_BACK;
        }
        else
        {
            if (frontFace)
            {
                face = GL_FRONT;
            }
            else
            {
                face = GL_BACK;
            }
        }

#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetStencilFunc(face, ToOpenGLFunc(func), static_cast<GLint>(ref), static_cast<GLuint>(mask));
        }
#endif

        glStencilFuncSeparate(face, ToOpenGLFunc(func), static_cast<GLint>(ref), static_cast<GLuint>(mask));
    }

    void Renderer::SetStencilOpSeparate(bool frontFace, bool backFace, StencilOp stencilFail, StencilOp depthFail, StencilOp pass)
    {
        GLenum face;
        if (frontFace && backFace)
        {
            face = GL_FRONT_AND_BACK;
        }
        else
        {
            if (frontFace)
            {
                face = GL_FRONT;
            }
            else
            {
                face = GL_BACK;
            }
        }

#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetStencilOp(face, ToOpenGLStencilOp(stencilFail), ToOpenGLStencilOp(depthFail), ToOpenGLStencilOp(pass));
        }
#endif

        glStencilOpSeparate(face, ToOpenGLStencilOp(stencilFail), ToOpenGLStencilOp(depthFail), ToOpenGLStencilOp(pass));
    }



    void Renderer::SetFaceCulling(bool cullFront, bool cullBack)
    {
        GLenum face = GL_NONE;
        if (cullFront && cullBack)
        {
            face = GL_FRONT_AND_BACK;
        }
        else
        {
            if (cullFront)
            {
                face = GL_FRONT;
            }
            else
            {
                face = GL_BACK;
            }
        }

#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetFaceCulling(face);
        }
#endif

        if (face == GL_NONE)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
            glCullFace(face);
        }
    }


    void Renderer::EnablePolygonOffset(PolygonMode mode)
    {
        GLenum modeGL;
        if (mode == PolygonMode_Fill)
        {
            modeGL = GL_POLYGON_OFFSET_FILL;
        }
        else if (mode == PolygonMode_Line)
        {
            modeGL = GL_POLYGON_OFFSET_LINE;
        }
        else
        {
            modeGL = GL_POLYGON_OFFSET_POINT;
        }

#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Enable(modeGL);
        }
#endif

        glEnable(modeGL);
    }

    void Renderer::DisablePolygonOffset(PolygonMode mode)
    {
        GLenum modeGL;
        if (mode == PolygonMode_Fill)
        {
            modeGL = GL_POLYGON_OFFSET_FILL;
        }
        else if (mode == PolygonMode_Line)
        {
            modeGL = GL_POLYGON_OFFSET_LINE;
        }
        else
        {
            modeGL = GL_POLYGON_OFFSET_POINT;
        }

#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->Disable(modeGL);
        }
#endif

        glDisable(modeGL);
    }

    void Renderer::SetPolygonMode(bool frontFaces, bool backFaces, PolygonMode mode)
    {
        GLenum faceGL;
        if (frontFaces && backFaces)
        {
            faceGL = GL_FRONT_AND_BACK;
        }
        else
        {
            if (frontFaces)
            {
                faceGL = GL_FRONT;
            }
            else
            {
                faceGL = GL_BACK;
            }
        }


        GLenum modeGL;
        if (mode == PolygonMode_Fill)
        {
            modeGL = GL_FILL;
        }
        else if (mode == PolygonMode_Line)
        {
            modeGL = GL_LINE;
        }
        else
        {
            assert(mode == PolygonMode_Point);
            modeGL = GL_LINE;
        }

#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetPolygonMode(faceGL, modeGL);
        }
#endif

        glPolygonMode(faceGL, modeGL);
    }

    void Renderer::SetPolygonOffset(float factor, float units)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetPolygonOffset(static_cast<GLfloat>(factor), static_cast<GLfloat>(units));
        }
#endif

        glPolygonOffset(static_cast<GLfloat>(factor), static_cast<GLfloat>(units));
    }


    void Renderer::SetDrawBuffers(size_t count, const int* buffers)
    {
#if 0
        UPDATE_CURRENT_RC(RCSetGlobalState);
        assert(State.currentRCSetGlobalState != nullptr);
        {
            State.currentRCSetGlobalState->SetDrawBuffers(count, buffers);
        }

        State.currentRCClear = nullptr;
#endif

        // TODO: Remove this malloc().
        GLenum* buffersGL = (GLenum*)malloc(sizeof(GLenum) * count);
        for (size_t i = 0; i < count; ++i)
        {
            buffersGL[i] = GL_COLOR_ATTACHMENT0_EXT + buffers[i];
        }

        glDrawBuffers(static_cast<size_t>(count), buffersGL);

        free(buffersGL);
    }


    void Renderer::SetSwapInterval(int interval)
    {
        State.swapInterval            = interval;
        State.swapIntervalNeedsUpdate = true;
    }



    ///////////////////////////
    // Drawing

    void Renderer::Clear(unsigned int bufferMask)
    {
        GLbitfield glmask = 0;
        if (bufferMask & BufferType_Colour)
        {
            glmask |= GL_COLOR_BUFFER_BIT;
        }
        if (bufferMask & BufferType_Depth)
        {
            glmask |= GL_DEPTH_BUFFER_BIT;
        }
        if (bufferMask & BufferType_Stencil)
        {
            glmask |= GL_STENCIL_BUFFER_BIT;
        }


#if 0
        UPDATE_CURRENT_RC(RCClear);
        assert(State.currentRCClear != nullptr);
        {
            State.currentRCClear->Clear(glmask);
        }

        State.currentRCSetGlobalState      = nullptr;
        State.currentRCSetFramebufferState = nullptr;
#endif

        glClear(glmask);
    }

    void Renderer::Draw(const VertexArray &vertexArray, DrawMode mode)
    {
        auto &vertexArrayGL33 = static_cast<const VertexArray_OpenGL21 &>(vertexArray);

        // If the vertex array needs anything updated, do so now.
        if (vertexArrayGL33.DoesVertexDataNeedUpdating())
        {
            Renderer::PushVertexArrayVertexData(vertexArray);
        }
        if (vertexArrayGL33.DoesIndexDataNeedUpdating())
        {
            Renderer::PushVertexArrayIndexData(vertexArray);
        }


#if 0
        UPDATE_CURRENT_RC(RCDraw);
        assert(State.currentRCDraw != nullptr);
        {
            State.currentRCDraw->Draw(vertexArrayGL33.GetOpenGLVertexObjectPtr(), vertexArrayGL33.GetOpenGLIndexObjectPtr(), static_cast<GLsizei>(vertexArrayGL33.GetIndexCount()), vertexArrayGL33.GetFormat(), ToOpenGLDrawMode(mode));
        }

        State.currentRCSetGlobalState      = nullptr;
        State.currentRCSetVertexArrayState = nullptr;
        State.currentRCSetTextureState     = nullptr;
        State.currentRCSetShaderState      = nullptr;
        State.currentRCSetFramebufferState = nullptr;
        State.currentRCClear               = nullptr;
        State.currentRCDraw                = nullptr;
#endif

        // Bind the vertex array.
        glBindBuffer(GL_ARRAY_BUFFER,         *vertexArrayGL33.GetOpenGLVertexObjectPtr());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *vertexArrayGL33.GetOpenGLIndexObjectPtr());

        // Set the vertex state from the given format.
        SetVertexAttribState(vertexArrayGL33.GetFormat(), nullptr);


        // Draw.
        glDrawElements(ToOpenGLDrawMode(mode), static_cast<GLsizei>(vertexArrayGL33.GetIndexCount()), GL_UNSIGNED_INT, 0);


        // Set state.
        ServerState_GL_ARRAY_BUFFER_BINDING         = *vertexArrayGL33.GetOpenGLVertexObjectPtr();
        ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING = *vertexArrayGL33.GetOpenGLIndexObjectPtr();
    }

    void Renderer::Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount, const VertexFormat &format, DrawMode mode)
    {
#if 0
        UPDATE_CURRENT_RC(RCDraw);
        assert(State.currentRCDraw != nullptr);
        {
            State.currentRCDraw->Draw(vertices, static_cast<GLsizei>(vertexCount), indices, static_cast<GLsizei>(indexCount), format, ToOpenGLDrawMode(mode));
        }

        State.currentRCSetGlobalState      = nullptr;
        State.currentRCSetVertexArrayState = nullptr;
        State.currentRCSetTextureState     = nullptr;
        State.currentRCSetShaderState      = nullptr;
        State.currentRCSetFramebufferState = nullptr;
        State.currentRCClear               = nullptr;
        State.currentRCDraw                = nullptr;
#endif

        (void)vertexCount;


        // Bind.
        glBindBuffer(GL_ARRAY_BUFFER,         0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Set the vertex state from the given format.
        SetVertexAttribState(format, vertices);


        // Draw.
        glDrawElements(ToOpenGLDrawMode(mode), static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, indices);


        // Set State.
        ServerState_GL_ARRAY_BUFFER_BINDING         = 0;
        ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING = 0;
    }


    void Renderer::Draw(const float* vertices, const unsigned int* indices, size_t indexCount, const VertexFormat &format, DrawMode mode)
    {
        size_t vertexCount = 0;

        // We need to determine the vertex count by looking at the indices. We need the vertex count so we can know how much data to copy.
        for (size_t i = 0; i < indexCount; ++i)
        {
            if (indices[i] > vertexCount)
            {
                vertexCount = indices[i];
            }
        }

        // We just +1 to make the vertex count 1 based.
        vertexCount += 1;


        // Now we just draw like normal.
        Renderer::Draw(vertices, vertexCount, indices, indexCount, format, mode);
    }



    ///////////////////////////
    // Resources

    VertexArray* Renderer::CreateVertexArray(VertexArrayUsage usage, const VertexFormat &format)
    {
        State.instantiatedBufferObjects.PushBack(new GLuint(0));
        GLuint* vertexBufferObject = State.instantiatedBufferObjects.GetBack();

        State.instantiatedBufferObjects.PushBack(new GLuint(0));
        GLuint* indexBufferObject  = State.instantiatedBufferObjects.GetBack();


        easyutil_lock_mutex(ResourceCreationLock);
        {
#if 0
            auto &command = RCCaches[BackCallCacheIndex].RCCreateVertexArrayCache.Acquire();
            command.CreateVertexArray(vertexBufferObject, indexBufferObject, format);

            ResourceCreationCallCaches[BackCallCacheIndex].Append(command);
#endif

            glGenBuffers(1, vertexBufferObject);
            glGenBuffers(1, indexBufferObject);
        }
        easyutil_unlock_mutex(ResourceCreationLock);


        return new VertexArray_OpenGL21(usage, format, vertexBufferObject, indexBufferObject);
    }

    void Renderer::DeleteVertexArray(VertexArray* vertexArrayToDelete)
    {
        auto vertexArrayToDeleteGL33 = static_cast<VertexArray_OpenGL21*>(vertexArrayToDelete);
        if (vertexArrayToDeleteGL33 != nullptr)
        {
            // The OpenGL objects need to be marked for deletion.
            GLuint* vertexBufferObject = vertexArrayToDeleteGL33->GetOpenGLVertexObjectPtr();
            GLuint* indexBufferObject  = vertexArrayToDeleteGL33->GetOpenGLIndexObjectPtr();

            assert(vertexBufferObject != nullptr);
            assert(indexBufferObject  != nullptr);
            {
                easyutil_lock_mutex(ResourceDeletionLock);
                {
#if 0
                    auto &command = RCCaches[BackCallCacheIndex].RCDeleteVertexArrayCache.Acquire();
                    command.DeleteVertexArray(vertexBufferObject, indexBufferObject);

                    ResourceDeletionCallCaches[BackCallCacheIndex].Append(command);
#endif

                    glDeleteBuffers(1, vertexBufferObject);
                    glDeleteBuffers(1, indexBufferObject);
                }
                easyutil_unlock_mutex(ResourceDeletionLock);



                // The objects need to be marked for deletion, but not actually deleted yet.
                State.MarkBufferObjectAsDeleted(vertexBufferObject);
                State.MarkBufferObjectAsDeleted(indexBufferObject);
            }


            // We can safely delete the main object at this point.
            delete vertexArrayToDelete;
        }
    }

    void Renderer::PushVertexArrayVertexData(const VertexArray &vertexArray)
    {
        auto &vertexArrayGL33 = static_cast<const VertexArray_OpenGL21 &>(vertexArray);
        {
            GLuint* vertexBufferObject = vertexArrayGL33.GetOpenGLVertexObjectPtr();
            assert(vertexBufferObject != nullptr);
            {
#if 0
                //if (State.currentRCSetVertexArrayState == nullptr)
                {
                    State.currentRCSetVertexArrayState = &RCCaches[BackCallCacheIndex].RCSetVertexArrayStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetVertexArrayState);
                }

                assert(State.currentRCSetVertexArrayState != nullptr);
                {
                    State.currentRCSetVertexArrayState->SetVertexData(vertexBufferObject, vertexArray.GetVertexDataPtr(), vertexArray.GetVertexCount(), vertexArray.GetFormat().GetSizeInBytes(), ToOpenGLBufferUsage(vertexArray.GetUsage()));
                    vertexArrayGL33.MarkVertexDataAsUpdated();
                }
#endif

                if (ServerState_GL_ARRAY_BUFFER_BINDING != *vertexBufferObject)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, *vertexBufferObject);
                }

                glBufferData(GL_ARRAY_BUFFER, vertexArray.GetVertexCount() * vertexArray.GetFormat().GetSizeInBytes(), vertexArray.GetVertexDataPtr(), ToOpenGLBufferUsage(vertexArray.GetUsage()));

                if (ServerState_GL_ARRAY_BUFFER_BINDING != *vertexBufferObject)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, ServerState_GL_ARRAY_BUFFER_BINDING);
                }

                vertexArrayGL33.MarkVertexDataAsUpdated();
            }
        }
    }

    void Renderer::PushVertexArrayIndexData(const VertexArray &vertexArray)
    {
        auto &vertexArrayGL33 = static_cast<const VertexArray_OpenGL21 &>(vertexArray);
        {
            GLuint* indexBufferObject = vertexArrayGL33.GetOpenGLIndexObjectPtr();
            assert(indexBufferObject != nullptr);
            {
#if 0
                //if (State.currentRCSetVertexArrayState == nullptr)
                {
                    State.currentRCSetVertexArrayState = &RCCaches[BackCallCacheIndex].RCSetVertexArrayStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetVertexArrayState);
                }

                assert(State.currentRCSetVertexArrayState != nullptr);
                {
                    State.currentRCSetVertexArrayState->SetIndexData(indexBufferObject, vertexArray.GetIndexDataPtr(), vertexArray.GetIndexCount(), ToOpenGLBufferUsage(vertexArray.GetUsage()));
                    vertexArrayGL33.MarkIndexDataAsUpdated();
                }
#endif

                if (ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING != *indexBufferObject)
                {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBufferObject);
                }

                glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexArray.GetIndexCount() * sizeof(unsigned int), vertexArray.GetIndexDataPtr(), ToOpenGLBufferUsage(vertexArray.GetUsage()));

                if (ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING != *indexBufferObject)
                {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING);
                }

                vertexArrayGL33.MarkIndexDataAsUpdated();
            }
        }
    }

    void Renderer::PushVertexArrayData(const VertexArray &vertexArray)
    {
        auto &vertexArrayGL33 = static_cast<const VertexArray_OpenGL21 &>(vertexArray);
        {
            GLuint* vertexBufferObject = vertexArrayGL33.GetOpenGLVertexObjectPtr();
            GLuint* indexBufferObject  = vertexArrayGL33.GetOpenGLIndexObjectPtr();

            assert(vertexBufferObject != nullptr);
            assert(indexBufferObject  != nullptr);
            {
#if 0
                //if (State.currentRCSetVertexArrayState == nullptr)
                {
                    State.currentRCSetVertexArrayState = &RCCaches[BackCallCacheIndex].RCSetVertexArrayStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetVertexArrayState);
                }

                assert(State.currentRCSetVertexArrayState != nullptr);
                {
                    State.currentRCSetVertexArrayState->SetVertexData(vertexBufferObject, vertexArray.GetVertexDataPtr(), vertexArray.GetVertexCount(), vertexArray.GetFormat().GetSizeInBytes(), ToOpenGLBufferUsage(vertexArray.GetUsage()));
                    State.currentRCSetVertexArrayState->SetIndexData( indexBufferObject,  vertexArray.GetIndexDataPtr(),  vertexArray.GetIndexCount(),  ToOpenGLBufferUsage(vertexArray.GetUsage()));

                    vertexArrayGL33.MarkVertexDataAsUpdated();
                    vertexArrayGL33.MarkIndexDataAsUpdated();
                }
#endif

                // Vertex data.
                {
                    if (ServerState_GL_ARRAY_BUFFER_BINDING != *vertexBufferObject)
                    {
                        glBindBuffer(GL_ARRAY_BUFFER, *vertexBufferObject);
                    }

                    glBufferData(GL_ARRAY_BUFFER, vertexArray.GetVertexCount() * vertexArray.GetFormat().GetSizeInBytes(), vertexArray.GetVertexDataPtr(), ToOpenGLBufferUsage(vertexArray.GetUsage()));

                    if (ServerState_GL_ARRAY_BUFFER_BINDING != *vertexBufferObject)
                    {
                        glBindBuffer(GL_ARRAY_BUFFER, ServerState_GL_ARRAY_BUFFER_BINDING);
                    }

                    vertexArrayGL33.MarkVertexDataAsUpdated();
                }

                // Index data.
                {
                    if (ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING != *indexBufferObject)
                    {
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBufferObject);
                    }

                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexArray.GetIndexCount() * sizeof(unsigned int), vertexArray.GetIndexDataPtr(), ToOpenGLBufferUsage(vertexArray.GetUsage()));

                    if (ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING != *indexBufferObject)
                    {
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING);
                    }

                    vertexArrayGL33.MarkIndexDataAsUpdated();
                }
            }
        }
    }



    TextureState_OpenGL21* Renderer_CreateOpenGL21Texture()
    {
        State.instantiatedTextureObjects.PushBack(new TextureState_OpenGL21);
        TextureState_OpenGL21* textureState  = State.instantiatedTextureObjects.GetBack();

        easyutil_lock_mutex(ResourceCreationLock);
        {
#if 0
            auto &command = RCCaches[BackCallCacheIndex].RCCreateTextureCache.Acquire();
            command.CreateTexture(textureState);

            ResourceCreationCallCaches[BackCallCacheIndex].Append(command);
#endif

            glGenTextures(1, &textureState->objectGL);
        }
        easyutil_unlock_mutex(ResourceCreationLock);

        assert(textureState != nullptr);
        return textureState;
    }

    void Renderer_DeleteOpenGL21Texture(TextureState_OpenGL21* textureStateToDelete)
    {
        assert(textureStateToDelete  != nullptr);
        {
            easyutil_lock_mutex(ResourceDeletionLock);
            {
#if 0
                auto &command = RCCaches[BackCallCacheIndex].RCDeleteTextureCache.Acquire();
                command.DeleteTexture(textureStateToDelete);

                ResourceDeletionCallCaches[BackCallCacheIndex].Append(command);
#endif

                glDeleteTextures(1, &textureStateToDelete->objectGL);
            }
            easyutil_unlock_mutex(ResourceDeletionLock);



            // The objects need to be marked for deletion, but not actually deleted yet.
            State.MarkTextureObjectAsDeleted(textureStateToDelete);
        }
    }

    void Renderer_SetOpenGL21TextureFilter(TextureState_OpenGL21* textureState, GLenum textureTarget, TextureFilter minification, TextureFilter magnification)
    {
        assert(textureState != nullptr);
#if 0
        assert(State.currentRCSetTextureState == nullptr);
        {

            State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
            CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);

            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureFilter(textureState, textureTarget, ToOpenGLTextureFilter(minification), ToOpenGLTextureFilter(magnification));
            }
        }

        State.currentRCSetTextureState = nullptr;       // <-- Force a new texture state draw call.
#endif


        GLuint prevTexture = BindOpenGL21Texture(textureTarget, textureState->objectGL);
        {
            glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, ToOpenGLTextureFilter(minification));
            glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, ToOpenGLTextureFilter(magnification));
        }
        BindOpenGL21Texture(textureTarget, prevTexture);
    }

    void Renderer_SetOpenGL21TextureAnisotropy(TextureState_OpenGL21* textureState, GLenum textureTarget, unsigned int anisotropy)
    {
        assert(textureState != nullptr);
#if 0
        assert(State.currentRCSetTextureState == nullptr);
        {
            State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
            CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);

            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureAnisotropy(textureState, textureTarget, static_cast<GLint>(anisotropy));
            }
        }

        State.currentRCSetTextureState = nullptr;       // <-- Force a new texture state draw call.
#endif


        if (GTGL_EXT_texture_filter_anisotropic)
        {
            GLuint prevTexture = BindOpenGL21Texture(textureTarget, textureState->objectGL);
            {
                glTexParameteri(textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(anisotropy));
            }
            BindOpenGL21Texture(textureTarget, prevTexture);
        }
    }

    void Renderer_SetOpenGL21TextureWrapMode(TextureState_OpenGL21* textureState, GLenum textureTarget, TextureWrapMode wrapMode)
    {
        assert(textureState != nullptr);
#if 0
        assert(State.currentRCSetTextureState == nullptr);
        {
            State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
            CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);

            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureWrapMode(textureState, textureTarget, ToOpenGLWrapMode(wrapMode));
            }
        }

        State.currentRCSetTextureState = nullptr;       // <-- Force a new texture state draw call.
#endif

        GLuint prevTexture = BindOpenGL21Texture(textureTarget, textureState->objectGL);
        {
            glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, ToOpenGLWrapMode(wrapMode));
            glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, ToOpenGLWrapMode(wrapMode));
        }
        BindOpenGL21Texture(textureTarget, prevTexture);
    }

    void Renderer_SetOpenGL21TextureMipmapLevels(TextureState_OpenGL21* textureState, GLenum textureTarget, unsigned int baseLevel, unsigned int maxLevel)
    {
        assert(textureState != nullptr);
#if 0
        assert(State.currentRCSetTextureState == nullptr);
        {
            State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
            CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);

            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->SetTextureMipmapLevels(textureState, textureTarget, static_cast<GLint>(baseLevel), static_cast<GLint>(maxLevel));
            }
        }

        State.currentRCSetTextureState = nullptr;       // <-- Force a new texture state draw call.
#endif

        GLuint prevTexture = BindOpenGL21Texture(textureTarget, textureState->objectGL);
        {
            glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, static_cast<GLint>(baseLevel));
            glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL,  static_cast<GLint>(maxLevel));
        }
        BindOpenGL21Texture(textureTarget, prevTexture);
    }

    void Renderer_GenerateOpenGL21TextureMipmaps(TextureState_OpenGL21* textureState, GLenum textureTarget)
    {
        assert(textureState != nullptr);
#if 0
        assert(State.currentRCSetTextureState == nullptr);
        {
            State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
            CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);

            assert(State.currentRCSetTextureState != nullptr);
            {
                State.currentRCSetTextureState->GenerateTextureMipmaps(textureState, textureTarget);
            }
        }

        State.currentRCSetTextureState = nullptr;       // <-- Force a new texture state draw call.
#endif

        GLuint prevTexture = BindOpenGL21Texture(textureTarget, textureState->objectGL);
        {
            glGenerateMipmap(textureTarget);
        }
        BindOpenGL21Texture(textureTarget, prevTexture);
    }




    Texture2D* Renderer::CreateTexture2D()
    {
        return new Texture2D_OpenGL21(Renderer_CreateOpenGL21Texture(), GL_TEXTURE_2D);
    }

    TextureCube* Renderer::CreateTextureCube()
    {
        // The texture object for each of the faces is actually the main cube map object.
        TextureState_OpenGL21* textureStateGL = Renderer_CreateOpenGL21Texture();

        auto positiveX = new Texture2D_OpenGL21(textureStateGL, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        auto negativeX = new Texture2D_OpenGL21(textureStateGL, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        auto positiveY = new Texture2D_OpenGL21(textureStateGL, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        auto negativeY = new Texture2D_OpenGL21(textureStateGL, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        auto positiveZ = new Texture2D_OpenGL21(textureStateGL, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        auto negativeZ = new Texture2D_OpenGL21(textureStateGL, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

        return new TextureCube_OpenGL21(textureStateGL, positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ);
    }


    void Renderer::DeleteTexture2D(Texture2D* textureToDelete)
    {
        auto textureToDeleteGL33 = static_cast<Texture2D_OpenGL21*>(textureToDelete);
        if (textureToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            Renderer_DeleteOpenGL21Texture(textureToDeleteGL33->GetOpenGLState());

            // We can safely delete the main object at this point.
            delete textureToDelete;
        }
    }

    void Renderer::DeleteTextureCube(TextureCube* textureToDelete)
    {
        auto textureToDeleteGL33 = static_cast<TextureCube_OpenGL21*>(textureToDelete);
        if (textureToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            Renderer_DeleteOpenGL21Texture(textureToDeleteGL33->GetOpenGLState());

            // We can safely delete the main object at this point.
            delete textureToDeleteGL33->PositiveX;
            delete textureToDeleteGL33->NegativeX;
            delete textureToDeleteGL33->PositiveY;
            delete textureToDeleteGL33->NegativeY;
            delete textureToDeleteGL33->PositiveZ;
            delete textureToDeleteGL33->NegativeZ;

            delete textureToDelete;
        }
    }


    void Renderer::PushTexture2DData(const Texture2D &texture, int mipmapIndex)
    {
        if (mipmapIndex == -1)
        {
            // If you fail this assert it means you haven't got any texture data, which in turn means you're calling this function unnecessarily. For performance,
            // the renderer assumes this function will be called when there is actually data to push, and thus will crash when there is none.
            assert(texture.GetMipmapCount() > 0);

            for (size_t i = 0; i < texture.GetMipmapCount(); ++i)
            {
                auto &mipmap = texture.GetMipmap(i);
                SetTexture2DData(texture, i, mipmap.width, mipmap.height, mipmap.format, mipmap.data);
            }
        }
        else
        {
            auto &mipmap = texture.GetMipmap(mipmapIndex);
            SetTexture2DData(texture, mipmapIndex, mipmap.width, mipmap.height, mipmap.format, mipmap.data);
        }
    }

    void Renderer::SetTexture2DData(const Texture2D &texture, int mipmapIndex, unsigned int width, unsigned int height, ImageFormat format, const void* data, bool flip)
    {
        auto &textureGL21 = static_cast<const Texture2D_OpenGL21 &>(texture);
        {
            GLenum textureTarget = textureGL21.GetTarget();
            auto   textureState  = textureGL21.GetOpenGLState();

            assert(textureState != nullptr);
#if 0
            assert(State.currentRCSetTextureState == nullptr);
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);

                assert(State.currentRCSetTextureState != nullptr);
                assert(mipmapIndex >= 0);
                {
                    State.currentRCSetTextureState->SetTexture2DData(textureState, textureTarget, mipmapIndex, format, width, height, data, ImageUtils::CalculateDataSize(width, height, format), flip);
                }
            }

            State.currentRCSetTextureState = nullptr;       // <-- Force a new texture state draw call.
#endif

            GLuint prevTexture = BindOpenGL21Texture(textureTarget, textureState->objectGL);
            {
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                {
                    // OpenGL requires us to flip the texture data upside down.
                    GLvoid* upsideDownData = static_cast<GLvoid*>(malloc(ImageUtils::CalculateDataSize(width, height, format)));
                    ImageUtils::CopyImageData(upsideDownData, data, width, height, format, flip);
                    {
                        glTexImage2D(textureTarget, mipmapIndex, ToOpenGLInternalFormat(format), width, height, 0, ToOpenGLFormat(format), ToOpenGLType(format), data);
                    }
                    free(upsideDownData);
                }
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            }
            BindOpenGL21Texture(textureTarget, prevTexture);
        }
    }

    void Renderer::SetTexture2DSubData(const Texture2D &texture, int mipmapIndex, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height, ImageFormat format, const void* data, bool flip)
    {
        auto &textureGL21 = static_cast<const Texture2D_OpenGL21 &>(texture);
        {
            GLenum textureTarget = textureGL21.GetTarget();
            auto   textureState  = textureGL21.GetOpenGLState();

            assert(textureState != nullptr);
#if 0
            assert(State.currentRCSetTextureState == nullptr);
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);

                assert(State.currentRCSetTextureState != nullptr);
                assert(mipmapIndex >= 0);
                {
                    State.currentRCSetTextureState->SetTexture2DSubData(textureState, textureTarget, mipmapIndex, format, xOffset, yOffset, width, height, data, ImageUtils::CalculateDataSize(width, height, format), flip);
                }
            }

            State.currentRCSetTextureState = nullptr;       // <-- Force a new texture state draw call.
#endif

            GLuint prevTexture = BindOpenGL21Texture(textureTarget, textureState->objectGL);
            {
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                {
                    // OpenGL requires us to flip the texture data upside down.
                    GLvoid* upsideDownData = static_cast<GLvoid*>(malloc(ImageUtils::CalculateDataSize(width, height, format)));
                    ImageUtils::CopyImageData(upsideDownData, data, width, height, format, flip);
                    {
                        glTexSubImage2D(textureTarget, mipmapIndex, xOffset, yOffset, width, height, ToOpenGLFormat(format), ToOpenGLType(format), upsideDownData);
                    }
                    free(upsideDownData);
                }
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            }
            BindOpenGL21Texture(textureTarget, prevTexture);
        }
    }

    void Renderer::PushTextureCubeData(const TextureCube &texture)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL21 &>(texture);
        {
            auto textureState = textureGL33.GetOpenGLState();
            assert(textureState != nullptr);

            auto &positiveX = static_cast<const Texture2D_OpenGL21*>(texture.PositiveX)->GetMipmap(0);
            auto &negativeX = static_cast<const Texture2D_OpenGL21*>(texture.NegativeX)->GetMipmap(0);
            auto &positiveY = static_cast<const Texture2D_OpenGL21*>(texture.PositiveY)->GetMipmap(0);
            auto &negativeY = static_cast<const Texture2D_OpenGL21*>(texture.NegativeY)->GetMipmap(0);
            auto &positiveZ = static_cast<const Texture2D_OpenGL21*>(texture.PositiveZ)->GetMipmap(0);
            auto &negativeZ = static_cast<const Texture2D_OpenGL21*>(texture.NegativeZ)->GetMipmap(0);

            auto width           = positiveX.width;
            auto height          = positiveX.height;
            //auto format          = positiveX.format;
            //auto dataSizeInBytes = positiveX.GetDataSizeInBytes();

#if 0
            assert(State.currentRCSetTextureState == nullptr);
            {
                State.currentRCSetTextureState = &RCCaches[BackCallCacheIndex].RCSetTextureStateCache.Acquire();
                CallCaches[BackCallCacheIndex].Append(*State.currentRCSetTextureState);

                assert(State.currentRCSetTextureState != nullptr);
                {
                    State.currentRCSetTextureState->SetTextureCubeData(textureState, format, width, height, dataSizeInBytes,
                        positiveX.data, negativeX.data,
                        positiveY.data, negativeY.data,
                        positiveZ.data, negativeZ.data);
                }
            }

            State.currentRCSetTextureState = nullptr;       // <-- Force a new texture state draw call.
#endif

            GLenum internalFormat = ToOpenGLInternalFormat(positiveX.format);
            GLenum format         = ToOpenGLFormat(positiveX.format);
            GLenum type           = ToOpenGLType(positiveX.format);

            GLuint prevTexture = BindOpenGL21Texture(GL_TEXTURE_CUBE_MAP, textureState->objectGL);
            {
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, width, height, 0, format, type, positiveX.data);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, width, height, 0, format, type, negativeX.data);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, width, height, 0, format, type, positiveY.data);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, width, height, 0, format, type, negativeY.data);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, width, height, 0, format, type, positiveZ.data);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, width, height, 0, format, type, negativeZ.data);
                }
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            }
            BindOpenGL21Texture(GL_TEXTURE_CUBE_MAP, prevTexture);
        }
    }


    void Renderer::SetTexture2DFilter(const Texture2D &texture, TextureFilter minification, TextureFilter magnification)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL21 &>(texture);
        {
            Renderer_SetOpenGL21TextureFilter(textureGL33.GetOpenGLState(), textureGL33.GetTarget(), minification, magnification);
        }
    }

    void Renderer::SetTextureCubeFilter(const TextureCube &texture, TextureFilter minification, TextureFilter magnification)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL21 &>(texture);
        {
            Renderer_SetOpenGL21TextureFilter(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP, minification, magnification);
        }
    }


    void Renderer::SetTexture2DAnisotropy(const Texture2D &texture, unsigned int anisotropy)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL21 &>(texture);
        {
            Renderer_SetOpenGL21TextureAnisotropy(textureGL33.GetOpenGLState(), textureGL33.GetTarget(), anisotropy);
        }
    }

    void Renderer::SetTextureCubeAnisotropy(const TextureCube &texture, unsigned int anisotropy)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL21 &>(texture);
        {
            Renderer_SetOpenGL21TextureAnisotropy(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP, anisotropy);
        }
    }


    void Renderer::SetTexture2DWrapMode(const Texture2D &texture, TextureWrapMode wrapMode)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL21 &>(texture);
        {
            Renderer_SetOpenGL21TextureWrapMode(textureGL33.GetOpenGLState(), textureGL33.GetTarget(), wrapMode);
        }
    }

    void Renderer::SetTextureCubeWrapMode(const TextureCube &texture, TextureWrapMode wrapMode)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL21 &>(texture);
        {
            Renderer_SetOpenGL21TextureWrapMode(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP, wrapMode);
        }
    }


    void Renderer::SetTexture2DMipmapLevels(const Texture2D &texture, unsigned int baseLevel, unsigned int maxLevel)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL21 &>(texture);
        {
            Renderer_SetOpenGL21TextureMipmapLevels(textureGL33.GetOpenGLState(), textureGL33.GetTarget(), baseLevel, maxLevel);
        }
    }

    void Renderer::SetTextureCubeMipmapLevels(const TextureCube &texture, unsigned int baseLevel, unsigned int maxLevel)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL21 &>(texture);
        {
            Renderer_SetOpenGL21TextureMipmapLevels(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP, baseLevel, maxLevel);
        }
    }


    void Renderer::GenerateTexture2DMipmaps(const Texture2D &texture)
    {
        auto &textureGL33 = static_cast<const Texture2D_OpenGL21 &>(texture);
        {
            Renderer_GenerateOpenGL21TextureMipmaps(textureGL33.GetOpenGLState(), textureGL33.GetTarget());
        }
    }

    void Renderer::GenerateTextureCubeMipmaps(const TextureCube &texture)
    {
        auto &textureGL33 = static_cast<const TextureCube_OpenGL21 &>(texture);
        {
            Renderer_GenerateOpenGL21TextureMipmaps(textureGL33.GetOpenGLState(), GL_TEXTURE_CUBE_MAP);
        }
    }



    Shader* Renderer::CreateShader(const char* vertexShaderSource, const char* fragmentShaderSource, const char* geometryShaderSource)
    {
        State.instantiatedProgramObjects.PushBack(new ShaderState_OpenGL21);
        auto programState = State.instantiatedProgramObjects.GetBack();


        easyutil_lock_mutex(ResourceCreationLock);
        {
#if 0
            auto &command = RCCaches[BackCallCacheIndex].RCCreateShaderCache.Acquire();
            command.CreateShader(programState, vertexShaderSource, fragmentShaderSource, geometryShaderSource);

            ResourceCreationCallCaches[BackCallCacheIndex].Append(command);
#endif

            // 1) Create the shader objects (vertex, fragment and geometry shader objects).
            GLuint vertexShaderObject   = CreateOpenGL21Shader(GL_VERTEX_SHADER,   vertexShaderSource);
            GLuint fragmentShaderObject = CreateOpenGL21Shader(GL_FRAGMENT_SHADER, fragmentShaderSource);
            GLuint geometryShaderObject = 0;
            if (geometryShaderSource != NULL && geometryShaderSource[0] != '\0')
            {
                geometryShaderObject = CreateOpenGL21Shader(GL_GEOMETRY_SHADER, geometryShaderSource);
            }


            // 2) Link the program.
            GLuint programObject = LinkOpenGL21Program(vertexShaderObject, fragmentShaderObject, geometryShaderObject);


            // 3) Grab the uniforms.
            if (programObject != 0)
            {
                GLint uniformCount = 0;
                glGetProgramiv(programObject, GL_ACTIVE_UNIFORMS, &uniformCount);


                for (GLint iUniform = 0; iUniform < uniformCount; ++iUniform)
                {
                    GLint  uniformSize;
                    GLenum uniformType;
                    char   uniformName[100];
                    glGetActiveUniform(programObject, iUniform, sizeof(uniformName) - 1, nullptr, &uniformSize, &uniformType, uniformName);

                    GLint uniformLocation = glGetUniformLocation(programObject, uniformName);

                    switch (uniformType)
                    {
                    case GL_FLOAT:
                        {
                            programState->floatUniformLocations.Add(uniformName, uniformLocation);
                            break;
                        }

                    case GL_FLOAT_VEC2:
                        {
                            programState->float2UniformLocations.Add(uniformName, uniformLocation);
                            break;
                        }

                    case GL_FLOAT_VEC3:
                        {
                            programState->float3UniformLocations.Add(uniformName, uniformLocation);
                            break;
                        }

                    case GL_FLOAT_VEC4:
                        {
                            programState->float4UniformLocations.Add(uniformName, uniformLocation);
                            break;
                        }


                    case GL_FLOAT_MAT2:
                        {
                            programState->float2x2UniformLocations.Add(uniformName, uniformLocation);
                            break;
                        }

                    case GL_FLOAT_MAT3:
                        {
                            programState->float3x3UniformLocations.Add(uniformName, uniformLocation);
                            break;
                        }

                    case GL_FLOAT_MAT4:
                        {
                            programState->float4x4UniformLocations.Add(uniformName, uniformLocation);
                            break;
                        }


                    case GL_SAMPLER_1D:
                    case GL_SAMPLER_1D_ARRAY:
                    case GL_SAMPLER_2D:
                    case GL_SAMPLER_2D_ARRAY:
                    case GL_SAMPLER_3D:
                    case GL_SAMPLER_CUBE:
                    case GL_SAMPLER_2D_MULTISAMPLE:
                    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
                    case GL_SAMPLER_BUFFER:
                    case GL_INT_SAMPLER_1D:
                    case GL_INT_SAMPLER_1D_ARRAY:
                    case GL_INT_SAMPLER_2D:
                    case GL_INT_SAMPLER_2D_ARRAY:
                    case GL_INT_SAMPLER_3D:
                    case GL_INT_SAMPLER_CUBE:
                    case GL_INT_SAMPLER_BUFFER:
                    case GL_UNSIGNED_INT_SAMPLER_1D:
                    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
                    case GL_UNSIGNED_INT_SAMPLER_2D:
                    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
                    case GL_UNSIGNED_INT_SAMPLER_3D:
                    case GL_UNSIGNED_INT_SAMPLER_CUBE:
                    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
                        {
                            programState->textureUniformLocations.Add(uniformName, uniformLocation);
                            break;
                        }


                    default:
                        {
                            break;
                        }
                    }
                }
            }


            // It's important that this is set AFTER creating the uniform locations. Reason for this is that the other thread uses the program object in
            // determining whether or not to search for uniform locations.
            programState->programObject = programObject;


            // 4) Delete the shader objects.
            if (vertexShaderObject   != 0) glDeleteShader(vertexShaderObject);
            if (fragmentShaderObject != 0) glDeleteShader(fragmentShaderObject);
            if (geometryShaderObject != 0) glDeleteShader(geometryShaderObject);
        }
        easyutil_unlock_mutex(ResourceCreationLock);



        return new Shader_OpenGL21(programState, vertexShaderSource, fragmentShaderSource, geometryShaderSource);
    }

    void Renderer::DeleteShader(Shader* shaderToDelete)
    {
        auto shaderToDeleteGL33 = static_cast<Shader_OpenGL21*>(shaderToDelete);
        if (shaderToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            auto programState = shaderToDeleteGL33->GetOpenGLState();

            assert(programState  != nullptr);
            {
                easyutil_lock_mutex(ResourceDeletionLock);
                {
#if 0
                    auto &command = RCCaches[BackCallCacheIndex].RCDeleteShaderCache.Acquire();
                    command.DeleteShader(programState);

                    ResourceDeletionCallCaches[BackCallCacheIndex].Append(command);
#endif

                    glDeleteProgram(programState->programObject);
                }
                easyutil_unlock_mutex(ResourceDeletionLock);



                // The objects need to be marked for deletion, but not actually deleted yet.
                State.MarkProgramObjectAsDeleted(programState);
            }


            // We can safely delete the main object at this point.
            delete shaderToDelete;
        }
    }


    void Renderer::PushPendingUniforms(const Shader &shader)
    {
        auto &shaderGL33 = static_cast<const Shader_OpenGL21 &>(shader);
        {
            auto programState = shaderGL33.GetOpenGLState();
            assert(programState != nullptr);
            {
#if 0
                if (State.currentRCSetShaderState == nullptr || State.currentRCSetShaderState->GetProgramState() != programState)
                {
                    State.currentRCSetShaderState = &RCCaches[BackCallCacheIndex].RCSetShaderStateCache.Acquire();
                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetShaderState);
                }

                assert(State.currentRCSetShaderState != nullptr);
                {
                    State.currentRCSetShaderState->SetFloatUniforms(programState);
                    State.currentRCSetShaderState->SetFloat2Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat3Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat4Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat2x2Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat3x3Uniforms(programState);
                    State.currentRCSetShaderState->SetFloat4x4Uniforms(programState);
                    State.currentRCSetShaderState->SetTextureUniforms(programState);

                    programState->ClearPendingUniforms();
                }
#endif


                if (ServerState_GL_CURRENT_PROGRAM != programState->programObject)
                {
                    glUseProgram(programState->programObject);
                }


                // Float
                for (size_t i = 0; i < programState->pendingFloatUniformsByName.count; ++i)
                {
                    auto  parameterName  = programState->pendingFloatUniformsByName.buffer[i]->key;
                    auto &parameterValue = programState->pendingFloatUniformsByName.buffer[i]->value;

                    glUniform1f(glGetUniformLocation(programState->programObject, parameterName), parameterValue.x);
                }

                for (size_t i = 0; i < programState->pendingFloatUniformsByLocation.count; ++i)
                {
                    auto &parameter = programState->pendingFloatUniformsByLocation.Get(i);
                    {
                        glUniform1f(parameter.location, parameter.x);
                    }
                }


                // Float2
                for (size_t i = 0; i < programState->pendingFloat2UniformsByName.count; ++i)
                {
                    auto  parameterName  = programState->pendingFloat2UniformsByName.buffer[i]->key;
                    auto &parameterValue = programState->pendingFloat2UniformsByName.buffer[i]->value;

                    glUniform2f(glGetUniformLocation(programState->programObject, parameterName), parameterValue.x, parameterValue.y);
                }

                for (size_t i = 0; i < programState->pendingFloat2UniformsByLocation.count; ++i)
                {
                    auto &parameter = programState->pendingFloat2UniformsByLocation.Get(i);
                    {
                        glUniform2f(parameter.location, parameter.x, parameter.y);
                    }
                }


                // Float3
                for (size_t i = 0; i < programState->pendingFloat3UniformsByName.count; ++i)
                {
                    auto  parameterName  = programState->pendingFloat3UniformsByName.buffer[i]->key;
                    auto &parameterValue = programState->pendingFloat3UniformsByName.buffer[i]->value;

                    glUniform3f(glGetUniformLocation(programState->programObject, parameterName), parameterValue.x, parameterValue.y, parameterValue.z);
                }

                for (size_t i = 0; i < programState->pendingFloat3UniformsByLocation.count; ++i)
                {
                    auto &parameter = programState->pendingFloat3UniformsByLocation.Get(i);
                    {
                        glUniform3f(parameter.location, parameter.x, parameter.y, parameter.z);
                    }
                }


                // Float4
                for (size_t i = 0; i < programState->pendingFloat4UniformsByName.count; ++i)
                {
                    auto  parameterName  = programState->pendingFloat4UniformsByName.buffer[i]->key;
                    auto &parameterValue = programState->pendingFloat4UniformsByName.buffer[i]->value;

                    glUniform4f(glGetUniformLocation(programState->programObject, parameterName), parameterValue.x, parameterValue.y, parameterValue.z, parameterValue.w);
                }

                for (size_t i = 0; i < programState->pendingFloat4UniformsByLocation.count; ++i)
                {
                    auto &parameter = programState->pendingFloat4UniformsByLocation.Get(i);
                    {
                        glUniform4f(parameter.location, parameter.x, parameter.y, parameter.z, parameter.w);
                    }
                }


                // Float2x2
                for (size_t i = 0; i < programState->pendingFloat2x2UniformsByName.count; ++i)
                {
                    auto  parameterName  = programState->pendingFloat2x2UniformsByName.buffer[i]->key;
                    auto &parameterValue = programState->pendingFloat2x2UniformsByName.buffer[i]->value;

                    glUniformMatrix2fv(glGetUniformLocation(programState->programObject, parameterName), 1, false, parameterValue.value);
                }

                for (size_t i = 0; i < programState->pendingFloat2x2UniformsByLocation.count; ++i)
                {
                    auto &parameter = programState->pendingFloat2x2UniformsByLocation.Get(i);
                    {
                        glUniformMatrix2fv(parameter.location, 1, false, parameter.value);
                    }
                }


                // Float3x3
                for (size_t i = 0; i < programState->pendingFloat3x3UniformsByName.count; ++i)
                {
                    auto  parameterName  = programState->pendingFloat3x3UniformsByName.buffer[i]->key;
                    auto &parameterValue = programState->pendingFloat3x3UniformsByName.buffer[i]->value;

                    glUniformMatrix3fv(glGetUniformLocation(programState->programObject, parameterName), 1, false, parameterValue.value);
                }

                for (size_t i = 0; i < programState->pendingFloat3x3UniformsByLocation.count; ++i)
                {
                    auto &parameter = programState->pendingFloat3x3UniformsByLocation.Get(i);
                    {
                        glUniformMatrix3fv(parameter.location, 1, false, parameter.value);
                    }
                }


                // Float4x4
                for (size_t i = 0; i < programState->pendingFloat4x4UniformsByName.count; ++i)
                {
                    auto  parameterName  = programState->pendingFloat4x4UniformsByName.buffer[i]->key;
                    auto &parameterValue = programState->pendingFloat4x4UniformsByName.buffer[i]->value;

                    glUniformMatrix4fv(glGetUniformLocation(programState->programObject, parameterName), 1, false, parameterValue.value);
                }

                for (size_t i = 0; i < programState->pendingFloat4x4UniformsByLocation.count; ++i)
                {
                    auto &parameter = programState->pendingFloat4x4UniformsByLocation.Get(i);
                    {
                        glUniformMatrix4fv(parameter.location, 1, false, parameter.value);
                    }
                }


                // Texture
                for (size_t i = 0; i < programState->pendingTextureUniformsByName.count; ++i)
                {
                    GLint uniformLocation = glGetUniformLocation(programState->programObject, programState->pendingTextureUniformsByName.buffer[i]->key);
                    auto &uniformValue    = programState->pendingTextureUniformsByName.buffer[i]->value;

                    SetOpenGL21TextureUniform(programState, uniformLocation, uniformValue);
                }

                for (size_t i = 0 ; i < programState->pendingTextureUniformsByLocation.count; ++i)
                {
                    auto &uniform = programState->pendingTextureUniformsByLocation.Get(i);
                    {
                        SetOpenGL21TextureUniform(programState, uniform.location, uniform);
                    }
                }



                if (ServerState_GL_CURRENT_PROGRAM != programState->programObject)
                {
                    glUseProgram(ServerState_GL_CURRENT_PROGRAM);
                }


                programState->ClearPendingUniforms();
            }
        }
    }



    Framebuffer* Renderer::CreateFramebuffer()
    {
        State.instantiatedFramebufferObjects.PushBack(new FramebufferState_OpenGL21);
        auto framebufferState = State.instantiatedFramebufferObjects.GetBack();


        easyutil_lock_mutex(ResourceCreationLock);
        {
#if 0
            RCCreateFramebuffer* command = nullptr;

            if (GTGL_ARB_framebuffer_object)
            {
                // Primary (ARB_framebuffer_object).
                command = &RCCaches[BackCallCacheIndex].RCCreateFramebufferCacheARB.Acquire();
            }
            else
            {
                // Secondary (EXT_framebuffer_object).
                command = &RCCaches[BackCallCacheIndex].RCCreateFramebufferCacheEXT.Acquire();
            }

            assert(command != nullptr);
            {
                command->CreateFramebuffer(framebufferState);
                ResourceCreationCallCaches[BackCallCacheIndex].Append(*command);
            }
#endif

            if (GTGL_ARB_framebuffer_object)
            {
                // Primary (ARB_framebuffer_object).
                glGenFramebuffers(1, &framebufferState->framebufferObject);
            }
            else
            {
                // Secondary (EXT_framebuffer_object).
                glGenFramebuffersEXT(1, &framebufferState->framebufferObject);
            }
            
        }
        easyutil_unlock_mutex(ResourceCreationLock);



        return new Framebuffer_OpenGL21(framebufferState);
    }

    void Renderer::DeleteFramebuffer(Framebuffer* framebufferToDelete)
    {
        auto framebufferToDeleteGL33 = static_cast<Framebuffer_OpenGL21*>(framebufferToDelete);
        if (framebufferToDeleteGL33 != nullptr)
        {
            // The OpenGL object needs to be marked for deletion.
            auto framebufferState = framebufferToDeleteGL33->GetOpenGLState();

            assert(framebufferState  != nullptr);
            {
                easyutil_lock_mutex(ResourceDeletionLock);
                {
#if 0
                    RCDeleteFramebuffer* command = nullptr;

                    if (GTGL_ARB_framebuffer_object)
                    {
                        // Primary (ARB_framebuffer_object).
                        command = &RCCaches[BackCallCacheIndex].RCDeleteFramebufferCacheARB.Acquire();
                    }
                    else
                    {
                        // Secondary (EXT_framebuffer_object).
                        command = &RCCaches[BackCallCacheIndex].RCDeleteFramebufferCacheEXT.Acquire();
                    }

                    assert(command != nullptr);
                    {
                        command->DeleteFramebuffer(framebufferState);
                        ResourceDeletionCallCaches[BackCallCacheIndex].Append(*command);
                    }
#endif

                    if (GTGL_ARB_framebuffer_object)
                    {
                        // Primary (ARB_framebuffer_object).
                        glDeleteFramebuffers(1, &framebufferState->framebufferObject);
                        if (framebufferState->depthStencilRenderbuffer != 0)
                        {
                            glDeleteRenderbuffers(1, &framebufferState->depthStencilRenderbuffer);
                        }
                    }
                    else
                    {
                        // Secondary (EXT_framebuffer_object).
                        glDeleteFramebuffersEXT(1, &framebufferState->framebufferObject);
                        if (framebufferState->depthStencilRenderbuffer != 0)
                        {
                            glDeleteRenderbuffersEXT(1, &framebufferState->depthStencilRenderbuffer);
                        }
                    }

                    framebufferState->framebufferObject = 0;
                    framebufferState->depthStencilRenderbuffer = 0;
                    framebufferState->depthStencilRenderbufferWidth  = 0;
                    framebufferState->depthStencilRenderbufferHeight = 0;
                }
                easyutil_unlock_mutex(ResourceDeletionLock);



                // The objects need to be marked for deletion, but not actually deleted yet.
                State.MarkFramebufferObjectAsDeleted(framebufferState);
            }


            // We can safely delete the main object at this point.
            delete framebufferToDeleteGL33;
        }
    }


    void Renderer::PushAttachments(const Framebuffer &framebuffer)
    {
        auto &framebufferGL33 = static_cast<const Framebuffer_OpenGL21 &>(framebuffer);
        {
            auto framebufferState = framebufferGL33.GetOpenGLState();
            assert(framebufferState != nullptr);
            {
#if 0
                if (State.currentRCSetFramebufferState == nullptr || State.currentRCSetFramebufferState->GetFramebufferState() != framebufferState)
                {
                    if (GTGL_ARB_framebuffer_object)
                    {
                        // Primary (ARB_framebuffer_object).
                        State.currentRCSetFramebufferState = &RCCaches[BackCallCacheIndex].RCSetFramebufferStateCacheARB.Acquire();
                    }
                    else
                    {
                        // Secondary (EXT_framebuffer_object).
                        State.currentRCSetFramebufferState = &RCCaches[BackCallCacheIndex].RCSetFramebufferStateCacheEXT.Acquire();
                    }

                    CallCaches[BackCallCacheIndex].Append(*State.currentRCSetFramebufferState);
                }


                assert(State.currentRCSetFramebufferState != nullptr);
                {
                    // We just notify the render command of the currently attached buffers. It will detach and switch stuff around appropriately when it's
                    // executed. We can't know at this point which attachments should be attached or detached because the server-side state may be in the
                    // middle of changing on another thread.

                    // Colour Buffers.
                    auto &colourAttachments = framebufferGL33.GetAttachedColourBuffers();
                    for (size_t i = 0; i < colourAttachments.count; ++i)
                    {
                        auto index   = static_cast<GLuint>(colourAttachments.buffer[i]->key);
                        auto texture = static_cast<Texture2D_OpenGL21*>(colourAttachments.buffer[i]->value);

                        assert(texture != nullptr);
                        {
                            State.currentRCSetFramebufferState->SetAttachedBuffer(framebufferState, GL_COLOR_ATTACHMENT0_EXT + index, texture->GetTarget(), texture->GetOpenGLState());
                        }
                    }

                    // Depth/Stencil Texture.
                    auto depthStencilAttachment = static_cast<const Texture2D_OpenGL21*>(framebufferGL33.GetDepthStencilBuffer());
                    if (depthStencilAttachment != nullptr && GTGL_ARB_framebuffer_object)
                    {
                        State.currentRCSetFramebufferState->SetAttachedBuffer(framebufferState, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilAttachment->GetTarget(), depthStencilAttachment->GetOpenGLState());

                        // This technique causes a crash on Intel's Windows drivers (not tested with Linux drivers). Leaving this here for future reference.
                        //State.currentRCSetFramebufferState->SetAttachedBuffer(framebufferState, GL_DEPTH_ATTACHMENT_EXT,   depthStencilAttachment->GetTarget(), depthStencilAttachment->GetOpenGLState());
                        //State.currentRCSetFramebufferState->SetAttachedBuffer(framebufferState, GL_STENCIL_ATTACHMENT_EXT, depthStencilAttachment->GetTarget(), depthStencilAttachment->GetOpenGLState());
                    }

                    // Write-Only Depth/Stencil Renderbuffer.
                    GLboolean writeOnlyDepthStencilAttached = framebufferGL33.IsWriteOnlyDepthStencilBufferAttached();
                    GLsizei   writeOnlyDepthStencilWidth    = framebufferGL33.GetWriteOnlyDepthStencilBufferWidth();
                    GLsizei   writeOnlyDepthStencilHeight   = framebufferGL33.GetWriteOnlyDepthStencilBufferHeight();
                    State.currentRCSetFramebufferState->SetWriteOnlyDepthStencilBuffer(framebufferState, writeOnlyDepthStencilAttached, writeOnlyDepthStencilWidth, writeOnlyDepthStencilHeight);
                }
#endif

                // We just notify the render command of the currently attached buffers. It will detach and switch stuff around appropriately when it's
                // executed. We can't know at this point which attachments should be attached or detached because the server-side state may be in the
                // middle of changing on another thread.
                if (ServerState_GL_FRAMEBUFFER_BINDING != framebufferState->framebufferObject)
                {
                    if (GTGL_ARB_framebuffer_object) {
                        glBindFramebuffer(GL_FRAMEBUFFER, framebufferState->framebufferObject);
                    } else {
                        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferState->framebufferObject);
                    }
                }


                // Colour Buffers.
                

                GT::Map<GLenum, FramebufferState_OpenGL21::Attachment> attachments;

                auto &colourAttachments = framebufferGL33.GetAttachedColourBuffers();
                for (size_t i = 0; i < colourAttachments.GetCount(); ++i)
                {
                    auto index   = static_cast<GLuint>(colourAttachments.buffer[i]->key);
                    auto texture = static_cast<Texture2D_OpenGL21*>(colourAttachments.buffer[i]->value);

                    FramebufferState_OpenGL21::Attachment attachment;
                    attachment.attachmentPoint = GL_COLOR_ATTACHMENT0_EXT + index;
                    attachment.textureTarget   = texture->GetTarget();
                    attachment.textureState    = texture->GetOpenGLState();

                    attachments.Add(attachment.attachmentPoint, attachment);
                }

                auto depthStencilAttachment = static_cast<const Texture2D_OpenGL21*>(framebufferGL33.GetDepthStencilBuffer());
                if (depthStencilAttachment != nullptr && GTGL_ARB_framebuffer_object)
                {
                    FramebufferState_OpenGL21::Attachment attachment;
                    attachment.attachmentPoint = GL_DEPTH_STENCIL_ATTACHMENT;
                    attachment.textureTarget   = depthStencilAttachment->GetTarget();
                    attachment.textureState    = depthStencilAttachment->GetOpenGLState();

                    attachments.Add(attachment.attachmentPoint, attachment);
                }


                

                // 1) Detach.
                for (size_t i = 0; i < framebufferState->attachments.count; )
                {
                    auto &attachmentToRemove = framebufferState->attachments.buffer[i]->value;

                    if (!attachments.Exists(attachmentToRemove.attachmentPoint))
                    {
                        if (GTGL_ARB_framebuffer_object) {
                            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentToRemove.attachmentPoint, attachmentToRemove.textureTarget, 0, 0);
                        } else {
                            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachmentToRemove.attachmentPoint, attachmentToRemove.textureTarget, 0, 0);
                        }

                        framebufferState->attachments.RemoveByIndex(i);
                    }
                    else
                    {
                        ++i;
                    }
                }


                // 2) Attach.
                for (size_t i = 0; i < attachments.count; ++i)
                {
                    auto &attachmentToAdd = attachments.buffer[i]->value;

                    if (GTGL_ARB_framebuffer_object) {
                        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentToAdd.attachmentPoint, attachmentToAdd.textureTarget, attachmentToAdd.textureState->objectGL, 0);
                    } else {
                        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachmentToAdd.attachmentPoint, attachmentToAdd.textureTarget, attachmentToAdd.textureState->objectGL, 0);
                    }

                    framebufferState->attachments.Add(attachmentToAdd.attachmentPoint, attachmentToAdd);
                }


                // Write-Only Depth/Stencil Renderbuffer.
                GLboolean writeOnlyDepthStencilAttached = framebufferGL33.IsWriteOnlyDepthStencilBufferAttached();
                GLsizei   writeOnlyDepthStencilWidth    = framebufferGL33.GetWriteOnlyDepthStencilBufferWidth();
                GLsizei   writeOnlyDepthStencilHeight   = framebufferGL33.GetWriteOnlyDepthStencilBufferHeight();

                if (writeOnlyDepthStencilAttached)
                {
                    // We're attaching.
                    bool needAttach = false;
                    if (framebufferState->depthStencilRenderbuffer == 0)
                    {
                        if (GTGL_ARB_framebuffer_object) {
                            glGenRenderbuffers(1, &framebufferState->depthStencilRenderbuffer);
                        } else {
                            glGenRenderbuffersEXT(1, &framebufferState->depthStencilRenderbuffer);
                        }

                        needAttach = true;
                    }

                    if (needAttach || framebufferState->depthStencilRenderbufferWidth != writeOnlyDepthStencilWidth || framebufferState->depthStencilRenderbufferHeight != writeOnlyDepthStencilHeight)
                    {
                        if (GTGL_ARB_framebuffer_object) {
                            glBindRenderbuffer(GL_RENDERBUFFER, framebufferState->depthStencilRenderbuffer);
                        } else {
                            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, framebufferState->depthStencilRenderbuffer);
                        }


                        if (GTGL_ARB_framebuffer_object) {
                            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, writeOnlyDepthStencilWidth, writeOnlyDepthStencilHeight);
                        } else {
                            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, writeOnlyDepthStencilWidth, writeOnlyDepthStencilHeight);
                        }
                            
                        framebufferState->depthStencilRenderbufferWidth  = writeOnlyDepthStencilWidth;
                        framebufferState->depthStencilRenderbufferHeight = writeOnlyDepthStencilHeight;


                        if (GTGL_ARB_framebuffer_object) {
                            glBindRenderbuffer(GL_RENDERBUFFER, 0);
                        } else {
                            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
                        }
                    }

                    if (needAttach)
                    {
                        if (GTGL_ARB_framebuffer_object) {
                            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,   GL_RENDERBUFFER_EXT, framebufferState->depthStencilRenderbuffer);
                            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER_EXT, framebufferState->depthStencilRenderbuffer);
                        } else {
                            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,   GL_RENDERBUFFER_EXT, framebufferState->depthStencilRenderbuffer);
                            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, framebufferState->depthStencilRenderbuffer);
                        }

                        
                    }
                }
                else
                {
                    // We're detaching.
                    if (framebufferState->depthStencilRenderbuffer != 0)
                    {
                        if (GTGL_ARB_framebuffer_object) {
                            glDeleteRenderbuffers(1, &framebufferState->depthStencilRenderbuffer);
                        } else {
                            glDeleteRenderbuffersEXT(1, &framebufferState->depthStencilRenderbuffer);
                        }

                        framebufferState->depthStencilRenderbuffer = 0;
                        framebufferState->depthStencilRenderbufferWidth  = 0;
                        framebufferState->depthStencilRenderbufferHeight = 0;
                    }
                }

                //State.currentRCSetFramebufferState->SetWriteOnlyDepthStencilBuffer(framebufferState, writeOnlyDepthStencilAttached, writeOnlyDepthStencilWidth, writeOnlyDepthStencilHeight);

#if 0
                auto &colourAttachments = framebufferGL33.GetAttachedColourBuffers();
                for (size_t i = 0; i < colourAttachments.count; ++i)
                {
                    auto index   = static_cast<GLuint>(colourAttachments.buffer[i]->key);
                    auto texture = static_cast<Texture2D_OpenGL21*>(colourAttachments.buffer[i]->value);

                    assert(texture != nullptr);
                    {
                        State.currentRCSetFramebufferState->SetAttachedBuffer(framebufferState, GL_COLOR_ATTACHMENT0_EXT + index, texture->GetTarget(), texture->GetOpenGLState());
                    }
                }

                // Depth/Stencil Texture.
                auto depthStencilAttachment = static_cast<const Texture2D_OpenGL21*>(framebufferGL33.GetDepthStencilBuffer());
                if (depthStencilAttachment != nullptr && GTGL_ARB_framebuffer_object)
                {
                    State.currentRCSetFramebufferState->SetAttachedBuffer(framebufferState, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilAttachment->GetTarget(), depthStencilAttachment->GetOpenGLState());
                }

                // Write-Only Depth/Stencil Renderbuffer.
                GLboolean writeOnlyDepthStencilAttached = framebufferGL33.IsWriteOnlyDepthStencilBufferAttached();
                GLsizei   writeOnlyDepthStencilWidth    = framebufferGL33.GetWriteOnlyDepthStencilBufferWidth();
                GLsizei   writeOnlyDepthStencilHeight   = framebufferGL33.GetWriteOnlyDepthStencilBufferHeight();
                State.currentRCSetFramebufferState->SetWriteOnlyDepthStencilBuffer(framebufferState, writeOnlyDepthStencilAttached, writeOnlyDepthStencilWidth, writeOnlyDepthStencilHeight);
#endif

                // Global state needs to be restored.
                if (ServerState_GL_FRAMEBUFFER_BINDING != framebufferState->framebufferObject)
                {
                    if (GTGL_ARB_framebuffer_object) {
                        glBindFramebuffer(GL_FRAMEBUFFER, ServerState_GL_FRAMEBUFFER_BINDING);
                    } else {
                        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ServerState_GL_FRAMEBUFFER_BINDING);
                    }
                }
            }
        }
    }




    /////////////////////////////////////////////////////////////
    // Support Queries

    bool Renderer::HasFlippedTextures()
    {
        return true;
    }

    unsigned int Renderer::GetMaxColourAttachments()
    {
        return static_cast<unsigned int>(RendererCaps.MaxColourAttachments);
    }

    unsigned int Renderer::GetMaxDrawBuffers()
    {
        return static_cast<unsigned int>(RendererCaps.MaxDrawBuffers);
    }

    unsigned int Renderer::GetMaxTextureUnits()
    {
        return static_cast<unsigned int>(RendererCaps.MaxTextureUnits);
    }

    bool Renderer::SupportsMixedSizedBufferAttachments()
    {
        return RendererCaps.SupportsMixedSizedBufferAttachments != 0;
    }
}
