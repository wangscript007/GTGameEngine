// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/SceneNode.hpp>
#include <GTGE/Scene.hpp>
#include <GTGE/GTEngine.hpp>

#undef GetNextSibling
#undef GetPrevSibling
#undef GetFirstChild
#undef GetClassName

namespace GT
{
    SceneNode::SceneNode()
        : uniqueID(0), m_name(),
          m_parent(nullptr),
          firstChild(nullptr), lastChild(nullptr), prevSibling(nullptr), nextSibling(nullptr),
          m_position(), m_orientation(), m_scale(1.0f, 1.0f, 1.0f),
          eventHandlers(), components(), dataPointers(),
          scene(nullptr),
          m_flags(0),
          eventLockCounter(0),
          modelComponent(nullptr), pointLightComponent(nullptr), spotLightComponent(nullptr), editorMetadataComponent(nullptr)
    {
    }

    SceneNode::~SceneNode()
    {
        this->RemoveFromScene();

        this->DetachFromParent(false);
        this->OnDestroy();

        this->DetachAllChildren(false);
        this->RemoveAllComponents();
    }

    const char* SceneNode::GetName() const
    {
        return m_name.c_str();
    }

    void SceneNode::SetName(const char* newName)
    {
        if (!Strings::Equal(newName, m_name.c_str()))
        {
            m_name = newName;

            if (this->scene != nullptr)
            {
                this->scene->OnSceneNodeNameChanged(*this);
            }
        }
    }


    uint64_t SceneNode::GetID() const
    {
        return this->uniqueID;
    }

    void SceneNode::SetID(uint64_t newID)
    {
        this->uniqueID = newID;
    }


    SceneNode* SceneNode::GetParent()
    {
        return m_parent;
    }

    const SceneNode* SceneNode::GetParent() const
    {
        return m_parent;
    }


    SceneNode* SceneNode::GetFirstChild()
    {
        return this->firstChild;
    }

    const SceneNode* SceneNode::GetFirstChild() const
    {
        return this->firstChild;
    }


    SceneNode* SceneNode::GetLastChild()
    {
        return this->lastChild;
    }

    const SceneNode* SceneNode::GetLastChild() const
    {
        return this->lastChild;
    }


    SceneNode* SceneNode::GetPrevSibling()
    {
        return this->prevSibling;
    }

    const SceneNode* SceneNode::GetPrevSibling() const
    {
        return this->prevSibling;
    }


    SceneNode* SceneNode::GetNextSibling()
    {
        return this->nextSibling;
    }

    const SceneNode* SceneNode::GetNextSibling() const
    {
        return this->nextSibling;
    }


    SceneNode* SceneNode::GetTopAncestor()
    {
        if (m_parent != nullptr)
        {
            return m_parent->GetTopAncestor();
        }

        return this;
    }

    const SceneNode* SceneNode::GetTopAncestor() const
    {
        if (m_parent != nullptr)
        {
            return m_parent->GetTopAncestor();
        }

        return this;
    }


    size_t SceneNode::GetChildCount() const
    {
        size_t count = 0;

        for (auto i = this->GetFirstChild(); i != nullptr; i = i->GetNextSibling())
        {
            ++count;
        }

        return count;
    }


    void SceneNode::AttachEventHandler(SceneNodeEventHandler &handler)
    {
        // If an event handler already exists, we can't attach it.
        if (this->eventHandlers.Find(&handler) == nullptr)
        {
            this->eventHandlers.Append(&handler);
        }
    }

    void SceneNode::DetachEventHandler(SceneNodeEventHandler &handler)
    {
        this->eventHandlers.Remove(this->eventHandlers.Find(&handler));
    }


    void SceneNode::AttachChild(SceneNode &child, bool keepWorldTransform)
    {
        if (&child != this && !this->IsAncestor(child))
        {
            auto previousParent = child.GetParent();

            // If the childs parent is equal to this, it is already attached and so we don't need to do anything.
            if (previousParent != this)
            {
                glm::vec3 worldPosition;
                glm::quat worldOrientation;
                glm::vec3 worldScale;
                if (keepWorldTransform)
                {
                    worldPosition    = child.GetWorldPosition();
                    worldOrientation = child.GetWorldOrientation();
                    worldScale       = child.GetWorldScale();
                }



                // If the child already has a parent, it needs to be detached.
                child.DetachFromParent(false, false);          // <-- 'false' = don't keep the world tranasfor and don't post OnParentChanged to the scene.

                // If we don't have children, this will be the first one...
                if (this->firstChild == nullptr)
                {
                    this->firstChild = &child;
                }

                // If we already have children, this child needs to be appended to the end of the children list.
                if (this->lastChild != nullptr)
                {
                    auto secondLastChild = this->lastChild;

                    secondLastChild->_SetNextSibling(&child);
                    child._SetPrevSibling(secondLastChild);
                }

                // The new item is always the last one.
                child._SetNextSibling(nullptr);
                this->lastChild = &child;


                // The child needs to know that this is it's new parent.
                child._SetParent(this);


                if (keepWorldTransform)
                {
                    child.SetWorldPosition(worldPosition, true);
                    child.SetWorldOrientation(worldOrientation, true);
                    child.SetWorldScale(worldScale);
                }


                // The event handlers need to know about the new child.
                if (!this->EventsLocked())
                {
                    this->OnAttach(child);
                }


                // Finally, we need to add the child to the current scene, if any.
                if (this->scene != nullptr)
                {
                    if (child.GetScene() != this->scene)
                    {
                        this->scene->AddSceneNode(child);
                    }
                    else
                    {
                        this->scene->OnSceneNodeParentChanged(child, previousParent);
                    }
                }
            }
        }
    }

