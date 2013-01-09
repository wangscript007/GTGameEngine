
#ifndef __GTEngine_DynamicsComponent_hpp_
#define __GTEngine_DynamicsComponent_hpp_

#include "../Component.hpp"
#include "../Physics.hpp"
#include "../VertexArrayLibrary.hpp"
#include "../ConvexHull.hpp"
#include "../Serialization.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    /// Class representing a dynamics component.
    ///
    /// The dynamics component is used to do collision detection and physics. 
    GTENGINE_DECL_COMPONENT_START(DynamicsComponent)
    public:

        /// Constructor.
        DynamicsComponent(SceneNode &node);

        /// Destructor.
        ~DynamicsComponent();


        /// Adds a cube collision shape.
        void AddBoxCollisionShape(float halfX, float halfY, float halfZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        
        /// Adds a sphere collision shape.
        void AddSphereCollisionShape(float radius, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Adds an elipsoid collision shape.
        void AddEllipsoidCollisionShape(float radiusX, float radiusY, float radiusZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Adds a cylinder collision shape.
        void AddCylinderXCollisionShape(float halfX, float halfY, float halfZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCylinderYCollisionShape(float halfX, float halfY, float halfZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCylinderZCollisionShape(float halfX, float halfY, float halfZ, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Adds a capsule collision shape.
        void AddCapsuleXCollisionShape(float radius, float length, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCapsuleYCollisionShape(float radius, float length, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);
        void AddCapsuleZCollisionShape(float radius, float length, float offsetX = 0.0f, float offsetY = 0.0f, float offsetZ = 0.0f);

        /// Adds a static plane shape. This does not move with the node.
        void AddStaticPlaneShape(float a, float b, float c, float d);


        /// Adds a convex hull shape.
        ///
        /// @param points     [in] A pointer to the buffer containing the points to build the hull from.
        /// @param pointCount [in] The number of points in the buffer.
        /// @param stride     [in] The vertex stride.
        void AddConvexHullShape(const float* points, size_t pointCount, size_t stride, float margin = 0.0);
        void AddConvexHullShape(const ConvexHull &convexHull, float margin = 0.0);

        /// Sets the collision shapes to the convex hulls of the given model.
        ///
        /// @param model  [in] A reference to the model whose convex hulls are being added.
        /// @param margin [in] The margin to apply. This default to 0.0, but it may change later if we have issues.
        ///
        /// @remarks
        ///     This removes all already attached shapes before adding the new ones.
        void SetCollisionShapesToModelConvexHulls(const Model &model, float margin = 0.0f);

        /// Sets the collision shapes to the convex hulls of the model of the attached Model component, if any.
        ///
        /// @param margin [in] The margin to apply.
        ///
        /// @remarks
        ///     This removes all already attached shapes before adding the new ones.
        void SetCollisionShapesToModelConvexHulls(float margin = 0.0f);


        /// Removes every collision shape.
        void RemoveAllCollisionShapes();

        /// Removes the collision shape at the given index.
        ///
        /// @param index [in] The index of the shape to remove.
        void RemoveCollisionShapeAtIndex(size_t index);


        /// Retrieves the number of collision shapes current attached to the component.
        size_t GetCollisionShapeCount() const { return static_cast<size_t>(this->collisionShape->getNumChildShapes()); }

        /// Retrieves a pointer to the shape at the given index.
        ///
        /// @param index [in] The index of the shape to retrieve.
              btCollisionShape* GetCollisionShapeAtIndex(size_t index);
        const btCollisionShape* GetCollisionShapeAtIndex(size_t index) const;


        /// Determines whether or not the convex hulls of a model is being used for the collision shape.
        bool IsUsingConvexHullsFromModel() const { return this->usingConvexHullsOfModel; }


        /// Sets the offset to use for the shape at the given index.
        ///
        /// @param index  [in] The index of the shape whose offset is being changed.
        /// @param offset [in] The new offset.
        void SetCollisionShapeOffset(size_t index, float offsetX, float offsetY, float offsetZ);

        /// Sets the half extents of the box shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not a box, this method will fail and return false.
        bool SetBoxCollisionShapeHalfExtents(size_t index, float halfX, float halfY, float halfZ);

        /// Sets the radius of the sphere shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not a sphere, this method will fail and return false.
        bool SetSphereCollisionShapeRadius(size_t index, float radius);

        /// Sets the radius of the ellipsoid shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not an ellipsoid, this method will fail and return false.
        bool SetEllipsoidCollisionShapeRadius(size_t index, float radiusX, float radiusY, float radiusZ);

        /// Sets the half extents of the cylinder shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not a cylinder, this method will fail and return false.
        bool SetCylinderCollisionShapeHalfExtents(size_t index, float halfX, float halfY, float halfZ);

        /// Sets the half extents of the capsule shape at the given index.
        ///
        /// @param index [in] The index of the shape.
        ///
        /// @remarks
        ///     If the shape at the given index is not a capsule, this method will fail and return false.
        bool SetCapsuleCollisionShapeSize(size_t index, float radius, float height);



        /// Sets the mass of the rigid body. If this is 0.0, it will be static. Defaults to 0.0.
        void SetMass(float mass);

        /// Retrieves the mass of the rigid body.
        float GetMass() const;

        /// Sets whether or not the component is kinematic.
        void IsKinematic(bool isKinematic);

        /// Returns whether or not the rigid body is kinematic.
        bool IsKinematic() const;

        /// Determines whether or not the rigid body is static.
        bool IsStatic() const;


        /// Sets the friction of the object.
        ///
        /// @param friction [in] The new friction value to use with the object. Larger values means more friction.
        void SetFriction(float friction);

        /// Retrieves the friction of the object.
        float GetFriction() const;

        /// Sets the restitution of the object (bounciness).
        ///
        /// @param restitution [in] The new restitution. Larger values means more bounciness.
        void SetRestitution(float restitution);

        /// Retrieves the restitution of the object.
        float GetRestitution() const;


        /// Sets the linear and angular damping.
        void SetDamping(float linear, float angular);

        /// Retrieves the linear damping.
        float GetLinearDamping() const;

        /// Retrieves the angular damping.
        float GetAngularDamping() const;


        /// Sets the collision group and masks to use with the scene node.
        ///
        /// @param group [in] The collision group(s) this scene node is part of.
        /// @param mask  [in] A mask of collision groups that this scene node will collide with.
        ///
        /// @remarks
        ///     Note that changing the collision filter requires that the internal dynamics objects are removed from the internal dynamics world
        ///     and then re-added. Thus, don't call this method too frequently.
        void SetCollisionFilter(short group, short mask);

        /// Retrieves the collision group as specified in SetCollisionFilter().
        short GetCollisionGroup() const { return this->collisionGroup; }
        
        /// Retrieves the collision mask as specified in SetCollisionFilter().
        short GetCollisionMask() const { return this->collisionMask; }


        /// Retrieves a reference to the rigid body.
              RigidBody & GetRigidBody()       { return *this->rigidBody; }
        const RigidBody & GetRigidBody() const { return *this->rigidBody; }


        /// Applies a scale to the rigid body.
        void ApplyScaling(float x, float y, float z);
        void ApplyScaling(const glm::vec3 &scale) { this->ApplyScaling(scale.x, scale.y, scale.z); }


        /// Applies the scaling from the scene node.
        void ApplySceneNodeScaling();

        /// Applies the transformation of the scene node to the rigid body.
        void ApplySceneNodeTransformation();



        /// Sets the linear velocity of the physics object.
        void SetLinearVelocity(float x, float y, float z);
        void SetLinearVelocity(const glm::vec3 &velocity) { this->SetLinearVelocity(velocity.x, velocity.y, velocity.z); }

        /// Retrieves the linear velocity of the object.
        glm::vec3 GetLinearVelocity() const;


        /// Sets the angular velocity of the physics object.
        void SetAngularVelocity(float x, float y, float z);
        void SetAngularVelocity(const glm::vec3 &velocity) { this->SetAngularVelocity(velocity.x, velocity.y, velocity.z); }

        /// Retrieves the angular velocity of the object.
        glm::vec3 GetAngularVelocity() const;
        

        /// Sets the linear factor.
        void SetLinearFactor(float x, float y, float z);
        void SetLinearFactor(const glm::vec3 &factor) { this->SetLinearFactor(factor.x, factor.y, factor.z); }

        /// Retrieves the linear factor.
        glm::vec3 GetLinearFactor() const;


        /// Sets the angular factor.
        void SetAngularFactor(float factor);
        void SetAngularFactor(float x, float y, float z);
        void SetAngularFactor(const glm::vec3 &factor) { this->SetLinearFactor(factor.x, factor.y, factor.z); }

        /// Retrieves the angular factor.
        glm::vec3 GetAngularFactor() const;


        /// Sets the gravity of the object.
        void SetGravity(float x, float y, float z);
        void SetGravity(const glm::vec3 &gravity) { this->SetGravity(gravity.x, gravity.y, gravity.z); }

        /// Retrieves the gravity of the object.
        glm::vec3 GetGravity() const;

        /// Applies the objects gravity.
        void ApplyGravity();


        /// Applies a central force to the body.
        void ApplyCentralForce(float x, float y, float z);
        void ApplyCentralForce(const glm::vec3 &force) { this->ApplyCentralForce(force.x, force.y, force.z); }

        /// Applies torque to the body.
        void ApplyTorque(float x, float y, float z);
        void ApplyTorque(const glm::vec3 &torque) { this->ApplyTorque(torque.x, torque.y, torque.z); }


        /// Applies an impulse to the body.
        void ApplyImpulse(float x, float y, float z, float relPosX, float relPosY, float relPosZ);
        void ApplyImpulse(const glm::vec3 &impulse, const glm::vec3 &relativePosition) { this->ApplyImpulse(impulse.x, impulse.y, impulse.z, relativePosition.x, relativePosition.y, relativePosition.z); }

        /// Applies a central impulse to the body.
        void ApplyCentralImpulse(float x, float y, float z);
        void ApplyCentralImpulse(const glm::vec3 &impulse) { this->ApplyCentralImpulse(impulse.x, impulse.y, impulse.z); }

        /// Applies a torque impulse.
        void ApplyTorqueImpulse(float x, float y, float z);
        void ApplyTorqueImpulse(const glm::vec3 &impulse) { this->ApplyCentralImpulse(impulse.x, impulse.y, impulse.z); }


        /// Disables deactivation of the rigid body.
        void DisableDeactivation();

        /// Enables deactivation of the rigid body.
        void EnableDeactivation();

        /// Determines whether or not deactivation is enabled.
        bool IsDeactivationEnabled() const;

        /// Activates a deactivated rigid body.
        void Activate();


        /// Disables navigation mesh generation.
        void DisableNavigationMeshGeneration();

        /// Enables navigation mesh generation.
        void EnableNavigationMeshGeneration();

        /// Determines whether or not the component should be used with navigation mesh generation.
        bool IsNavigationMeshGenerationEnabled() const;



        /// Creates a vertex array containing the collision mesh vertices.
        ///
        /// @param applyNodeTransform [in] Whether or not the scene node's world transformation should be applied.
        ///
        /// @remarks
        ///     This only contains vertex positions. No texture coordinates. No normals.
        ///     @par
        ///     This is useful for use by Recast (for navigation mesh generation).
        VertexArray* CreateCollisionShapeMesh(bool applyNodeTransform = false);


        /// Retrieves a reference to the collision shape.
        btCompoundShape & GetCollisionShape() { return *this->collisionShape; }




        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);



    private:

        /// Helper method for generically adding a collision shape.
        void AddCollisionShape(btCollisionShape* shape, float offsetX, float offsetY, float offsetZ);

        /// Updates the mass of the rigid body. This will remove the the rigid body from the world, if it has one, and then re-add it.
        void UpdateMass();


    private:

        /// The collision shape we're using. This starts life as an empty shape. When the first collision shape is added it is turned into
        /// a compound shape. We need to use a compound shape in order to get offsets working properly.
        btCompoundShape* collisionShape;

        /// The motion state of the body. This will be different depending on whether or not the rigid body is kinematic.
        SceneNodeMotionState motionState;

        /// The rigid body associated with this component.
        RigidBody* rigidBody;

        
        /// The mass of the rigid body. Defaults to 0 (static).
        float mass;

        /// Keeps track of whether or not the object is kinematic.
        bool isKinematic;

        /// Keeps track of whether or not this object should be used for navigation mesh generation. Defaults to true.
        bool useWithNavigationMesh;

        /// Keeps track of whether or not this object is using the convex hulls from the attached model for it's convex hulls.
        bool usingConvexHullsOfModel;


        /// The collision group the scene node is part of. This is set with SetCollisionFilter(). Defaults to 1.
        short collisionGroup;

        /// The collision mask for controlling the collision groups the scene node can collide with. This is set with SetCollisionFilter(). Defaults to -1 (collides with everything).
        short collisionMask;



    GTENGINE_DECL_COMPONENT_END()
}

#endif