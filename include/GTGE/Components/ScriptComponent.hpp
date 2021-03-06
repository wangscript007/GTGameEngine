// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ScriptComponent
#define GT_ScriptComponent

#include "../Component.hpp"
#include "../ScriptDefinition.hpp"
#include <GTGE/Core/Vector.hpp>

namespace GT
{
    class ScriptComponent : public Component
    {
    public:

        /// Constructor.
        ScriptComponent(SceneNode &node);

        /// Destructor.
        ~ScriptComponent();



        /// Adds a script by it's file name.
        ///
        /// @param relativePath [in] The relative path of the file to add.
        ///
        /// @remarks
        ///     If the script is already loaded, this will do nothing and return the existing definition.
        ///     @par
        ///     If the script can not be loaded the component will be left unmodified and null will be returned.
        ScriptDefinition* AddScript(const char* relativePath, bool dontPostOnChange = false);

        /// Removes a script by it's file name.
        ///
        /// @param relativePath [in] The relative path of the file to remove.
        void RemoveScriptByRelativePath(const char* relativePath);

        /// Removes a script by it's index.
        ///
        /// @param index [in] The index of the script to remove.
        void RemoveScriptByIndex(size_t index);

        /// Reloads a script at the given index.
        ///
        /// @param index [in] The index of the item being reloaded.
        void ReloadScript(size_t index);



        /// Retrieves the number of scripts used by this component.
        size_t GetScriptCount() const;

        /// Retrieves the definition of the script at the given index.
        ///
        /// @param index [in] The index of the script definition to retrieve.
        ScriptDefinition* GetScriptDefinitionByIndex(size_t index) const;


        /// Retrieves the definition of the script by the absolute path.
        ScriptDefinition* GetScriptDefinitionByAbsolutePath(const char* absolutePath, size_t &indexOut);
        ScriptDefinition* GetScriptDefinitionByAbsolutePath(const char* absolutePath)
        {
            size_t devnull;
            return this->GetScriptDefinitionByAbsolutePath(absolutePath, devnull);
        }

        /// Retrieves the definition of the script by the relative path.
        ScriptDefinition* GetScriptDefinitionByRelativePath(const char* relativePath, size_t &indexOut);
        ScriptDefinition* GetScriptDefinitionByRelativePath(const char* relativePath)
        {
            size_t devnull;
            return this->GetScriptDefinitionByRelativePath(relativePath, devnull);
        }

        

        /// Retrieves the absolute path of the script at the given index.
        const char* GetScriptAbsolutePathByIndex(size_t index) const;

        /// Retrieves the relative path of the script at the given index.
        const char* GetScriptRelativePathByIndex(size_t index) const;



        /// Determines if any of the attached scripts has an OnUpdate event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs an OnUpdate event posted to the scene node.
        bool HasOnUpdate() const;

        /// Determines if any of the attached scripts has an OnStartup event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnStartup() const;

        /// Determines if any of the attached scripts has an OnStartup event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnShutdown() const;

        /// Determines if any of the attached scripts has an OnShow event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnShow() const;

        /// Determines if any of the attached scripts has an OnShow event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnHide() const;

        /// Determines if any of the attached scripts has an OnSceneNodeEnter event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnSceneNodeEnter() const;

        /// Determines if any of the attached scripts has an OnEnterSceneNode event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnEnterSceneNode() const;

        /// Determines if any of the attached scripts has an OnSceneNodeLeave event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnSceneNodeLeave() const;

        /// Determines if any of the attached scripts has an OnLeaveSceneNode event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnLeaveSceneNode() const;

        /// Determines if any of the attached scripts has an OnMouseMove event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnMouseMove() const;

        /// Determines if any of the attached scripts has an OnMouseWheel event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnMouseWheel() const;

        /// Determines if any of the attached scripts has an OnMouseButtonDown event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnMouseButtonDown() const;

        /// Determines if any of the attached scripts has an OnMouseButtonUp event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnMouseButtonUp() const;

        /// Determines if any of the attached scripts has an OnMouseButtonDoubleClick event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnMouseButtonDoubleClick() const;

        /// Determines if any of the attached scripts has an OnKeyPressed event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnKeyPressed() const;

        /// Determines if any of the attached scripts has an OnKeyReleased event handler.
        ///
        /// @remarks
        ///     This is used to help the owner scene decide whether or not the scene node needs the event posted to the scene node.
        bool HasOnKeyReleased() const;

        /// Determines if any of the attached scripts has an OnSerialize event handler.
        ///
        /// @remarks
        ///     This is used to determine whether or not OnSerialize needs to be called.
        bool HasOnSerialize() const;

        /// Determines if any of the attached scripts has an OnDeserialize event handler.
        ///
        /// @remarks
        ///     This is used to determine whether or not OnDeserialize needs to be called.
        bool HasOnDeserialize() const;

