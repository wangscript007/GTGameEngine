
#include <GTEngine/Editor.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Game.hpp>
#include <GTGUI/Server.hpp>


#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    Editor::Editor(Game &game)
        : game(game), GUI(), modelEditor(game, *this), isStarted(false), isOpen(false)
    {
    }

    Editor::~Editor()
    {
    }

    bool Editor::Startup(GTGUI::Server &guiServer)
    {
        if (!this->isStarted)
        {
            auto &script = guiServer.GetScriptServer().GetScript();

            // We need to load the standard library first.
            script.LoadAndExecute("Editor = {};");


            // We need to grab the main elements from the server.
            if (guiServer.LoadFromFile("engine/editor/main.xml"))
            {
                this->GUI.EditorMain = guiServer.GetElementByID("EditorMain");
                

                // Here is where we startup our sub-editors.
                this->modelEditor.Startup(guiServer);


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

            this->isOpen = true;
        }
    }

    void Editor::Close()
    {
        if (this->isOpen)
        {
            this->GUI.EditorMain->Hide();

            this->isOpen = false;
        }
    }


    void Editor::Update(double deltaTimeInSeconds)
    {
        if (this->isOpen)
        {
            this->modelEditor.Update(deltaTimeInSeconds);
        }
    }


    void Editor::SwapRCQueues()
    {
        if (this->isOpen)
        {
            this->modelEditor.SwapRCQueues();
        }
    }
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