    void SceneNode::AttachTo(SceneNode &parent, bool keepWorldTransform)
    {
        parent.AttachChild(*this, keepWorldTransform);
    }

    void SceneNode::DetachChild(SceneNode &child, bool keepWorldTransform, bool postParentChangedEventToScene)
    {
        auto previousParent = child.GetParent();

        if (previousParent == this)
        {
            auto childPrevSibling = child.GetPrevSibling();
            auto childNextSibling = child.GetNextSibling();

            if (childPrevSibling)
            {
                childPrevSibling->_SetNextSibling(childNextSibling);
            }
            else
            {
                // The child is the root.
                this->firstChild = childNextSibling;
            }

            if (childNextSibling)
            {
                childNextSibling->_SetPrevSibling(childPrevSibling);
            }
            else
            {
                // The child is the end.
                this->lastChild = childPrevSibling;
            }



            glm::vec3 worldPosition;
            glm::quat worldOrientation;
            glm::vec3 worldScale;
            if (keepWorldTransform)
            {
                worldPosition    = child.GetWorldPosition();
                worldOrientation = child.GetWorldOrientation();
                worldScale       = child.GetWorldScale();
            }


            // The child needs to be orphaned.
            child._SetParent(nullptr);
            child._SetPrevSibling(nullptr);
            child._SetNextSibling(nullptr);


            if (keepWorldTransform)
            {
                child.SetWorldPosition(worldPosition, true);
                child.SetWorldOrientation(worldOrientation, true);
                child.SetWorldScale(worldScale);
            }


            if (!this->EventsLocked())
            {
                this->OnDetach(child);
            }


            if (postParentChangedEventToScene && child.GetScene() != nullptr)
            {
                child.GetScene()->OnSceneNodeParentChanged(child, previousParent);
            }


            // It makes more sense to leave the scene as-is, so don't be tempted to set it to nullptr like we do the parent.
        }
    }

    void SceneNode::DetachAllChildren(bool keepWorldTransform)
    {
        while (this->firstChild != nullptr)
        {
            this->DetachChild(*this->firstChild, keepWorldTransform);
        }
    }

    void SceneNode::DetachFromParent(bool keepWorldTransform, bool postParentChangedEventToScene)
    {
        auto parent = this->GetParent();
        if (parent != nullptr)
        {
            parent->DetachChild(*this, keepWorldTransform, postParentChangedEventToScene);
        }
    }

    SceneNode* SceneNode::FindFirstChild(const char* name, bool recursive)
    {
        for (auto iChild = this->firstChild; iChild != nullptr; iChild = iChild->GetNextSibling())
        {
            if (Strings::Equal(iChild->GetName(), name))
            {
                return iChild;
            }
            else
            {
                if (recursive)
                {
                    iChild = iChild->FindFirstChild(name, recursive);
                    if (iChild != nullptr)
                    {
                        return iChild;
                    }
                }
            }
        }

        return nullptr;
    }

    SceneNode* SceneNode::FindFirstChildWithComponent(const char *componentName, bool recursive)
    {
        for (auto iChild = this->firstChild; iChild != nullptr; iChild = iChild->GetNextSibling())
        {
            if (iChild->HasComponent(componentName))
            {
                return iChild;
            }
            else
            {
                if (recursive)
                {
                    iChild = FindFirstChildWithComponent(componentName, recursive);
                    if (iChild != nullptr)
                    {
                        return iChild;
                    }
                }
            }
        }

        return nullptr;
    }

    bool SceneNode::IsAncestor(const SceneNode &other) const
    {
        if (m_parent != nullptr)
        {
            if (m_parent == &other)
            {
                return true;
            }

            return m_parent->IsAncestor(other);
        }

        return false;
    }

    bool SceneNode::IsDescendant(const SceneNode &other) const
    {
        for (auto iChild = this->firstChild; iChild != nullptr; iChild = iChild->nextSibling)
        {
            if (iChild == &other)
            {
                return true;
            }
            else
            {
                if (iChild->IsDescendant(other))
                {
                    return true;
                }
            }
        }

        return false;
    }


    void SceneNode::_SetParent(SceneNode *parent)
    {
        m_parent = parent;
    }

    void SceneNode::_SetPrevSibling(SceneNode* newPrevSibling)
    {
        this->prevSibling = newPrevSibling;
    }

    void SceneNode::_SetNextSibling(SceneNode* newNextSibling)
    {
        this->nextSibling = newNextSibling;
    }



    ////////////////////////////////////////////////
    // Transformation

