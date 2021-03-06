// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_DefaultScenePhysicsManager
#define GT_DefaultScenePhysicsManager

#include "ScenePhysicsManager.hpp"
#include "Physics.hpp"
#include "AlignedType.hpp"

namespace GT
{
    /// The default physics manager for scenes.
    ///
    /// This can oftern act as the base class for custom managers instead of ScenePhysicsManager.
    class DefaultScenePhysicsManager : public ScenePhysicsManager, public SIMDAlignedType
    {
    public:

        /// Constructor.
        DefaultScenePhysicsManager();

        /// Destructor.
        virtual ~DefaultScenePhysicsManager();


        /// ScenePhysicsManager::AddCollisionObject().
        virtual void AddCollisionObject(CollisionObject &object, short group, short mask);

        /// ScenePhysicsManager::AddRigidBody().
        virtual void AddRigidBody(RigidBody &body, short group, short mask);

        /// ScanePhysicsManager::AddGhostObject().
        virtual void AddGhostObject(GhostObject &object, short group, short mask);


        /// ScenePhysicsManager::RemoveCollisionObject().
        virtual void RemoveCollisionObject(CollisionObject &object);

        /// ScenePhysicsManager::RemoveRigidBody().
        virtual void RemoveRigidBody(RigidBody &object);

        /// ScenePhysicsManager::RemoveGhostObject().
        virtual void RemoveGhostObject(GhostObject &object);



        /// ScenePhysicsManager::UpdateTransform().
        virtual void UpdateTransform(RigidBody &object,   const glm::mat4 &newTransform, short group, short mask);
        virtual void UpdateTransform(GhostObject &object, const glm::mat4 &newTransform);


        
        /// ScenePhysicsManager::AddConstraint().
        virtual void AddConstraint(GenericConstraint &constraint);
        virtual void AddConstraint(ConeTwistConstraint &constraint);
        virtual void AddConstraint(PointToPointConstraint &constraint);

        /// ScenePhysicsManager::RemoveConstraint().
        virtual void RemoveConstraint(GenericConstraint &constraint);
        virtual void RemoveConstraint(ConeTwistConstraint &constraint);
        virtual void RemoveConstraint(PointToPointConstraint &constraint);



        /// ScenePhysicsManager::SetGravity().
        virtual void SetGravity(float x, float y, float z);

        /// ScenePhysicsManager::GetGravity().
        virtual void GetGravity(float &x, float &y, float &z) const;


        /// ScenePhysicsManager::RayTest().
        virtual void RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, btCollisionWorld::RayResultCallback &callback) const;

        /// ScenePhysicsManager::ContactTest().
        virtual void ContactTest(const CollisionObject &object, btCollisionWorld::ContactResultCallback &callback) const;
        virtual void ContactTest(const RigidBody       &object, btCollisionWorld::ContactResultCallback &callback) const;
        virtual void ContactTest(const GhostObject     &object, btCollisionWorld::ContactResultCallback &callback) const;


        /// ScenePhysicsManager::Step().
        virtual void Step(double deltaTimeInSeconds);


        /// ScenePhysicsManager::SetSpeedScale().
        virtual void SetSpeedScale(double scale);



        /// Activates every rigid body in the scene.
        void ActivateAllRigidBodies();


    protected:

        /// The dynamics world containing all of our physics objects.
        DynamicsWorld world;

        /// The speed scale to apply to the entire physics simulation.
        double speedScale;
    };
}

#endif