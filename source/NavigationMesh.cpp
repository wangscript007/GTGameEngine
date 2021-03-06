// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/NavigationMesh.hpp>
#include <GTGE/Scene.hpp>
#include <GTGE/GTEngine.hpp>
#include <GTGE/Recast/RecastAlloc.h>

namespace GT
{
    // A function taken from the Recast/Detour source for use when drawing the navigation mesh.
    float distancePtLine2d(const float* pt, const float* p, const float* q)
    {
        float pqx = q[0] - p[0];
        float pqz = q[2] - p[2];
        float dx = pt[0] - p[0];
        float dz = pt[2] - p[2];
        float d = pqx*pqx + pqz*pqz;
        float t = pqx*dx + pqz*dz;
        if (d != 0) t /= d;
        dx = p[0] + t*pqx - pt[0];
        dz = p[2] + t*pqz - pt[2];
        return dx*dx + dz*dz;
    }


    NavigationMesh::NavigationMesh()
        : config(),
          m_mesh(nullptr), detailMesh(nullptr),
          detourNavMesh(nullptr), navMeshQuery(nullptr),
          walkableHeight(2.0f), walkableRadius(0.85f), walkableSlope(27.5f), walkableClimb(0.25f),
          visualVA(Renderer::CreateVertexArray(VertexArrayUsage_Static, VertexFormat::P3T2N3))
    {
        memset(&this->config, 0, sizeof(this->config));
        this->SetCellSize(0.25f);

        this->config.tileSize               = 32;
        this->config.maxSimplificationError = 2.0f;
        this->config.maxVertsPerPoly        = 3;        // Triangles. Good for rendering.
        this->config.detailSampleDist       = 1.0f;
        this->config.detailSampleMaxError   = 1.0f;
        this->config.minRegionArea          = static_cast<int>(rcSqr(8.0f));
        this->config.mergeRegionArea        = static_cast<int>(rcSqr(20.0f));
    }

    NavigationMesh::~NavigationMesh()
    {
        rcFreePolyMesh(m_mesh);
        rcFreePolyMeshDetail(this->detailMesh);
        dtFreeNavMesh(this->detourNavMesh);
        dtFreeNavMeshQuery(this->navMeshQuery);

        Renderer::DeleteVertexArray(this->visualVA);
    }


    bool NavigationMesh::Build(const Scene &scene)
    {
        bool successful = false;

        glm::vec3 aabbMin;
        glm::vec3 aabbMax;
        scene.GetAABB(aabbMin, aabbMax);

        this->config.walkableHeight     = static_cast<int>(ceilf( this->walkableHeight / this->config.ch));
        this->config.walkableRadius     = static_cast<int>(floorf(this->walkableRadius / this->config.cs));
        this->config.walkableSlopeAngle = this->walkableSlope;
        this->config.walkableClimb      = static_cast<int>(ceilf(this->walkableClimb / this->config.ch));
        //this->config.borderSize         = this->config.walkableRadius + 3;
        //this->config.width              = this->config.tileSize + this->config.borderSize * 2;
        //this->config.height             = this->config.tileSize + this->config.borderSize * 2;
        
	    rcVcopy(this->config.bmin, &aabbMin[0]);
	    rcVcopy(this->config.bmax, &aabbMax[0]);
        rcCalcGridSize(this->config.bmin, this->config.bmax, this->config.cs, &this->config.width, &this->config.height);

        rcFreePolyMesh(m_mesh);
        m_mesh = nullptr;

        rcFreePolyMeshDetail(this->detailMesh);
        this->detailMesh = nullptr;


        // The context. This only exists so we can pass it around to the rc* functions. We get a failed assertion if we pass null, unfortunately.
        rcContext context;

        // NOTE: WE WILL CRASH IF THERE IS A STATIC PLANE IN THE SCENE. NEED TO FIX.

        // We need a heightfield...
        auto heightfield = rcAllocHeightfield();
        assert(heightfield != nullptr);

        if (rcCreateHeightfield(&context, *heightfield, this->config.width, this->config.height, this->config.bmin, this->config.bmax, this->config.cs, this->config.ch))
        {
            // Here is where we create the geometric data for the applicable scene nodes. For now we will look only at static meshes, but we will consider
            // obstacles later on.
            auto &nodes = scene.GetSceneNodes();
            for (size_t i = 0; i < nodes.GetCount(); ++i)
            {
                auto node = nodes.GetSceneNodeAtIndex(i);
                assert(node != nullptr);
                {
                    auto dynamics = node->GetComponent<DynamicsComponent>();
                    if (dynamics != nullptr && dynamics->IsNavigationMeshGenerationEnabled())
                    {
                        CollisionShapeMeshBuilder mesh;
                        mesh.Build(dynamics->GetCollisionShape(), dynamics->GetNode().GetWorldTransformWithoutScale());     // Don't want to include the scale in the transform because shapes are already pre-scaled.

                        // With the mesh information retrieved we can now rasterize the mesh on the heightfield.
                        auto vertices      = mesh.GetVertexData();
                        auto vertexCount   = mesh.GetVertexCount();
                        auto indices       = reinterpret_cast<const int*>(mesh.GetIndexData());
                        auto indexCount    = mesh.GetIndexCount();
                        auto triangleCount = indexCount / 3;

                        //auto walkableAreas = new unsigned char[triangleCount];
                        auto walkableAreas = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * triangleCount));
                        memset(walkableAreas, 0, triangleCount * sizeof(unsigned char));

                        rcMarkWalkableTriangles(&context, this->config.walkableSlopeAngle, vertices, vertexCount, indices, triangleCount, walkableAreas);
                        rcRasterizeTriangles(&context, vertices, vertexCount, indices, walkableAreas, triangleCount, *heightfield, this->config.walkableClimb);

                        free(walkableAreas);
                        //delete [] walkableAreas;


                        //auto mesh = dynamics->CreateCollisionShapeMesh(true);   // <-- 'true' means to apply the scene node's transformation.
                        //if (mesh != nullptr)
                        {
                            
                            //delete mesh;
                        }
                    }
                }
            }