    void SceneNode::SetPosition(const glm::vec3 &position, bool updateDynamicsObject)
    {
        if (m_position.x != position.x || m_position.y != position.y || m_position.z != position.z)
        {
            m_position = position;

            if (!this->EventsLocked())
            {
                this->OnTransform(updateDynamicsObject);
            }
        }
    }

    glm::vec3 SceneNode::GetWorldPosition() const
    {
        if (m_parent != nullptr && this->IsPositionInheritanceEnabled())
        {
            glm::quat orientation;
            if (this->IsOrientationInheritanceEnabled())
            {
                orientation = m_parent->GetWorldOrientation();
            }

            glm::vec3 scale(1.0f, 1.0f, 1.0f);
            if (this->IsScaleInheritanceEnabled())
            {
                scale = m_parent->GetWorldScale();
            }

            return m_parent->GetWorldPosition() + (orientation * (scale * m_position));
        }

        return m_position;
    }

    void SceneNode::SetWorldPosition(const glm::vec3 &worldPosition, bool updateDynamicsObject)
    {
        if (m_parent != nullptr && this->IsPositionInheritanceEnabled())
        {
            glm::vec3 Pp = m_parent->GetWorldPosition();
            glm::vec3 Ps = m_parent->GetWorldScale();
            glm::quat Po = m_parent->GetOrientation();

            this->SetPosition(((worldPosition - Pp) * Po) / Ps, updateDynamicsObject);
        }
        else
        {
            this->SetPosition(worldPosition, updateDynamicsObject);
        }
    }


    void SceneNode::SetOrientation(const glm::quat &orientation, bool updateDynamicsObject)
    {
        if (m_orientation[0] != orientation[0] ||
            m_orientation[1] != orientation[1] ||
            m_orientation[2] != orientation[2] ||
            m_orientation[3] != orientation[3])
        {
            m_orientation = orientation;

            if (!this->EventsLocked())
            {
                this->OnTransform(updateDynamicsObject);
            }
        }
    }

    glm::quat SceneNode::GetWorldOrientation() const
    {
        if (m_parent != nullptr && this->IsOrientationInheritanceEnabled())
        {
            return m_parent->GetWorldOrientation() * m_orientation;
        }

        return m_orientation;
    }

    void SceneNode::SetWorldOrientation(const glm::quat &worldOrientation, bool updateDynamicsObject)
    {
        if (m_parent != nullptr && this->IsOrientationInheritanceEnabled())
        {
            this->SetOrientation(glm::inverse(m_parent->GetWorldOrientation()) * worldOrientation, updateDynamicsObject);
        }
        else
        {
            this->SetOrientation(worldOrientation, updateDynamicsObject);
        }
    }


    void SceneNode::SetScale(const glm::vec3 &scale)
    {
        if (m_scale.x != scale.x || m_scale.y != scale.y || m_scale.z != scale.z)
        {
            m_scale = scale;

            if (!this->EventsLocked())
            {
                this->OnScale();
            }
        }
    }

    glm::vec3 SceneNode::GetWorldScale() const
    {
        if (m_parent != nullptr && this->IsScaleInheritanceEnabled())
        {
            return m_parent->GetWorldScale() * m_scale;
        }

        return m_scale;
    }

    void SceneNode::SetWorldScale(const glm::vec3 &worldScale)
    {
        if (m_parent != nullptr && this->IsScaleInheritanceEnabled())
        {
            this->SetScale(worldScale / m_parent->GetWorldScale());
        }
        else
        {
            this->SetScale(worldScale);
        }
    }


    void SceneNode::LookAt(const glm::vec3 &target, const glm::vec3 &up)
    {
        this->SetWorldOrientation(glm::quat_cast(Math::CalculateLookAtMatrix(this->GetWorldPosition(), target, up)));
    }

    void SceneNode::LookAt(const SceneNode &target, const glm::vec3 &up)
    {
        this->LookAt(target.GetWorldPosition(), up);
    }


    void SceneNode::LookInDirection(const glm::vec3 &direction, const glm::vec3 &up)
    {
        this->LookAt(this->GetWorldPosition() + direction, up);
    }


