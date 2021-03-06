// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SceneStateStackRestoreCommands
#define GT_SceneStateStackRestoreCommands

#include <GTGE/Core/Vector.hpp>
#include <GTGE/Core/Map.hpp>
#include <GTGE/Core/Serializer.hpp>
#include <GTGE/Core/Deserializer.hpp>

namespace GT
{
    class Scene;
    class SceneStateStackBranch;


    /// Simple structure containing the commands that need to be executed in order to restore a scene to a different state.
    struct SceneStateStackRestoreCommands
    {
        struct Command
        {
            Command(BasicSerializer* serializerIn, bool isOwnerIn = false)
                : serializer(serializerIn), isOwner(isOwnerIn)
            {
            }


            /// A pointer to the serializer that contains the scene node's data.
            BasicSerializer* serializer;

            /// Keeps track of whether or not the serializer is memory managed by the commands structure.
            bool isOwner;
        };



        /// Constructor.
        SceneStateStackRestoreCommands(unsigned int serializationFlags, unsigned int deserializationFlags);

        /// Destructor.
        ~SceneStateStackRestoreCommands();


        /// Adds an insert command.
        void AddInsert(uint64_t sceneNodeID, uint64_t parentSceneNodeID, Scene* scene, BasicSerializer* sceneNodeSerializer);
        void AddInsert(uint64_t sceneNodeID, uint64_t parentSceneNodeID, Scene &scene, BasicSerializer* sceneNodeSerializer = nullptr) { this->AddInsert(sceneNodeID, parentSceneNodeID, &scene, sceneNodeSerializer);  }
        void AddInsert(uint64_t sceneNodeID, uint64_t parentSceneNodeID, BasicSerializer* sceneNodeSerializer)                         { this->AddInsert(sceneNodeID, parentSceneNodeID, nullptr, sceneNodeSerializer); }
        
        /// Adds a delete command.
        void AddDelete(uint64_t sceneNodeID, uint64_t parentSceneNodeID, Scene* scene, BasicSerializer* sceneNodeSerializer);
        void AddDelete(uint64_t sceneNodeID, uint64_t parentSceneNodeID, Scene &scene, BasicSerializer* sceneNodeSerializer = nullptr) { this->AddDelete(sceneNodeID, parentSceneNodeID, &scene, sceneNodeSerializer);  }
        void AddDelete(uint64_t sceneNodeID, uint64_t parentSceneNodeID, BasicSerializer* sceneNodeSerializer)                         { this->AddDelete(sceneNodeID, parentSceneNodeID, nullptr, sceneNodeSerializer); }

        /// Adds an update command.
        void AddUpdate(uint64_t sceneNodeID, uint64_t parentSceneNodeID, Scene* scene, BasicSerializer* sceneNodeSerializer);
        void AddUpdate(uint64_t sceneNodeID, uint64_t parentSceneNodeID, Scene &scene, BasicSerializer* sceneNodeSerializer = nullptr) { this->AddUpdate(sceneNodeID, parentSceneNodeID, &scene, sceneNodeSerializer);  }
        void AddUpdate(uint64_t sceneNodeID, uint64_t parentSceneNodeID, BasicSerializer* sceneNodeSerializer)                         { this->AddUpdate(sceneNodeID, parentSceneNodeID, nullptr, sceneNodeSerializer); }

        
        /// Adds a scene node to the hierarchy.
        void AddToHierarchy(uint64_t sceneNodeID, uint64_t parentSceneNodeID);

        /// Removes a scene node from the hierarchy.
        void RemoveFromHierarchy(uint64_t sceneNodeID);


        /// Goes through every command and updates the serialized data with the most recent data, starting from the current frame on the given branch.
        void UpdateToMostRecentSerializers(SceneStateStackBranch &branch, uint32_t startFrameIndex);

        /// Goes through every scene node in the hierarchy and updates it to the most recent values.
        void UpdateToMostRecentHierarchy(SceneStateStackBranch &branch, uint32_t startFrameIndex);


        /// Clears the commands.
        void Clear();
        
        /// Executes the commands on the given scene.
        void Execute(Scene &scene);



        ///////////////////////////////////////////////////////
        // Serialization / Deserialization

        /// Serializes the commands.
        ///
        /// @param serializer [in] A reference to the serializer to write the data to.
        ///
        /// @remarks
        ///     This does not modify the scene, but rather simply allows the structure itself to be saved and restored.
        void Serialize(Serializer &serializer) const;

        /// Deserializes the commands.
        ///
        /// @param deserializer [in] A reference to the deserializer to read the data from.
        ///
        /// @remarks
        ///     After deserializing, the internal serializers are all owned and memory managed by the structure itself.
        void Deserialize(Deserializer &deserializer);



        /// The insert commands.
        Map<uint64_t, Command> inserts;

        /// The deletes commands.
        Map<uint64_t, Command> deletes;

        /// The update commands.
        Map<uint64_t, Command> updates;


        /// The hierarchy.
        Map<uint64_t, uint64_t> hierarchy;


        /// The flags to use when serializing scene nodes.
        unsigned int serializationFlags;

        /// The flags to use when deserializing scene nodes.
        unsigned int deserializationFlags;
    };
}

#endif
