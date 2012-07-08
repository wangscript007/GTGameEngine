
#include <GTEngine/DefaultScene.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Components/CameraComponent.hpp>
#include <GTEngine/Components/SpriteComponent.hpp>
#include <GTEngine/Components/ModelComponent.hpp>
#include <GTEngine/Components/LightComponent.hpp>
#include <GTEngine/SceneNodeFactory.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/SceneCullingDbvtPolicy.hpp>
#include <GTEngine/CollisionGroups.hpp>
#include <GTEngine/MaterialLibrary.hpp>
#include <GTEngine/Logging.hpp>
#include <GTCore/Strings/Equal.hpp>


namespace GTEngine
{
    /// Dbvt culling policy for the default scene.
    struct DefaultSceneCullingDbvtPolicy : SceneCullingDbvtPolicy
    {
        /// Constructor.
        DefaultSceneCullingDbvtPolicy(SceneViewport &viewport)
            : SceneCullingDbvtPolicy(viewport.GetMVPMatrix()), viewport(viewport)
        {
        }

        /// Destructor.
        ~DefaultSceneCullingDbvtPolicy()
        {
        }


        /// SceneCullingDbvtPolicy::ProcessModel(ModelComponent &)
        void ProcessModel(ModelComponent &modelComponent)
        {
            viewport.AddModelComponent(modelComponent);
        }

        /// SceneCullingDbvtPolicy::ProcessModel(PointLightComponent &)
        void ProcessPointLight(PointLightComponent &pointLightComponent)
        {
            viewport.AddPointLightComponent(pointLightComponent);
        }

        /// SceneCullingDbvtPolicy::ProcessModel(SpotLightComponent &)
        void ProcessSpotLight(SpotLightComponent &spotLightComponent)
        {
            viewport.AddSpotLightComponent(spotLightComponent);
        }



        //////////////////////////////////////////////////////////////////
        // Attributes

        /// The viewport this policy will be retrieving nodes for.
        SceneViewport &viewport;


    private:    // No copying.
        DefaultSceneCullingDbvtPolicy(const DefaultSceneCullingDbvtPolicy &);
        DefaultSceneCullingDbvtPolicy & operator=(const DefaultSceneCullingDbvtPolicy &);
    };
}


// Bullet collision test callbacks.
namespace GTEngine
{
    struct DefaultSceneContactTestCallback : public btCollisionWorld::ContactResultCallback
    {
        ContactTestCallback &callback;


        DefaultSceneContactTestCallback(ContactTestCallback &callback)
            : callback(callback)
        {
            this->m_collisionFilterGroup = callback.collisionGroup;
            this->m_collisionFilterMask  = callback.collisionMask;
        }

        virtual bool needsCollision(btBroadphaseProxy *proxy0) const
		{
            auto collisionObject = static_cast<btCollisionObject*>(proxy0->m_clientObject);
            assert(collisionObject != nullptr);

            auto sceneNode = static_cast<SceneNode*>(collisionObject->getUserPointer());
            if (sceneNode != nullptr)
            {
                return this->callback.NeedsCollision(proxy0->m_collisionFilterGroup, proxy0->m_collisionFilterMask, *sceneNode);
            }

            return false;
		}

        virtual	btScalar addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper* colObj0, int, int, const btCollisionObjectWrapper* colObj1, int, int)
        {
            assert(colObj0 != nullptr);
            assert(colObj1 != nullptr);

            // We assume the user pointer is the scene node.
            auto sceneNodeA = static_cast<SceneNode*>(colObj0->getCollisionObject()->getUserPointer());
            auto sceneNodeB = static_cast<SceneNode*>(colObj1->getCollisionObject()->getUserPointer());

            if (sceneNodeA != nullptr && sceneNodeB != nullptr)
            {
                callback.ProcessCollision(*sceneNodeA, *sceneNodeB, cp);
            }

            return 0.0f;
        }


