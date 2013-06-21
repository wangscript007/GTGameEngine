// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Editor_PackagingToolEditor_hpp_
#define __GTEngine_Editor_PackagingToolEditor_hpp_

#include "../SubEditor.hpp"

namespace GTEngine
{
    class PackagingToolEditor : public SubEditor
    {
    public:

        /// Constructor.
        PackagingToolEditor(Editor &ownerEditor, const char* path);

        /// Destructor.
        ~PackagingToolEditor();



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::GetMainElement()
              GTGUI::Element* GetMainElement()       { return this->mainElement; }
        const GTGUI::Element* GetMainElement() const { return this->mainElement; }

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();



    private:

        /// The main GUI element.
        GTGUI::Element* mainElement;
    };
}

#endif