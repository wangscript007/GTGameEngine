// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/TextEditor/TextEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Scripting.hpp>
#include <GTCore/Path.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    TextEditor::TextEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          mainElement(nullptr), textArea(nullptr), panelElement(nullptr), errorListElement(nullptr),
          textAreaEventHandler(new TextAreaEventHandler(this)),
          compilationErrorHandler(*this),
          proxyGame(nullptr),
          compilationScript(nullptr),
          isScriptFile(false)
    {
        GTCore::String fileContent;
        if (GTCore::IO::OpenAndReadTextFile(absolutePath, fileContent))
        {
            auto &gui    = this->GetGUI();
            auto &script = this->GetScript();

            // We need to determine whether or not we are running a script.
            auto extension = GTCore::Path::Extension(absolutePath);
            assert(extension != nullptr);
            {
                this->isScriptFile = GTCore::Strings::Equal<false>(extension, "lua")    ||
                                     GTCore::Strings::Equal<false>(extension, "script") ||
                                     GTCore::Strings::Equal<false>(extension, "gtscript");
            }


            // The file contents have been loaded, so now we need to create the text-box element that will display it.
            this->mainElement = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='text-editor' />");
            assert(this->mainElement != nullptr);
            {
                // The element has been created, but we need to execute a script to have it turn into a proper multi-line text box.
                script.Execute(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s'):TextEditor();", this->mainElement->id).c_str());


                // Now what we need to do is actually set the text. This will be much quicker if done on the C++ side so that the script parser doesn't need to
                // parse potentially very large files.
                this->textArea = gui.GetElementByID(script.GetString(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s').TextBox.TextArea:GetID();", this->mainElement->id).c_str()));
                assert(this->textArea != nullptr);
                {
                    this->textArea->SetText(fileContent.c_str());
                    this->textArea->SetStyleAttribute("background-color", "inherit");
                    this->textArea->SetStyleAttribute("text-color",       "inherit");
                     
                    this->textArea->textManager.MoveCursorToStart();
                    gui.FocusElement(this->textArea);


                    // Now we need to attach an event handler to the text area so we can detect changes to the text. We propagate this to the scripting
                    // environment so that the owner tab can be marked as modified.
                    this->textArea->AttachEventHandler(*this->textAreaEventHandler);
                }

                
                // The panel.
                this->panelElement = gui.GetElementByID(script.GetString(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s').Panel:GetID();", this->mainElement->id).c_str()));
                assert(this->panelElement != nullptr);
                {
                    // If we're editting a regular text file (not a script), we don't want to show the panel.
                    if (!this->IsScriptFile())
                    {
                        this->panelElement->Hide();
                    }
                }


                // Error list.
                this->errorListElement = gui.GetElementByID(script.GetString(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s').ErrorList:GetID();", this->panelElement->id).c_str()));
                assert(this->errorListElement != nullptr);
                {
                }
            }

            // Do an initial compilation.
            if (this->IsScriptFile())
            {
                this->proxyGame = new Game;
                if (this->proxyGame->GetScript().Startup())
                {
                    // We need to setup a few things with the scripting.
                    this->compilationScript = &this->proxyGame->GetScript();
                    this->compilationScript->AttachErrorHandler(this->compilationErrorHandler);

                    this->CompileAndUpdateErrorOutput();
                }
            }
        }
    }

    TextEditor::~TextEditor()
    {
        this->GetGUI().DeleteElement(this->mainElement);
        delete this->textAreaEventHandler;

        delete this->proxyGame;
    }


    bool TextEditor::IsScriptFile() const
    {
        return this->isScriptFile;
    }

    void TextEditor::CompileAndUpdateErrorOutput()
    {
        if (this->IsScriptFile())
        {
            assert(this->proxyGame         != nullptr);
            assert(this->compilationScript != nullptr);


            // The the error list first.
            this->ClearErrorList();

            // Now we compile.
            this->compilationScript->Execute("self = {}");
            this->compilationScript->Execute(this->textArea->GetText(), this->GetAbsolutePath());
        }
    }

    void TextEditor::OnScriptSyntaxError(GTCore::Script &script, int lineNumber, const char* message)
    {
        (void)script;

        this->AddItemToErrorList(lineNumber, message);
    }


    ///////////////////////////////////////////////////
    // Virtual Methods.

    void TextEditor::Show()
    {
        this->mainElement->Show();
        this->GetGUI().FocusElement(this->textArea);
    }

    void TextEditor::Hide()
    {
        this->mainElement->Hide();
    }

    bool TextEditor::Save()
    {
        bool wasSaved = false;

        auto text = this->textArea->GetText();
        if (text != nullptr)
        {
            wasSaved = GTCore::IO::OpenAndWriteTextFile(this->GetAbsolutePath(), text);
        }
        else
        {
            wasSaved = GTCore::IO::OpenAndWriteTextFile(this->GetAbsolutePath(), "");
        }

        if (wasSaved)
        {
            this->UnmarkAsModified();

            // If the file was a script file, we want to do an immediate check for modifications so that any open scene editors can see
            // the changes seemlessly.
            if (this->IsScriptFile())
            {
                this->CompileAndUpdateErrorOutput();
                this->GetOwnerEditor().GetGame().GetDataFilesWatcher().CheckForChangesAndDispatchEvents();
            }
        }

        return wasSaved;
    }


    ///////////////////////////////////////////////////
    // Private

    void TextEditor::AddItemToErrorList(int lineNumber, const char* message)
    {
        auto &script = this->GetScript();

        script.GetGlobal("GTGUI");
        assert(script.IsTable(-1));
        {
            script.Push("Server");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("GetElementByID");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(this->errorListElement->id);
                    script.Call(1, 1);
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("AddItem");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);    // 'self'
                                script.Push(lineNumber);
                                script.Push(message);
                                script.Call(3, 0);
                            }
                        }
                        script.Pop(1);      // <-- return value from GetElementByID().
                    }
                }
            }
            script.Pop(1);
        }
        script.Pop(1);
    }

    void TextEditor::ClearErrorList()
    {
        auto &script = this->GetScript();

        script.GetGlobal("GTGUI");
        assert(script.IsTable(-1));
        {
            script.Push("Server");
            script.GetTableValue(-2);
            assert(script.IsTable(-1));
            {
                script.Push("GetElementByID");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.Push(this->errorListElement->id);
                    script.Call(1, 1);
                    {
                        assert(script.IsTable(-1));
                        {
                            script.Push("Clear");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);    // 'self'
                                script.Call(1, 0);
                            }
                        }
                        script.Pop(1);      // <-- return value from GetElementByID().
                    }
                }
            }
            script.Pop(1);
        }
        script.Pop(1);
    }




    ///////////////////////////////////////////////////
    // TextAreaEventHandler.

    TextEditor::TextAreaEventHandler::TextAreaEventHandler(TextEditor* ownerTextEditorIn)
        : ownerTextEditor(ownerTextEditorIn)
    {
    }

    void TextEditor::TextAreaEventHandler::OnTextChanged(GTGUI::Element &)
    {
        assert(this->ownerTextEditor != nullptr);
        {
            if (!this->ownerTextEditor->IsMarkedAsModified())
            {
                this->ownerTextEditor->MarkAsModified();
            }
        }
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif