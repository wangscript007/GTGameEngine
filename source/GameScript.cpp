
#include <GTEngine/Scripting.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/ApplicationConfig.hpp>

namespace GTEngine
{
    GameScript::GameScript(Game &game)
        : game(game), lastError(), loadedFiles()
    {
    }

    GameScript::~GameScript()
    {
    }

    bool GameScript::Startup()
    {
        // First we load the GTEngine scripting stuff.
        bool success = Scripting::LoadGTEngineScriptLibrary(*this);

        // The first thing we want to do is load some defaults.
        success = success && this->Execute
        (
            "Game   = {};"
            "Editor = {};"
            ""
            "Display ="
            "{"
            "    Width      = 1280;"
            "    Height     = 720;"
            "    FOV        = 90;"
            "    Fullscreen = false;"
            ""
            "    Textures ="
            "    {"
            "        Anisotropy = 16;"
            "    };"
            "};"
            ""
            "Directories ="
            "{"
            "    Data = {};"
            "}"
        );

        // Here we load the data directories from the application config. We need to do this so that the editor has access to them. Might also come
        // in handy for game code, too. Who knows.
        this->GetGlobal("Directories");
        if (this->IsTable(-1))
        {
            this->Push("Data");
            this->GetTableValue(-2);
            if (this->IsTable(-1))
            {
                auto &dataDirectories = ApplicationConfig::GetDataDirectories();
                for (size_t iDirectory = 0; iDirectory < dataDirectories.count; ++iDirectory)
                {
                    this->SetTableValue(-1, iDirectory + 1, dataDirectories[iDirectory].c_str());
                }
            }
            this->Pop(1);
        }
        this->Pop(1);

        // Here is where we register the foreign function interface.
        success = success && this->RegisterFFI();

        return success;
    }

    bool GameScript::Load(const char* script)
    {
        if (!GTCore::Script::Load(script))
        {
            this->lastError = this->ToString(-1);
            return false;
        }

        return true;
    }

    bool GameScript::LoadFile(const char* fileName)
    {
        if (!this->HasFileBeenLoaded(fileName))
        {
            GTCore::String absolutePath;
            if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
            {
                this->loadedFiles.PushBack(absolutePath);
            }
        }

        return GTCore::Script::LoadFile(fileName);
    }




    bool GameScript::Execute()
    {
        if (!GTCore::Script::Execute())
        {
            this->lastError = this->ToString(-1);
            PostError("Script Error: %s", this->lastError.c_str());

            return false;
        }

        return true;
    }


    bool GameScript::HasFileBeenLoaded(const char* fileName) const
    {
        GTCore::String absolutePath;
        if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
        {
            return this->loadedFiles.Exists(absolutePath);
        }

        return false;
    }
}

///////////////////////////////////////////////////////////
// FFI

namespace GTEngine
{
    Game & GameScript::FFI::GetGameObject(GTCore::Script &script)
    {
        script.GetGlobal("__GamePtr");
        auto game = static_cast<Game*>(script.ToPointer(-1));
        script.Pop(1);

        assert(game != nullptr);

        return *game;
    }


    ////////////////////////////////////////////////////////////////
    // Game FFI

    int FFI_Game_Close(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.Close();
        return 0;
    }

    int FFI_Game_Pause(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.Pause();
        return 0;
    }

    int FFI_Game_Resume(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.Resume();
        return 0;
    }

    int FFI_Game_EnableFullscreen(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.EnableFullscreen();
        return 0;
    }

    int FFI_Game_DisableFullscreen(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.DisableFullscreen();
        return 0;
    }


    int FFI_Game_ExecuteFile(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        script.Push(game.GetScript().ExecuteFile(script.ToString(1)));
        return 1;
    }

    int FFI_Game_ExecuteScript(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        script.Push(game.ExecuteScript(script.ToString(1)));
        return 1;
    }

    int FFI_Game_GetLastScriptError(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        script.Push(game.GetScript().GetLastError());
        return 1;
    }


    int FFI_Game_ShowDebug(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.ShowDebugging();
        return 0;
    }

    int FFI_Game_HideDebug(GTCore::Script &script)
    {
        auto &game = GameScript::FFI::GetGameObject(script);

        game.HideDebugging();
        return 0;
    }



    //////////////////////////////////////////////////////
    // RegisterFFI()

    bool GameScript::RegisterFFI()
    {
        // We assert that the tables have already been created.

        this->Push(&this->game);
        this->SetGlobal("__GamePtr");

        this->GetGlobal("Game");
        if (this->IsTable(-1))
        {
            this->SetTableFunction(-1, "Close",              FFI_Game_Close);

            this->SetTableFunction(-1, "Pause",              FFI_Game_Pause);
            this->SetTableFunction(-1, "Resume",             FFI_Game_Resume);

            this->SetTableFunction(-1, "EnableFullscreen",   FFI_Game_EnableFullscreen);
            this->SetTableFunction(-1, "DisableFullscreen",  FFI_Game_DisableFullscreen);

            this->SetTableFunction(-1, "ExecuteFile",        FFI_Game_ExecuteFile);
            this->SetTableFunction(-1, "ExecuteScript",      FFI_Game_ExecuteScript);
            this->SetTableFunction(-1, "GetLastScriptError", FFI_Game_GetLastScriptError);

            this->SetTableFunction(-1, "ShowDebug",          FFI_Game_ShowDebug);
            this->SetTableFunction(-1, "HideDebug",          FFI_Game_HideDebug);
        }
        this->Pop(1);    // Game


        return true;
    }
}
