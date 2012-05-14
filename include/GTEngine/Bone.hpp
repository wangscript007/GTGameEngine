
#ifndef __GTEngine_Bone_hpp_
#define __GTEngine_Bone_hpp_

#include <GTCore/Vector.hpp>
#include <GTCore/String.hpp>
#include "Math.hpp"

// Temp for testing.
#include "Logging.hpp"

namespace GTEngine
{
    /// Class representing a bone in a skeleton.
    class Bone
    {
    public:

        /// Structure reprsenting a vertex weight.
        struct VertexWeight
        {
            VertexWeight(unsigned int vertexIDIn, float weightIn)
                : vertexID(vertexIDIn), weight(weightIn)
            {
            }

            unsigned int vertexID;
            float        weight;
        };



    public:

        /// Constructor.
        Bone();

        /// Copy constructor.
        ///
        /// @remarks
        ///     This does NOT attach the new bone to a parent, nor does it attach any children. Thus, the new bone will need to be attached to a parent manually.
        Bone(const Bone &other);

        /// Destructor.
        ~Bone();


        /// Attaches a child.
        void AttachChild(Bone &child);

        /// Detaches a child.
        void DetachChild(Bone &child);


        /// Retrieves a pointer to the parent bone, or nullptr if the bone does not have a parent.
        Bone* GetParent() { return this->parent; }

        /// Retrieves a direct reference to the internal vector containing the pointers to the child bones.
              GTCore::Vector<Bone*> & GetChildren()       { return this->children; }
        const GTCore::Vector<Bone*> & GetChildren() const { return this->children; }


        /// Sets the name of the bone.
        void SetName(const char* newName) { this->name = newName; }

        /// Retrieves the name of the bone.
        const char* GetName() const { return this->name.c_str(); }



        /// Sets the transform of the bone.
        //void SetTransform(const glm::mat4 &transform);

        /// Retrieves the transform of the bone.
        //const glm::mat4 & GetTransform() const;


        /// Retrieves the relative position of the bone.
        const glm::vec3 & GetPosition() const;

        /// Retrieves the relative rotation of the bone.
        const glm::quat & GetRotation() const;

        /// Retrieves the relative scale of the bone.
        const glm::vec3 & GetScale() const;


        /// Sets the relative position of the bone.
        void SetPosition(const glm::vec3 &position);

        /// Sets the relative rotation of the bone.
        void SetRotation(const glm::quat &rotation);

        /// Sets the relative scale of the bone.
        void SetScale(const glm::vec3 &scale);


        /// Retrieves the absolute position of the bone.
        glm::vec3 GetAbsolutePosition() const;

        /// Retrieves the absolute orientation of the bone.
        glm::quat GetAbsoluteRotation() const;

        /// Retrieves the absolute scale of the bone.
        glm::vec3 GetAbsoluteScale() const;


        /// Optimized method for retrieving the absolute transformation components (position, rotation, scale).
        void GetAbsoluteTransformComponents(glm::vec3 &position, glm::quat &rotation, glm::vec3 &scale) const;


        /// Retrieves the local transformation matrix.
        glm::mat4 GetTransform() const;

        /// Retrieves the absolute transformation of the bone.
        glm::mat4 GetAbsoluteTransform() const;

        /// Retrieves the transformation to use when applying vertex blending.
        glm::mat4 GetAbsoluteSkinningTransform() const;



        /// Sets the offset matrix of the bone.
        void SetOffsetMatrix(const glm::mat4 &transform);

        /// Retrieves the offset matrix of the bone.
        const glm::mat4 & GetOffsetMatrix() const;



        /// Sets the vertex weights.
        void AddWeight(unsigned int vertexID, float weight);


        /// Determines whether or not the bone is empty (has any weights).
        bool IsEmpty() const { return this->weights.count == 0; }


        /// Retrieves the top level bone. If this bone is already the top level bone, <this> will be returned.
        Bone* GetTopLevelBone();


        /// Retrieves the number of weights this bone has.
        size_t GetWeightCount() const { return this->weights.count; }

        /// Retrieves a reference to the weight at the given index.
              VertexWeight & GetWeight(size_t index)       { return this->weights[index]; }
        const VertexWeight & GetWeight(size_t index) const { return this->weights[index]; }


        

        // For testing.
        void LogInfo()
        {
            GTEngine::Log("Bone: %s/%s", name.c_str(), (parent != nullptr) ? parent->GetName() : "none");

            for (size_t i = 0; i < this->children.count; ++i)
            {
                this->children[i]->LogInfo();
            }
        }


    private:

        void AccumulateTransform(glm::mat4 &result) const;


    private:

        /// The parent bone. If this is null then the bone is the root bone in the skeleton.
        Bone* parent;

        /// The children.
        GTCore::Vector<Bone*> children;


        /// The name of the bone.
        GTCore::String name;


        /// The bones transformation.
        //glm::mat4 transform;

        /// The relative position of the bone.
        glm::vec3 position;

        /// The relative rotation of the bone.
        glm::quat rotation;

        /// The relative scale of the bone.
        glm::vec3 scale;


        /// The offset matrix for this bone.
        glm::mat4 offsetMatrix;


        /// The list of vertex weights.
        GTCore::Vector<VertexWeight> weights;
    };
}


#endif
