// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_PrefabComponent
#define GT_PrefabComponent

#include "../Component.hpp"
#include "../Core/String.hpp"
#include <cstdint>

namespace GT
{
    /// A special component used for helping synchronize with prefabs.
    class PrefabComponent : public Component
    {
    public:

        /// Constructor.
        PrefabComponent(SceneNode &node);

        /// Destructor.
        ~PrefabComponent();


        /// Retrieves the absolute path of the prefab the scene node is linked to.
        const char* GetPrefabAbsolutePath() const;


        /// Retrieves the relative path of the prefab the scene node is linked to.
        ///
        /// @return The relative path of the prefab the scene node is linked to.
        const char* GetPrefabRelativePath() const;

        /// Sets the relative path the scene node is linked to.
        ///
        /// @param relativePath [in] The relative path of the prefab the scene node is linked to.
        void SetPrefabRelativePath(const char* relativePath);


        /// Retrieves the local hierarchy ID of the scene node.
        uint64_t GetLocalHierarchyID() const;

        /// Sets the local hirerachy ID of the scene node.
        ///
        /// @param id [in] The new local hierarchy ID.
        void SetLocalHierarchyID(uint64_t id);
        
        
        /// A helper method for finding the scene node that roots the prefab.
        SceneNode & GetRootSceneNode();



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(Deserializer &deserializer);



    private:

        /// The absolute path of the prefab the scene node is linked to. This is not serialized and is set based on the
        /// relative path set by SetPrefabRelativePath().
        String prefabAbsolutePath;

        /// The relative path of the prefab the scene node is linked to.
        String prefabRelativePath;

        /// The local hierarchy ID of the scene node. This maps to an ID from the prefab.
        uint64_t localHierarchyID;



        GTENGINE_DECL_COMPONENT_ATTRIBS(PrefabComponent)
    };
}

#endif