            // By this point we will have the triangles rasterized. Now we filter a bunch of stuff.
            rcFilterLowHangingWalkableObstacles(&context, this->config.walkableClimb, *heightfield);
	        rcFilterLedgeSpans(&context, this->config.walkableHeight, this->config.walkableClimb, *heightfield);
	        rcFilterWalkableLowHeightSpans(&context, this->config.walkableHeight, *heightfield);

            // Here is where we create the compact heightfield. After this is done we won't need the original heightfield anymore.
            auto compactHeightfield = rcAllocCompactHeightfield();
            assert(compactHeightfield != nullptr);

            if (rcBuildCompactHeightfield(&context, this->config.walkableHeight, this->config.walkableClimb, *heightfield, *compactHeightfield))
            {
                rcErodeWalkableArea(&context, this->config.walkableRadius, *compactHeightfield);
                rcBuildDistanceField(&context, *compactHeightfield);
                rcBuildRegions(&context, *compactHeightfield, this->config.borderSize, this->config.minRegionArea, this->config.mergeRegionArea);

                // In order to build the polygond mesh we'll need contours.
                auto contours = rcAllocContourSet();
                assert(contours != nullptr);

                if (rcBuildContours(&context, *compactHeightfield, this->config.maxSimplificationError, this->config.maxEdgeLen, *contours))
                {
                    m_mesh = rcAllocPolyMesh();
                    assert(m_mesh != nullptr);

                    if (rcBuildPolyMesh(&context, *contours, this->config.maxVertsPerPoly, *m_mesh))
                    {
                        // Now we create the detail mesh.
                        this->detailMesh = rcAllocPolyMeshDetail();
                        assert(this->detailMesh != nullptr);

                        if (rcBuildPolyMeshDetail(&context, *m_mesh, *compactHeightfield, this->config.detailSampleDist, this->config.detailSampleMaxError, *this->detailMesh))
                        {
                            // Update poly flags from areas.
		                    for (int i = 0; i < m_mesh->npolys; ++i)
		                    {
			                    if (m_mesh->areas[i] == RC_WALKABLE_AREA)
                                {
				                    m_mesh->areas[i] = 1;
                                    m_mesh->flags[i] = 1;
                                }
		                    }

                            // If we've made it here we can now create the detour navigation mesh.
                            dtNavMeshCreateParams params;
		                    memset(&params, 0, sizeof(params));
                            params.cs               = this->config.cs;
		                    params.ch               = this->config.ch;
		                    params.buildBvTree      = true;
		                    params.verts            = m_mesh->verts;
		                    params.vertCount        = m_mesh->nverts;
		                    params.polys            = m_mesh->polys;
		                    params.polyAreas        = m_mesh->areas;
		                    params.polyFlags        = m_mesh->flags;
		                    params.polyCount        = m_mesh->npolys;
		                    params.nvp              = m_mesh->nvp;
                            params.detailMeshes     = this->detailMesh->meshes;
		                    params.detailVerts      = this->detailMesh->verts;
		                    params.detailVertsCount = this->detailMesh->nverts;
		                    params.detailTris       = this->detailMesh->tris;
		                    params.detailTriCount   = this->detailMesh->ntris;
                            params.walkableHeight   = this->walkableHeight;
                            params.walkableRadius   = this->walkableRadius;
                            params.walkableClimb    = this->walkableClimb;
		                    rcVcopy(params.bmin, m_mesh->bmin);
		                    rcVcopy(params.bmax, m_mesh->bmax);
		                    

                            unsigned char* navData     = nullptr;
		                    int            navDataSize = 0;
                            if (dtCreateNavMeshData(&params, &navData, &navDataSize))
                            {
                                dtFreeNavMesh(this->detourNavMesh);
                                this->detourNavMesh = dtAllocNavMesh();
                                assert(this->detourNavMesh != nullptr);

                                dtStatus status = this->detourNavMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
                                if (!dtStatusFailed(status))
                                {
                                    // Finally we're going to create the query object so we can do pathfinding queries.
                                    dtFreeNavMeshQuery(this->navMeshQuery);
                                    this->navMeshQuery = dtAllocNavMeshQuery();
                                    assert(this->navMeshQuery);

                                    if (this->navMeshQuery->init(this->detourNavMesh, 512))
                                    {
                                        successful = true;
                                    }
                                    else
                                    {
                                        g_Context->LogErrorf("NavigationMesh: Failed to init nav mesh query object.");
                                    }
                                }
                                else
                                {
                                    dtFree(navData);
                                    g_Context->LogErrorf("NavigationMesh: Failed to init detail nav mesh.");
                                }
                            }
                            else
                            {
                                dtFree(navData);
                                g_Context->LogErrorf("NavigationMesh: Error creating Detour nav mesh data.");
                            }
                        }
                        else
                        {
                            g_Context->LogErrorf("NavigationMesh: Error creating detail mesh.");
                        }
                    }
                    else
                    {
                        g_Context->LogErrorf("NavigationMesh: Error creating main navigation mesh.");
                    }
                }
                else
                {
                    g_Context->LogErrorf("NavigationMesh: Error creating contours.");
                }


                rcFreeContourSet(contours);
            }
            else
            {
                g_Context->LogErrorf("NavigationMesh: Error creating compact heightfield.");
            }

