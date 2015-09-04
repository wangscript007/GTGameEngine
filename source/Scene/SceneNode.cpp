// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNode.hpp>
#include <GTGameEngine/Scene/SceneNodeComponent.hpp>
#include <cassert>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    SceneNode::SceneNode(Scene &scene)
        : SceneNode(scene, 0)
    {
    }

    SceneNode::SceneNode(Scene &scene, uint64_t id)
        : m_id(id),
          m_name(),
          m_tag(),
          m_components(),
          m_componentCount(0),
          m_flags(0),
          m_padding0(0),
          m_position(0.0f, 0.0f, 0.0f, 0.0f),
          m_rotation(0.0f, 0.0f, 0.0f, 1.0f),
          m_scale(1.0f, 1.0f, 1.0f, 0.0f),
          m_scene(scene)
    {
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_name)[i] = 0;
        }

        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_tag)[i] = 0;
        }

        for (int i = 0; i < GT_MAX_SCENE_NODE_COMPONENTS; ++i)
        {
            m_components[0] = nullptr;
        }
    }

    

    Scene & SceneNode::GetScene()
    {
        return m_scene;
    }

    uint64_t SceneNode::GetID() const
    {
        return m_id;
    }


    const char* SceneNode::GetName() const
    {
        return m_name;
    }

    void SceneNode::SetName(const char* newName)
    {
        // Pad with zero.
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_name)[i] = 0;
        }

        // Copy without going past 31 characters.
        for (int i = 0; i < 31 && newName[i] != '\0'; ++i)
        {
            m_name[i] = newName[i];
        }
    }


    const char* SceneNode::GetTag() const
    {
        return m_tag;
    }

    void SceneNode::SetTag(const char* newTag)
    {
        // Pad with zero.
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_tag)[i] = 0;
        }

        // Copy without going past 31 characters.
        for (int i = 0; i < 31 && newTag[i] != '\0'; ++i)
        {
            m_tag[i] = newTag[i];
        }
    }


    unsigned int SceneNode::GetComponentCount() const
    {
        return m_componentCount;
    }

    SceneNodeComponent* SceneNode::GetComponentByIndex(unsigned int index)
    {
        if (index < m_componentCount)
        {
            return m_components[index];
        }
        else
        {
            return nullptr;
        }
    }

    SceneNodeComponent* SceneNode::GetComponentByTypeID(SceneNodeComponentTypeID typeID)
    {
        for (uint16_t i = 0; i < m_componentCount; ++i)
        {
            auto pComponent = m_components[i];
            assert(pComponent != nullptr);
            {
                if (pComponent->GetTypeID() == typeID)
                {
                    return pComponent;
                }
            }
        }

        return nullptr;
    }

    const SceneNodeComponent* SceneNode::GetComponentByTypeID(SceneNodeComponentTypeID typeID) const
    {
        return const_cast<SceneNode*>(this)->GetComponentByTypeID(typeID);
    }


    bool SceneNode::AttachComponent(SceneNodeComponent &component)
    {
        if (m_componentCount < GT_MAX_SCENE_NODE_COMPONENTS)
        {
            // Check a component of the same type is not already attached.
            if (this->GetComponentByTypeID(component.GetTypeID()) == nullptr)
            {
                m_components[m_componentCount] = &component;
                m_componentCount += 1;

                return true;
            }
            else
            {
                // A component of the same type is already attached.
                return false;
            }
        }
        else
        {
            // No available slots. Max GT_MAX_SCENE_NODE_COMPONENTS.
            return false;
        }
    }

    void SceneNode::DetachComponent(SceneNodeComponent &component)
    {
        for (uint16_t i = 0; i < m_componentCount; ++i)
        {
            auto pComponent = m_components[i];
            if (pComponent == &component)
            {
                this->DetachComponentByIndex(i);
                break;
            }
        }
    }

    void SceneNode::DetachComponentByTypeID(SceneNodeComponentTypeID typeID)
    {
        for (uint16_t i = 0; i < m_componentCount; ++i)
        {
            auto pComponent = m_components[i];
            assert(pComponent != nullptr);
            {
                if (pComponent->GetTypeID() == typeID)
                {
                    this->DetachComponentByIndex(i);
                    break;
                }
            }
        }
    }

    void SceneNode::DetachComponentByIndex(unsigned int index)
    {
        if (index < m_componentCount)
        {
            m_componentCount -= 1;


            // Move everything down.
            for (uint16_t iComponent = static_cast<uint16_t>(index); iComponent < m_componentCount - 1; ++iComponent)
            {
                m_components[iComponent] = m_components[iComponent + 1];
            }

            // Make sure the last item is null.
            m_components[m_componentCount] = nullptr;
        }
    }


    void SceneNode::SetStatic(bool isStatic)
    {
        if (isStatic)
        {
            this->_SetFlags(this->_GetFlags() | _IsStatic);
        }
        else
        {
            this->_SetFlags(this->_GetFlags() & ~_IsStatic);
        }
    }

    bool SceneNode::IsStatic() const
    {
        return (this->_GetFlags() & _IsStatic) != 0;
    }


    vec4 SceneNode::GetPosition() const
    {
        return m_position;
    }
    vec2 SceneNode::GetPosition2D() const
    {
        return vec2(m_position.x, m_position.y);
    }

    quat SceneNode::GetRotation() const
    {
        return m_rotation;
    }

    vec4 SceneNode::GetScale() const
    {
        return m_scale;
    }
    vec2 SceneNode::GetScale2D() const
    {
        return vec2(m_scale.x, m_scale.y);
    }


    void SceneNode::SetPosition(const vec4 &position)
    {
        m_position = position;
        m_flags |= _PositionChanged;
    }
    void SceneNode::SetPosition2D(const vec2 &position)
    {
        this->SetPosition(vec4(position.x, position.y, m_position.z, m_position.w));
    }

    void SceneNode::SetRotation(const quat &rotation)
    {
        m_rotation = rotation;
        m_flags |= _RotationChanged;
    }

    void SceneNode::SetScale(const vec4 &scale)
    {
        m_scale = scale;
        m_flags |= _ScaleChanged;
    }
    void SceneNode::SetScale2D(const vec2 &scale)
    {
        this->SetScale(vec4(scale.x, scale.y, m_scale.z, m_scale.w));
    }


    vec4 SceneNode::GetForward() const
    {
        return m_rotation * vec4(0, 0, -1, 1);
    }
    vec2 SceneNode::GetForward2D() const
    {
        vec4 result = GetForward();
        return vec2(result.x, result.y);
    }

    vec4 SceneNode::GetRight() const
    {
        return m_rotation * vec4(1, 0, 0, 1);
    }
    vec2 SceneNode::GetRight2D() const
    {
        vec4 result = GetRight();
        return vec2(result.x, result.y);
    }

    vec4 SceneNode::GetUp() const
    {
        return m_rotation * vec4(0, 1, 0, 1);
    }
    vec2 SceneNode::GetUp2D() const
    {
        vec4 result = GetUp();
        return vec2(result.x, result.y);
    }



    ///////////////////////////////////
    // Internal Use Only

    uint16_t SceneNode::_GetFlags() const
    {
        return m_flags;
    }

    void SceneNode::_SetFlags(uint16_t flags)
    {
        m_flags = flags;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
