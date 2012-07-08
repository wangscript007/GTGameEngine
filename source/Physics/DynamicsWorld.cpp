
#include <GTEngine/Physics.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialiser list.
#endif

namespace GTEngine
{
    DynamicsWorld::DynamicsWorld()
        : solver(),
          world(&dispatcher, &broadphase, &solver, &configuration),
          ghostPairCallback()
    {
        this->broadphase.getOverlappingPairCache()->setInternalGhostPairCallback(&this->ghostPairCallback);
    }

    DynamicsWorld::~DynamicsWorld()
    {
        // Now we need to remove all of our rigid bodies. We go backwards here to avoid reshuffling of the internal buffer.
        for (int i = this->world.getNumCollisionObjects() - 1; i >= 0; --i)
        {
            auto obj  = this->world.getCollisionObjectArray()[i];
            auto body = btRigidBody::upcast(obj);

            if (body != nullptr)
            {
                this->RemoveRigidBody(*body);
            }
            else if (obj != nullptr)
            {
                this->RemoveCollisionObject(*obj);
            }
        }

        // We need to remove the ghost callback. Not doing this will result in a crash... Also, this must be done AFTER
        // removing all of the collision objects.
        this->broadphase.getOverlappingPairCache()->setInternalGhostPairCallback(nullptr);
    }


    void DynamicsWorld::AddRigidBody(btRigidBody &bodyIn, short group, short mask)
    {
        auto body = dynamic_cast<RigidBody*>(&bodyIn);
        if (body != nullptr)
        {
            this->AddRigidBody(*body, group, mask);
        }
        else
        {
            this->world.addRigidBody(&bodyIn, group, mask);
        }
    }

    void DynamicsWorld::AddRigidBody(RigidBody &body, short group, short mask)
    {
        auto prevWorld = body.getWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->RemoveRigidBody(body);
        }

        body.setWorld(this);
        this->world.addRigidBody(&body, group, mask);
    }

    void DynamicsWorld::RemoveRigidBody(btRigidBody &bodyIn)
    {
        auto body = dynamic_cast<RigidBody*>(&bodyIn);
        if (body != nullptr)
        {
            this->RemoveRigidBody(*body);
        }
        else
        {
            this->world.removeRigidBody(&bodyIn);
        }
    }

    void DynamicsWorld::RemoveRigidBody(RigidBody &body)
    {
        body.setWorld(nullptr);
        this->world.removeRigidBody(&body);
    }


    void DynamicsWorld::AddGhostObject(btGhostObject &ghostIn, short group, short mask)
    {
        auto ghost = dynamic_cast<GhostObject*>(&ghostIn);
        if (ghost != nullptr)
        {
            this->AddGhostObject(*ghost, group, mask);
        }
        else
        {
            this->world.addCollisionObject(&ghostIn, group, mask);
        }
    }

    void DynamicsWorld::AddGhostObject(GhostObject &ghost, short group, short mask)
    {
        auto prevWorld = ghost.getWorld();
        if (prevWorld != nullptr)
        {
            prevWorld->RemoveGhostObject(ghost);
        }

        ghost.setWorld(this);
        this->world.addCollisionObject(&ghost, group, mask);
    }

    void DynamicsWorld::RemoveGhostObject(btGhostObject &ghostIn)
    {
        auto ghost = dynamic_cast<GhostObject*>(&ghostIn);
        if (ghost != nullptr)
        {
            this->RemoveGhostObject(*ghost);
        }
        else
        {
            this->world.removeCollisionObject(&ghostIn);
        }
    }

    void DynamicsWorld::RemoveGhostObject(GhostObject &ghost)
    {
        ghost.setWorld(nullptr);
        this->world.removeCollisionObject(&ghost);
    }


    void DynamicsWorld::Step(double timeStep, int maxSubSteps, double fixedTimeStep)
    {
        this->world.stepSimulation(static_cast<btScalar>(timeStep), maxSubSteps, static_cast<btScalar>(fixedTimeStep));
    }


    void DynamicsWorld::SetGravity(float x, float y, float z)
    {
        this->world.setGravity(btVector3(x, y, z));
    }

    void DynamicsWorld::GetGravity(float &x, float &y, float &z) const
    {
        btVector3 gravity = this->world.getGravity();
        x = gravity.x();
        y = gravity.y();
        z = gravity.z();
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
