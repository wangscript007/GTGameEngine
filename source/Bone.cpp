
#include <GTEngine/Bone.hpp>

namespace GTEngine
{
    Bone::Bone()
        : parent(nullptr), children(),
          name(), position(), rotation(), scale(1.0f, 1.0f, 1.0f), offsetMatrix()
    {
    }

    Bone::Bone(const Bone &other)
        : parent(nullptr), children(),
          name(other.name), position(other.position), rotation(other.rotation), scale(other.scale), offsetMatrix(other.offsetMatrix)
    {
    }

    Bone::~Bone()
    {
    }

    void Bone::AttachChild(Bone &child)
    {
        if (child.parent != nullptr)
        {
            child.parent->DetachChild(child);
        }

        child.parent = this;

        this->children.PushBack(&child);
    }

    void Bone::DetachChild(Bone &child)
    {
        if (child.parent == this)
        {
            for (size_t i = 0; i < this->children.count; ++i)
            {
                if (this->children[i] == &child)
                {
                    child.parent = nullptr;
                    this->children.Remove(i);

                    break;
                }
            }
        }
    }


    const glm::vec3 & Bone::GetPosition() const
    {
        return this->position;
    }

    const glm::quat & Bone::GetRotation() const
    {
        return this->rotation;
    }

    const glm::vec3 & Bone::GetScale() const
    {
        return this->scale;
    }


    void Bone::SetPosition(const glm::vec3 &newPosition)
    {
        this->position = newPosition;
    }

    void Bone::SetRotation(const glm::quat &newRotation)
    {
        this->rotation = newRotation;
    }

    void Bone::SetScale(const glm::vec3 &newScale)
    {
        this->scale = newScale;
    }


    glm::vec3 Bone::GetAbsolutePosition() const
    {
        if (this->parent != nullptr)
        {
            glm::quat rotation = this->parent->GetAbsoluteRotation();
            glm::vec3 scale    = this->parent->GetAbsoluteScale();

            return this->parent->GetAbsolutePosition() + (rotation * (scale * this->position));
        }

        return this->position;
    }

    glm::quat Bone::GetAbsoluteRotation() const
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetAbsoluteRotation() * this->rotation;
        }

        return this->rotation;
    }

    glm::vec3 Bone::GetAbsoluteScale() const
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetAbsoluteScale() * this->scale;
        }

        return this->scale;
    }

    
    void Bone::GetAbsoluteTransformComponents(glm::vec3 &positionOut, glm::quat &rotationOut, glm::vec3 &scaleOut) const
    {
        if (this->parent != nullptr)
        {
            this->parent->GetAbsoluteTransformComponents(positionOut, rotationOut, scaleOut);

            positionOut = positionOut + (rotationOut * (scaleOut * this->position));
            rotationOut = rotationOut * this->rotation;
            scaleOut    = scaleOut    * this->scale;
        }
        else
        {
            positionOut = this->position;
            rotationOut = this->rotation;
            scaleOut    = this->scale;
        }
    }



    glm::mat4 Bone::GetTransform() const
    {
        return glm::translate(this->position) * glm::mat4_cast(this->rotation) * glm::scale(this->scale);
    }
    
    glm::mat4 Bone::GetAbsoluteTransform() const
    {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        this->GetAbsoluteTransformComponents(position, rotation, scale);
        
        return glm::translate(position) * glm::mat4_cast(rotation) * glm::scale(scale);
    }

    glm::mat4 Bone::GetSkinningTransform() const
    {
        glm::mat4 result(this->offsetMatrix);

        this->AccumulateTransform(result);

        return result;


        //return this->GetAbsoluteTransform() * this->offsetMatrix;
        //return this->offsetMatrix * this->GetAbsoluteTransform();
    }


    void Bone::SetOffsetMatrix(const glm::mat4 &offsetMatrix)
    {
        this->offsetMatrix = offsetMatrix;
    }

    const glm::mat4 & Bone::GetOffsetMatrix() const
    {
        return this->offsetMatrix;
    }


    


    Bone* Bone::GetTopLevelBone()
    {
        if (this->parent != nullptr)
        {
            return this->parent->GetTopLevelBone();
        }

        return this;
    }
}


// PRIVATE
namespace GTEngine
{
    void Bone::AccumulateTransform(glm::mat4 &result) const
    {
        result = this->GetTransform() * result;

        if (this->parent != nullptr)
        {
            this->parent->AccumulateTransform(result);
        }
    }
}






// BoneWithWeights
namespace GTEngine
{
    BoneWithWeights::BoneWithWeights()
        : Bone(), weights()
    {
    }

    BoneWithWeights::BoneWithWeights(const BoneWithWeights &other)
        : Bone(other), weights(other.weights)
    {

    }

    BoneWithWeights::~BoneWithWeights()
    {
    }


    void BoneWithWeights::AddWeight(unsigned int vertexID, float weight)
    {
        this->weights.PushBack(VertexWeight(vertexID, weight));
    }
}