        /// Determines if any of the attached scripts has an OnSerializeGlobalData event handler.
        ///
        /// @remarks
        ///     This is used to determine whether or not OnSerializeGlobalData needs to be called.
        bool HasOnSerializeGlobalData() const;

        /// Determines if any of the attached scripts has an OnDeserializeGlobalData event handler.
        ///
        /// @remarks
        ///     This is used to determine whether or not OnDeserializeGlobalData needs to be called.
        bool HasOnDeserializeGlobalData() const;

        /// Determines if any of the attached scripts has an OnGamePause event handler.
        ///
        /// @remarks
        ///     This is used to determine whether or not OnGamePause needs to be called.
        bool HasOnGamePause() const;

        /// Determines if any of the attached scripts has an OnGameResume event handler.
        ///
        /// @remarks
        ///     This is used to determine whether or not OnGameResume needs to be called.
        bool HasOnGameResume() const;



        /// Clears the component of all scripts.
        void Clear(bool clearPublicVariables = true);


        /// Sets the internal flag keeping track of whether of not the OnStartup event has been called.
        void MarkOnStartupAsCalled();

        /// Unsets the internal flag keeping track of whether or not the OnStartup event has been called.
        void UnmarkOnStartupAsCalled();

        /// Determines whether or not the internal flag keeping track of whether or not the OnStartup event has been called is set.
        bool HasOnStartupBeenCalled() const;



        /// Retrieves the number of public variables.
        size_t GetPublicVariableCount() const;

        /// Retrieves a public variable by it's index.
        ///
        /// @param index [in] The index of the variable to retrieve.
        ScriptVariable* GetPublicVariableByIndex(size_t index) const;


        /// Retrieves a pointer to a public variable by it's name.
        ///
        /// @param name [in] The name of the variable to retrieve.
        ///
        /// @remarks
        ///     This will return null if the variable does not exist.
        ///     @par
        ///     This runs in O(n) time.
        ScriptVariable* GetPublicVariableByName(const char* name, size_t &indexOut) const;
        ScriptVariable* GetPublicVariableByName(const char* name) const
        {
            size_t devnull;
            return this->GetPublicVariableByName(name, devnull);
        }


        /// Set the value of a public variable.
        ///
        /// @param name [in] The name of the variable being set.
        void SetPublicVariableValue(const char* variableName, double x);
        void SetPublicVariableValue(const char* variableName, double x, double y);
        void SetPublicVariableValue(const char* variableName, double x, double y, double z);
        void SetPublicVariableValue(const char* variableName, double x, double y, double z, double w);
        void SetPublicVariableValue(const char* variableName, bool value);
        void SetPublicVariableValue(const char* variableName, const char* value);


        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(Serializer &serializer) const { this->Serialize(serializer, 0); }
        void Serialize(Serializer &serializer, unsigned int flags) const;

        /// Component::Deserialize()
        void Deserialize(Deserializer &deserializer, bool noPublicVariableOverride, bool noOnDeserialize);
        void Deserialize(Deserializer &deserializer) { this->Deserialize(deserializer, false, false); }



    private:

        /// Merges the variables from the given script definition into our own.
        void MergePublicVariables(ScriptDefinition &definition);

        /// Removes the public variables from the given script definition.
        void RemovePublicVariables(ScriptDefinition &definition);

        /// Removes a public variable by it's name.
        void RemovePublicVariableByName(const char* name);

        /// Removes a public variable by it's index.
        void RemovePublicVariableByIndex(size_t index);

        /// Removes all public variables that are no longer contained in the attached scripts.
        void RemoveUnusedPublicVariables();

        /// Determines if the given variable exists in a definition other than the one specified.
        bool DoesPublicVariableExistInOtherDefinition(const char* name, ScriptDefinition &definitionToExclude);

        /// Determiens if the given variables exists in any attached definition.
        bool DoesPublicVariableExistInAnyDefinition(const char* name);


    private:

        /// The list of scripts attached to the component.
        Vector<ScriptDefinition*> scripts;

        /// The list of relative paths of the scripts attached to this component. This maps to an item in "scripts" at the same location.
        Vector<String> scriptRelativePaths;

        /// We keep a list of every public variable here. We keep these separate from the definitions because we want to allow these to have
        /// their own values.
        Vector<ScriptVariable*> publicVariables;

        /// This keeps track of whether or not the OnStartup event has been called. When OnStartup is called, this will be set to true. When
        /// OnShutdown has been called, it will revert back to false. We use this for keeping track of whether or not OnStartup and OnShutdown
        /// events should be called in certain situations. We only ever want one OnShutdown for every OnStartup and vice versa.
        bool hasOnStartupBeenCalled;



        GTENGINE_DECL_COMPONENT_ATTRIBS(ScriptComponent)
    };
}

#endif