    glm::vec3 SceneNode::GetWorldForwardVector() const
    {
        return this->GetWorldOrientation() * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 SceneNode::GetWorldRightVector() const
    {
        return this->GetWorldOrientation() * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 SceneNode::GetWorldUpVector() const
    {
        return this->GetWorldOrientation() * glm::vec3(0.0f, 1.0f, 0.0f);
    }


    void SceneNode::GetWorldTransformComponents(glm::vec3 &positionOut, glm::quat &orientationOut, glm::vec3 &scaleOut) const
    {
        if (m_parent != nullptr)
        {
            m_parent->GetWorldTransformComponents(positionOut, orientationOut, scaleOut);

            glm::vec3 temp;
            if (!this->IsPositionInheritanceEnabled())
            {
                positionOut = m_position;

                if (!this->IsScaleInheritanceEnabled())
                {
                    scaleOut = m_scale;
                }
                else
                {
                    scaleOut = scaleOut * m_scale;
                }

                if (!this->IsOrientationInheritanceEnabled())
                {
                    orientationOut = m_orientation;
                }
                else
                {
                    orientationOut = orientationOut * m_orientation;
                }
            }
            else
            {
                glm::vec3 offset = m_position;

                if (!this->IsScaleInheritanceEnabled())
                {
                    scaleOut = m_scale;
                }
                else
                {
                    offset   = scaleOut * offset;
                    scaleOut = scaleOut * m_scale;
                }

                if (!this->IsOrientationInheritanceEnabled())
                {
                    orientationOut = m_orientation;
                }
                else
                {
                    offset         = orientationOut * offset;
                    orientationOut = orientationOut * m_orientation;
                }

                positionOut += offset;
            }
        }
        else
        {
            positionOut    = m_position;
            orientationOut = m_orientation;
            scaleOut       = m_scale;
        }
    }

    void SceneNode::SetWorldTransformComponents(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale, bool updateDynamicsObject)
    {
        if (m_parent != nullptr)
        {
            glm::vec3 Pp;
            glm::quat Po;
            glm::vec3 Ps;
            m_parent->GetWorldTransformComponents(Pp, Po, Ps);

            this->SetPosition(((position - Pp) * Po) / Ps, updateDynamicsObject);
            this->SetOrientation(glm::inverse(Po) * orientation, updateDynamicsObject);
            this->SetScale(scale / Ps);
        }
        else
        {
            this->SetPosition(position, updateDynamicsObject);
            this->SetOrientation(orientation, updateDynamicsObject);
            this->SetScale(scale);
        }
    }


    glm::mat4 SceneNode::GetWorldTransform() const
    {
        glm::vec3 position;
        glm::quat orientation;
        glm::vec3 scale;
        this->GetWorldTransformComponents(position, orientation, scale);

        glm::mat4 result;
        Math::CalculateTransformMatrix(position, orientation, scale, result);

        return result;
    }

    glm::mat4 SceneNode::GetWorldTransformWithoutScale() const
    {
        glm::vec3 position;
        glm::quat orientation;
        glm::vec3 devnull;
        this->GetWorldTransformComponents(position, orientation, devnull);

        glm::mat4 result;
        Math::CalculateTransformMatrix(position, orientation, result);

        return result;
    }

    void SceneNode::GetWorldTransform(btTransform &worldTransform) const
    {
        glm::vec3 position;
        glm::quat orientation;
        glm::vec3 devnull;
        this->GetWorldTransformComponents(position, orientation, devnull);

        worldTransform.setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));
        worldTransform.setOrigin(btVector3(position.x, position.y, position.z));


        (void)devnull;  // <-- warning silencer.
    }

    void SceneNode::SetWorldTransform(const btTransform &worldTransform, bool updateDynamicsObject)
    {
        // We need to lock transformation event posting because we want to do it in a single event for the sake of efficiency.
        this->LockEvents();
        {
            this->SetWorldPosition(ToGLMVector3(worldTransform.getOrigin()), false);
            this->SetWorldOrientation(ToGLMQuaternion(worldTransform.getRotation()), false);
        }
        this->UnlockEvents();

        // Now is where we post the transformation event.
        this->OnTransform(updateDynamicsObject);
    }



    void SceneNode::Translate(const glm::vec3 &offset)
    {
        this->SetPosition(m_position + (m_orientation * offset));
    }

    void SceneNode::Rotate(float angle, const glm::vec3 &axis)
    {
        this->SetOrientation(m_orientation * glm::angleAxis(glm::radians(angle), axis));
    }

    void SceneNode::Scale(const glm::vec3 &scale)
    {
        this->SetScale(m_scale * scale);
    }


    void SceneNode::InterpolatePosition(const glm::vec3 &dest, float a)
    {
        this->SetPosition(glm::mix(m_position, dest, a));
    }

    void SceneNode::InterpolateOrientation(const glm::quat &dest, float a)
    {
        this->SetOrientation(glm::mix(m_orientation, dest, a));
    }

    void SceneNode::InterpolateScale(const glm::vec3 &dest, float a)
    {
        this->SetScale(glm::mix(m_scale, dest, a));
    }

    void SceneNode::Slerp(const glm::quat &dest, float a)
    {
        this->SetOrientation(glm::slerp(m_orientation, dest, a));
    }



    void SceneNode::ClampPosition(const glm::vec3 &posMin, const glm::vec3 &posMax)
    {
        this->SetPosition(glm::clamp(this->GetPosition(), posMin, posMax));
    }

    void SceneNode::ClampWorldPosition(const glm::vec3 &posMin, const glm::vec3 &posMax)
    {
        this->SetWorldPosition(glm::clamp(this->GetWorldPosition(), posMin, posMax));
    }

    void SceneNode::ClampScale(const glm::vec3 &scaleMin, const glm::vec3 &scaleMax)
    {
        this->SetScale(glm::clamp(this->GetPosition(), scaleMin, scaleMax));
    }

    void SceneNode::ClampWorldScale(const glm::vec3 &scaleMin, const glm::vec3 &scaleMax)
    {
        this->SetWorldScale(glm::clamp(this->GetWorldPosition(), scaleMin, scaleMax));
    }