    private:
        DefaultSceneContactTestCallback(const DefaultSceneContactTestCallback &);
        DefaultSceneContactTestCallback & operator=(const DefaultSceneContactTestCallback &);
    };



    struct DefaultSceneBulletRayResultCallback : public btCollisionWorld::RayResultCallback
    {
        /// A reference to the GTEngine ray test callback object that is used for input and output.
        RayTestCallback &callback;

        /// A pointer to the closest scene node.
        SceneNode* closestSceneNode;


        /// Constructor.
        DefaultSceneBulletRayResultCallback(RayTestCallback &callback)
            : callback(callback), closestSceneNode(nullptr)
        {
            this->m_collisionFilterGroup = callback.collisionGroup;
            this->m_collisionFilterMask  = callback.collisionMask;
        }

        virtual bool needsCollision(btBroadphaseProxy* proxy0) const
		{
            auto collisionObject = static_cast<btCollisionObject*>(proxy0->m_clientObject);
            assert(collisionObject != nullptr);

            auto sceneNode = static_cast<SceneNode*>(collisionObject->getUserPointer());
            if (sceneNode != nullptr)
            {
                return this->callback.NeedsCollision(proxy0->m_collisionFilterGroup, proxy0->m_collisionFilterMask, *sceneNode);
            }

            return false;
		}

        virtual	btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace)
		{
            // The user data of the collision object should be a scene node.
            auto sceneNode = static_cast<SceneNode*>(rayResult.m_collisionObject->getUserPointer());
            if (sceneNode != nullptr)
            {
                this->closestSceneNode = sceneNode;

                // Some stuff for Bullet.
                this->m_closestHitFraction = rayResult.m_hitFraction;
                this->m_collisionObject    = rayResult.m_collisionObject;

                // Now we need to find the world position and normal.
                glm::vec3 worldHitPosition = glm::mix(callback.rayStart, callback.rayEnd, rayResult.m_hitFraction);
                glm::vec3 worldHitNormal;

                if (normalInWorldSpace)
                {
                    worldHitNormal = ToGLMVector3(rayResult.m_hitNormalLocal);
                }
                else
                {
                    worldHitNormal = ToGLMVector3(rayResult.m_collisionObject->getWorldTransform().getBasis() * rayResult.m_hitNormalLocal);
                }

                // Now we can call the corresponding handler on the GTEngine callback.
                this->callback.ProcessResult(*sceneNode, worldHitPosition, worldHitNormal);
            }

            return this->m_closestHitFraction;
		}


    private:    // No copying.
        DefaultSceneBulletRayResultCallback(const DefaultSceneBulletRayResultCallback &);
        DefaultSceneBulletRayResultCallback & operator=(const DefaultSceneBulletRayResultCallback &);
    };
}


namespace GTEngine
{
    DefaultScene::DefaultScene()
        : viewports(), nodes(),
          updateManager(),
          ambientLightComponents(), directionalLightComponents(),
          occluderComponents(),
          dynamicsWorld(), occlusionCollisionWorld(),
          navigationMesh()/*, navigationMeshNode(), navigationMeshModel()*/
    {
        //this->AddSceneNode(this->navigationMeshNode);
        //this->navigationMeshNode.Hide();
    }

    DefaultScene::~DefaultScene()
    {
        while (this->nodes.root != nullptr)
        {
            this->RemoveSceneNode(*this->nodes.root->value);
        }
    }


    void DefaultScene::AddSceneNode(SceneNode &node)
    {
        node.SetScene(this);
    }

    void DefaultScene::RemoveSceneNode(SceneNode &node)
    {
        // We just set the scene on the node to null. This will also apply the changes to children.
        node.SetScene(nullptr);
    }

    void DefaultScene::Update(double deltaTimeInSeconds)
    {
        // First we need to do our pre-update cleanup of caches, dead nodes, etc.
        this->DoPreUpdateClean();

        // Before doing anything we're going to step the dynamics.
        if (!this->IsPaused())
        {
            this->dynamicsWorld.Step(static_cast<btScalar>(deltaTimeInSeconds), 4);

            // Here is where we're going to check for collisions with other rigid bodies.
            int numManifolds = this->dynamicsWorld.GetCollisionDispatcher().getNumManifolds();
	        for (int i = 0; i < numManifolds; i++)
	        {
		        auto contactManifold = this->dynamicsWorld.GetCollisionDispatcher().getManifoldByIndexInternal(i);
		        auto obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
		        auto obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());

                // We'll just use the first contact point for ours. Should probably experiment with looping over all points.
                for (int iContact = 0; iContact < contactManifold->getNumContacts(); ++iContact)
                {
                    btManifoldPoint& pt = contactManifold->getContactPoint(iContact);

                    auto dataA = static_cast<SceneNode*>(obA->getUserPointer());
                    auto dataB = static_cast<SceneNode*>(obB->getUserPointer());

                    if (dataA != nullptr && dataB != nullptr)
                    {
                        dataA->OnContact(*dataB, pt);
                        dataB->OnContact(*dataA, pt);
                    }
                }
            }
        }


        // Now we need to update via the update manager.
        if (!this->IsPaused())
        {
            this->updateManager.Step(deltaTimeInSeconds);
        }

        

