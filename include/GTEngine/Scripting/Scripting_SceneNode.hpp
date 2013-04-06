// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_SceneNode_hpp_
#define __GTEngine_Scripting_SceneNode_hpp_

#include "../GameScript.hpp"
#include "../SceneNode.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the scene node library.
        ///
        /// @param script [in] A reference to the script to load the library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        bool LoadSceneNodeLibrary(GTCore::Script &script);



        /// Instantiates a scene node on it's owner scene.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node being instantiated.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered.
        void InstantiateSceneNode(GTCore::Script &script, SceneNode &sceneNode);

        /// Uninstantiates the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node being uninstantiated.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered.
        void UninstantiateSceneNode(GTCore::Script &script, SceneNode &sceneNode);

        /// Updates the public variables of the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node whose public variables are being updated.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered and the given scene node has a script component.
        void UpdatePublicVariables(GTCore::Script &script, SceneNode &sceneNode);

        /// Updates the Lua representation of a registered scene node so that it is synced with the current state of the attached script component.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node whose public variables are being updated.
        ///
        /// @remarks
        ///     This is only performed if the owner scene is registered and the given scene node has a script component.
        ///     @par
        ///     This does not change the state of any variables.
        void RegisterScriptComponent(GTCore::Script &script, SceneNode &sceneNode);

        /// Pushes a scene node object to the top of the stack.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node whose Lua counterpart is being pushed.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        ///     @par
        ///     The scene node will need to be popped when no longer needed.
        ///     @par
        ///     This function asserts that the scene node has an OnUpdate function.
        void PushSceneNode(GTCore::Script &script, SceneNode &sceneNode);


        /// Calls the OnUpdate event on the given scene node.
        ///
        /// @param script             [in] A reference to the main script object.
        /// @param sceneNode          [in] A reference to the scene node having it's event called.
        /// @param deltaTimeInSeconds [in] The time since the last update. Used for time-based operations.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnUpdate(GTCore::Script &script, SceneNode &sceneNode, double deltaTimeInSeconds);

        /// Calls the OnStartup event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnStartup(GTCore::Script &script, SceneNode &sceneNode);

        /// Calls the OnShutdown event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnShutdown(GTCore::Script &script, SceneNode &sceneNode);

        /// Calls the OnShow event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnShow(GTCore::Script &script, SceneNode &sceneNode);

        /// Calls the OnHide event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnHide(GTCore::Script &script, SceneNode &sceneNode);

        /// Calls the OnObjectEnter event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnObjectEnter(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

        /// Calls the OnEnterObject event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnEnterObject(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

        /// Calls the OnObjectLeave event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnObjectLeave(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);

        /// Calls the OnLeaveObject event on the given scene node.
        ///
        /// @param script    [in] A reference to the main script object.
        /// @param sceneNode [in] A reference to the scene node having it's event called.
        ///
        /// @remarks
        ///     The scene node must be instantiated on a registered scene for this to work.
        void PostSceneNodeEvent_OnLeaveObject(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode);



        namespace SceneNodeFFI
        {
            /// Creates a new empty scene node, returning a pointer to it.
            int Create(GTCore::Script &script);

            /// Deletes a scene node that was created with CreateSceneNode(GTCore::Script &).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node returned by CreateSceneNode(GTCore::Script &).
            int Delete(GTCore::Script &script);


            /// Retrieves the ID of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose ID is being retrieved.
            ///
            ///     An important note. In the C++ side, the ID is 64-bits. On the Lua side, it is 32-bits.
            int GetID(GTCore::Script &script);


            /// Retrieves the name of the given component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose name is being retrieved.
            int GetName(GTCore::Script &script);

            /// Sets the name of the given component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose name is being set.
            ///     Argument 2: The new name for the scene node.
            int SetName(GTCore::Script &script);


            /// Retrieves a pointer to the parent scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose parent is being retrieved.
            int GetParentPtr(GTCore::Script &script);

            /// Attaches a scene node to the given node as a child.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose having a child attached.
            ///     Argument 2: A pointer to the child node.
            ///     Argument 3: A boolean controlling whether or not the world transformation should remain constant. 'true' = keep the world transform.
            int AttachChild(GTCore::Script &script);

            /// Orphans a scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node being orphaned.
            ///     Argument 2: A boolean controlling whether or not the world transformation should remain constant. 'true' = keep the world transform.
            int Orphan(GTCore::Script &script);

            /// Determines whether or not the given scene node is an ancestor of another.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the main scene node.
            ///     Argument 2: A pointer to the ancestor scene node.
            int IsAncestor(GTCore::Script &script);

            /// Determines whether or not the given scene node is a descendant of another.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the main scene node.
            ///     Argument 2: A pointer to the descendant scene node.
            int IsDescendant(GTCore::Script &script);

            /// Determines whether or not the given scene node is related to another.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the main scene node.
            ///     Argument 2: A pointer to the relative scene node.
            int IsRelative(GTCore::Script &script);


            /// Retrieves a table containing the IDs of the child nodes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node in question.
            int GetChildrenIDs(GTCore::Script &script);


            /// Adds a new component to the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose having the component added to it.
            ///     Argument 2: The name of the component to add, as a string.
            ///     @par
            ///     Returns a table representing the new component.
            int AddComponent(GTCore::Script &script);

            /// Removes a component from the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose having the component removed.
            ///     Argument 2: The name of the componet to remove, as a string.
            int RemoveComponent(GTCore::Script &script);

            /// Retrieves a component from the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose having a component retrieved.
            ///     Argument 2: The name of the component to retrieve, as a string.
            ///     @par
            ///     This will instantiate a new table every time it is called. If the given scene node does not have a component of the given name, this will return nil.
            int GetComponent(GTCore::Script &script);


            /// Retrieves the local position of the given scene node as 3 return values.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose position is being retrieved.
            int GetPosition(GTCore::Script &script);

            /// Retrieves the world position of the given scene node as 3 floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose world position is being retrieved.
            int GetWorldPosition(GTCore::Script &script);

            /// Sets the local position of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose position is being set.
            ///     Argument 2: The x position.
            ///     Argument 3: The y position.
            ///     Argument 4: The z position.
            int SetPosition(GTCore::Script &script);

            /// Sets the world position of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose position is being set.
            ///     Argument 2: The x position.
            ///     Argument 3: The y position.
            ///     Argument 4: The z position.
            int SetWorldPosition(GTCore::Script &script);


            /// Retrieves the local XYZ rotation of the given scene node as 3 return values.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose rotation is being retrieved.
            int GetRotationXYZ(GTCore::Script &script);

            /// Retrieves the world XYZ rotation of the given scene node as 3 return values.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose world rotation is being retrieved.
            int GetWorldRotationXYZ(GTCore::Script &script);

            /// Sets the local rotation of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose rotation is being set.
            ///     Argument 2: The x rotation.
            ///     Argument 3: The y rotation.
            ///     Argument 4: The z rotation.
            int SetRotationXYZ(GTCore::Script &script);

            /// Sets the world rotation of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose rotation is being set.
            ///     Argument 2: The x rotation.
            ///     Argument 3: The y rotation.
            ///     Argument 4: The z rotation.
            int SetWorldRotationXYZ(GTCore::Script &script);


            /// Retrieves the local scale of the given scene node as 3 return values.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose scale is being retrieved.
            int GetScale(GTCore::Script &script);

            /// Retrieves the world scale of the given scene node as 3 return values.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose scale is being retrieved.
            int GetWorldScale(GTCore::Script &script);

            /// Sets the local scale of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose scale is being set.
            ///     Argument 2: The x scale.
            ///     Argument 3: The y scale.
            ///     Argument 4: The z scale.
            int SetScale(GTCore::Script &script);

            /// Sets the world scale of the given scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose scale is being set.
            ///     Argument 2: The x scale.
            ///     Argument 3: The y scale.
            ///     Argument 4: The z scale.
            int SetWorldScale(GTCore::Script &script);


            /// Returns a table containing the component IDs (names) that are attached to the given node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node whose component IDs are being retrieved.
            int GetAttachedComponentIDs(GTCore::Script &script);


            /// Shows the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int Show(GTCore::Script &script);

            /// Hides the scene node.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int Hide(GTCore::Script &script);

            /// Determines whether or not the node is visible.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int IsVisible(GTCore::Script &script);



            /// Retrieves a pointer to the scene that this node is part of, if any.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the scene node.
            int GetScenePtr(GTCore::Script &script);




            ////////////////////////////////////////////////////////////////
            // Functions below are just helpers.

            /// A helper function for pushing a new instantiation of script-side component object.
            ///
            /// @param script             [in] The script where the result will be pushed to.
            /// @param componentClassName [in] The class name of the component being pushed.
            /// @param component          [in] A pointer to the C++ component.
            void PushComponent(GTCore::Script &script, const char* componentClassName, Component* component);
        }
    }
}

#endif