    glm::vec3 SceneNode::GetForwardVector() const
    {
        return m_orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 SceneNode::GetRightVector() const
    {
        return m_orientation * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 SceneNode::GetUpVector() const
    {
        return m_orientation * glm::vec3(0.0f, 1.0f, 0.0f);
    }



    void SceneNode::RotateAroundWorldAxis(float angle, const glm::vec3 &axis)
    {
        this->SetOrientation(glm::angleAxis(glm::radians(angle), axis) * this->GetOrientation());
    }

    void SceneNode::RotateAtPivotAroundWorldAxis(float angle, const glm::vec3 &axis, const glm::vec3 &pivot)
    {
        glm::quat rotation       = glm::angleAxis(glm::radians(angle), axis);
        glm::quat newOrientation = rotation * this->GetOrientation();
        glm::vec3 newPosition    = rotation * (this->GetPosition() - pivot) + pivot;

        this->SetOrientation(newOrientation);
        this->SetPosition(newPosition);
    }


    void SceneNode::GetTransform(glm::mat4 &transform)
    {
        Math::CalculateTransformMatrix(m_position, m_orientation, m_scale, transform);
    }



    /////////////////////////////////////////
    // Components

    Component* SceneNode::GetComponentByName(const char* componentName)
    {
        if (Strings::Equal(componentName, ModelComponent::Name))
        {
            return this->modelComponent;
        }

        if (Strings::Equal(componentName, PointLightComponent::Name))
        {
            return this->pointLightComponent;
        }

        if (Strings::Equal(componentName, SpotLightComponent::Name))
        {
            return this->spotLightComponent;
        }

        if (Strings::Equal(componentName, EditorMetadataComponent::Name))
        {
            return this->editorMetadataComponent;
        }


        auto item = this->components.Find(componentName);
        if (item != nullptr)
        {
            return item->value;
        }

        return nullptr;
    }

    Component* SceneNode::AddComponentByName(const char* componentName)
    {
        // A component of the same name can't already exist. If it doesn, we just return the existing one.
        auto component = this->GetComponentByName(componentName);
        if (component == nullptr)
        {
            if (Strings::Equal(componentName, ModelComponent::Name))
            {
                assert(this->modelComponent == nullptr);
                {
                    component = this->modelComponent = new ModelComponent(*this);
                }
            }
            else if (Strings::Equal(componentName, PointLightComponent::Name))
            {
                assert(this->pointLightComponent == nullptr);
                {
                    component = this->pointLightComponent = new PointLightComponent(*this);
                }
            }
            else if (Strings::Equal(componentName, SpotLightComponent::Name))
            {
                assert(this->spotLightComponent == nullptr);
                {
                    component = this->spotLightComponent = new SpotLightComponent(*this);
                }
            }
            else if (Strings::Equal(componentName, EditorMetadataComponent::Name))
            {
                assert(this->editorMetadataComponent == nullptr);
                {
                    component = this->editorMetadataComponent = new EditorMetadataComponent(*this);
                }
            }
            else
            {
                component = CreateComponentByName(this->GetContext(), componentName, *this);
                this->components.Add(componentName, component);
            }


            // Here we're just letting the scene know about the new component. We'll also post a warning if the component is null while we're here.
            if (component != nullptr)
            {
                if (this->scene != nullptr)
                {
                    this->scene->OnSceneNodeComponentAdded(*this, *component);
                }
            }
            else
            {
                g_Context->Logf("Warning: Failed to add component '%s' to scene node.", componentName);
            }
        }


        return component;
    }

    void SceneNode::RemoveComponentByName(const char* componentName)
    {
        auto component = this->GetComponentByName(componentName);
        if (component != nullptr)
        {
            if (component == this->modelComponent)
            {
                this->modelComponent = nullptr;
            }
            else if (component == this->pointLightComponent)
            {
                this->pointLightComponent = nullptr;
            }
            else if (component == this->spotLightComponent)
            {
                this->spotLightComponent = nullptr;
            }
            else if (component == this->editorMetadataComponent)
            {
                this->editorMetadataComponent = nullptr;
            }

            this->components.Remove(componentName);


            // Post the event after the component has been removed, but before it is deleted.
            if (this->scene != nullptr)
            {
                this->scene->OnSceneNodeComponentRemoved(*this, *component);
            }


            // Important that the component is deleted only after everything else is finished.
            delete component;
        }
    }

    void SceneNode::RemoveAllComponents()
    {
        // I've had too many bugs where I've forgotten to handle the specialised cases properly, so here I'm going
        // to take it slow and do this in a way where we avoid at least one case of this kind of bug.
        //
        // What we'll do is just retrieve the names of all of the attached components, and then iterate over that
        // and remove by name.
        Vector<String> componentNames;
        this->GetAttachedComponentNames(componentNames);

        for (size_t i = 0; i < componentNames.count; ++i)
        {
            this->RemoveComponentByName(componentNames[i].c_str());
        }


        // And now at the end we'll assert that we have no components.
        assert(this->components.count == 0);
    }

    void SceneNode::GetAttachedComponentNames(Vector<String> &output) const
    {
        // First is our specialised cases.
        if (this->modelComponent          != nullptr) { output.PushBack(ModelComponent::Name);          }
        if (this->pointLightComponent     != nullptr) { output.PushBack(PointLightComponent::Name);     }
        if (this->spotLightComponent      != nullptr) { output.PushBack(SpotLightComponent::Name);      }
        if (this->editorMetadataComponent != nullptr) { output.PushBack(EditorMetadataComponent::Name); }


        // Now we can just read the rest from the map.
        for (size_t i = 0; i < this->components.count; ++i)
        {
            output.PushBack(this->components.buffer[i]->key);
        }
    }




    Scene* SceneNode::GetScene()
    {
        return this->scene;
    }
    const Scene* SceneNode::GetScene() const
    {
        return this->scene;
    }

    void SceneNode::_SetScene(Scene* newScene)
    {
        this->scene = newScene;
    }

    void SceneNode::RemoveFromScene()
    {
        if (this->scene != nullptr)
        {
            this->scene->RemoveSceneNode(*this);
        }
    }
    

    Context* SceneNode::GetContext()
    {
        if (this->scene != nullptr) {
            return &this->scene->GetContext();
        }

        return nullptr;
    }



    void SceneNode::SetStatic(bool isStaticIn)
    {
        if (isStaticIn != this->IsStatic())
        {
            if (isStaticIn)
            {
                m_flags = m_flags | Static;
            }
            else
            {
                m_flags = m_flags & ~Static;
            }


            if (!this->EventsLocked())
            {
                this->OnStaticChanged();
            }
        }
    }

    bool SceneNode::IsStatic() const
    {
        return (m_flags & Static) != 0;
    }



    void SceneNode::SetVisible(bool isVisibleIn)
    {
        if (isVisibleIn != this->IsVisible())
        {
            if (isVisibleIn)
            {
                m_flags = m_flags & ~Invisible;
            }
            else
            {
                m_flags = m_flags | Invisible;
            }


            if (!this->EventsLocked())
            {
                this->OnVisibleChanged();
            }
        }
    }

    bool SceneNode::IsVisible(bool recursive) const
    {
        bool locallyVisible = (m_flags & Invisible) == 0;

        if (locallyVisible)
        {
            if (recursive && m_parent != nullptr)
            {
                return m_parent->IsVisible();
            }

            return true;
        }
        else
        {
            return false;
        }
    }


    void SceneNode::DisablePositionInheritance()
    {
        auto worldPosition = this->GetWorldPosition();

        m_flags = m_flags | NoPositionInheritance;
        this->SetWorldPosition(worldPosition);     // This will ensure the position is correct.
    }

    void SceneNode::EnablePositionInheritance()
    {
        auto worldPosition = this->GetWorldPosition();

        m_flags = m_flags & ~NoPositionInheritance;
        this->SetWorldPosition(worldPosition);     // This will ensure the position is correct.
    }

    bool SceneNode::IsPositionInheritanceEnabled() const
    {
        return (m_flags & NoPositionInheritance) == 0;
    }



    void SceneNode::DisableOrientationInheritance()
    {
        auto worldOrientation = this->GetWorldOrientation();

        m_flags = m_flags | NoOrientationInheritance;
        this->SetWorldOrientation(worldOrientation);
    }

    void SceneNode::EnableOrientationInheritance()
    {
        auto worldOrientation = this->GetWorldOrientation();

        m_flags = m_flags & ~NoOrientationInheritance;
        this->SetWorldOrientation(worldOrientation);
    }

    bool SceneNode::IsOrientationInheritanceEnabled() const
    {
        return (m_flags & NoOrientationInheritance) == 0;
    }



    void SceneNode::DisableScaleInheritance()
    {
        auto worldScale = this->GetWorldScale();

        m_flags = m_flags | NoScaleInheritance;
        this->SetWorldScale(worldScale);
    }

    void SceneNode::EnableScaleInheritance()
    {
        auto worldScale = this->GetWorldScale();

        m_flags = m_flags & ~NoOrientationInheritance;
        this->SetWorldScale(worldScale);
    }

    bool SceneNode::IsScaleInheritanceEnabled() const
    {
        return (m_flags & NoScaleInheritance) == 0;
    }



    //////////////////////////////////////////////////
    // Serialization/Deserialization.

    void SceneNode::Serialize(Serializer &serializer, unsigned int flags) const
    {
        // The first scene node chunk, besides SceneObject, is the general attributes. We're going to use an intermediate serializer here
        // because we're writing a string.
        BasicSerializer secondarySerializer;

        if ((flags & NoID) == 0)
        {
            secondarySerializer.Write(this->uniqueID);
        }
        else
        {
            secondarySerializer.Write(static_cast<uint64_t>(0));
        }

        secondarySerializer.WriteString(m_name);
        secondarySerializer.Write(m_position);
        secondarySerializer.Write(m_orientation);
        secondarySerializer.Write(m_scale);
        secondarySerializer.Write(static_cast<uint32_t>(m_flags));


        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_SceneNode_General;
        header.version     = 1;
        header.sizeInBytes = secondarySerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(secondarySerializer.GetBuffer(), header.sizeInBytes);



        // The next chunk contains our components. What we want to in deserialization is the ability to skip over unknown components. To do this,
        // we need to employ a mechanism to allow the deserializer to skip over. The most intuitive way is to just supply a variable containing
        // the size of the component data, which is what we'll be doing.
        //
        // We need to use an intermediary serializer to do this so we can obtain an exact chunk size.
        BasicSerializer componentSerializer;

        Vector<String> componentNames;
        this->GetAttachedComponentNames(componentNames);

        if ((flags & NoEditorMetadataComponent) != 0)
        {
            componentNames.RemoveFirstOccuranceOf(EditorMetadataComponent::Name);
        }


        // We first write the number of components we are saving.
        componentSerializer.Write(static_cast<uint32_t>(componentNames.count));

        // Now we need to loop over every component. Because we are saving the size of the data before the actual data itself, we need to use
        // yet another intermediary serializer so we can retrieve the size.
        for (size_t iComponent = 0; iComponent < componentNames.count; ++iComponent)
        {
            auto component = this->GetComponentByName(componentNames[iComponent].c_str());
            assert(component != nullptr);
            {
                BasicSerializer componentSubSerializer;

                // Special cases.
                if (Strings::Equal(component->GetName(), EditorMetadataComponent::Name))
                {
                    static_cast<const EditorMetadataComponent*>(component)->Serialize(componentSubSerializer, flags);
                }
                else if (Strings::Equal(component->GetName(), ScriptComponent::Name))
                {
                    static_cast<const ScriptComponent*>(component)->Serialize(componentSubSerializer, flags);
                }
                else
                {
                    component->Serialize(componentSubSerializer);
                }

                // We write the name, the size, then the actual data.
                componentSerializer.WriteString(component->GetName());
                componentSerializer.Write(static_cast<uint32_t>(componentSubSerializer.GetBufferSizeInBytes()));
                componentSerializer.Write(componentSubSerializer.GetBuffer(), componentSubSerializer.GetBufferSizeInBytes());
            }
        }



        header.id          = Serialization::ChunkID_SceneNode_Components;
        header.version     = 1;
        header.sizeInBytes = componentSerializer.GetBufferSizeInBytes();

        serializer.Write(header);
        serializer.Write(componentSerializer.GetBuffer(), header.sizeInBytes);
    }

    void SceneNode::Deserialize(Deserializer &deserializer, unsigned int flags)
    {
        String newName;
        auto wasStatic  = this->IsStatic();
        auto wasVisible = this->IsVisible();

        glm::vec3 newPosition;
        glm::quat newOrientation;
        glm::vec3 newScale;

        // This list is used at the end to iterate over and post the OnPostSceneNodeDeserialized() event to each component.
        Vector<Component*> deserializedComponents;

        Serialization::ChunkHeader header;
        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneNode_General);
            {
                switch (header.version)
                {
                case 1:
                    {
                        uint64_t newID;
                        deserializer.Read(newID);

                        if ((flags & NoID) == 0)
                        {
                            this->uniqueID = newID;
                        }

                        deserializer.ReadString(newName);
                        deserializer.Read(newPosition);
                        deserializer.Read(newOrientation);
                        deserializer.Read(newScale);
                        deserializer.Read(reinterpret_cast<uint32_t &>(m_flags));

                        break;
                    }

                default:
                    {
                        g_Context->Logf("Error deserializing SceneNode. The main chunk is an unsupported version (%d).", header.version);
                        deserializer.Seek(header.sizeInBytes);

                        break;
                    }
                }
            }
        }

        deserializer.Read(header);
        {
            assert(header.id == Serialization::ChunkID_SceneNode_Components);
            {
                // We need to remove components that were not part of the serialized data. Otherwise, the scene node won't be in the correct state.
                Vector<String> componentsToRemove;
                this->GetAttachedComponentNames(componentsToRemove);


                uint32_t componentCount;
                deserializer.Read(componentCount);

                for (uint32_t iComponent = 0; iComponent < componentCount; ++iComponent)
                {
                    String name;
                    deserializer.ReadString(name);

                    uint32_t componentDataSizeInBytes;
                    deserializer.Read(componentDataSizeInBytes);

                    auto component = this->AddComponentByName(name.c_str());
                    if (component != nullptr)
                    {
                        // Special cases.
                        if (name == ScriptComponent::Name)
                        {
                            static_cast<ScriptComponent*>(component)->Deserialize(deserializer, (flags & SceneNode::NoScriptPublicVariableOverride) != 0, (flags & SceneNode::NoScriptOnDeserialize) != 0);
                        }
                        else
                        {
                            component->Deserialize(deserializer);
                        }

                        // We need to keep track of the deserialized component for use later on at the end.
                        deserializedComponents.PushBack(component);
                    }
                    else
                    {
                        // If we get here it means the component is unknown to both the engine and the client application. For now we're going to
                        // just skip over the data, but in the future what we'll do is keep hold of it (placing it into a separate list) so that
                        // future serializations can keep hold of the data rather than losing it.
                        deserializer.Seek(componentDataSizeInBytes);
                    }


                    // We don't want to remove this component later on.
                    componentsToRemove.RemoveFirstOccuranceOf(name);
                }


                // Now we need to remove some no-longer-used components.
                for (size_t i = 0; i < componentsToRemove.count; ++i)
                {
                    this->RemoveComponentByName(componentsToRemove[i].c_str());
                }
            }
        }


        // The new name needs to be set with SetName() so that the OnNameChanged event is called.
        this->SetName(newName.c_str());


        // The transformation now needs to be set.
        this->SetPosition(newPosition);
        this->SetOrientation(newOrientation);
        this->SetScale(newScale);


        if (wasStatic != this->IsStatic())
        {
            this->OnStaticChanged();
        }

        if (wasVisible != this->IsVisible())
        {
            this->OnVisibleChanged();
        }


        // We now want to iterate over each component and let them know the scene node has finished deserializing.
        for (size_t iComponent = 0; iComponent < deserializedComponents.count; ++iComponent)
        {
            auto component = deserializedComponents[iComponent];
            assert(component != nullptr);
            {
                component->OnPostSceneNodeDeserialized();
            }
        }
    }