            rcFreeCompactHeightfield(compactHeightfield);
        }
        else
        {
            g_Context->LogErrorf("NavigationMesh: Error creating heightfield.");
        }

        rcFreeHeightField(heightfield);

        // Here we will rebuild the visual vertex array.
        if (successful)
        {
            this->RebuildVisualVA();
        }

        return successful;
    }



    void NavigationMesh::SetCellSize(float size)
    {
        this->config.cs = size;
        this->config.ch = size;
    }


    void NavigationMesh::SetWalkableHeight(float height)
    {
        this->walkableHeight = height;
    }

    void NavigationMesh::SetWalkableRadius(float radius)
    {
        this->walkableRadius = radius;
    }

    void NavigationMesh::SetWalkableSlope(float angle)
    {
        this->walkableSlope = angle;
    }

    void NavigationMesh::SetWalkableClimb(float height)
    {
        this->walkableClimb = height;
    }


    float NavigationMesh::GetWalkableHeight() const
    {
        return this->walkableHeight;
    }

    float NavigationMesh::GetWalkableRadius() const
    {
        return this->walkableRadius;
    }

    float NavigationMesh::GetWalkableSlope() const
    {
        return this->walkableSlope;
    }

    float NavigationMesh::GetWalkableClimb() const
    {
        return this->walkableClimb;
    }



    bool NavigationMesh::FindPath(const glm::vec3 &start, const glm::vec3 &end, Vector<glm::vec3> &output)
    {
        bool success = false;

        if (this->navMeshQuery != nullptr)
        {
            dtQueryFilter filter;

            glm::vec3 extents(this->walkableRadius * 2.0f, this->walkableHeight * 2.0f, this->walkableRadius * 2.0f);

            // We first need to find the polygons we should start and end on.
            dtPolyRef nearestPolyStart;
            dtPolyRef nearestPolyEnd;
            float     nearestPointStart[3];
            float     nearestPointEnd[3];
            if (!dtStatusFailed(this->navMeshQuery->findNearestPoly(&start[0], &extents[0], &filter, &nearestPolyStart, nearestPointStart)))
            {
                if (!dtStatusFailed(this->navMeshQuery->findNearestPoly(&end[0], &extents[0], &filter, &nearestPolyEnd, nearestPointEnd)))
                {
                    // We have our start and end points, so now we need to find the polys making up a path between the two points.
                    dtPolyRef path[128];
                    int       pathCount;
                    if (!dtStatusFailed(this->navMeshQuery->findPath(nearestPolyStart, nearestPolyEnd, nearestPointStart, nearestPointEnd, &filter, path, &pathCount, 128)))
                    {
                        // We have the path, so now we find a straight line between the path. This will return the points that we can use for output.
                        float* straightPath = new float[pathCount * 3];
                        int straightPathCount;
                        if (!dtStatusFailed(this->navMeshQuery->findStraightPath(nearestPointStart, nearestPointEnd, path, pathCount, straightPath, nullptr, nullptr, &straightPathCount, pathCount)))
                        {
                            // Now we need to append our results to the output list.
                            for (int i = 0; i < straightPathCount; ++i)
                            {
                                auto sourcePoint = straightPath + (i * 3);
                                output.PushBack(glm::vec3(sourcePoint[0], sourcePoint[1], sourcePoint[2]));
                            }

                            // If nearestPolyStart and nearestPolyEnd are both the same, it means we're navigating over the same poly. In this case, straightPathCount will
                            // be set to 1 which means the end point will not be copied over by default. To fix, we just do the appropriate checks here and do it manually.
                            if (nearestPolyStart == nearestPolyEnd && straightPathCount == 1)
                            {
                                output.PushBack(glm::vec3(nearestPointEnd[0], nearestPointEnd[1], nearestPointEnd[2]));
                            }

                            success = true;
                        }

                        delete [] straightPath;
                    }
                }
            }
        }

        return success;
    }


    void NavigationMesh::BuildMeshVisualization(MeshBuilderP3 &mainMesh, MeshBuilderP3 &innerEdgeMesh, MeshBuilderP3 &outerEdgeMesh) const
    {
        mainMesh.Clear();
        innerEdgeMesh.Clear();
        outerEdgeMesh.Clear();

        if (this->detourNavMesh != nullptr)
        {
            for (int i = 0; i < this->detourNavMesh->getMaxTiles(); ++i)
            {
                auto tile = const_cast<const dtNavMesh*>(this->detourNavMesh)->getTile(i);
                if (tile->header != nullptr)
                {
                    // Main mesh.
                    for (int j = 0; j < tile->header->polyCount; ++j)
                    {
                        auto &poly = tile->polys[j];
                        auto &pd   = tile->detailMeshes[j];

                        if (poly.getType() != DT_POLYTYPE_OFFMESH_CONNECTION)
                        {
                            // Main mesh.
                            for (int k = 0; k < pd.triCount; ++k)
		                    {
			                    const unsigned char* t = &tile->detailTris[(pd.triBase+k) * 4];
			                    for (int l = 0; l < 3; ++l)
			                    {
                                    float* vertex = nullptr;

				                    if (t[l] < poly.vertCount)
                                    {
                                        vertex = &tile->verts[poly.verts[t[l]] * 3];
                                    }
				                    else
                                    {
                                        vertex = &tile->detailVerts[(pd.vertBase + t[l] - poly.vertCount) * 3];
                                    }

                                    mainMesh.EmitVertex(glm::vec3(vertex[0], vertex[1], vertex[2]));
			                    }
		                    }


                            // Inner edges mesh.
                            for (int iVert = 0; iVert < poly.vertCount; ++iVert)
                            {
                                const auto v0 = &tile->verts[poly.verts[iVert] * 3];
                                const auto v1 = &tile->verts[poly.verts[(iVert + 1) % poly.vertCount] * 3];

                                for (int k = 0; k < pd.triCount; ++k)
                                {
                                    const auto t = &tile->detailTris[(pd.triBase + k) * 4];
                                    const float* tv[3];

                                    for (int m = 0; m < 3; ++m)
                                    {
                                        if (t[m] < poly.vertCount)
                                        {
                                            tv[m] = &tile->verts[poly.verts[t[m]] * 3];
                                        }
                                        else
                                        {
                                            tv[m] = &tile->detailVerts[(pd.vertBase+(t[m] - poly.vertCount)) * 3];
                                        }
                                    }

                                    for (int m = 0, n = 2; m < 3; n = m++)
                                    {
                                        if (((t[3] >> (n * 2)) & 0x3) != 0)     // Skip inner detail edges.
                                        {
                                            if (distancePtLine2d(tv[n], v0, v1) < 0.01f && distancePtLine2d(tv[m], v0, v1) < 0.01f)
                                            {
                                                innerEdgeMesh.EmitVertex(glm::vec3(tv[n][0], tv[n][1], tv[n][2]));
                                                innerEdgeMesh.EmitVertex(glm::vec3(tv[m][0], tv[m][1], tv[m][2]));
                                            }
                                        }
                                    }
                                }
                            }


                            // Outer edges mesh.
                            for (int iVert = 0; iVert < poly.vertCount; ++iVert)
                            {
                                if (poly.neis[iVert] == 0)
                                {
                                    const auto v0 = &tile->verts[poly.verts[iVert] * 3];
                                    const auto v1 = &tile->verts[poly.verts[(iVert + 1) % poly.vertCount] * 3];

                                    for (int k = 0; k < pd.triCount; ++k)
                                    {
                                        const auto t = &tile->detailTris[(pd.triBase + k) * 4];
                                        const float* tv[3];

                                        for (int m = 0; m < 3; ++m)
                                        {
                                            if (t[m] < poly.vertCount)
                                            {
                                                tv[m] = &tile->verts[poly.verts[t[m]] * 3];
                                            }
                                            else
                                            {
                                                tv[m] = &tile->detailVerts[(pd.vertBase+(t[m] - poly.vertCount)) * 3];
                                            }
                                        }

                                        for (int m = 0, n = 2; m < 3; n = m++)
                                        {
                                            if (((t[3] >> (n * 2)) & 0x3) != 0)     // Skip inner detail edges.
                                            {
                                                if (distancePtLine2d(tv[n], v0, v1) < 0.01f && distancePtLine2d(tv[m], v0, v1) < 0.01f)
                                                {
                                                    outerEdgeMesh.EmitVertex(glm::vec3(tv[n][0], tv[n][1], tv[n][2]));
                                                    outerEdgeMesh.EmitVertex(glm::vec3(tv[m][0], tv[m][1], tv[m][2]));
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    /////////////////////////////////////////////
    // Serialization/Deserialization

    void NavigationMesh::Serialize(Serializer &serializer) const
    {
        BasicSerializer intermediarySerializer;
        Serialization::ChunkHeader header;


        // General variables.
        {
            intermediarySerializer.Clear();
            intermediarySerializer.Write(this->config);
            intermediarySerializer.Write(this->walkableHeight);
            intermediarySerializer.Write(this->walkableRadius);
            intermediarySerializer.Write(this->walkableSlope);
            intermediarySerializer.Write(this->walkableClimb);


            header.id          = Serialization::ChunkID_NavigationMesh_Main;
            header.version     = 1;
            header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
        }


        // The recast poly-mesh.
        if (m_mesh != nullptr)
        {
            intermediarySerializer.Clear();

            intermediarySerializer.Write(static_cast<int32_t>(m_mesh->nverts));
            intermediarySerializer.Write(static_cast<int32_t>(m_mesh->npolys));
            intermediarySerializer.Write(static_cast<int32_t>(m_mesh->maxpolys));
            intermediarySerializer.Write(static_cast<int32_t>(m_mesh->nvp));
            intermediarySerializer.Write(static_cast<float>(m_mesh->bmin[0]));
            intermediarySerializer.Write(static_cast<float>(m_mesh->bmin[1]));
            intermediarySerializer.Write(static_cast<float>(m_mesh->bmin[2]));
            intermediarySerializer.Write(static_cast<float>(m_mesh->bmax[0]));
            intermediarySerializer.Write(static_cast<float>(m_mesh->bmax[1]));
            intermediarySerializer.Write(static_cast<float>(m_mesh->bmax[2]));
            intermediarySerializer.Write(static_cast<float>(m_mesh->cs));
            intermediarySerializer.Write(static_cast<float>(m_mesh->ch));
            intermediarySerializer.Write(static_cast<int32_t>(m_mesh->borderSize));

            intermediarySerializer.Write(m_mesh->verts, sizeof(uint16_t) * m_mesh->nverts * 3);     // <-- 3 components for each vertex (x, y, z).
            intermediarySerializer.Write(m_mesh->polys, sizeof(uint16_t) * m_mesh->maxpolys * 2 * m_mesh->nvp);
            intermediarySerializer.Write(m_mesh->regs,  sizeof(uint16_t) * m_mesh->maxpolys);
            intermediarySerializer.Write(m_mesh->flags, sizeof(uint16_t) * m_mesh->maxpolys);
            intermediarySerializer.Write(m_mesh->areas, sizeof(uint8_t)  * m_mesh->maxpolys);



            header.id          = Serialization::ChunkID_NavigationMesh_RecastPolyMesh;
            header.version     = 1;
            header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
        }


        // The detour nav-mesh
        if (this->detourNavMesh != nullptr)
        {
            intermediarySerializer.Clear();

            int32_t tileCount = 0;
            for (int iTile = 0; iTile < this->detourNavMesh->getMaxTiles(); ++iTile)
            {
                auto tile = const_cast<const dtNavMesh*>(this->detourNavMesh)->getTile(iTile);
                if (tile != nullptr && tile->header != nullptr && tile->dataSize > 0)
                {
                    tileCount += 1;
                }
            }

            auto params = this->detourNavMesh->getParams();
            assert(params != nullptr);
            {
                intermediarySerializer.Write(params->orig[0]);
                intermediarySerializer.Write(params->orig[1]);
                intermediarySerializer.Write(params->orig[2]);
                intermediarySerializer.Write(params->tileWidth);
                intermediarySerializer.Write(params->tileHeight);
                intermediarySerializer.Write(static_cast<int32_t>(params->maxTiles));
                intermediarySerializer.Write(static_cast<int32_t>(params->maxPolys));
            }

            intermediarySerializer.Write(tileCount);
            
            for (int iTile = 0; iTile < this->detourNavMesh->getMaxTiles(); ++iTile)
            {
                auto tile = const_cast<const dtNavMesh*>(this->detourNavMesh)->getTile(iTile);
                if (tile != nullptr && tile->header != nullptr && tile->dataSize > 0)
                {
                    intermediarySerializer.Write(static_cast<uint32_t>(this->detourNavMesh->getTileRef(tile)));
                    intermediarySerializer.Write(static_cast<uint32_t>(tile->dataSize));
                    intermediarySerializer.Write(tile->data, static_cast<size_t>(tile->dataSize));
                }
            }


            header.id          = Serialization::ChunkID_NavigationMesh_DetourNavMesh;
            header.version     = 1;
            header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();

            serializer.Write(header);
            serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
        }


        // Null-terminating chunk.
        {
            header.id          = Serialization::ChunkID_Null;
            header.version     = 1;
            header.sizeInBytes = 0;
            serializer.Write(header);
        }
    }

    bool NavigationMesh::Deserialize(Deserializer &deserializer)
    {
        bool successful = true;

        // We keep looping until we hit the null-terminating chunk.
        Serialization::ChunkHeader header;
        while (deserializer.Read(&header, sizeof(header)) == sizeof(header) && header.id != Serialization::ChunkID_Null)
        {
            switch (header.id)
            {
            case Serialization::ChunkID_NavigationMesh_Main:
                {
                    if (header.version == 1)
                    {
                        //deserializer.Seek(header.sizeInBytes);

                        deserializer.Read(this->config);
                        deserializer.Read(this->walkableHeight);
                        deserializer.Read(this->walkableRadius);
                        deserializer.Read(this->walkableSlope);
                        deserializer.Read(this->walkableClimb);
                    }
                    else
                    {
                        g_Context->Logf("Error deserializing main chunk of navigation mesh. Unsupported version (%d).", header.version);
                        successful = false;
                    }

                    break;
                }

            case Serialization::ChunkID_NavigationMesh_RecastPolyMesh:
                {
                    if (header.version == 1)
                    {
                        //deserializer.Seek(header.sizeInBytes);

                        // Old mesh must be deleted.
                        if (m_mesh != nullptr)
                        {
                            rcFreePolyMesh(m_mesh);
                        }

                        // New mesh must be created.
                        m_mesh = rcAllocPolyMesh();

                        int32_t nverts;
                        int32_t npolys;
                        int32_t maxpolys;
                        int32_t nvp;
                        deserializer.Read(nverts);
                        deserializer.Read(npolys);
                        deserializer.Read(maxpolys);
                        deserializer.Read(nvp);

                        float bmin[3];
                        float bmax[3];
                        deserializer.Read(bmin[0]);
                        deserializer.Read(bmin[1]);
                        deserializer.Read(bmin[2]);
                        deserializer.Read(bmax[0]);
                        deserializer.Read(bmax[1]);
                        deserializer.Read(bmax[2]);

                        float cs;
                        float ch;
                        deserializer.Read(cs);
                        deserializer.Read(ch);

                        int32_t borderSize;
                        deserializer.Read(borderSize);


                        Vector<uint16_t> verts;
                        Vector<uint16_t> polys;
                        Vector<uint16_t> regs;
                        Vector<uint16_t> flags;
                        Vector<uint8_t> areas;

                        verts.Resize(nverts * 3);
                        deserializer.Read(verts.buffer, sizeof(uint16_t) * verts.count);

                        polys.Resize(npolys * 2 * nvp);
                        deserializer.Read(polys.buffer, sizeof(uint16_t) * polys.count);

                        regs.Resize(npolys);
                        deserializer.Read(regs.buffer, sizeof(uint16_t) * regs.count);

                        flags.Resize(npolys);
                        deserializer.Read(flags.buffer, sizeof(uint16_t) * flags.count);

                        areas.Resize(npolys);
                        deserializer.Read(areas.buffer, sizeof(uint8_t) * areas.count);


                        // I'm unaware of a public API for creating a mesh from raw data like this, so we're going to copy the implementation of
                        // rcCopyPolyMesh(). We use the same memory allocation routines as that function.

                        m_mesh->nverts     = static_cast<int>(nverts);
                        m_mesh->npolys     = static_cast<int>(npolys);
                        m_mesh->maxpolys   = static_cast<int>(maxpolys);
                        m_mesh->nvp        = static_cast<int>(nvp);
                        m_mesh->bmin[0]    = bmin[0];
                        m_mesh->bmin[1]    = bmin[1];
                        m_mesh->bmin[2]    = bmin[2];
                        m_mesh->bmax[0]    = bmax[0];
                        m_mesh->bmax[1]    = bmax[1];
                        m_mesh->bmax[2]    = bmax[2];
                        m_mesh->cs         = cs;
                        m_mesh->ch         = ch;
                        m_mesh->borderSize = static_cast<int>(borderSize);

                        m_mesh->verts = static_cast<unsigned short*>(rcAlloc(sizeof(unsigned short) * verts.count, RC_ALLOC_PERM));
                        m_mesh->polys = static_cast<unsigned short*>(rcAlloc(sizeof(unsigned short) * polys.count, RC_ALLOC_PERM));
                        m_mesh->regs  = static_cast<unsigned short*>(rcAlloc(sizeof(unsigned short) * regs.count,  RC_ALLOC_PERM));
                        m_mesh->flags = static_cast<unsigned short*>(rcAlloc(sizeof(unsigned short) * flags.count, RC_ALLOC_PERM));
                        m_mesh->areas = static_cast<unsigned char* >(rcAlloc(sizeof(unsigned char)  * areas.count, RC_ALLOC_PERM));

                        memcpy(m_mesh->verts, verts.buffer, sizeof(unsigned short) * verts.count);
                        memcpy(m_mesh->polys, polys.buffer, sizeof(unsigned short) * polys.count);
                        memcpy(m_mesh->regs,  regs.buffer,  sizeof(unsigned short) * regs.count);
                        memcpy(m_mesh->flags, flags.buffer, sizeof(unsigned short) * flags.count);
                        memcpy(m_mesh->areas, areas.buffer, sizeof(unsigned char)  * areas.count);
                    }
                    else
                    {
                        g_Context->Logf("Error deserializing Recast Poly Mesh chunk of navigation mesh. Unsupported version (%d).", header.version);
                        successful = false;
                    }

                    break;
                }

            case Serialization::ChunkID_NavigationMesh_DetourNavMesh:
                {
                    if (header.version == 1)
                    {
                        //deserializer.Seek(header.sizeInBytes);

                        // Old mesh must be deleted.
                        if (this->detourNavMesh != nullptr)
                        {
                            dtFreeNavMesh(this->detourNavMesh);
                        }

                        // New mesh must be created.
                        this->detourNavMesh = dtAllocNavMesh();


                        dtNavMeshParams params;
                        deserializer.Read(params.orig[0]);
                        deserializer.Read(params.orig[1]);
                        deserializer.Read(params.orig[2]);
                        deserializer.Read(params.tileWidth);
                        deserializer.Read(params.tileHeight);
                        
                        int32_t maxTiles;
                        int32_t maxPolys;
                        deserializer.Read(maxTiles);
                        deserializer.Read(maxPolys);

                        params.maxTiles = static_cast<int>(maxTiles);
                        params.maxPolys = static_cast<int>(maxPolys);

                        if (this->detourNavMesh->init(&params))
                        {
                            int32_t tileCount;
                            deserializer.Read(tileCount);

                            for (int32_t iTile = 0; iTile < tileCount; ++iTile)
                            {
                                uint32_t tileRef;
                                deserializer.Read(tileRef);

                                uint32_t dataSize;
                                deserializer.Read(dataSize);

                                auto data = rcAlloc(dataSize, RC_ALLOC_PERM);
                                assert(data != nullptr);
                                {
                                    deserializer.Read(data, dataSize);

                                    // Everything has been read, now we just read the tile.
                                    this->detourNavMesh->addTile(reinterpret_cast<unsigned char*>(data), static_cast<int>(dataSize), DT_TILE_FREE_DATA, static_cast<dtTileRef>(tileRef), nullptr);
                                }
                            }
                        }
                        else
                        {
                            // Failed to initialize the nav mesh for whatever reason. We'll skip over the rest of the chunk.
                            deserializer.Seek(header.sizeInBytes - 28);
                        }
                    }
                    else
                    {
                        g_Context->Logf("Error deserializing Detour Nav Mesh chunk of navigation mesh. Unsupported version (%d).", header.version);
                        successful = false;
                    }

                    break;
                }


            default:
                {
                    // We don't know the chunk. It needs to be skipped.
                    assert(false);
                    deserializer.Seek(header.sizeInBytes);

                    break;
                }
            }
        }

        return successful;
    }



    /////////////////////////////////////////////
    // Private

    void NavigationMesh::RebuildVisualVA()
    {
        // We use a mesh builder here.
        MeshBuilderP3T2N3 builder;

        for (int i = 0; i < this->detourNavMesh->getMaxTiles(); ++i)
        {
            auto tile = const_cast<const dtNavMesh*>(this->detourNavMesh)->getTile(i);
            if (tile->header != nullptr)
            {
                for (int j = 0; j < tile->header->polyCount; ++j)
                {
                    auto &poly = tile->polys[j];

                    if (poly.getType() != DT_POLYTYPE_OFFMESH_CONNECTION)
                    {
		                auto &pd = tile->detailMeshes[j];

                        for (int k = 0; k < pd.triCount; ++k)
		                {
			                const unsigned char* t = &tile->detailTris[(pd.triBase+k)*4];
			                for (int l = 0; l < 3; ++l)
			                {
                                float* vertex = nullptr;

				                if (t[l] < poly.vertCount)
                                {
                                    vertex = &tile->verts[poly.verts[t[l]] * 3];
                                }
				                else
                                {
                                    vertex = &tile->detailVerts[(pd.vertBase + t[l] - poly.vertCount) * 3];
                                }

                                builder.EmitVertex(glm::vec3(vertex[0], vertex[1], vertex[2]), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			                }
		                }
                    }
                }
            }
        }


        // Now we can set the VA data.
        this->visualVA->SetData(builder.GetVertexData(), builder.GetVertexCount(), builder.GetIndexData(), builder.GetIndexCount());
    }
}

