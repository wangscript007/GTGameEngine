// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/GraphicsComponent.hpp>
#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/Assets/Asset.hpp>

namespace GT
{
    SceneNodeComponentTypeID GraphicsComponentDescriptor::TypeID()
    {
        return SceneNodeComponentType_Graphics;
    }

    const char* GraphicsComponentDescriptor::Name()
    {
        return "Graphics";
    }


    GraphicsComponentDescriptor::GraphicsComponentDescriptor()
    {
    }

    GraphicsComponentDescriptor::~GraphicsComponentDescriptor()
    {
    }

    SceneNodeComponentTypeID GraphicsComponentDescriptor::GetTypeID() const
    {
        return GraphicsComponentDescriptor::TypeID();
    }

    const char* GraphicsComponentDescriptor::GetName() const
    {
        return GraphicsComponentDescriptor::Name();
    }


    SceneNodeComponent* GraphicsComponentDescriptor::CreateComponent() const
    {
        return new GraphicsComponent(*this);
    }

    void GraphicsComponentDescriptor::DeleteComponent(SceneNodeComponent* pComponent) const
    {
        delete pComponent;
    }







    GraphicsComponent::GraphicsComponent(const GraphicsComponentDescriptor &descriptor)
        : SceneNodeComponent(descriptor),
          m_pGraphicsResourceManager(nullptr),
          m_pModelResource(nullptr),
          m_pModelObject(nullptr)
    {
    }

    GraphicsComponent::~GraphicsComponent()
    {
        this->UnsetModel();
    }


    bool GraphicsComponent::SetModel(const char* modelPath, GraphicsAssetResourceManager &graphicsResourceManager)
    {
        this->UnsetModel();

        if (modelPath != nullptr)
        {
            return this->SetModel(graphicsResourceManager.LoadModel(modelPath), graphicsResourceManager);
        }
        
        return false;
    }

    bool GraphicsComponent::SetModel(Asset* pModelAsset, GraphicsAssetResourceManager &graphicsResourceManager)
    {
        if (pModelAsset != nullptr && pModelAsset->GetClass() == AssetClass_Model)
        {
            return this->SetModel(graphicsResourceManager.Load(pModelAsset), graphicsResourceManager);
        }
        
        return false;
    }

    bool GraphicsComponent::SetModel(GraphicsAssetResource *pModelResource, GraphicsAssetResourceManager &graphicsResourceManager)
    {
        if (pModelResource != nullptr)
        {
            m_pModelResource           = reinterpret_cast<GraphicsAssetResource_Model*>(pModelResource);
            m_pGraphicsResourceManager = &graphicsResourceManager;
                
            // All good. We don't create the graphics objects for each mesh until AddModelToGraphicsWorld() is called.
            return true;
        }

        return false;
    }

    void GraphicsComponent::UnsetModel()
    {
        if (m_pModelResource != nullptr && m_pGraphicsResourceManager != nullptr)
        {
            // Make sure the model is removed from the graphics world first.
            this->RemoveModelFromGraphicsWorld();

            // Now we can unload the resources.
            m_pGraphicsResourceManager->Unload(m_pModelResource);
            m_pModelResource           = nullptr;
            m_pGraphicsResourceManager = nullptr;
        }
    }


    void GraphicsComponent::AddModelToGraphicsWorld(const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        if (m_pModelObject == nullptr && m_pModelResource != nullptr)
        {
            m_pModelObject = new GraphicsModelObject;
            m_pModelObject->Initialize(m_pModelResource, &m_pGraphicsResourceManager->GetGraphicsWorld(), position, rotation, scale);
        }
    }

    void GraphicsComponent::RemoveModelFromGraphicsWorld()
    {
        if (m_pModelObject != nullptr)
        {
            m_pModelObject->Uninitialize();

            delete m_pModelObject;
            m_pModelObject = nullptr;
        }
    }

    void GraphicsComponent::SetModelTransform(const vec4 &position, const quat &rotation, const vec4 &scale)
    {
        if (m_pModelObject != nullptr)
        {
            m_pModelObject->SetTransform(position, rotation, scale);
        }
    }
}
