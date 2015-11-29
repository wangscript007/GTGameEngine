// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Scripting_hpp_
#define __GTEngine_Scripting_hpp_

#include <GTLib/Scripting.hpp>
#include "Scripting/Scripting_Game.hpp"
#include "Scripting/Scripting_Math.hpp"
#include "Scripting/Scripting_Rendering.hpp"
#include "Scripting/Scripting_Audio.hpp"
#include "Scripting/Scripting_Scene.hpp"
#include "Scripting/Scripting_SceneNode.hpp"
#include "Scripting/Scripting_Components.hpp"
#include "Scripting/Scripting_Particles.hpp"
#include "Scripting/Scripting_Editor.hpp"
#include "Scripting/Scripting_Animation.hpp"

namespace GT
{
    /// Loads the GTEngine script library into the given script.
    ///
    /// @param script [in] The script to load the GTEngine script library into.
    ///
    /// @return True if the library is loaded successfully; false otherwise.
    ///
    /// @remarks
    ///     This only loads the content in the GTEngine namespace/table. This will NOT load any game-specific functionality.
    bool LoadGTEngineScriptLibrary(GT::Script &script);



    /// Post a global MouseMove event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    void PostEvent_OnMouseMove(GT::Script &script, int mousePosX, int mousePosY);

    /// Post a global MouseWheel event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    /// @param delta     [in] The scroll delta.
    void PostEvent_OnMouseWheel(GT::Script &script, int mousePosX, int mousePosY, int delta);

    /// Post a global MouseButtonDown event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    /// @param button    [in] The button code.
    void PostEvent_OnMouseButtonDown(GT::Script &script, int mousePosX, int mousePosY, GTLib::MouseButton button);

    /// Post a global MouseButtonUp event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    /// @param button    [in] The button code.
    void PostEvent_OnMouseButtonUp(GT::Script &script, int mousePosX, int mousePosY, GTLib::MouseButton button);

    /// Post a global MouseButtonDoubleClick event to everything relevant.
    ///
    /// @param script    [in] A reference to the main script object.
    /// @param mousePosX [in] The x mouse position.
    /// @param mousePosY [in] The y mouse position.
    /// @param button    [in] The button code.
    void PostEvent_OnMouseButtonDoubleClick(GT::Script &script, int mousePosX, int mousePosY, GTLib::MouseButton button);

    /// Post a global KeyPressed event to everything relevant.
    ///
    /// @param script [in] A reference to the main script object.
    /// @param key    [in] The key code.
    void PostEvent_OnKeyPressed(GT::Script &script, GTLib::Key key);

    /// Post a global KeyReleased event to everything relevant.
    ///
    /// @param script [in] A reference to the main script object.
    /// @param key    [in] The key code.
    void PostEvent_OnKeyReleased(GT::Script &script, GTLib::Key key);

    /// Posts a global OnGamePause event to everything relevant.
    ///
    /// @param script [in] A reference to the main script object.
    void PostEvent_OnGamePause(GT::Script &script);

    /// Posts a global OnGamePause event to everything relevant.
    ///
    /// @param script [in] A reference to the main script object.
    void PostEvent_OnGameResume(GT::Script &script);


    ///////////////////////////////////////////////////////////////
    // Script Definitions

    /// Loads a scene node script, replacing the old one if it exists.
    ///
    /// @param script             [in] A reference to the script to load the scene node script into.
    /// @param scriptRelativePath [in] The relative path of the script.
    /// @param scriptString       [in] The actual script content.
    bool LoadScriptDefinition(GT::Script &script, const char* scriptRelativePath, const char* scriptString);

    /// Unloads a scene node script.
    ///
    /// @param script             [in] A reference to the script to unload the scene node script from.
    /// @param scriptRelativePath [in] The relative path of the script to unload.
    void UnloadScriptDefinition(GT::Script &script, const char* scriptRelativePath);



    namespace FFI
    {
        /// Retrieves the executable directory.
        int GetExecutableDirectory(GT::Script &script);

        /// Retrieves the version string.
        int GetVersionString(GT::Script &script);

        /// Determines if the given file path is a model file, based on it's extension.
        int IsModelFile(GT::Script &script);

        /// Determines if the given file path is an image file, based on it's extension.
        int IsImageFile(GT::Script &script);

        /// Determines if the given file path is a sound file, based on it's extension.
        int IsSoundFile(GT::Script &script);

        /// Determines if the given file path is a scene file, based on it's extension.
        int IsSceneFile(GT::Script &script);

        /// Determines if the given file path is a prefab file, based on it's extension.
        int IsPrefabFile(GT::Script &script);

        /// Determines if the given file path is a script file, based on it's extension.
        int IsScriptFile(GT::Script &script);


        /// Creates a prefab file from a scene node.
        ///
        /// @remarks
        ///     Argument 1: The absolute path of the file to create or replace.
        ///     Argument 2: The path to make the absolute path relative to.
        ///     Argument 3: A pointer to the scene node to create the prefrab file from.
        int CreatePrefab(GT::Script &script);


        /// Executes the script defined in the given file.
        ///
        /// @remarks
        ///     Argument 1: The name of the script file to load and execute.
        int ExecuteFile(GT::Script &script);

        /// Executes the given script text.
        ///
        /// @remarks
        ///     Argument 1: The script text to execute.
        int ExecuteScript(GT::Script &script);

        /// Retrieves the last script error.
        int GetLastScriptError(GT::Script &script);


        /// Generates a random number between the two given numbers.
        ///
        /// @remarks
        ///     Argument 1: The low bound.
        ///     Argument 2: The high bound.
        int RandomInteger(GT::Script &script);
        int RandomFloat(GT::Script &script);
    }
}

#endif
