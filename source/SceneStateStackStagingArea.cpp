// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/SceneStateStackStagingArea.hpp>
#include <GTGE/SceneStateStackBranch.hpp>
#include <GTGE/Scene.hpp>
#include <GTGE/GTEngine.hpp>

namespace GT
{
    SceneStateStackStagingArea::SceneStateStackStagingArea(SceneStateStackBranch &branchIn)
        : branch(branchIn), inserts(), deletes(), updates(), hierarchy()
    {
    }

    SceneStateStackStagingArea::~SceneStateStackStagingArea()
    {
        this->Clear();
    }


    Scene & SceneStateStackStagingArea::GetScene()
    {
        return this->branch.GetScene();
    }

    const Scene & SceneStateStackStagingArea::GetScene() const
    {
        return this->branch.GetScene();
    }


    void SceneStateStackStagingArea::StageInsert(uint64_t sceneNodeID)
    {
        // If a delete command with the scene node is already staged, what we actually want to do is remove the delete command and
        // turn the insert into an update.
        auto iDelete = this->deletes.Find(sceneNodeID);
        if (iDelete != nullptr)
        {
            // Remove the delete command.
            delete iDelete->value;
            this->deletes.RemoveByIndex(iDelete->index);

            // Convert to an update command.
            this->inserts.RemoveFirstOccuranceOf(sceneNodeID);
            this->StageUpdate(sceneNodeID);
        }
        else
        {
            // If the scene node is in the updates list we need to remove it.
            this->updates.RemoveFirstOccuranceOf(sceneNodeID);

            if (!this->inserts.Exists(sceneNodeID))
            {
                this->inserts.PushBack(sceneNodeID);
                this->AddToHierarchy(sceneNodeID);
            }
        }
    }

    void SceneStateStackStagingArea::StageDelete(uint64_t sceneNodeID)
    {
        // If an insert command with the scene node is already staged, all we want to do is remove it from the inserts and just
        // ignore everything.
        size_t index;
        if (this->inserts.FindFirstIndexOf(sceneNodeID, index))
        {
            this->inserts.Remove(index);
            this->RemoveFromHierarchy(sceneNodeID);
        }
        else
        {
            // If the scene node is in the updates list we need to remove it.
            this->updates.RemoveFirstOccuranceOf(sceneNodeID);

            if (!this->deletes.Exists(sceneNodeID))
            {
                auto sceneNode = this->GetScene().GetSceneNodeByID(sceneNodeID);
                assert(sceneNode != nullptr);
                {
                    auto sceneNodeSerializer = new BasicSerializer;
                    sceneNode->Serialize(*sceneNodeSerializer, this->branch.GetStateStack().GetSceneNodeSerializationFlags());

                    this->deletes.Add(sceneNodeID, sceneNodeSerializer);
                    this->AddToHierarchy(sceneNodeID);
                }
            }
        }
    }

    void SceneStateStackStagingArea::StageUpdate(uint64_t sceneNodeID)
    {
        // We ignore update commands if an insert or delete command is already present.
        if (!this->inserts.Exists(sceneNodeID) &&
            !this->deletes.Exists(sceneNodeID) &&
            !this->updates.Exists(sceneNodeID))
        {
            this->updates.PushBack(sceneNodeID);
        }

        // Always make sure the hierarchy is updated.
        this->AddToHierarchy(sceneNodeID);
    }


    void SceneStateStackStagingArea::Clear()
    {
        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            delete this->deletes.buffer[i]->value;
        }

        this->inserts.Clear();
        this->deletes.Clear();
        this->updates.Clear();