    bool SceneNode::Deserialize_PeekID(Deserializer &deserializer, uint64_t &idOut)
    {
        // The ID should be position right after the header. We're going to read straight into a structure.
        struct ExtendedHeader
        {
            Serialization::ChunkHeader header;
            uint32_t sceneNodeID0;
            uint32_t sceneNodeID1;
        }headerEX;

        if (deserializer.Peek(&headerEX, sizeof(headerEX)) == sizeof(headerEX))
        {
            if (headerEX.header.id == Serialization::ChunkID_SceneNode_General)
            {
                if (headerEX.header.version == 1)
                {
                    idOut = (static_cast<uint64_t>(headerEX.sceneNodeID1) << 32) | headerEX.sceneNodeID0;
                    return true;
                }
            }
        }

        return false;
    }


    void SceneNode::DisableSerialization()
    {
        m_flags = m_flags | NoSerialization;
    }

    void SceneNode::EnableSerialization()
    {
        m_flags = m_flags & ~NoSerialization;
    }

    bool SceneNode::IsSerializationEnabled() const
    {
        bool locallyEnabled = (m_flags & NoSerialization) == 0;

        if (locallyEnabled && m_parent != nullptr)
        {
            return m_parent->IsSerializationEnabled();
        }
        else
        {
            return locallyEnabled;
        }
    }


