// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorBodyControl_hpp_
#define __GT_EditorBodyControl_hpp_

#include "EditorControl.hpp"

namespace GT
{
    /// Control representing the central part of the editor window.
    class EditorBody : public EditorControl
    {
    public:

        /// Constructor.
        EditorBody(Editor &editor);

        /// Destructor.
        ~EditorBody();


    private:
    };
}

#endif