        this->hierarchy.Clear();
    }


    void SceneStateStackStagingArea::GetRevertCommands(SceneStateStackRestoreCommands &commands)
    {
        // TODO: Investigate why I'm not using commands.AddInsert(), .AddDelete() and .AddUpdate().

        // We need to do opposites. Inserts become deletes, deletes become inserts and updates are back traced until we find the most recent one.
        
        // Inserts
        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            auto sceneNodeID         = this->inserts[i];
            commands.deletes.Add(sceneNodeID, nullptr);
        }

        // Deletes
        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto sceneNodeID         = this->deletes.buffer[i]->key;
            auto sceneNodeSerializer = this->deletes.buffer[i]->value;
            commands.inserts.Add(sceneNodeID, sceneNodeSerializer);
        }

        // Updates
        for (size_t i = 0; i < this->updates.count; ++i)
        {
            auto sceneNodeID         = this->updates[i];
            auto sceneNodeSerializer = this->branch.FindMostRecentSerializer(sceneNodeID, this->branch.GetCurrentFrameIndex());
            commands.updates.Add(sceneNodeID, sceneNodeSerializer);
        }


        // Hierarchy
        for (size_t i = 0; i < this->hierarchy.count; ++i)
        {
            auto sceneNodeID       = this->hierarchy.buffer[i]->key;
            auto parentSceneNodeID = this->branch.FindMostRecentParentSceneNodeID(sceneNodeID, this->branch.GetCurrentFrameIndex());

            if (!commands.deletes.Exists(sceneNodeID))
            {
                commands.hierarchy.Add(sceneNodeID, parentSceneNodeID);
            }
        }
    }

    void SceneStateStackStagingArea::GetRestoreCommands(SceneStateStackRestoreCommands &commands)
    {
        // Inserts
        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            auto sceneNodeID       = this->inserts.buffer[i];
            auto parentSceneNodeID = this->GetParentSceneNodeIDFromHierarchy(sceneNodeID);
            commands.AddInsert(sceneNodeID, parentSceneNodeID, this->GetScene());
        }

        // Deletes
        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto sceneNodeID = this->deletes.buffer[i]->key;
            auto parentSceneNodeID = this->GetParentSceneNodeIDFromHierarchy(sceneNodeID);
            commands.AddDelete(sceneNodeID, parentSceneNodeID, nullptr, nullptr);
        }

        // Updates
        for (size_t i = 0; i < this->updates.count; ++i)
        {
            auto sceneNodeID = this->updates.buffer[i];
            auto parentSceneNodeID = this->GetParentSceneNodeIDFromHierarchy(sceneNodeID);
            commands.AddUpdate(sceneNodeID, parentSceneNodeID, this->GetScene());
        }
    }




    /////////////////////////////////////////////////
    // Serialization/Deserialization

    void SceneStateStackStagingArea::Serialize(Serializer &serializer) const
    {
        // We need to use an intermediary serializer to get an accurate size.
        BasicSerializer intermediarySerializer;


        // Inserts.
        intermediarySerializer.Write(static_cast<uint32_t>(this->inserts.count));

        for (size_t i = 0; i < this->inserts.count; ++i)
        {
            intermediarySerializer.Write(this->inserts[i]);
        }


        // Deletes.
        intermediarySerializer.Write(static_cast<uint32_t>(this->deletes.count));

        for (size_t i = 0; i < this->deletes.count; ++i)
        {
            auto sceneNodeID         = this->deletes.buffer[i]->key;
            auto sceneNodeSerializer = this->deletes.buffer[i]->value;

            intermediarySerializer.Write(sceneNodeID);

            intermediarySerializer.Write(static_cast<uint32_t>(sceneNodeSerializer->GetBufferSizeInBytes()));
            intermediarySerializer.Write(sceneNodeSerializer->GetBuffer(), sceneNodeSerializer->GetBufferSizeInBytes());
        }


        // Updates.
        intermediarySerializer.Write(static_cast<uint32_t>(this->updates.count));

        for (size_t i = 0; i < this->updates.count; ++i)
        {
            intermediarySerializer.Write(this->updates[i]);
        }



        // Hierarchy.
        intermediarySerializer.Write(static_cast<uint32_t>(this->hierarchy.count));

        for (size_t i = 0; i < this->hierarchy.count; ++i)
        {
            intermediarySerializer.Write(this->hierarchy.buffer[i]->key);
            intermediarySerializer.Write(this->hierarchy.buffer[i]->value);
        }



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SceneStateStackStagingArea;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), intermediarySerializer.GetBufferSizeInBytes());
    }

    void SceneStateStackStagingArea::Deserialize(Deserializer &deserializer)
    {
        // We should clear the staging area just in case.
        this->Clear();


        Serialization::ChunkHeader header;
        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneStateStackStagingArea);
            {
                switch (header.version)
                {
                case 1:
                    {
                        // Inserts.
                        uint32_t insertsCount;
                        deserializer.Read(insertsCount);

                        for (uint32_t i = 0; i < insertsCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            this->inserts.PushBack(sceneNodeID);
                        }


                        // Deletes.
                        uint32_t deletesCount;
                        deserializer.Read(deletesCount);

                        for (uint32_t i = 0; i < deletesCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            // The next chunk of data is the serialized data of the scene node. What we do here is ready the data into a temp buffer, and then
                            // write that to a new BasicSerializer object.
                            uint32_t serializerSizeInBytes;
                            deserializer.Read(serializerSizeInBytes);

                            void* serializerData = malloc(serializerSizeInBytes);
                            deserializer.Read(serializerData, serializerSizeInBytes);


                            auto sceneNodeSerializer = new BasicSerializer;
                            sceneNodeSerializer->Write(serializerData, serializerSizeInBytes);


                            this->deletes.Add(sceneNodeID, sceneNodeSerializer);


                            free(serializerData);
                        }


                        // Updates.
                        uint32_t updatesCount;
                        deserializer.Read(updatesCount);

                        for (uint32_t i = 0; i < updatesCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            this->updates.PushBack(sceneNodeID);
                        }



                        // Hierarchy.
                        uint32_t hierarchyCount;
                        deserializer.Read(hierarchyCount);

                        for (uint32_t i = 0; i < hierarchyCount; ++i)
                        {
                            uint64_t sceneNodeID;
                            deserializer.Read(sceneNodeID);

                            uint64_t parentSceneNodeID;
                            deserializer.Read(parentSceneNodeID);

                            this->hierarchy.Add(sceneNodeID, parentSceneNodeID);
                        }


                        break;
                    }


                default:
                    {
                        g_Context->Logf("Error deserializing SceneStateStackStagingArea. The main chunk is an unsupported version (%d).", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        break;
                    }
                }
            }
        }
    }


    ////////////////////////////////////////
    // Private

    void SceneStateStackStagingArea::AddToHierarchy(uint64_t sceneNodeID)
    {
        auto &scene = this->GetScene();

        auto sceneNode = scene.GetSceneNodeByID(sceneNodeID);
        assert(sceneNode != nullptr);
        {
            auto parentSceneNode = sceneNode->GetParent();
            if (parentSceneNode != nullptr)
            {
                this->hierarchy.Add(sceneNodeID, parentSceneNode->GetID());
            }
            else
            {
                this->hierarchy.Add(sceneNodeID, 0);            // 0 = no parent.
            }
        }
    }

    void SceneStateStackStagingArea::RemoveFromHierarchy(uint64_t sceneNodeID)
    {
        this->hierarchy.RemoveByKey(sceneNodeID);
    }

    uint64_t SceneStateStackStagingArea::GetParentSceneNodeIDFromHierarchy(uint64_t childSceneNodeID) const
    {
        auto iHierarchy = this->hierarchy.Find(childSceneNodeID);
        if (iHierarchy != nullptr)
        {
            return iHierarchy->value;
        }

        return 0;
    }
}