    void SceneNode::DisableStateStackStaging()
    {
        m_flags = m_flags | NoStateStackStaging;
    }

    void SceneNode::EnableStateStackStaging()
    {
        m_flags = m_flags & ~NoStateStackStaging;
    }

    bool SceneNode::IsStateStackStagingEnabled() const
    {
        bool locallyEnabled = (m_flags & NoStateStackStaging) == 0;

        if (locallyEnabled && m_parent != nullptr)
        {
            return m_parent->IsStateStackStagingEnabled();
        }
        else
        {
            return locallyEnabled;
        }
    }



    void SceneNode::OnAttach(SceneNode &childNode)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnAttach(*this, childNode);
        }
    }

    void SceneNode::OnDetach(SceneNode &childNode)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnDetach(*this, childNode);
        }
    }

    void SceneNode::OnDestroy()
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnDestroy(*this);
        }
    }

    void SceneNode::OnTransform(bool updateDynamicsObject)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnTransform(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeTransform(*this, updateDynamicsObject);
        }


        // When the transformation changes on a parent, the children will also have changed if they have inheritance.
        for (auto i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            if (i->IsPositionInheritanceEnabled() || i->IsOrientationInheritanceEnabled())
            {
                i->OnTransform(updateDynamicsObject);
            }
        }
    }

    void SceneNode::OnScale()
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnScale(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeScale(*this);
        }


        // When the scale changes on a parent, the children will also have changed scales if they have inheritted scaling enabled.
        for (auto i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            if (i->IsScaleInheritanceEnabled())
            {
                i->OnScale();
            }
        }
    }

    void SceneNode::OnLayerChanged(unsigned int prevLayer)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnLayerChanged(*this, prevLayer);
        }
    }


    void SceneNode::OnStaticChanged()
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnStaticChanged(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeStaticChanged(*this);
        }
    }

    void SceneNode::OnVisibleChanged()
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnVisibleChanged(*this);
        }

        if (this->scene != nullptr)
        {
            this->scene->OnSceneNodeVisibleChanged(*this);
        }

        // When the visibility of a node changes, there is a chance the visibility of the child has also changed. We are going to call the OnVisibleChange is also changing.
        for (auto i = this->firstChild; i != nullptr; i = i->nextSibling)
        {
            i->OnVisibleChanged();
        }
    }

    void SceneNode::OnUpdate(double deltaTimeInSeconds)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnUpdate(*this, deltaTimeInSeconds);
        }
    }

    void SceneNode::OnContact(SceneNode &other, const btManifoldPoint &pt)
    {
        for (auto i = this->eventHandlers.root; i != nullptr; i = i->next)
        {
            i->value->OnContact(*this, other, pt);
        }
    }
}

// Private
namespace GT
{
    void SceneNode::LockEvents()
    {
        ++this->eventLockCounter;
    }

    void SceneNode::UnlockEvents()
    {
        --this->eventLockCounter;
    }

    bool SceneNode::EventsLocked() const
    {
        return this->eventLockCounter > 0;
    }
}
