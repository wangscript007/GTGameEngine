
#include <GTEngine/Editor.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Game.hpp>
#include <GTGUI/Server.hpp>
#include <GTCore/Path.hpp>


#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    Editor::Editor(Game &game)
        : game(game), GUI(),
          modelEditor(*this), sandbox(*this),
          currentMode(nullptr), previousMode(nullptr),
          imageEditor(*this),
          isStarted(false), isOpen(false),
          dataFilesWatcherEventHandler(*this)
    {
    }

    Editor::~Editor()
    {
    }

    bool Editor::Startup(GTGUI::Server &guiServer)
    {
        if (!this->isStarted)
        {
            this->StartupScripting();

            auto &script = guiServer.GetScriptServer().GetScript();

            // We need to grab the main elements from the server.
            if (guiServer.LoadFromFile("engine/editor/main.xml"))
            {
                this->GUI.EditorMain = guiServer.GetElementByID("EditorMain");

                // Here we startup our sub-editors.
                //this->modelEditor.Startup();
                this->imageEditor.Startup();
                //this->soundEditor.Startup();


                // Here we need to attach our files watcher event handler.
                this->game.GetDataFilesWatcher().AddEventHandler(this->dataFilesWatcherEventHandler);
                
                // Here is where we startup our editor modes.
                this->modelEditor.Startup(guiServer);
                this->sandbox.Startup(guiServer);

                // Here we enable the default mode.
                this->SwitchToModelEditorMode();
                this->isStarted = true;
            }
            else
            {
                GTEngine::PostError("Error initializing editor: Failed to load editor/main.xml. Ensure the 'editor' directory exists. %s.", script.ToString(-1));
                return false;
            }
        }

        return true;
    }

    void Editor::Open()
    {
        if (!this->isOpen && this->GUI.EditorMain != nullptr)
        {
            this->game.ShowCursor();
            this->GUI.EditorMain->Show();

            // We want to watch the data files.
            this->game.EnableDataFilesWatching();

            // We also want to get an update on the data files immediately.
            this->game.GetDataFilesWatcher().CheckForChanges(false);
            this->game.GetDataFilesWatcher().DispatchEvents();

            this->isOpen = true;
        }
    }

    void Editor::Close()
    {
        if (this->isOpen)
        {
            this->GUI.EditorMain->Hide();

            // We don't want to be watching data files anymore.
            this->game.DisableDataFilesWatching();

            this->isOpen = false;
        }
    }


    void Editor::SwitchToModelEditorMode()
    {
        this->SetEditorMode(&this->modelEditor);
    }

    void Editor::SwitchToSandboxMode()
    {
        this->SetEditorMode(&this->sandbox);
    }

    void Editor::SwitchToPreviousMode()
    {
        this->SetEditorMode(this->previousMode);
    }


    void Editor::Update(double deltaTimeInSeconds)
    {
        if (this->currentMode != nullptr)
        {
            this->currentMode->OnUpdate(deltaTimeInSeconds);
        }
    }


    void Editor::OnModelActivated(const char* fileName)
    {
        (void)fileName;
    }

    void Editor::OnImageActivated(const char* fileName)
    {
        this->imageEditor.LoadImage(fileName);
    }

    void Editor::OnSoundActivated(const char* fileName)
    {
        (void)fileName;
    }


    void Editor::OnFileInsert(const DataFilesWatcher::Item &item)
    {
        GTCore::String script;
        script.AssignFormatted
        (
            "local info = GTCore.IO.FileInfo:New();"
            "info.path             = '%s';"
            "info.absolutePath     = '%s';"
            "info.size             = %d;"
            "info.lastModifiedTime = %d;"
            "info.isDirectory      = %s;"
            "Editor.DataFilesWatcher.OnInsert(info);",

            item.info.absolutePath.c_str(),
            item.info.absolutePath.c_str(),
            static_cast<int>(item.info.size),
            static_cast<int>(item.info.lastModifiedTime),
            item.info.isDirectory ? "true" : "false"
        );

        this->game.GetScript().Execute(script.c_str());
    }

    void Editor::OnFileRemove(const DataFilesWatcher::Item &item)
    {
        GTCore::String script;
        script.AssignFormatted
        (
            "local info = GTCore.IO.FileInfo:New();"
            "info.path             = '%s';"
            "info.absolutePath     = '%s';"
            "info.size             = %d;"
            "info.lastModifiedTime = %d;"
            "info.isDirectory      = %s;"
            "Editor.DataFilesWatcher.OnRemove(info);",

            item.info.absolutePath.c_str(),
            item.info.absolutePath.c_str(),
            static_cast<int>(item.info.size),
            static_cast<int>(item.info.lastModifiedTime),
            item.info.isDirectory ? "true" : "false"
        );

        this->game.GetScript().Execute(script.c_str());
    }

    void Editor::OnFileUpdate(const DataFilesWatcher::Item &item)
    {
        GTCore::String script;
        script.AssignFormatted
        (
            "local info = GTCore.IO.FileInfo:New();"
            "info.path             = '%s';"
            "info.absolutePath     = '%s';"
            "info.size             = %d;"
            "info.lastModifiedTime = %d;"
            "info.isDirectory      = %s;"
            "Editor.DataFilesWatcher.OnUpdate(info);",

            item.info.absolutePath.c_str(),
            item.info.absolutePath.c_str(),
            static_cast<int>(item.info.size),
            static_cast<int>(item.info.lastModifiedTime),
            item.info.isDirectory ? "true" : "false"
        );

        this->game.GetScript().Execute(script.c_str());
    }



    void Editor::StartupScripting()
    {
        auto &script = this->game.GetScript();

        script.GetGlobal("Editor");
        if (script.IsTable(-1))
        {
            script.SetTableFunction(-1, "Open",  FFI::Open);
            script.SetTableFunction(-1, "Close", FFI::Close);

            script.SetTableFunction(-1, "OnModelActivated", FFI::OnModelActivated);
            script.SetTableFunction(-1, "OnImageActivated", FFI::OnImageActivated);
            script.SetTableFunction(-1, "OnSoundActivated", FFI::OnSoundActivated);
        }
        script.Pop(1);
    }

    void Editor::SetEditorMode(EditorMode* newMode)
    {
        if (this->currentMode != newMode)
        {
            this->previousMode = this->currentMode;
            if (this->previousMode != nullptr)
            {
                this->previousMode->OnDeactivate();
            }

            this->currentMode = newMode;
            if (this->currentMode != nullptr)
            {
                this->currentMode->OnActivate();
            }
        }
    }




    /////////////////////////////////////////////
    // FFI.
    Editor & Editor::FFI::GetEditor(GTCore::Script &script)
    {
        return GameScript::FFI::GetGameObject(script).GetEditor();
    }


    int Editor::FFI::Open(GTCore::Script &script)
    {
        GameScript::FFI::GetGameObject(script).OpenEditor();
        return 0;
    }

    int Editor::FFI::Close(GTCore::Script &script)
    {
        GameScript::FFI::GetGameObject(script).CloseEditor();
        return 0;
    }


    int Editor::FFI::OnModelActivated(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnModelActivated(script.ToString(1));
        return 0;
    }

    int Editor::FFI::OnImageActivated(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnImageActivated(script.ToString(1));
        return 0;
    }

    int Editor::FFI::OnSoundActivated(GTCore::Script &script)
    {
        FFI::GetEditor(script).OnSoundActivated(script.ToString(1));
        return 0;
    }
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
