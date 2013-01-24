
#include <GTEngine/Editor/SceneEditor/SceneEditor.hpp>
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/Logging.hpp>
#include <GTCore/Serializer.hpp>
#include <GTCore/Deserializer.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    // TODO: Make this a script variable so we can change this around.
    static const size_t MaxStateStackFrames = 20;


    SceneEditor::SceneEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          viewport(), camera(), cameraXRotation(0.0f), cameraYRotation(0.0f),
          updateManager(camera), physicsManager(), cullingManager(),
          scene(updateManager, physicsManager, cullingManager), sceneEventHandler(*this),
          viewportEventHandler(ownerEditor.GetGame(), viewport),
          sceneNodes(), nextSceneNodeID(0),
          selectedNodes(),
          pickingWorld(),
          transformGizmo(), gizmoDragAxis(1.0f, 0.0f, 0.0f), gizmoDragFactor(1.0f, 0.0f),
          gizmoDragMode(GizmoDragMode_None), gizmoTransformMode(GizmoTransformMode_Translate), gizmoTransformSpace(GizmoTransformSpace_Global),
          snapTranslation(), snapAngle(0.0f), snapScale(), isSnapping(false),
          translateSnapSize(0.25f), rotateSnapSize(5.625f), scaleSnapSize(0.25f),
          transformedObjectWithGizmo(false),
          simulationSerializer(), transformationSerializer(),
          sceneStateStack(), sceneStateIndex(0),
          isDeserializing(false),
          GUI()
    {
        this->scene.AttachEventHandler(this->sceneEventHandler);


        auto &gui    = this->GetGUI();
        auto &script = this->GetScript();



        // Here we'll setup the built-in nodes.
        this->camera.AddComponent<GTEngine::CameraComponent>();
        this->camera.AddComponent<GTEngine::AmbientLightComponent>()->SetColour(0.0f, 0.0f, 0.0f);
        this->camera.AddComponent<GTEngine::EditorMetadataComponent>();
        this->camera.SetDataPointer(0, this);
        this->camera.DisableSerialization();
        this->camera.DisableStateStackStaging();


        this->viewport.SetCameraNode(this->camera);
        this->scene.AddViewport(this->viewport);
        this->scene.GetRenderer().EnableBackgroundColourClearing(0.5f, 0.5f, 0.5f);

        this->scene.AddSceneNode(this->camera);

        this->transformGizmo.Initialise();
        this->transformGizmo.GetSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetXArrowSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetYArrowSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetZArrowSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetXCircleSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetYCircleSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetZCircleSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetCameraFacingCircleSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetXScaleSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetYScaleSceneNode().SetDataPointer(0, this);
        this->transformGizmo.GetZScaleSceneNode().SetDataPointer(0, this);

        this->scene.AddSceneNode(this->transformGizmo.GetSceneNode());
        this->transformGizmo.Hide();



        // We need to load the scene.
        auto file = GTCore::IO::Open(absolutePath, GTCore::IO::OpenMode::Read);
        if (file != nullptr)
        {
            // We need to now create the GUI elements for this particular file. We start with the main element.
            this->GUI.Main = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='scene-editor-main' />");
            assert(this->GUI.Main != nullptr);
            {
                // The main element has been created, but we need to run a script to have it turn it into a proper SceneEditor object.
                script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
                assert(script.IsTable(-1));
                {
                    script.Push("SceneEditor");
                    script.GetTableValue(-2);
                    assert(script.IsFunction(-1));
                    {
                        script.PushValue(-2);   // <-- 'self'.
                        script.Push(this);      // <-- '_internalPtr'
                        script.Call(2, 0);

                        // At this point we will have the scene editor initialized in the scripting environment, and is sitting at the top of the stack. So now we
                        // need to retrieve pointers to some of it's objects.
                        script.Push("Viewport");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("GetID");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);   // <-- 'self'
                                script.Call(1, 1);
                                assert(script.IsString(-1));
                                {
                                    this->GUI.Viewport = gui.GetElementByID(script.ToString(-1));

                                    this->GUI.Viewport->AttachEventHandler(this->viewportEventHandler);
                                    this->GUI.Viewport->OnSize();
                                }
                                script.Pop(1);          // <-- return value from GetID()
                            }
                        }
                        script.Pop(1);

                        script.Push("Panel");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.Push("GetID");
                            script.GetTableValue(-2);
                            assert(script.IsFunction(-1));
                            {
                                script.PushValue(-2);   // <-- 'self'
                                script.Call(1, 1);
                                assert(script.IsString(-1));
                                {
                                    this->GUI.Panel = gui.GetElementByID(script.ToString(-1));
                                }
                                script.Pop(1);          // <-- return value from GetID()
                            }
                        }
                        script.Pop(1);
                    }
                }
                script.Pop(1);
            }



            // To place the camera correctly, we'll just reset it.
            this->ResetCamera();


            // At this point we should actually load the scene file. If this is an empty file, we'll just load an empty scene.
            if (GTCore::IO::Size(file) > 0)
            {
                GTCore::FileDeserializer deserializer(file);
                this->DeserializeScene(deserializer);
            }


            // We want to do an initial commit.
            this->scene.CommitStateStackFrame();

            // We want an undo/redo stack item for the initial state of the scene.
            this->AppendStateStackFrame();


            // The scene will be done loading by this pointer, so we can close the file.
            GTCore::IO::Close(file);
        }
    }

    SceneEditor::~SceneEditor()
    {
        // For any scene node still loaded, we need to iterate over and destroy them. Note how we don't increment every time, because deleting
        // the node will in turn remove it from the list as a result from the event handlers.
        for (size_t i = 0; i < this->sceneNodes.count; )
        {
            auto node = this->sceneNodes.buffer[i]->value;
            assert(node != nullptr);
            {
                auto metadata = node->GetComponent<EditorMetadataComponent>();
                if (metadata != nullptr)
                {
                    if (metadata->DeleteOnClose())
                    {
                        delete node;
                        continue;
                    }
                }
            }

            // We'll only get here if the scene node was not deleted.
            ++i;
        }



        // GUI elements need to be deleted. We will delete the toolbar first, via the scripting interface.
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("DeleteToolBar");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);

        // We need to delete the main GUI element, along with it's children.
        this->GetGUI().DeleteElement(this->GUI.Main);
    }



    void SceneEditor::ResetCamera()
    {
        this->camera.SetPosition(0.0f, 0.0f, 10.0f);
        this->SetCameraRotation(0.0f, 0.0f);
    }

    void SceneEditor::SetCameraRotation(float xRotation, float yRotation)
    {
        this->cameraXRotation = xRotation;
        this->cameraYRotation = yRotation;
        this->ApplyCameraRotation();
    }


    void SceneEditor::EnablePhysicsSimulation()
    {
        // We don't really want to be staging anything while doing physics simulation.
        this->scene.DisableStateStackStaging();

        this->simulationSerializer.Clear();
        this->SerializeScene(this->simulationSerializer);

        this->physicsManager.EnableSimulation();
    }

    void SceneEditor::DisablePhysicsSimulation()
    {
        this->physicsManager.DisableSimulation();

        if (this->simulationSerializer.GetBuffer() != nullptr && this->simulationSerializer.GetBufferSizeInBytes() > 0)
        {
            GTCore::BasicDeserializer deserializer(this->simulationSerializer.GetBuffer(), this->simulationSerializer.GetBufferSizeInBytes());
            this->DeserializeScene(deserializer);
        }


        // We disable state stack staging, so we'll want to re-enable that now.
        this->scene.EnableStateStackStaging();
    }

    bool SceneEditor::IsPhysicsSimulationEnabled() const
    {
        return this->physicsManager.IsSimulationEnabled();
    }



    ///////////////////////////////////////////////////
    // Scene Events

    bool SceneEditor::TryGizmoMouseSelect()
    {
        if (this->GUI.Main->IsVisible())
        {
            int clickPosX;
            int clickPosY;
            this->viewportEventHandler.GetMousePosition(clickPosX, clickPosY);

            glm::vec3 rayStart;
            glm::vec3 rayEnd;
            this->viewport.CalculatePickingRay(clickPosX, clickPosY, rayStart, rayEnd);


            CollisionWorld::ClosestRayTestCallback rayTestCallback(rayStart, rayEnd);
            rayTestCallback.m_collisionFilterGroup = CollisionGroups::EditorSelectionRay;
            rayTestCallback.m_collisionFilterMask  = CollisionGroups::EditorGizmo;
            this->pickingWorld.RayTest(rayStart, rayEnd, rayTestCallback);
            if (rayTestCallback.hasHit())
            {
                auto metadata = static_cast<EditorMetadataComponent*>(rayTestCallback.m_collisionObject->getUserPointer());
                assert(metadata != nullptr);
                {
                    auto &selectedNode = metadata->GetNode();

                    // Arrows
                    if (&selectedNode == &this->transformGizmo.GetXArrowSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(1.0f, 0.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Translate;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetYArrowSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 1.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Translate;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetZArrowSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 0.0f, 1.0f);
                        this->gizmoDragMode    = GizmoDragMode_Translate;
                    }
                    // Circles
                    else if (&selectedNode == &this->transformGizmo.GetXCircleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(1.0f, 0.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Rotate;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetYCircleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 1.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Rotate;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetZCircleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 0.0f, 1.0f);
                        this->gizmoDragMode    = GizmoDragMode_Rotate;
                    }
                    // Scale handles.
                    else if (&selectedNode == &this->transformGizmo.GetXScaleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(1.0f, 0.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Scale;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetYScaleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 1.0f, 0.0f);
                        this->gizmoDragMode    = GizmoDragMode_Scale;
                    }
                    else if (&selectedNode == &this->transformGizmo.GetZScaleSceneNode())
                    {
                        this->gizmoDragAxis    = glm::vec3(0.0f, 0.0f, 1.0f);
                        this->gizmoDragMode    = GizmoDragMode_Scale;
                    }
                    else
                    {
                        return false;
                    }


                    // The mouse dragging has a different level of influence depending on the direction of the axis. We need to calculate that now. We project two points - the
                    // center of the gizmo and the end point of the selected axis. From that we can get a normalised direction vector and use that as the influence.
                    glm::vec3 gizmoWorldPos        = this->transformGizmo.GetPosition();
                    glm::vec3 gizmoCenterWindowPos = this->viewport.Project(gizmoWorldPos);
                    glm::vec3 axisTipWindowPos     = this->viewport.Project(gizmoWorldPos + (selectedNode.GetWorldForwardVector() * selectedNode.GetWorldScale()));

                    this->gizmoDragFactor = glm::vec2(axisTipWindowPos - gizmoCenterWindowPos);
                    if (glm::dot(this->gizmoDragFactor, this->gizmoDragFactor) > 0.0f)
                    {
                        this->gizmoDragFactor = glm::normalize(this->gizmoDragFactor);
                    }
                    else
                    {
                        this->gizmoDragFactor.x = 1.0f;
                        this->gizmoDragFactor.y = 0.0f;
                    }


                    this->transformGizmo.ChangeAxisColour(selectedNode, 1.0f, 1.0f, 1.0f);


                    // What we need to do now is serialize the state of every selected node so that we can create an update command for the undo/redo stack.
                    this->transformationSerializer.Clear();
                    this->SerializeSceneNodes(this->selectedNodes, this->transformationSerializer);
                }

                return true;
            }
            else
            {
                this->gizmoDragMode = GizmoDragMode_None;
            }
        }

        return false;
    }

    void SceneEditor::DoMouseSelection()
    {
        if (this->GUI.Main->IsVisible())
        {
            // We want to do a few ray tests here. Some stuff will have priority over other stuff. For example, any gizmo will have a higher priority over everything else.

            int clickPosX;
            int clickPosY;
            this->viewportEventHandler.GetMousePosition(clickPosX, clickPosY);

            glm::vec3 rayStart;
            glm::vec3 rayEnd;
            this->viewport.CalculatePickingRay(clickPosX, clickPosY, rayStart, rayEnd);


            CollisionWorld::ClosestRayTestCallback rayTestCallback(rayStart, rayEnd);
            rayTestCallback.m_collisionFilterGroup = CollisionGroups::EditorSelectionRay;
            rayTestCallback.m_collisionFilterMask  = CollisionGroups::EditorSelectionVolume;
            this->pickingWorld.RayTest(rayStart, rayEnd, rayTestCallback);
            if (rayTestCallback.hasHit())
            {
                // The use data of the object will be a pointer to the EditorMetadataComponent object. From this, we can grab the actual scene node.
                auto metadata = static_cast<EditorMetadataComponent*>(rayTestCallback.m_collisionObject->getUserPointer());
                assert(metadata != nullptr);
                {
                    auto &selectedNode = metadata->GetNode();

                    // The way we do the selection depends on what we're doing. If shift is being held down, we don't want to deselect anything and instead just add
                    // or remove the node to the selection. If the selected node is already selected, it needs to be deselected. Otherwise it needs to be selected.
                    if (this->GetOwnerEditor().GetGame().IsKeyDown(GTCore::Keys::Shift))
                    {
                        if (this->IsSceneNodeSelected(selectedNode))
                        {
                            this->DeselectSceneNode(selectedNode);
                        }
                        else
                        {
                            this->SelectSceneNode(selectedNode);
                        }
                    }
                    else
                    {
                        // If the node is already the selected one, we don't do anything.
                        if (!(this->selectedNodes.count == 1 && this->selectedNodes[0] == metadata->GetID()))
                        {
                            this->DeselectAll();
                            this->SelectSceneNode(selectedNode);
                        }
                    }
                }
            }
            else
            {
                this->DeselectAll();
            }
        }
    }

    void SceneEditor::DeselectAll()
    {
        while (this->selectedNodes.count > 0)
        {
            auto iNode = this->sceneNodes.Find(this->selectedNodes[0]);
            assert(iNode != nullptr);
            {
                auto node = iNode->value;
                assert(node != nullptr);
                {
                    this->DeselectSceneNode(*node);
                }
            }
        }
    }


    bool SceneEditor::IsSceneNodeSelected(const SceneNode &node) const
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            return metadata->IsSelected();
        }

        return false;
    }

    void SceneEditor::SelectSceneNode(SceneNode &node, bool force)
    {
        if (!this->IsSceneNodeSelected(node) || force)
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Select();

                if (!this->selectedNodes.Exists(metadata->GetID()))
                {
                    this->selectedNodes.PushBack(metadata->GetID());
                }

                // The scripting environment needs to be aware of this change.
                this->PostOnSelectionChangedEventToScript();


                // With a change in selection, we will need to update the position of the gizmos.
                this->ShowTransformGizmo();
            }
        }
    }

    void SceneEditor::SelectSceneNodes(const GTCore::Vector<size_t> &selectedNodeIDs)
    {
        for (size_t i = 0; i < selectedNodeIDs.count; ++i)
        {
            auto node = this->GetSceneNodeByID(selectedNodeIDs[i]);
            assert(node != nullptr);
            {
                this->SelectSceneNode(*node);
            }
        }
    }

    void SceneEditor::DeselectSceneNode(SceneNode &node)
    {
        if (this->IsSceneNodeSelected(node))
        {
            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                metadata->Deselect();

                assert(this->selectedNodes.Exists(metadata->GetID()) == true);
                {
                    this->selectedNodes.RemoveFirstOccuranceOf(metadata->GetID());

                    // The scripting environment needs to be aware of this change.
                    this->PostOnSelectionChangedEventToScript();


                    // With a change in selection, we will need to update the gizmos.
                    if (this->selectedNodes.count == 0)
                    {
                        this->HideGizmo();
                    }
                    else
                    {
                        this->RepositionGizmo();
                    }
                }
            }
        }
    }

    glm::vec3 SceneEditor::GetSelectionCenterPoint() const
    {
        if (this->selectedNodes.count > 0)
        {
            glm::vec3 aabbMin( FLT_MAX,  FLT_MAX,  FLT_MAX);
            glm::vec3 aabbMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);


            for (size_t i = 0; i < this->selectedNodes.count; ++i)
            {
                auto iNode = this->sceneNodes.Find(this->selectedNodes[i]);
                assert(iNode != nullptr);
                {
                    auto node = iNode->value;
                    assert(node != nullptr);
                    {
                        glm::vec3 position = node->GetWorldPosition();

                        aabbMin = glm::min(aabbMin, position);
                        aabbMax = glm::max(aabbMax, position);
                    }
                }
            }


            return (aabbMin + aabbMax) * 0.5f;
        }

        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    glm::quat SceneEditor::GetGizmoRotation() const
    {
        if (this->selectedNodes.count == 1 && (this->gizmoTransformSpace == GizmoTransformSpace_Local || this->gizmoTransformMode == GizmoTransformMode_Scale))
        {
            auto node = this->GetSceneNodeByID(this->selectedNodes[0]);
            assert(node != nullptr);
            {
                return node->GetWorldOrientation();
            }
        }

        return glm::quat();
    }

    size_t SceneEditor::GetSelectedSceneNodeCount() const
    {
        return this->selectedNodes.count;
    }

    SceneNode* SceneEditor::GetFirstSelectedSceneNode()
    {
        if (this->selectedNodes.count > 0)
        {
            return this->GetSceneNodeByID(this->selectedNodes[0]);
        }

        return nullptr;
    }


    ///////////////////////////////////////////////////
    // Editting

    void SceneEditor::DeleteSelectedSceneNodes()
    {
        if (this->selectedNodes.count > 0)
        {
            auto nodesToDelete = this->selectedNodes;
            this->DeleteSceneNodes(nodesToDelete);

            this->AppendStateStackFrame();
        }
    }

    void SceneEditor::DeleteSceneNodes(const GTCore::Vector<size_t> &sceneNodeIDs)
    {
        for (size_t i = 0; i < sceneNodeIDs.count; ++i)
        {
            delete this->GetSceneNodeByID(sceneNodeIDs[i]);
        }
    }

    void SceneEditor::DuplicateSelectedSceneNodes()
    {
        if (this->selectedNodes.count > 0)
        {
            GTCore::Vector<size_t>     prevSelectedNodes(this->selectedNodes);
            GTCore::Vector<SceneNode*> newNodes(prevSelectedNodes.count);

            // TODO: Get this working with children.
            for (size_t iNode = 0; iNode < prevSelectedNodes.count; ++iNode)
            {
                auto nodeToCopy = this->GetSceneNodeByID(prevSelectedNodes[iNode]);
                assert(nodeToCopy != nullptr);
                {
                    auto newNode = new SceneNode;

                    // To copy a node, we're going to use the serialization/deserialization system.
                    GTCore::BasicSerializer serializer;
                    nodeToCopy->Serialize(serializer);

                    GTCore::BasicDeserializer deserializer(serializer.GetBuffer(), serializer.GetBufferSizeInBytes());
                    newNode->Deserialize(deserializer);

                    newNodes.PushBack(newNode);
                }
            }


            // TODO: Link nodes to parents!

            // At this point we have our list of new nodes. We now want to deselect everything and then select the new ones.
            this->DeselectAll();

            for (size_t iNode = 0; iNode < newNodes.count; ++iNode)
            {
                auto node = newNodes[iNode];
                assert(node != nullptr);
                {
                    // Now before we add the new node to the scene, we need to set the unique ID to 0. That way, the new nodes will have new
                    // ID's generated when they are added to the scene. If we don't do this, they will have the same ID as the node it was
                    // copied from, which will then break a few things.
                    auto metadata = node->GetComponent<EditorMetadataComponent>();
                    assert(metadata != nullptr);
                    {
                        metadata->SetID(0);
                    }


                    this->scene.AddSceneNode(*node);
                }
            }
        }
    }


    void SceneEditor::Undo()
    {
        assert(this->sceneStateStack.count > 0);
        {
            if (this->sceneStateIndex > 0)
            {
                --this->sceneStateIndex;

                auto serializer = this->sceneStateStack[this->sceneStateIndex];
                assert(serializer != nullptr);
                {
                    GTCore::BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());
                    this->DeserializeScene(deserializer);
                }
            }
        }
    }

    void SceneEditor::Redo()
    {
        assert(this->sceneStateStack.count > 0);
        {
            if (this->sceneStateIndex < this->sceneStateStack.count - 1)
            {
                ++this->sceneStateIndex;

                auto serializer = this->sceneStateStack[this->sceneStateIndex];
                assert(serializer != nullptr);
                {
                    GTCore::BasicDeserializer deserializer(serializer->GetBuffer(), serializer->GetBufferSizeInBytes());
                    this->DeserializeScene(deserializer);
                }
            }
        }
    }

    void SceneEditor::CommitStateStackFrame()
    {
        this->scene.CommitStateStackFrame();
        this->MarkAsModified();
    }

    void SceneEditor::AppendStateStackFrame()
    {
        // We do not mark as modified
        bool markAsModified = this->sceneStateStack.count > 0;


        // Everything after the current index needs to be removed.
        while (this->sceneStateStack.count > this->sceneStateIndex + 1)
        {
            this->sceneStateStack.PopBack();
        }

        // If already have too many items in the stack, we'll need to get rid of the oldest one.
        if (this->sceneStateStack.count >= MaxStateStackFrames && this->sceneStateStack.count > 0)
        {
            auto oldestState = this->sceneStateStack[0];
            assert(oldestState != nullptr);
            {
                delete oldestState;
                this->sceneStateStack.Remove(0);
            }
        }


        auto serializer = new GTCore::BasicSerializer;
        this->SerializeScene(*serializer, false);

        this->sceneStateStack.PushBack(serializer);
        this->sceneStateIndex = this->sceneStateStack.count - 1;

        // A change was made, so we need to mark the scene as modified.
        if (markAsModified)
        {
            this->MarkAsModified();
        }
    }



    ///////////////////////////////////////////////////
    // Gizmo Control

    void SceneEditor::SwitchGizmoToTranslateMode()
    {
        this->gizmoTransformMode = GizmoTransformMode_Translate;
        this->UpdateGizmo();
    }

    void SceneEditor::SwitchGizmoToRotateMode()
    {
        this->gizmoTransformMode = GizmoTransformMode_Rotate;
        this->UpdateGizmo();
    }

    void SceneEditor::SwitchGizmoToScaleMode()
    {
        this->gizmoTransformMode = GizmoTransformMode_Scale;
        this->UpdateGizmo();
    }


    void SceneEditor::SwitchGizmoToLocalSpace()
    {
        this->gizmoTransformSpace = GizmoTransformSpace_Local;
        this->UpdateGizmo();
    }

    void SceneEditor::SwitchGizmoToGlobalSpace()
    {
        this->gizmoTransformSpace = GizmoTransformSpace_Global;
        this->UpdateGizmo();
    }

    void SceneEditor::ToggleGizmoSpace()
    {
        if (this->IsGizmoInLocalSpace())
        {
            this->SwitchGizmoToGlobalSpace();
        }
        else
        {
            this->SwitchGizmoToLocalSpace();
        }
    }

    bool SceneEditor::IsGizmoInLocalSpace() const
    {
        return this->gizmoTransformSpace == GizmoTransformSpace_Local;
    }

    bool SceneEditor::IsGizmoInGlobalSpace() const
    {
        return this->gizmoTransformSpace == GizmoTransformSpace_Global;
    }


    ///////////////////////////////////////////////////
    // Scene Events
    //
    // These method should only be called for the active scene.

    void SceneEditor::OnObjectAdded(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            // The default data pointer is going to be a pointer to the editor state that owns the scene node. If this has already been set, we leave it
            // alone. Otherwise, we set it to the current state.
            if (node.GetDataPointer<SceneEditor>(0) == nullptr)
            {
                node.SetDataPointer(0, this);
            }



            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata == nullptr)
            {
                metadata = node.AddComponent<EditorMetadataComponent>();

                // When a scene node is added without a metadata component (which is true if we've made it here), we know that it must be deleted when the
                // state is also deleted. We need to mark it as such.
                metadata->DeleteOnClose(true);
            }


            assert(metadata != nullptr);
            {
                size_t uniqueID = metadata->GetID();

                // If the unique ID is 0, it means one needs to be generated.
                if (uniqueID == 0)
                {
                    uniqueID = ++this->nextSceneNodeID;
                    metadata->SetID(uniqueID);
                }


                if (this->sceneNodes.Find(uniqueID) == nullptr)
                {
                    this->sceneNodes.Add(uniqueID, &node);
                }
            }



            // We can cheat here and just act as if the object has been refreshed.
            this->OnObjectRefreshed(object);
        }
    }

    void SceneEditor::OnObjectRemoved(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            auto metadata = node.GetComponent<EditorMetadataComponent>();
            assert(metadata != nullptr);
            {
                // Collision shapes need to be removed.
                this->pickingWorld.RemoveCollisionObject(metadata->GetPickingCollisionObject());

                if (metadata->IsUsingSprite() && metadata->GetSpritePickingCollisionObject() != nullptr)
                {
                    this->pickingWorld.RemoveCollisionObject(*metadata->GetSpritePickingCollisionObject());
                }


                // We need to make sure scene nodes are deseleted when they are removed from the scene.
                this->DeselectSceneNode(node);


                // The state needs to know that it no longer has the node.
                this->sceneNodes.RemoveByKey(metadata->GetID());


                // The data pointer at position 0 will be a pointer to the Editor_SceneEditor::State object that previously owned the scene node. This needs to be cleared.
                node.SetDataPointer(0, nullptr);
            }
        }
    }

    void SceneEditor::OnObjectRefreshed(SceneObject &object)
    {
        if (object.GetType() == SceneObjectType_SceneNode)
        {
            auto &node = static_cast<SceneNode &>(object);

            auto metadata = node.GetComponent<EditorMetadataComponent>();
            if (metadata != nullptr)
            {
                // If we have a model, we'll want to set the collision shape to that of the model.
                if (node.HasComponent<ModelComponent>())
                {
                    if (metadata->UseModelForPickingShape())
                    {
                        metadata->SetPickingCollisionShapeToModel();
                    }
                }


                // We need to remove and re-add the collision shape since it might have changed. We only re-add if it's visible.
                auto &pickingCollisionObject = metadata->GetPickingCollisionObject();

                auto world = pickingCollisionObject.GetWorld();
                if (world != nullptr)
                {
                    world->RemoveCollisionObject(pickingCollisionObject);
                }


                // If the node is visible, we'll need to include the picking collision objects.
                if (node.IsVisible())
                {
                    // If the picking shape is set to the model, we want to update it here just to make sure everything is valid.
                    if (metadata->UseModelForPickingShape())
                    {
                        metadata->SetPickingCollisionShapeToModel();

                        if (metadata->GetPickingCollisionShape())
                        {
                            pickingCollisionObject.getCollisionShape()->setLocalScaling(ToBulletVector3(node.GetWorldScale()));
                        }
                    }


                    if (metadata->GetPickingCollisionShape() != nullptr)
                    {
                        btTransform transform;
                        node.GetWorldTransform(transform);

                        pickingCollisionObject.setWorldTransform(transform);
                        this->pickingWorld.AddCollisionObject(pickingCollisionObject, metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                    }


                    // If we have a sprite, we'll want to add it's picking object to the picking world.
                    if (metadata->IsUsingSprite() && metadata->GetSpritePickingCollisionObject() != nullptr)
                    {
                        this->pickingWorld.AddCollisionObject(*metadata->GetSpritePickingCollisionObject(), metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                    }
                }


                // The the node is selected, we need to make sure everything is aware of it.
                if (metadata->IsSelected())
                {
                    this->SelectSceneNode(node, true);      // <-- 'true' means to force the selection so that the scripting environment is aware of it.
                }
            }
        }
    }

    void SceneEditor::OnSceneNodeTransform(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            // We need to remove and re-add the collision shape since it might have changed.
            auto &pickingCollisionObject = metadata->GetPickingCollisionObject();


            btTransform transform;
            node.GetWorldTransform(transform);

            pickingCollisionObject.setWorldTransform(transform);

            auto world = pickingCollisionObject.GetWorld();
            if (world != nullptr)
            {
                world->UpdateAABB(pickingCollisionObject);
            }


            // The picking collision shape of the sprite will also need to be updated.
            if (metadata->IsUsingSprite())
            {
                metadata->ApplyTransformToSprite();
            }



            // If the scene node was selected, we should reposition the gizmo.
            if (metadata->IsSelected())
            {
                this->RepositionGizmo();
            }


            // If the node that was transformed is the main camera we'll need to scale the gizmos so that they look a constant size.
            if (&node == &this->camera || metadata->IsSelected())
            {
                this->RescaleGizmo();
            }

            if (this->selectedNodes.count == 1 && metadata->GetID() == this->selectedNodes[0])
            {
                auto &script = this->GetScript();

                script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
                assert(script.IsTable(-1));
                {
                    script.Push("UpdateTransformPanel");
                    script.GetTableValue(-2);
                    assert(script.IsFunction(-1));
                    {
                        script.PushValue(-2);       // 'self'.
                        script.Call(1, 0);
                    }
                }
                script.Pop(1);
            }
        }
    }

    void SceneEditor::OnSceneNodeScale(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            // We need to remove and re-add the collision shape since it might have changed.
            auto &pickingCollisionObject = metadata->GetPickingCollisionObject();

            auto world = pickingCollisionObject.GetWorld();
            if (world != nullptr)
            {
                world->RemoveCollisionObject(pickingCollisionObject);
            }

            if (metadata->GetPickingCollisionShape() != nullptr)
            {
                if (metadata->UseModelForPickingShape())
                {
                    metadata->GetPickingCollisionShape()->setLocalScaling(ToBulletVector3(node.GetWorldScale()));
                }

                if (node.IsVisible())
                {
                    this->pickingWorld.AddCollisionObject(pickingCollisionObject, metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                }
            }
        }
    }

    void SceneEditor::OnSceneNodeHide(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            this->pickingWorld.RemoveCollisionObject(metadata->GetPickingCollisionObject());

            if (metadata->IsUsingSprite())
            {
                assert(metadata->GetSpritePickingCollisionObject() != nullptr);
                {
                    this->pickingWorld.RemoveCollisionObject(*metadata->GetSpritePickingCollisionObject());
                }
            }
        }
    }

    void SceneEditor::OnSceneNodeShow(SceneNode &node)
    {
        auto metadata = node.GetComponent<EditorMetadataComponent>();
        if (metadata != nullptr)
        {
            // The picking objects need to be positioned when shown, so we'll grab the transform now.
            btTransform transform;
            node.GetWorldTransform(transform);


            if (metadata->GetPickingCollisionShape() != nullptr)
            {
                metadata->GetPickingCollisionObject().setWorldTransform(transform);
                this->pickingWorld.AddCollisionObject(metadata->GetPickingCollisionObject(), metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
            }

            if (metadata->IsUsingSprite())
            {
                assert(metadata->GetSpritePickingCollisionObject() != nullptr);
                {
                    metadata->GetSpritePickingCollisionObject()->setWorldTransform(transform);
                    this->pickingWorld.AddCollisionObject(*metadata->GetSpritePickingCollisionObject(), metadata->GetPickingCollisionGroup(), CollisionGroups::EditorSelectionRay);
                }
            }
        }
    }

    void SceneEditor::OnSceneNodeComponentChanged(SceneNode &node, Component &)
    {
        // We don't want to do anything here if we're deserializing or if the node is not actually being saved on the state stack.
        if (!this->isDeserializing && node.IsStateStackStagingEnabled())
        {
            // We'll commit the changes to the state stack so we can undo/redo this change.
            this->CommitStateStackFrame();


            // TEMP
            this->AppendStateStackFrame();
        }
    }

    void SceneEditor::OnStateStackFrameCommitted()
    {
        // We'll commit a new frame whenever something worth of an undo/redo operation has happened. And when that happens, we want the scene to be marked as modified.
        //
        // We only mark as modified if this is not the initial commit. We can determine this by looking at the number of frames. If there is only 1, it was the initial
        // commit and we don't want to mark as modified in that case.
        if (!this->isDeserializing && this->scene.GetStateStackFrameCount() > 1)
        {
            this->MarkAsModified();
        }
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void SceneEditor::Show()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("ShowToolBar");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);


        this->GUI.Main->Show();
    }

    void SceneEditor::Hide()
    {
        this->Save();               // TEMP!!!!



        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("HideToolBar");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);


        this->GUI.Main->Hide();
    }

    bool SceneEditor::Save()
    {
        FILE* file = GTCore::IO::Open(this->GetAbsolutePath(), GTCore::IO::OpenMode::Write);
        if (file != nullptr)
        {
            GTCore::FileSerializer serializer(file);
            this->SerializeScene(serializer);

            GTCore::IO::Close(file);

            this->UnmarkAsModified();

            return true;
        }

        return false;
    }

    void SceneEditor::OnUpdate(double deltaTimeInSeconds)
    {
        if (this->GUI.Main->IsVisible())
        {
            auto &game = this->GetOwnerEditor().GetGame();

            // If the mouse is captured we may need to move the screen around.
            if (game.IsMouseCaptured())
            {
                const float moveSpeed   = 0.05f;
                const float rotateSpeed = 0.1f;

                float mouseOffsetX;
                float mouseOffsetY;
                game.GetSmoothedMouseOffset(mouseOffsetX, mouseOffsetY);

                if (this->IsDraggingGizmo())
                {
                    // TODO: If the parent of a node being processed here is selected, we don't actually want to do anything with it, unless it is ignoring
                    //       the parents transformation.

                    if (mouseOffsetX != 0.0f || mouseOffsetY != 0.0f)
                    {
                        this->transformedObjectWithGizmo = true;


                        glm::vec3 dragAxis      = this->gizmoDragAxis;
                        float     dragDistance  = glm::length(glm::vec2(mouseOffsetX, -mouseOffsetY));
                        float     dragDirection = glm::dot(glm::normalize(glm::vec2(mouseOffsetX, -mouseOffsetY)), this->gizmoDragFactor);

                        if (this->gizmoDragMode == GizmoDragMode_Rotate)
                        {
                            dragDirection = glm::dot(glm::normalize(glm::vec2(mouseOffsetY, -mouseOffsetX)), this->gizmoDragFactor);
                        }


                        glm::vec3 startPosition = this->GetSelectionCenterPoint();
                        float     startAngle    = glm::length(glm::eulerAngles(this->GetGizmoRotation()) * dragAxis);



                        bool wasSnapping = this->isSnapping;
                        this->isSnapping = this->GetOwnerEditor().GetGame().IsKeyDown(GTCore::Keys::Ctrl);

                        if (!wasSnapping && this->isSnapping)
                        {
                            // If we get here, we've just started snapping.
                            this->snapTranslation = startPosition;
                            this->snapAngle       = startAngle;
                        }


                        // We need to drag the selected objects.
                        if (this->gizmoDragMode == GizmoDragMode_Translate)
                        {
                            glm::vec3 translation = dragAxis * (dragDirection * dragDistance * moveSpeed);

                            // What we do is simulate us moving the gizmo and then move the selected nodes by the offset.
                            glm::vec3 endPosition = startPosition;

                            if (this->gizmoTransformSpace == GizmoTransformSpace_Global || this->selectedNodes.count > 1)
                            {
                                endPosition           = startPosition         + translation;
                                this->snapTranslation = this->snapTranslation + translation;
                            }
                            else
                            {
                                endPosition           = startPosition         + (this->GetGizmoRotation() * translation);
                                this->snapTranslation = this->snapTranslation + (this->GetGizmoRotation() * translation);
                            }

                            // If we're snapping, we need to offset the endPosition in such a way that it causes it to snap to the "grid".
                            if (this->isSnapping)
                            {
                                glm::vec3 gridPosition = this->translateSnapSize * glm::floor(this->snapTranslation / glm::vec3(this->translateSnapSize));

                                // At this point we have an end position without snapping, and that same position by snapped to the grid ('gridPosition'). We need
                                // to get the difference, multiply it by the drag axis, and then add that different to the endPosition.
                                endPosition += (gridPosition - endPosition) * dragAxis;
                            }

                            // We we need to grab the difference between the start and end positions and add to the positions of the selected nodes.
                            translation = endPosition - startPosition;



                            for (size_t i = 0; i < this->selectedNodes.count; ++i)
                            {
                                auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                                assert(node != nullptr);
                                {
                                    // We change the world position here.
                                    node->SetWorldPosition(node->GetWorldPosition() + translation);
                                }
                            }
                        }
                        else if (this->gizmoDragMode == GizmoDragMode_Rotate)
                        {
                            float dragAngle = dragDirection * dragDistance * rotateSpeed;

                            // If we have multiple selections, we only ever do a world rotation. Otherwise, we'll do a local rotation.
                            if (this->selectedNodes.count == 1)
                            {
                                auto node = this->GetSceneNodeByID(this->selectedNodes[0]);
                                assert(node != nullptr);
                                {
                                    if (this->gizmoTransformSpace == GizmoTransformSpace_Global)
                                    {
                                        node->RotateAroundWorldAxis(dragAngle, dragAxis);
                                    }
                                    else
                                    {
                                        node->Rotate(dragAngle, dragAxis);
                                    }
                                }
                            }
                            else
                            {
                                glm::vec3 pivot = this->GetSelectionCenterPoint();

                                for (size_t i = 0; i < this->selectedNodes.count; ++i)
                                {
                                    auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                                    assert(node != nullptr);
                                    {
                                        node->RotateAtPivotAroundWorldAxis(dragAngle, dragAxis, pivot);
                                    }
                                }
                            }
                        }
                        else if (this->gizmoDragMode == GizmoDragMode_Scale)
                        {
                            // Scaling is always done in local space. Global space scaling is sheering, which will never be supported.

                            glm::vec3 dragAxis      = this->gizmoDragAxis;
                            float     dragDistance  = glm::length(glm::vec2(mouseOffsetX, -mouseOffsetY));
                            float     dragDirection = glm::dot(glm::normalize(glm::vec2(mouseOffsetX, -mouseOffsetY)), this->gizmoDragFactor);
                            float     dragSpeed     = 0.05f;

                            glm::vec3 scaleOffset = dragAxis * (dragDirection * dragDistance * dragSpeed);

                            for (size_t i = 0; i < this->selectedNodes.count; ++i)
                            {
                                auto node = this->GetSceneNodeByID(this->selectedNodes[i]);
                                assert(node != nullptr);
                                {
                                    glm::vec3 newScale = node->GetWorldScale() + scaleOffset;

                                    // Negative scaling not yet supported.
                                    newScale = glm::max(newScale, glm::vec3(0.0f, 0.0f, 0.0f));

                                    node->SetWorldScale(newScale);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (game.IsMouseButtonDown(GTCore::MouseButton_Left))
                    {
                        if (game.IsMouseButtonDown(GTCore::MouseButton_Right))
                        {
                            this->camera.MoveUp(-mouseOffsetY * moveSpeed);
                            this->camera.MoveRight(mouseOffsetX * moveSpeed);
                        }
                        else
                        {
                            this->camera.MoveForward(-mouseOffsetY * moveSpeed);
                            this->cameraYRotation += -mouseOffsetX * rotateSpeed;
                        }
                    }
                    else
                    {
                        if (game.IsMouseButtonDown(GTCore::MouseButton_Right))
                        {
                            this->cameraXRotation += -mouseOffsetY * rotateSpeed;
                            this->cameraYRotation += -mouseOffsetX * rotateSpeed;
                        }
                    }

                    this->ApplyCameraRotation();

                    if (this->transformGizmo.GetSceneNode().IsVisible())
                    {
                        this->transformGizmo.SetRotation(this->GetGizmoRotation(), this->camera);
                    }
                }
            }


            this->scene.Update(deltaTimeInSeconds);
        }
    }

    void SceneEditor::OnMouseButtonDown(GTCore::MouseButton, int, int)
    {
    }

    void SceneEditor::OnMouseButtonUp(GTCore::MouseButton button, int, int)
    {
        if (button == GTCore::MouseButton_Left)
        {
            this->gizmoDragMode = GizmoDragMode_None;
            this->transformGizmo.RestoreColours();

            if (this->transformedObjectWithGizmo)
            {
                this->CommitStateStackFrame();
                this->AppendStateStackFrame();
                this->transformedObjectWithGizmo = false;
            }
        }
    }





    ///////////////////////////////////////////////////
    // Private Methods.

    void SceneEditor::ApplyCameraRotation()
    {
        this->camera.SetOrientation(glm::quat());
        this->camera.RotateY(this->cameraYRotation);
        this->camera.RotateX(this->cameraXRotation);
    }

    void SceneEditor::SerializeScene(GTCore::Serializer &serializer, bool serializeMetadata) const
    {
        this->scene.Serialize(serializer);

        // We now want to save our own chunk. This will contain metadata such as the camera position and whatnot.
        if (serializeMetadata)
        {
            GTCore::BasicSerializer metadataSerializer;

            metadataSerializer.Write(static_cast<uint32_t>(this->nextSceneNodeID));

            this->camera.Serialize(metadataSerializer);
            metadataSerializer.Write(this->cameraXRotation);
            metadataSerializer.Write(this->cameraYRotation);



            Serialization::ChunkHeader header;
            header.id          = Serialization::ChunkID_Scene_EditorMetadata;
            header.version     = 1;
            header.sizeInBytes = metadataSerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(metadataSerializer.GetBuffer(), header.sizeInBytes);
        }
    }


    void SceneEditor::DeserializeScene(GTCore::Deserializer &deserializer)
    {
        this->isDeserializing = true;
        {
            this->transformGizmo.Hide();

            this->DeleteAllMarkedSceneNodes();

            // With pre-deserialization done, we can now do a full deserialization of the scene.
            this->scene.Deserialize(deserializer);

            // We now want to load the metadata chunk. We'll peek at the next chunk and see if that's it. We should probably do an iteration type
            // system later on.
            Serialization::ChunkHeader header;
            deserializer.Peek(&header, sizeof(Serialization::ChunkHeader));

            if (header.id == Serialization::ChunkID_Scene_EditorMetadata)
            {
                // Since we only peeked at the header, we'll need to now seek past it.
                deserializer.Seek(sizeof(Serialization::ChunkHeader));

                deserializer.Read(reinterpret_cast<uint32_t &>(this->nextSceneNodeID));

                // The camera node needs to be deserialized.
                this->camera.Deserialize(deserializer);
                deserializer.Read(this->cameraXRotation);
                deserializer.Read(this->cameraYRotation);

                this->camera.DisableSerialization();
                this->camera.DisableStateStackStaging();
            }


            // The deserializer will clear the scene, so we'll need to re-add the camera and gizmo.
            this->scene.AddSceneNode(this->camera);
            this->scene.AddSceneNode(this->transformGizmo.GetSceneNode());

            this->UpdateGizmo();
        }
        this->isDeserializing = false;
    }

    void SceneEditor::SerializeSceneNodes(const GTCore::Vector<size_t> &sceneNodeIDs, GTCore::Serializer &serializer)
    {
        for (size_t i = 0; i < sceneNodeIDs.count; ++i)
        {
            auto node = this->GetSceneNodeByID(sceneNodeIDs[i]);
            assert(node != nullptr);
            {
                node->Serialize(serializer);
            }
        }
    }

    void SceneEditor::DeserializeSceneNodes(const GTCore::Vector<size_t> &sceneNodeIDs, GTCore::Deserializer &deserializer)
    {
        for (size_t i = 0; i < sceneNodeIDs.count; ++i)
        {
            auto node = this->GetSceneNodeByID(sceneNodeIDs[i]);
            if (node == nullptr)
            {
                node = new SceneNode;
            }

            node->Deserialize(deserializer);

            // If the node is not contained in the editor's scene, we need to add it.
            if (node->GetScene() != &this->scene)
            {
                this->scene.AddSceneNode(*node);
            }
            else
            {
                node->Refresh();
            }
        }
    }


    void SceneEditor::DeleteAllMarkedSceneNodes()
    {
        // For any scene node still loaded, we need to iterate over and destroy them. Note how we don't increment every time, because deleting
        // the node will in turn remove it from the list as a result from the event handlers.
        for (size_t i = 0; i < this->sceneNodes.count; )
        {
            auto node = this->sceneNodes.buffer[i]->value;
            assert(node != nullptr);
            {
                auto metadata = node->GetComponent<EditorMetadataComponent>();
                if (metadata != nullptr)
                {
                    if (metadata->DeleteOnClose())
                    {
                        delete node;
                        continue;
                    }
                }
            }

            // We'll only get here if the scene node was not deleted.
            ++i;
        }
    }


    SceneNode* SceneEditor::GetSceneNodeByID(size_t id)
    {
        auto iNode = this->sceneNodes.Find(id);
        if (iNode != nullptr)
        {
            return iNode->value;
        }

        return nullptr;
    }

    const SceneNode* SceneEditor::GetSceneNodeByID(size_t id) const
    {
        auto iNode = this->sceneNodes.Find(id);
        if (iNode != nullptr)
        {
            return iNode->value;
        }

        return nullptr;
    }


    void SceneEditor::ShowTransformGizmo()
    {
        this->transformGizmo.Show();
        this->UpdateGizmo();
    }

    void SceneEditor::HideGizmo()
    {
        this->transformGizmo.Hide();
    }

    void SceneEditor::RepositionGizmo()
    {
        this->transformGizmo.SetPosition(this->GetSelectionCenterPoint());
        this->transformGizmo.SetRotation(this->GetGizmoRotation(), this->camera);
    }

    void SceneEditor::RescaleGizmo()
    {
        // We're going to determine the new scale of the gizmos by using a project/unproject system. We first project the actual position of the gizmo
        // into window coordinates. We then add a value to the y result that will represent the size of the object on the screen. Then, we unproject
        // that position back into world space. The length between the gizmo position and unprojected position will be the new scale.
        glm::vec3 gizmoPosition = this->GetSelectionCenterPoint();

        glm::vec3 windowPos = this->viewport.Project(gizmoPosition);
        windowPos.y += 64.0f;

        glm::vec3 gizmoScale(glm::distance(this->viewport.Unproject(windowPos), gizmoPosition));
        this->transformGizmo.SetScale(gizmoScale);
    }

    void SceneEditor::ShowGizmoHandles()
    {
        switch (this->gizmoTransformMode)
        {
        case GizmoTransformMode_Translate:
            {
                this->transformGizmo.GetXArrowSceneNode().Show();
                this->transformGizmo.GetYArrowSceneNode().Show();
                this->transformGizmo.GetZArrowSceneNode().Show();

                this->transformGizmo.GetCameraFacingCircleSceneNode().Hide();
                this->transformGizmo.GetXCircleSceneNode().Hide();
                this->transformGizmo.GetYCircleSceneNode().Hide();
                this->transformGizmo.GetZCircleSceneNode().Hide();

                this->transformGizmo.GetXScaleSceneNode().Hide();
                this->transformGizmo.GetYScaleSceneNode().Hide();
                this->transformGizmo.GetZScaleSceneNode().Hide();


                break;
            }

        case GizmoTransformMode_Rotate:
            {
                this->transformGizmo.GetXArrowSceneNode().Hide();
                this->transformGizmo.GetYArrowSceneNode().Hide();
                this->transformGizmo.GetZArrowSceneNode().Hide();

                this->transformGizmo.GetCameraFacingCircleSceneNode().Show();
                this->transformGizmo.GetXCircleSceneNode().Show();
                this->transformGizmo.GetYCircleSceneNode().Show();
                this->transformGizmo.GetZCircleSceneNode().Show();

                this->transformGizmo.GetXScaleSceneNode().Hide();
                this->transformGizmo.GetYScaleSceneNode().Hide();
                this->transformGizmo.GetZScaleSceneNode().Hide();

                break;
            }

        case GizmoTransformMode_Scale:
            {
                this->transformGizmo.GetXArrowSceneNode().Hide();
                this->transformGizmo.GetYArrowSceneNode().Hide();
                this->transformGizmo.GetZArrowSceneNode().Hide();

                this->transformGizmo.GetCameraFacingCircleSceneNode().Hide();
                this->transformGizmo.GetXCircleSceneNode().Hide();
                this->transformGizmo.GetYCircleSceneNode().Hide();
                this->transformGizmo.GetZCircleSceneNode().Hide();

                this->transformGizmo.GetXScaleSceneNode().Show();
                this->transformGizmo.GetYScaleSceneNode().Show();
                this->transformGizmo.GetZScaleSceneNode().Show();

                break;
            }

        default:
            {
                break;
            }
        }
    }

    void SceneEditor::UpdateGizmo()
    {
        this->RepositionGizmo();
        this->RescaleGizmo();
        this->ShowGizmoHandles();
    }

    bool SceneEditor::IsDraggingGizmo() const
    {
        return this->gizmoDragMode != GizmoDragMode_None;
    }


    void SceneEditor::PostOnSelectionChangedEventToScript()
    {
        auto &script = this->GetScript();

        script.Get(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->GUI.Main->id).c_str());
        assert(script.IsTable(-1));
        {
            script.Push("OnSelectionChanged");
            script.GetTableValue(-2);
            assert(script.IsFunction(-1));
            {
                script.PushValue(-2);   // <-- 'self'.
                script.Call(1, 0);
            }
        }
        script.Pop(1);
    }
}



#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