        // Now we need to draw everything on every attached viewport.
        for (auto iViewport = this->viewports.root; iViewport != nullptr; iViewport = iViewport->next)
        {
            auto viewport = iViewport->value;
            assert(viewport != nullptr);

            viewport->Render();
        }
    }

    SceneNode * DefaultScene::FindFirstNode(const char* name)
    {
        // Here we check every node and their children.
        for (auto i = this->nodes.root; i != nullptr; i = i->next)
        {
            if (GTCore::Strings::Equal(i->value->GetName(), name))
            {
                return i->value;
            }
            else
            {
                GTEngine::SceneNode *temp = i->value->FindFirstChild(name, true);
                if (temp != nullptr)
                {
                    return temp;
                }
            }
        }

        return nullptr;
    }

    SceneNode * DefaultScene::FindFirstNodeWithComponent(const char* componentName)
    {
        for (auto i = this->nodes.root; i != nullptr; i = i->next)
        {
            if (i->value->HasComponent(componentName))
            {
                return i->value;
            }
            else
            {
                GTEngine::SceneNode *temp = i->value->FindFirstChildWithComponent(componentName, true);
                if (temp != nullptr)
                {
                    return temp;
                }
            }
        }

        return nullptr;
    }


    void DefaultScene::AddViewport(SceneViewport &viewport)
    {
        // The viewport needs to be removed from the previous scene if it has one.
        if (viewport.GetScene() != nullptr)
        {
            viewport.GetScene()->RemoveViewport(viewport);
        }

        this->viewports.Append(&viewport);

        viewport.SetScene(this);
    }

    void DefaultScene::RemoveViewport(SceneViewport &viewport)
    {
        if (viewport.GetScene() == this)
        {
            this->viewports.Remove(this->viewports.Find(&viewport));

            viewport.SetScene(nullptr);
        }
    }


    void DefaultScene::GetAABB(glm::vec3 &min, glm::vec3 &max) const
    {
        // TODO: For some reason using the dynamics world will given too-large a bounding box. Using the occlusion collision world
        //       gives good results. Not sure why dynamicsWorld is not working properly...

        btVector3 tempMin;
        btVector3 tempMax;
        this->occlusionCollisionWorld.GetBroadphase().getBroadphaseAabb(tempMin, tempMax);

        min = ToGLMVector3(tempMin);
        max = ToGLMVector3(tempMax);
    }


    SceneNode* DefaultScene::PickSceneNode(const glm::vec3 &rayStart, const glm::vec3 &rayEnd)
    {
        // This will store the result of our ray-test query.
        btCollisionWorld::ClosestRayResultCallback rayTestResult(GTEngine::ToBulletVector3(rayStart), GTEngine::ToBulletVector3(rayEnd));
        rayTestResult.m_collisionFilterGroup = CollisionGroups::Picking;
        rayTestResult.m_collisionFilterMask  = CollisionGroups::Picking;      // We only want collisions with model picking objects.

        // We use the occlusion world for picking. This will cause objects to be picked based on their mesh volumes. The scene node pointer
        // is stored as the user pointer on the collision object.
        this->occlusionCollisionWorld.RayTest(rayStart, rayEnd, rayTestResult);
        if (rayTestResult.hasHit())
        {
            return static_cast<SceneNode*>(rayTestResult.m_collisionObject->getUserPointer());
        }

        return nullptr;
    }


    SceneNode* DefaultScene::RayTest(const glm::vec3 &rayStart, const glm::vec3 &rayEnd, RayTestCallback &callback)
    {
        // Before creating the Bullet ray test callback object, we first need to set the rayStart and rayEnd attributes of the input callback structure. We do this
        // because the Bullet callback will use them in calculating the world position of interestion points.
        callback.rayStart = rayStart;
        callback.rayEnd   = rayEnd;

        // We need to use our own ray test callback for this.
        DefaultSceneBulletRayResultCallback rayTestResult(callback);
        this->dynamicsWorld.RayTest(rayStart, rayEnd, rayTestResult);

        return rayTestResult.closestSceneNode;
    }


    void DefaultScene::ContactTest(const SceneNode &node, ContactTestCallback &callback)
    {
        // We do the contact test against the nodes proximity component.
        auto proximity = node.GetComponent<GTEngine::ProximityComponent>();
        if (proximity != nullptr)
        {
            callback.collisionGroup = proximity->GetCollisionGroup();
            callback.collisionMask  = proximity->GetCollisionMask();

            DefaultSceneContactTestCallback bulletCallback(callback);
            this->dynamicsWorld.ContactTest(proximity->GetGhostObject(), bulletCallback);
        }
    }



    void DefaultScene::AddVisibleComponents(SceneViewport &viewport)
    {
        // First we need to grab the veiwport's camera. If we don't have one, nothing will be visible...
        auto cameraNode = viewport.GetCameraNode();
        if (cameraNode != nullptr)
        {
            // Now we need to grab the camera component. If we don't have one, nothing will be visible...
            auto camera = cameraNode->GetComponent<CameraComponent>();
            if (camera != nullptr)
            {
                // We're using Bullet for this. Specifically, we're using the Dbvt broadphase of the collision world.
                btDbvtBroadphase &broadphase = this->occlusionCollisionWorld.GetBroadphase();

                glm::mat4 projection = camera->GetProjectionMatrix();
                glm::mat4 view       = camera->GetViewMatrix();
                glm::mat4 mvp        = projection * view;
                glm::vec3 forward    = cameraNode->GetWorldForwardVector();

                Math::Plane planes[6];
                Math::CalculateFrustumPlanes(mvp, planes, true);           // 'false' means to NOT normalize the planes (not needed).


                btVector3 sortaxis(forward.x, forward.y, forward.z);
                btVector3 planes_n[6];
		        btScalar  planes_o[6];

                planes_n[0] = btVector3(planes[0].a, planes[0].b, planes[0].c); planes_o[0] = planes[0].d;
                planes_n[1] = btVector3(planes[1].a, planes[1].b, planes[1].c); planes_o[1] = planes[1].d;
                planes_n[2] = btVector3(planes[2].a, planes[2].b, planes[2].c); planes_o[2] = planes[2].d;
                planes_n[3] = btVector3(planes[3].a, planes[3].b, planes[3].c); planes_o[3] = planes[3].d;
                planes_n[4] = btVector3(planes[4].a, planes[4].b, planes[4].c); planes_o[4] = planes[4].d;
                planes_n[5] = btVector3(planes[5].a, planes[5].b, planes[5].c); planes_o[5] = planes[5].d;

                
                DefaultSceneCullingDbvtPolicy dbvtPolicy(viewport);

                btDbvt::collideOCL(broadphase.m_sets[1].m_root, planes_n, planes_o, sortaxis, 6, dbvtPolicy);
			    btDbvt::collideOCL(broadphase.m_sets[0].m_root, planes_n, planes_o, sortaxis, 6, dbvtPolicy);

                // Below is for only frustum culling.
                //btDbvt::collideKDOP(broadphase.m_sets[1].m_root, planes_n, planes_o, 5, dbvtPolicy);
			    //btDbvt::collideKDOP(broadphase.m_sets[0].m_root, planes_n, planes_o, 5, dbvtPolicy);



                // Here is where we manually add visible components. Ambient and Directional lights are always visible, so they are added.
                
                // Ambient.
                for (auto i = this->ambientLightComponents.root; i != nullptr; i = i->next)
                {
                    auto light = i->value;
                    if (light != nullptr && light->GetNode().IsVisible())
                    {
                        viewport.AddAmbientLightComponent(*light);
                    }
                }

                // Directional.
                for (auto i = this->directionalLightComponents.root; i != nullptr; i = i->next)
                {
                    auto light = i->value;
                    if (light != nullptr && light->GetNode().IsVisible())
                    {
                        viewport.AddDirectionalLightComponent(*light);
                    }
                }
            }
        }
    }

    void DefaultScene::SetGravity(float x, float y, float z)
    {
        this->dynamicsWorld.SetGravity(x, y, z);
    }

    void DefaultScene::GetGravity(float &x, float &y, float &z) const
    {
        this->dynamicsWorld.GetGravity(x, y, z);
    }


    void DefaultScene::SetWalkableHeight(float height)
    {
        this->navigationMesh.SetWalkableHeight(height);
    }
    void DefaultScene::SetWalkableRadius(float radius)
    {
        this->navigationMesh.SetWalkableRadius(radius);
    }
    void DefaultScene::SetWalkableSlopeAngle(float angle)
    {
        this->navigationMesh.SetWalkableSlope(angle);
    }
    void DefaultScene::SetWalkableClimbHeight(float height)
    {
        this->navigationMesh.SetWalkableClimb(height);
    }


    float DefaultScene::GetWalkableHeight() const
    {
        return this->navigationMesh.GetWalkableHeight();
    }
    float DefaultScene::GetWalkableRadius() const
    {
        return this->navigationMesh.GetWalkableRadius();
    }
    float DefaultScene::GetWalkableSlopeAngle() const
    {
        return this->navigationMesh.GetWalkableSlope();
    }
    float DefaultScene::GetWalkableClimbHeight() const
    {
        return this->navigationMesh.GetWalkableClimb();
    }

    void DefaultScene::BuildNavigationMesh()
    {
        this->navigationMesh.Build(*this);

        // TODO: Update the navigation meshe's bounding volume so that it's not erroneously culled.
    }

    void DefaultScene::FindNavigationPath(const glm::vec3 &start, const glm::vec3 &end, GTCore::Vector<glm::vec3> &output)
    {
        this->navigationMesh.FindPath(start, end, output);
    }


    void DefaultScene::ShowNavigationMesh()
    {
        /*
        if (!this->navigationMeshNode.IsVisible())
        {
            auto component = this->navigationMeshNode.GetComponent<GTEngine::ModelComponent>();
            if (component == nullptr)
            {
                this->navigationMeshModel.AttachMesh(&this->navigationMesh.visualVA, MaterialLibrary::Create("engine/materials/simple-emissive.material"));
                this->navigationMeshModel.meshes[0]->GetMaterial()->SetParameter("EmissiveColour", 0.5f, 0.75f, 0.75f);

                component = this->navigationMeshNode.AddModelComponent(&this->navigationMeshModel);
            }

            this->navigationMeshNode.Show();
        }
        */
    }

    void DefaultScene::HideNavigationMesh()
    {
        /*
        if (this->navigationMeshNode.IsVisible())
        {
            this->navigationMeshNode.Hide();
        }
        */
    }



    void DefaultScene::UpdateNode(SceneNode &node, double deltaTimeInSeconds)
    {
        // If the node has a model, and that model is animating, we should step the animation.
        auto modelComponent = node.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            auto model = modelComponent->GetModel();
            if (model != nullptr && model->IsAnimating())
            {
                model->StepAnimation(deltaTimeInSeconds);
            }
        }

        // The node needs to know that it's being updated.
        node.OnUpdate(deltaTimeInSeconds);
    }

    void DefaultScene::DoPreUpdateClean()
    {
    }


    void DefaultScene::AddModelCullingObjects(ModelComponent &modelComponent)
    {
        auto metadata = modelComponent.GetNode().GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)
        {
            auto model = modelComponent.GetModel();
            if (model != nullptr)
            {
                auto &node = modelComponent.GetNode();

                // We first need to ensure we have the objects allocated.
                metadata->AllocateModelCollisionObject(*model, node.GetWorldScale());

                metadata->modelCollisionObject->setCollisionShape(metadata->modelCollisionShape);
                metadata->modelCollisionObject->setUserPointer(&node);

                // At this point the collision objects will have been created and all we need to do is add them to the collision world.
                btTransform transform;
                node.GetWorldTransform(transform);
                metadata->modelCollisionObject->setWorldTransform(transform);

                this->occlusionCollisionWorld.AddCollisionObject(*metadata->modelCollisionObject,
                    CollisionGroups::Picking | CollisionGroups::Model,          // The collision group
                    CollisionGroups::Picking | CollisionGroups::PointLight);    // The collision mask (what this object can collide with)
            }
        }
    }

    void DefaultScene::RemoveModelCullingObjects(ModelComponent &modelComponent)
    {
        auto metadata = modelComponent.GetNode().GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)
        {
            metadata->DeleteModelCollisionObject();
        }
    }

    void DefaultScene::AddPointLightCullingObjects(PointLightComponent &light)
    {
        auto metadata = light.GetNode().GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)
        {
            metadata->AllocatePointLightCollisionObject(light.GetApproximateRadius());

            metadata->pointLightCollisionObject->setCollisionShape(metadata->pointLightCollisionShape);
            metadata->pointLightCollisionObject->setUserPointer(&light.GetNode());


            btTransform transform;
            light.GetNode().GetWorldTransform(transform);
            metadata->pointLightCollisionObject->setWorldTransform(transform);

            this->occlusionCollisionWorld.AddCollisionObject(*metadata->pointLightCollisionObject,
                    CollisionGroups::PointLight,          // The collision group
                    CollisionGroups::Model);              // The collision mask (what this object can collide with)
        }
    }

    void DefaultScene::RemovePointLightCullingObjects(PointLightComponent &light)
    {
        auto metadata = light.GetNode().GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)
        {
            metadata->DeletePointLightCollisionObject();
        }
    }

    void DefaultScene::AddSpotLightCullingObjects(SpotLightComponent &light)
    {
        auto metadata = light.GetNode().GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)
        {
            metadata->AllocateSpotLightCollisionObject(light.GetOuterAngle(), light.GetApproximateLength());

            metadata->spotLightCollisionObject->setCollisionShape(metadata->spotLightCollisionShape);
            metadata->spotLightCollisionObject->setUserPointer(&light.GetNode());


            btTransform transform;
            light.GetNode().GetWorldTransform(transform);
            metadata->spotLightCollisionObject->setWorldTransform(transform);

            this->occlusionCollisionWorld.AddCollisionObject(*metadata->spotLightCollisionObject,
                    CollisionGroups::SpotLight,           // The collision group
                    CollisionGroups::Model);              // The collision mask (what this object can collide with)
        }
    }

    void DefaultScene::RemoveSpotLightCullingObjects(SpotLightComponent &light)
    {
        auto metadata = light.GetNode().GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)
        {
            metadata->DeleteSpotLightCollisionObject();
        }
    }


    void DefaultScene::OnSceneNodeAdded(SceneNode &node)
    {
        this->nodes.Append(&node);

        // We need to add the node to the update manager.
        if ((node.GetFlags() & SceneNode::NoUpdate) == 0)
        {
            if (this->updateManager.NeedsUpdate(node))
            {
                this->updateManager.AddObject(node);
            }
        }

        
        // Here we'll check the lighting components.
        auto ambientLightComponent = node.GetComponent<AmbientLightComponent>();
        if (ambientLightComponent != nullptr)
        {
            this->ambientLightComponents.Append(ambientLightComponent);
        }
        auto directionalLightComponent = node.GetComponent<DirectionalLightComponent>();
        if (directionalLightComponent != nullptr)
        {
            this->directionalLightComponents.Append(directionalLightComponent);
        }


        // If the scene node has a dynamics component, we need to add it's rigid body.
        auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            // The very first thing we're going to do is ensure the scaling has been applied. We do this in OnSceneNodeScaled(), too.
            dynamicsComponent->ApplyScaling(node.GetWorldScale());

            // Now we need to ensure the rigid body is transformed correctly.
            auto &rigidBody = dynamicsComponent->GetRigidBody();

            btTransform transform;
            if (rigidBody.getMotionState() != nullptr)
            {
                rigidBody.getMotionState()->getWorldTransform(transform);
                rigidBody.setWorldTransform(transform);
            }

            if (dynamicsComponent->GetCollisionShape().getNumChildShapes() > 0)
            {
                this->dynamicsWorld.AddRigidBody(rigidBody, dynamicsComponent->GetCollisionGroup(), dynamicsComponent->GetCollisionMask());
            }
            else
            {
                Log("Warning: Attempting to add a dynamics component without collision shapes. The rigid body has not been added to the dynamics world.");
            }
        }

        // Just like DynamicsComponent, if we have proximity component, we need to add that also.
        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            // The very first thing we're going to do is ensure the scaling has been applied. We do this in OnSceneNodeScaled(), too.
            proximityComponent->ApplyScaling(node.GetWorldScale());

            // Now we need to ensure the rigid body is transformed correctly.
            auto &ghostObject = proximityComponent->GetGhostObject();

            btTransform transform;
            node.GetWorldTransform(transform);
            ghostObject.setWorldTransform(transform);

            this->dynamicsWorld.AddGhostObject(ghostObject, proximityComponent->GetCollisionGroup(), proximityComponent->GetCollisionMask());
        }


        // Occluders.
        auto occluderComponent = node.GetComponent<OccluderComponent>();
        if (occluderComponent != nullptr)
        {
            auto &collisionObject = occluderComponent->GetCollisionObject();

            btTransform transform;
            node.GetWorldTransform(transform);
            collisionObject.setWorldTransform(transform);

            this->occlusionCollisionWorld.AddCollisionObject(collisionObject, CollisionGroups::Occluder, CollisionGroups::All);
        }


        // The node needs to have some metadata associated with it. It's important that we set the data pointer straight away so that helper functions
        // can gain access to it easily.
        auto metadata = new SceneNodeMetadata;
        node.SetDataPointer(reinterpret_cast<size_t>(this), metadata);

        // If the node has a model component, we need to create a collision object for the picking/occlusion world.
        auto modelComponent = node.GetComponent<ModelComponent>();
        if (modelComponent != nullptr)
        {
            this->AddModelCullingObjects(*modelComponent);
        }
        
        // We also need to add culling objects for point lights...
        auto pointLightComponent = node.GetComponent<PointLightComponent>();
        if (pointLightComponent != nullptr)
        {
            this->AddPointLightCullingObjects(*pointLightComponent);
        }

        // ... and spot lights.
        auto spotLightComponent = node.GetComponent<SpotLightComponent>();
        if (spotLightComponent != nullptr)
        {
            this->AddSpotLightCullingObjects(*spotLightComponent);
        }
    }

    void DefaultScene::OnSceneNodeRemoved(SceneNode& node)
    {
        this->nodes.Remove(this->nodes.Find(&node));

        // The node must be removed from the update manager.
        this->updateManager.RemoveObject(node);



        // The lighting components needs to be removed if applicable.
        auto ambientLightComponent = node.GetComponent<AmbientLightComponent>();
        if (ambientLightComponent != nullptr)
        {
            this->ambientLightComponents.Remove(this->ambientLightComponents.Find(ambientLightComponent));
        }
        auto directionalLightComponent = node.GetComponent<DirectionalLightComponent>();
        if (directionalLightComponent != nullptr)
        {
            this->directionalLightComponents.Remove(this->directionalLightComponents.Find(directionalLightComponent));
        }


        // TODO: Need to handle cases where we may be in the middle of a simulation...
        // If the node has a dynamics component, the rigid body needs to be removed.
        auto dynamicsComponent = node.GetComponent<DynamicsComponent>();
        if (dynamicsComponent != nullptr)
        {
            this->dynamicsWorld.RemoveRigidBody(dynamicsComponent->GetRigidBody());
        }

        // Same for the proximity component as the dynamics component.
        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            this->dynamicsWorld.RemoveGhostObject(proximityComponent->GetGhostObject());
        }

        // Occluder.
        auto occluderComponent = node.GetComponent<OccluderComponent>();
        if (occluderComponent != nullptr)
        {
            this->occlusionCollisionWorld.RemoveCollisionObject(occluderComponent->GetCollisionObject());
        }

        // If we have metadata, it needs to be removed. this will delete any culling objects.
        auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        delete metadata;

        node.RemoveDataPointer(reinterpret_cast<size_t>(this));
    }



    void DefaultScene::OnSceneNodeTransform(SceneNode &node)
    {
        // We need to update the transformations of the ghost objects in the proximity component, if applicable.
        auto proximityComponent = node.GetComponent<ProximityComponent>();
        if (proximityComponent != nullptr)
        {
            auto &ghostObject = proximityComponent->GetGhostObject();

            auto world = ghostObject.GetWorld();
            if (world != nullptr)
            {
                btTransform transform;
                node.GetWorldTransform(transform);

                ghostObject.setWorldTransform(transform);
                world->GetInternalDynamicsWorld().updateSingleAabb(&ghostObject);
            }
        }

        // Occluders.
        auto occluderComponent = node.GetComponent<OccluderComponent>();
        if (occluderComponent != nullptr)
        {
            auto &collisionObject = occluderComponent->GetCollisionObject();

            auto world = collisionObject.GetWorld();
            if (world != nullptr)
            {
                btTransform transform;
                node.GetWorldTransform(transform);

                collisionObject.setWorldTransform(transform);
                world->UpdateAABB(collisionObject);
            }
        }



        auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)    // Use an assert?
        {
            btTransform transform;
            node.GetWorldTransform(transform);

            if (metadata->modelCollisionObject != nullptr)
            {
                auto world = metadata->modelCollisionObject->GetWorld();
                if (world != nullptr)
                {
                    metadata->modelCollisionObject->setWorldTransform(transform);
                    this->occlusionCollisionWorld.UpdateAABB(*metadata->modelCollisionObject);
                }
            }

            if (metadata->pointLightCollisionObject != nullptr)
            {
                auto world = metadata->pointLightCollisionObject->GetWorld();
                if (world != nullptr)
                {
                    metadata->pointLightCollisionObject->setWorldTransform(transform);
                    this->occlusionCollisionWorld.UpdateAABB(*metadata->pointLightCollisionObject);
                }
            }

            if (metadata->spotLightCollisionObject != nullptr)
            {
                auto world = metadata->spotLightCollisionObject->GetWorld();
                if (world != nullptr)
                {
                    metadata->spotLightCollisionObject->setWorldTransform(transform);
                    this->occlusionCollisionWorld.UpdateAABB(*metadata->spotLightCollisionObject);
                }
            }
        }
    }

    void DefaultScene::OnSceneNodeScale(SceneNode &node)
    {
        auto metadata = node.GetDataPointer<SceneNodeMetadata>(reinterpret_cast<size_t>(this));
        if (metadata != nullptr)    // Use an assert?
        {
            //glm::vec3 scale = node.GetWorldScale();

            if (metadata->modelCollisionObject != nullptr)
            {
                auto modelComponent = node.GetComponent<ModelComponent>();
                if (modelComponent != nullptr)
                {
                    // We need to scale the collision shapes of the models in the occlusion collision world. Unfortunately just setting the scale
                    // with setLocalScaling() doesn't want to work. For now we will just remove and re-add the model culling objects.
                    this->RemoveModelCullingObjects(*modelComponent);
                    this->AddModelCullingObjects(*modelComponent);
                }

                /*
                // We need to scale the collision shapes. Unfortunately the only way I could figure this out is to completely delete
                // the children and recreate them. The loop below does just that.
                // TODO: Have a look into this problem. If it's a bug, it needs to be reported.
                int meshCount = metadata->modelCollisionShape->getNumChildShapes();
                for (int i = 0; i < meshCount; ++i)
                {
                    auto oldChild = static_cast<btGImpactMeshShape*>(metadata->modelCollisionShape->getChildShape(0));
                    metadata->modelCollisionShape->removeChildShapeByIndex(0);

                    auto newChild = new btGImpactMeshShape(oldChild->getMeshInterface());
                    newChild->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
                    newChild->updateBound();

                    metadata->modelCollisionShape->addChildShape(btTransform::getIdentity(), newChild);

                    delete oldChild;
                }

                this->occlusionCollisionWorld.updateSingleAabb(metadata->modelCollisionObject);
                */
            }

            // NOTE: We are not supporting light scaling at the moment. The light radius should be controlled via it's attenuation.
        }

        // The dynamics component needs to have scaling applied.
        auto dynamics = node.GetComponent<DynamicsComponent>();
        if (dynamics != nullptr)
        {
            dynamics->ApplyScaling(node.GetWorldScale());
        }

        // Like dynamics, scaling must be applied to the proximity component.
        auto proximity = node.GetComponent<DynamicsComponent>();
        if (proximity != nullptr)
        {
            proximity->ApplyScaling(node.GetWorldScale());
        }

        // Occluders.
        auto occluder = node.GetComponent<OccluderComponent>();
        if (occluder != nullptr)
        {
            occluder->ApplyScaling(node.GetWorldScale());
        }
    }




    void DefaultScene::OnSceneNodeComponentAttached(SceneNode&, Component& component)
    {
        if (GTCore::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            this->ambientLightComponents.Append(static_cast<AmbientLightComponent*>(&component));
        }
        else if (GTCore::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            this->directionalLightComponents.Append(static_cast<DirectionalLightComponent*>(&component));
        }
        else if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            this->AddPointLightCullingObjects(static_cast<PointLightComponent&>(component));
        }
        else if (GTCore::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            this->AddSpotLightCullingObjects(static_cast<SpotLightComponent&>(component));
        }

        // TODO: Occluders.
    }

    void DefaultScene::OnSceneNodeComponentDetached(SceneNode&, Component& component)
    {
        if (GTCore::Strings::Equal(component.GetName(), AmbientLightComponent::Name))
        {
            this->ambientLightComponents.Remove(this->ambientLightComponents.Find(static_cast<AmbientLightComponent*>(&component)));
        }
        else if (GTCore::Strings::Equal(component.GetName(), DirectionalLightComponent::Name))
        {
            this->directionalLightComponents.Remove(this->directionalLightComponents.Find(static_cast<DirectionalLightComponent*>(&component)));
        }
        else if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            this->RemovePointLightCullingObjects(static_cast<PointLightComponent&>(component));
        }
        else if (GTCore::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            this->RemoveSpotLightCullingObjects(static_cast<SpotLightComponent&>(component));
        }

        // TODO: Occluders.
    }

    void DefaultScene::OnSceneNodeComponentChanged(SceneNode&, Component &component)
    {
        if (GTCore::Strings::Equal(component.GetName(), ModelComponent::Name))
        {
            // The component is a model. We'll need to update the culling information.
            this->RemoveModelCullingObjects(static_cast<ModelComponent&>(component));
            this->AddModelCullingObjects(static_cast<ModelComponent&>(component));
        }
        if (GTCore::Strings::Equal(component.GetName(), PointLightComponent::Name))
        {
            // The component is a point light. We'll need to update the culling information.
            this->RemovePointLightCullingObjects(static_cast<PointLightComponent&>(component));
            this->AddPointLightCullingObjects(static_cast<PointLightComponent&>(component));
        }
        if (GTCore::Strings::Equal(component.GetName(), SpotLightComponent::Name))
        {
            // The component is a spot light. We'll need to update the culling information.
            this->RemoveSpotLightCullingObjects(static_cast<SpotLightComponent&>(component));
            this->AddSpotLightCullingObjects(static_cast<SpotLightComponent&>(component));
        }

        // TODO: Proximity, occluders.
    }
}
