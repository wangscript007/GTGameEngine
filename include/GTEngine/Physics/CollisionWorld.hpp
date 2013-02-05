// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Physics_CollisionWorld_hpp_
#define __GTEngine_Physics_CollisionWorld_hpp_

#include "BaseCollisionWorld.hpp"

namespace GTEngine
{
    /// Class representing a world for doing collision detection.
    class CollisionWorld : public BaseCollisionWorld
    {
    public:

        struct ClosestRayTestCallback : public btCollisionWorld::ClosestRayResultCallback
        {
            ClosestRayTestCallback(const glm::vec3 &rayFromWorld, const glm::vec3 &rayToWorld)
                : btCollisionWorld::ClosestRayResultCallback(btVector3(rayFromWorld.x, rayFromWorld.y, rayFromWorld.z), btVector3(rayToWorld.x, rayToWorld.y, rayToWorld.z))
            {
            }
        };



    public:

        /// Constructor.
        CollisionWorld();

        /// Destructor.
        virtual ~CollisionWorld();

        /// Retrieves a reference to the internal dynamics world.
              btCollisionWorld & GetInternalWorld()       { return this->world; }
        const btCollisionWorld & GetInternalWorld() const { return this->world; }


    private:

        /// The main dynamics world.
        btCollisionWorld world;
    };
}

#endif