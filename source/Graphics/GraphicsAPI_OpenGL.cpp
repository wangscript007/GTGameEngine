// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Graphics/GraphicsAPI_OpenGL.hpp>
#include <GTLib/Strings/Tokenizer.hpp>      // For extracting extensions.

namespace GT
{
    GraphicsAPI_OpenGL::GraphicsAPI_OpenGL()
        : GraphicsAPI(),
#if defined(GT_PLATFORM_WINDOWS)
          m_hOpenGL32(NULL),
          m_hDummyHWND(NULL),
          m_hDummyDC(NULL),
          m_hRC(NULL),
          m_pixelFormat(0),
          m_pfd(),
#endif
          m_majorVersion(0),
          m_minorVersion(0),
          m_extensions()
    {
    }

    GraphicsAPI_OpenGL::~GraphicsAPI_OpenGL()
    {
        
    }


    bool GraphicsAPI_OpenGL::Startup()
    {
        m_hOpenGL32 = LoadLibraryW(L"OpenGL32.dll");
        if (m_hOpenGL32 != NULL)
        {
            m_hDummyHWND = CreateWindowExW(0, L"STATIC", L"", 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
            m_hDummyDC   = GetDC(m_hDummyHWND);

            memset(&m_pfd, 0, sizeof(m_pfd));
            m_pfd.nSize        = sizeof(m_pfd);
            m_pfd.nVersion     = 1;
            m_pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            m_pfd.iPixelType   = PFD_TYPE_RGBA;
            m_pfd.cStencilBits = 8;
            m_pfd.cDepthBits   = 24;
            m_pfd.cColorBits   = 32;
            m_pixelFormat = ChoosePixelFormat(m_hDummyDC, &m_pfd);

            if (m_pixelFormat != 0)
            {
                if (SetPixelFormat(m_hDummyDC, m_pixelFormat,  &m_pfd))
                {
                    this->CreateContext = reinterpret_cast<PFNWGLCREATECONTEXTPROC>(::GetProcAddress(m_hOpenGL32, "wglCreateContext"));
                    this->DeleteContext = reinterpret_cast<PFNWGLDELETECONTEXTPROC>(::GetProcAddress(m_hOpenGL32, "wglDeleteContext"));

                    if (this->CreateContext != nullptr && this->DeleteContext != nullptr)
                    {
                        m_hRC = this->CreateContext(m_hDummyDC);
                        if (m_hRC != NULL)
                        {
                            this->MakeCurrent = reinterpret_cast<PFNWGLMAKECURRENTPROC>(::GetProcAddress(m_hOpenGL32, "wglMakeCurrent"));
                            if (this->MakeCurrent != nullptr && this->MakeCurrent(m_hDummyDC, m_hRC))
                            {
                                this->GetProcAddress = reinterpret_cast<PFNWGLGETPROCADDRESSPROC>(::GetProcAddress(m_hOpenGL32, "wglGetProcAddress"));
                                if (this->GetProcAddress != nullptr)
                                {
                                    // We now need to get the version and extensions. With newer versions of OpenGL we need to use glGetStringi() in conjuction with glGetIntegerv() to get extensions.
                                    this->GetString   = reinterpret_cast<PFNGLGETSTRINGPROC  >(this->GetGLProcAddress("glGetString"));
                                    this->GetStringi  = reinterpret_cast<PFNGLGETSTRINGIPROC >(this->GetGLProcAddress("glGetStringi"));
                                    this->GetIntegerv = reinterpret_cast<PFNGLGETINTEGERVPROC>(this->GetGLProcAddress("glGetIntegerv"));
                                    if (this->GetString != nullptr && this->GetIntegerv != nullptr)
                                    {
                                        const char* versionStr = reinterpret_cast<const char*>(this->GetString(GL_VERSION));
                                        if (versionStr != nullptr)
                                        {
                                            const char* majorStart = versionStr;
                                                  char* minorStart;

                                            m_majorVersion = strtoul(majorStart, &minorStart, 0);
                                            m_minorVersion = strtoul(minorStart + 1, NULL, 0);

                                            // Clamp at OpenGL 2.1 since this graphics API is only intended for backwards compatibility.
                                            if (m_majorVersion > 2)
                                            {
                                                m_majorVersion = 2;
                                                m_minorVersion = 1;
                                            }

                                            if (m_majorVersion == 2 && m_minorVersion >= 0)
                                            {
                                                // At this point we know we support at least OpenGL 2.0, so initialize the rest of the API by retrieving all of the
                                                // extensions and relevant function pointers.
                                                if (this->InitExtensions())
                                                {
                                                    if (this->InitAPI())
                                                    {
                                                        // If we get here there were no errors. It's been a long journey...
                                                        return true;
                                                    }
                                                    else
                                                    {
                                                        // Failed to initialize the API.
                                                        this->Shutdown();
                                                    }
                                                }
                                                else
                                                {
                                                    // Failed to initialize the extensions.
                                                    this->Shutdown();
                                                }
                                            }
                                            else
                                            {
                                                // OpenGL 2.0 is not supported.
                                                this->Shutdown();
                                            }
                                        }
                                        else
                                        {
                                            // Failed to retrieve the version string.
                                            this->Shutdown();
                                        }
                                    }
                                    else
                                    {
                                        // Failed to retrieve glGetString and glGetIntegerv (for version and extension detection).
                                        this->Shutdown();
                                    }
                                }
                                else
                                {
                                    // Failed to make context current.
                                    this->Shutdown();
                                }
                            }
                            else
                            {
                                // Failed to retrieve wglMakeCurrent.
                                this->Shutdown();
                            }
                        }
                        else
                        {
                            // Failed to create context.
                            this->Shutdown();
                        }
                    }
                    else
                    {
                        // Failed to retrieve wglCreateContext and wglDeleteContext.
                        this->Shutdown();
                    }
                }
                else
                {
                    // Failed to set pixel format.
                    this->Shutdown();
                }
            }
            else
            {
                // Failed to find pixel format.
                this->Shutdown();
            }
        }
        else
        {
            // Failed to load OpenGL32.dll.
        }

        return false;
    }

    void GraphicsAPI_OpenGL::Shutdown()
    {
        for (size_t iExtension = 0; iExtension < m_extensions.GetCount(); ++iExtension)
        {
            free(const_cast<void*>(reinterpret_cast<const void*>(m_extensions[iExtension])));
        }
        m_extensions.Clear();


#if defined(GT_PLATFORM_WINDOWS)
        if (this->DeleteContext != nullptr)
        {
            this->DeleteContext(m_hRC);
            m_hRC = NULL;
        }

        DestroyWindow(m_hDummyHWND);
        m_hDummyHWND = NULL;

        FreeLibrary(m_hOpenGL32);
        m_hOpenGL32 = NULL;
#endif
    }


    GraphicsAPIType GraphicsAPI_OpenGL::GetType() const
    {
        return GraphicsAPIType_OpenGL;
    }


    void GraphicsAPI_OpenGL::GetVersion(unsigned int &majorVersionOut, unsigned int &minorVersionOut) const
    {
        majorVersionOut = m_majorVersion;
        minorVersionOut = m_minorVersion;
    }

    bool GraphicsAPI_OpenGL::IsExtensionSupported(const char* extension) const
    {
        for (size_t iExtension = 0; iExtension < m_extensions.GetCount(); ++iExtension)
        {
            if (strcmp(m_extensions[iExtension], extension) == 0)
            {
                return true;
            }
        }

        return false;
    }


#if defined(GT_PLATFORM_WINDOWS)
    HGLRC GraphicsAPI_OpenGL::GetRenderingContext() const
    {
        return m_hRC;
    }

    int GraphicsAPI_OpenGL::GetPixelFormat() const
    {
        return m_pixelFormat;
    }

    const PIXELFORMATDESCRIPTOR & GraphicsAPI_OpenGL::GetPFD() const
    {
        return m_pfd;
    }

    HDC GraphicsAPI_OpenGL::GetDummyDC() const
    {
        return m_hDummyDC;
    }
#endif



    /////////////////////////////////
    // Private

    void* GraphicsAPI_OpenGL::GetGLProcAddress(const char* procName) const
    {
#if defined(GT_PLATFORM_WINDOWS)
        assert(this->GetProcAddress != nullptr);

        void* proc = ::GetProcAddress(m_hOpenGL32, procName);
        if (proc == nullptr)
        {
            proc = this->GetProcAddress(procName);
        }
            
        return proc;
#endif

#if defined(GT_PLATFORM_LINUX)
#endif
    }

    bool GraphicsAPI_OpenGL::InitExtensions()
    {
        if (this->GetStringi != nullptr)
        {
            // Use the new way.
            GLuint extensionCount = 0;
            this->GetIntegerv(GL_NUM_EXTENSIONS, reinterpret_cast<GLint*>(&extensionCount));

            for (GLuint iExtension = 0; iExtension < extensionCount; ++iExtension)
            {
                const char* extensionStrSource = reinterpret_cast<const char*>(this->GetStringi(GL_EXTENSIONS, iExtension));

                size_t extensionStrLength = strlen(extensionStrSource);
                char* extensionStr = reinterpret_cast<char*>(malloc(extensionStrLength + 1));  // +1 for null terminator.
                memcpy(extensionStr, extensionStrSource, extensionStrLength);
                extensionStr[extensionStrLength] = '\0';


                m_extensions.PushBack(extensionStr);
            }
        }
        else
        {
            // Use the old way.
            const char* extensions = reinterpret_cast<const char*>(this->GetString(GL_EXTENSIONS));
            if (extensions != nullptr)
            {
                GTLib::Strings::WhitespaceTokenizerUTF8 extension(extensions);
                while (extension)
                {
                    char* extensionStr = reinterpret_cast<char*>(malloc(extension.GetSizeInBytes() + 1));  // +1 for null terminator.
                    extension.Copy(extensionStr);

                    m_extensions.PushBack(extensionStr);

                    ++extension;
                }
            }
        }


#if defined(GT_PLATFORM_WINDOWS)
        // WGL extensions.
        const char* extensionsWGL = nullptr;

        auto _wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(this->GetProcAddress("wglGetExtensionsStringARB"));
        if (_wglGetExtensionsStringARB != nullptr)
        {
            extensionsWGL = reinterpret_cast<const char*>(_wglGetExtensionsStringARB(m_hDummyDC));
        }
        else
        {
            auto _wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(this->GetProcAddress("wglGetExtensionsStringARB"));
            if (_wglGetExtensionsStringEXT != nullptr)
            {
                extensionsWGL = reinterpret_cast<const char*>(_wglGetExtensionsStringEXT());
            }
        }


        if (extensionsWGL != nullptr)
        {
            GTLib::Strings::WhitespaceTokenizerUTF8 extension(extensionsWGL);
            while (extension)
            {
                char* extensionStr = reinterpret_cast<char*>(malloc(extension.GetSizeInBytes() + 1));  // +1 for null terminator.
                extension.Copy(extensionStr);

                m_extensions.PushBack(extensionStr);

                ++extension;
            }
        }
#endif


        return true;
    }


    bool GraphicsAPI_OpenGL::InitAPI()
    {
#if defined(GT_PLATFORM_WINDOWS)
        this->GetCurrentContext = reinterpret_cast<PFNWGLGETCURRENTCONTEXTPROC>(::GetProcAddress(m_hOpenGL32, "wglGetCurrentContext"));
        this->GetCurrentDC      = reinterpret_cast<PFNWGLGETCURRENTDCPROC     >(::GetProcAddress(m_hOpenGL32, "wglGetCurrentDC"));
                
        if (this->IsExtensionSupported("WGL_EXT_swap_control"))
        {
            this->SwapIntervalEXT    = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC   >(this->GetGLProcAddress("wglSwapIntervalEXT"));
            this->GetSwapIntervalEXT = reinterpret_cast<PFNWGLGETSWAPINTERVALEXTPROC>(this->GetGLProcAddress("wglGetSwapIntervalEXT"));
        }
#endif

        this->GetIntegerv              = reinterpret_cast<PFNGLGETINTEGERVPROC             >(this->GetGLProcAddress("glGetIntegerv"));
        this->DepthFunc                = reinterpret_cast<PFNGLDEPTHFUNCPROC               >(this->GetGLProcAddress("glDepthFunc"));
        this->DepthMask                = reinterpret_cast<PFNGLDEPTHMASKPROC               >(this->GetGLProcAddress("glDepthMask"));
        this->StencilOpSeparate        = reinterpret_cast<PFNGLSTENCILOPSEPARATEPROC       >(this->GetGLProcAddress("glStencilOpSeparate"));
        this->StencilFuncSeparate      = reinterpret_cast<PFNGLSTENCILFUNCSEPARATEPROC     >(this->GetGLProcAddress("glStencilFuncSeparate"));
        this->StencilMaskSeparate      = reinterpret_cast<PFNGLSTENCILMASKSEPARATEPROC     >(this->GetGLProcAddress("glStencilMaskSeparate"));
        this->Enable                   = reinterpret_cast<PFNGLENABLEPROC                  >(this->GetGLProcAddress("glEnable"));
        this->Disable                  = reinterpret_cast<PFNGLDISABLEPROC                 >(this->GetGLProcAddress("glDisable"));
        this->ClearColor               = reinterpret_cast<PFNGLCLEARCOLORPROC              >(this->GetGLProcAddress("glClearColor"));
        this->ClearDepth               = reinterpret_cast<PFNGLCLEARDEPTHPROC              >(this->GetGLProcAddress("glClearDepth"));
        this->ClearStencil             = reinterpret_cast<PFNGLCLEARSTENCILPROC            >(this->GetGLProcAddress("glClearStencil"));
        this->Clear                    = reinterpret_cast<PFNGLCLEARPROC                   >(this->GetGLProcAddress("glClear"));
        this->FrontFace                = reinterpret_cast<PFNGLFRONTFACEPROC               >(this->GetGLProcAddress("glFrontFace"));
        this->CullFace                 = reinterpret_cast<PFNGLCULLFACEPROC                >(this->GetGLProcAddress("glCullFace"));
        this->PolygonMode              = reinterpret_cast<PFNGLPOLYGONMODEPROC             >(this->GetGLProcAddress("glPolygonMode"));
        this->PolygonOffset            = reinterpret_cast<PFNGLPOLYGONOFFSETPROC           >(this->GetGLProcAddress("glPolygonOffset"));
        this->BlendEquationSeparate    = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEPROC   >(this->GetGLProcAddress("glBlendEquationSeparate"));
        this->BlendFuncSeparate        = reinterpret_cast<PFNGLBLENDFUNCSEPARATEPROC       >(this->GetGLProcAddress("glBlendFuncSeparate"));
        this->BlendColor               = reinterpret_cast<PFNGLBLENDCOLORPROC              >(this->GetGLProcAddress("glBlendColor"));
        this->ColorMask                = reinterpret_cast<PFNGLCOLORMASKPROC               >(this->GetGLProcAddress("glColorMask"));
        this->Flush                    = reinterpret_cast<PFNGLFLUSHPROC                   >(this->GetGLProcAddress("glFlush"));
        this->Finish                   = reinterpret_cast<PFNGLFINISHPROC                  >(this->GetGLProcAddress("glFinish"));

        this->DrawElements             = reinterpret_cast<PFNGLDRAWELEMENTSPROC            >(this->GetGLProcAddress("glDrawElements"));

        this->CreateShader             = reinterpret_cast<PFNGLCREATESHADERPROC            >(this->GetGLProcAddress("glCreateShader"));
        this->DeleteShader             = reinterpret_cast<PFNGLDELETESHADERPROC            >(this->GetGLProcAddress("glDeleteShader"));
        this->ShaderSource             = reinterpret_cast<PFNGLSHADERSOURCEPROC            >(this->GetGLProcAddress("glShaderSource"));
        this->CompileShader            = reinterpret_cast<PFNGLCOMPILESHADERPROC           >(this->GetGLProcAddress("glCompileShader"));
        this->GetShaderiv              = reinterpret_cast<PFNGLGETSHADERIVPROC             >(this->GetGLProcAddress("glGetShaderiv"));
        this->GetShaderInfoLog         = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC        >(this->GetGLProcAddress("glGetShaderInfoLog"));
        this->CreateProgram            = reinterpret_cast<PFNGLCREATEPROGRAMPROC           >(this->GetGLProcAddress("glCreateProgram"));
        this->DeleteProgram            = reinterpret_cast<PFNGLDELETEPROGRAMPROC           >(this->GetGLProcAddress("glDeleteProgram"));
        this->AttachShader             = reinterpret_cast<PFNGLATTACHSHADERPROC            >(this->GetGLProcAddress("glAttachShader"));
        this->DetachShader             = reinterpret_cast<PFNGLDETACHSHADERPROC            >(this->GetGLProcAddress("glDetachShader"));
        this->LinkProgram              = reinterpret_cast<PFNGLLINKPROGRAMPROC             >(this->GetGLProcAddress("glLinkProgram"));
        this->GetProgramiv             = reinterpret_cast<PFNGLGETPROGRAMIVPROC            >(this->GetGLProcAddress("glGetProgramiv"));
        this->GetProgramInfoLog        = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC       >(this->GetGLProcAddress("glGetProgramInfoLog"));
        this->UseProgram               = reinterpret_cast<PFNGLUSEPROGRAMPROC              >(this->GetGLProcAddress("glUseProgram"));
        this->GetAttribLocation        = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC       >(this->GetGLProcAddress("glGetAttribLocation"));

        this->GenTextures              = reinterpret_cast<PFNGLGENTEXTURESPROC             >(this->GetGLProcAddress("glGenTextures"));
        this->DeleteTextures           = reinterpret_cast<PFNGLDELETETEXTURESPROC          >(this->GetGLProcAddress("glDeleteTextures"));
        this->BindTexture              = reinterpret_cast<PFNGLBINDTEXTUREPROC             >(this->GetGLProcAddress("glBindTexture"));
        this->TexImage1D               = reinterpret_cast<PFNGLTEXIMAGE1DPROC              >(this->GetGLProcAddress("glTexImage1D"));
        this->TexImage2D               = reinterpret_cast<PFNGLTEXIMAGE2DPROC              >(this->GetGLProcAddress("glTexImage2D"));
        this->TexImage3D               = reinterpret_cast<PFNGLTEXIMAGE3DPROC              >(this->GetGLProcAddress("glTexImage3D"));


        return true;   // No error.
    }
}
