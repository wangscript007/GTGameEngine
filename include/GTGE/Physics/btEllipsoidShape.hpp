// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef BT_ELLIPSOID_SHAPE_H
#define BT_ELLIPSOID_SHAPE_H

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include <BulletCollision/CollisionShapes/btConvexInternalShape.h>
#include <BulletCollision/BroadphaseCollision/btBroadphaseProxy.h> // for the types
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

class btEllipsoidShape : public btConvexInternalShape
{
public:

    /// Constructor.
    /// @param semiAxes The three semi-axes of the of the ellipsoid.
    btEllipsoidShape(const btVector3 &semiAxes);

    /// btCollisionShape::calculateLocalInertia()
    virtual void calculateLocalInertia(btScalar mass, btVector3 &inertia) const;

    /// btConvexShape::localGetSupportingVertexWithoutMargin()
    virtual btVector3 localGetSupportingVertexWithoutMargin(const btVector3 &vec) const;
    
    /// btConvexShape::batchedUnitVectorGetSupportingVertexWithoutMargin()
    virtual void batchedUnitVectorGetSupportingVertexWithoutMargin(const btVector3* vectors, btVector3* supportVerticesOut, int numVectors) const;

    /// btConvexShape::setMargin()
    virtual void setMargin(btScalar collisionMargin);

    /// btConvexShape::setLocalScaling()
    virtual void setLocalScaling(const btVector3 &scaling);

    /// btConvexShape::getAabb()
	virtual void getAabb(const btTransform &t, btVector3 &aabbMin, btVector3 &aabbMax) const;

    /// btCollisionShape::getName()
    virtual const char*	getName() const { return "EllipsoidShape"; }
};

#endif
