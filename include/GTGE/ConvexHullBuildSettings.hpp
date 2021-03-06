// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ConvexHullBuildSettings
#define GT_ConvexHullBuildSettings

namespace GT
{
    /// Structure containing settings to use when building convex hulls.
    struct ConvexHullBuildSettings
    {
        ConvexHullBuildSettings()
            : compacityWeight(0.001f),
              volumeWeight(500.0f),
              minClusters(1),
              verticesPerCH(100),
              concavity(0.001f),
              smallClusterThreshold(0.001f),
              connectedComponentsDist(0.001f),
              simplifiedTriangleCountTarget(0),
              addExtraDistPoints(false),
              addFacesPoints(false),
              padding0(false), padding1(false)
        {
        }


        float        compacityWeight;
        float        volumeWeight;
        unsigned int minClusters;
        unsigned int verticesPerCH;
        float        concavity;
        float        smallClusterThreshold;
        float        connectedComponentsDist;
        unsigned int simplifiedTriangleCountTarget;
        bool         addExtraDistPoints;
        bool         addFacesPoints;

        // Just some padding for easy file reading and writing.
        bool padding0;
        bool padding1;
    };
}

#endif
