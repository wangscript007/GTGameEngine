// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

// HACD headers need to be first.
#include <GTEngine/HACD/hacdCircularList.h>
#include <GTEngine/HACD/hacdVector.h>
#include <GTEngine/HACD/hacdICHull.h>
#include <GTEngine/HACD/hacdGraph.h>
#include <GTEngine/HACD/hacdHACD.h>

#include <GTEngine/ConvexHull.hpp>


namespace GTEngine
{
    ConvexHull::ConvexHull()
        : vertices(), indices()
    {
    }

    ConvexHull::ConvexHull(const ConvexHull &other)
        : vertices(other.vertices), indices(other.indices)
    {
    }

    ConvexHull::ConvexHull(const float* verticesIn, size_t vertexCount, const unsigned int* indicesIn, size_t indexCount)
        : vertices(), indices()
    {
        this->Build(verticesIn, vertexCount, indicesIn, indexCount);
    }

    ConvexHull::~ConvexHull()
    {
    }
    
    void ConvexHull::Build(const float* verticesIn, size_t vertexCount, const unsigned int* indicesIn, size_t indexCount)
    {
        this->vertices.Clear();
        this->indices.Clear();

        for (size_t i = 0; i < vertexCount; ++i)
        {
            this->vertices.PushBack(glm::vec3(verticesIn[i * 3 + 0], verticesIn[i * 3 + 1], verticesIn[i * 3 + 2]));
        }

        for (size_t i = 0; i < indexCount; ++i)
        {
            this->indices.PushBack(indicesIn[i]);
        }
    }



    //////////////////////////////////////////////////////////
    // Static Functions

    void ConvexHull::BuildConvexHulls(const VertexArray &va, ConvexHull* &outputArray, size_t &outputCount, ConvexHullBuildSettings &settings)
    {
        outputArray = nullptr;
        outputCount = 0;

        auto vertexData = va.GetVertexDataPtr();
        auto indexData  = va.GetIndexDataPtr();

        auto vertexCount = va.GetVertexCount();
        auto indexCount  = va.GetIndexCount();
        
        if (vertexData != nullptr && indexData != nullptr)
        {
            auto vertexSize     = va.GetFormat().GetSize();
            auto positionOffset = va.GetFormat().GetAttributeOffset(VertexAttribs::Position);

            HACD::HACD* hacd = HACD::CreateHACD(nullptr);    // heh.
            if (hacd != nullptr)
            {
                GTLib::Vector<HACD::Vec3<HACD::Real>> points(vertexCount);
                GTLib::Vector<HACD::Vec3<long>>       triangles(indexCount / 3);

                for (size_t i = 0; i < vertexCount; ++i)
                {
                    auto position = vertexData + (i * vertexSize) + positionOffset;
                    points.PushBack(HACD::Vec3<HACD::Real>(position[0], position[1], position[2]));
                }

                for (size_t i = 0; i < indexCount; i += 3)
                {
                    triangles.PushBack(HACD::Vec3<long>(indexData[i + 0], indexData[i + 1], indexData[i + 2]));
                }

                hacd->SetPoints(&points[0]);
                hacd->SetNPoints(points.count);
                hacd->SetTriangles(&triangles[0]);
                hacd->SetNTriangles(triangles.count);

                hacd->SetCompacityWeight(              settings.compacityWeight);
                hacd->SetVolumeWeight(                 settings.volumeWeight);
                hacd->SetNClusters(                    settings.minClusters);               // Minimum number of clusters to generate.
                hacd->SetNVerticesPerCH(               settings.verticesPerCH);             // CH = Convex-Hull.
                hacd->SetConcavity(                    settings.concavity);
                hacd->SetSmallClusterThreshold(        settings.smallClusterThreshold);
                hacd->SetConnectDist(                  settings.connectedComponentsDist);
                hacd->SetNTargetTrianglesDecimatedMesh(settings.simplifiedTriangleCountTarget);
                hacd->SetAddExtraDistPoints(           settings.addExtraDistPoints);
                hacd->SetAddFacesPoints(               settings.addFacesPoints);


                if (hacd->Compute())
                {
                    auto clusterCount = hacd->GetNClusters();

                    outputArray = new ConvexHull[clusterCount];
                    outputCount = clusterCount;

                    for (size_t iCluster = 0U; iCluster < clusterCount; ++iCluster)
                    {
                        size_t pointCount    = hacd->GetNPointsCH(iCluster);
                        size_t triangleCount = hacd->GetNTrianglesCH(iCluster);

                        auto pointsCH    = new HACD::Vec3<HACD::Real>[pointCount];
				        auto trianglesCH = new HACD::Vec3<long>[triangleCount];
				        hacd->GetCH(iCluster, pointsCH, trianglesCH);

                        auto points = new float[pointCount * 3];
                        for (size_t iPoint = 0; iPoint < pointCount; ++iPoint)
                        {
                            points[iPoint * 3 + 0] = static_cast<float>(pointsCH[iPoint].X());
                            points[iPoint * 3 + 1] = static_cast<float>(pointsCH[iPoint].Y());
                            points[iPoint * 3 + 2] = static_cast<float>(pointsCH[iPoint].Z());
                        }

                        auto indices = new unsigned int[triangleCount * 3];
                        for (size_t iTriangle = 0; iTriangle < triangleCount; ++iTriangle)
                        {
                            indices[iTriangle * 3 + 0] = static_cast<unsigned int>(trianglesCH[iTriangle].X());
                            indices[iTriangle * 3 + 1] = static_cast<unsigned int>(trianglesCH[iTriangle].Y());
                            indices[iTriangle * 3 + 2] = static_cast<unsigned int>(trianglesCH[iTriangle].Z());
                        }

                        outputArray[iCluster].Build(points, pointCount, indices, triangleCount * 3);


                        delete [] pointsCH;
                        delete [] trianglesCH;
                        delete [] points;
                        delete [] indices;
                    }
                }

                HACD::DestroyHACD(hacd);
            }
        }
    }

    void ConvexHull::DeleteConvexHulls(ConvexHull* convexHulls)
    {
        delete [] convexHulls;
    }
}