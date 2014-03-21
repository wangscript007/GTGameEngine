// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneViewport.hpp>
#include <GTEngine/Scene.hpp>

#define GLM_FORCE_RADIANS
#include <glm/gtc/epsilon.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // Unsigned is still unsigned after negation.
#endif

namespace GTEngine
{
    SceneViewport::SceneViewport()
        : scene(nullptr), cameraNodes(),
          width(1), height(1),
          colourBuffer(nullptr)
    {
        cameraNodes[ViewportLayer::Background] = nullptr;
        cameraNodes[ViewportLayer::Main]       = nullptr;
    }

    SceneViewport::~SceneViewport()
    {
    }


    void SceneViewport::SetScene(Scene* scene)
    {
        this->scene = scene;
    }

    Scene* SceneViewport::GetScene()
    {
        return this->scene;
    }


    void SceneViewport::SetCameraNode(SceneNode* cameraNode, int layer)
    {
        assert(layer >= 0);
        assert(layer <= 1);

        this->cameraNodes[layer] = cameraNode;
    }

    SceneNode* SceneViewport::GetCameraNode(int layer)
    {
        assert(layer >= 0);
        assert(layer <= 1);

        return this->cameraNodes[layer];
    }
    const SceneNode* SceneViewport::GetCameraNode(int layer) const
    {
        assert(layer >= 0);
        assert(layer <= 1);

        return this->cameraNodes[layer];
    }


    unsigned int SceneViewport::GetWidth() const
    {
        return this->width;
    }

    unsigned int SceneViewport::GetHeight() const
    {
        return this->height;
    }

    void SceneViewport::Resize(unsigned int newWidth, unsigned int newHeight)
    {
        this->width  = GTCore::Max(newWidth,  1U);
        this->height = GTCore::Max(newHeight, 1U);


        // If the viewport is attached to a scene, we will let it's renderer know that the viewport needs a resize.
        if (this->scene != nullptr)
        {
            this->scene->GetRenderer().OnViewportResized(*this);
        }
    }



    /*** Picking ***/
    void SceneViewport::CalculatePickingRay(int x, int y, glm::vec3 &rayNear, glm::vec3 &rayFar, int layer)
    {
        auto cameraNode = this->GetCameraNode(layer);
        if (cameraNode != nullptr)
        {
            auto viewportCamera = cameraNode->GetComponent<GTEngine::CameraComponent>();
            assert(viewportCamera != nullptr);

            const glm::mat4 &projection = viewportCamera->GetProjectionMatrix();
                  glm::mat4 view        = viewportCamera->GetViewMatrix();

            glm::vec3 winCoords;
            winCoords.x = static_cast<float>(x);
            winCoords.y = static_cast<float>(this->height) - static_cast<float>(y);

            glm::uvec4 viewportCoords(0, 0, this->width, this->height);

            winCoords.z = 0.0f;
            rayNear = glm::unProject(winCoords, view, projection, viewportCoords);

            winCoords.z = 1.0f;
            rayFar  = glm::unProject(winCoords, view, projection, viewportCoords);
        }
    }

    


    /*** Misc Stuff ***/
    glm::vec3 SceneViewport::Project(const glm::vec3 &position, int layer)
    {
        auto cameraNode = this->GetCameraNode(layer);
        if (cameraNode != nullptr)
        {
            // If the point we want to project is at exactly the same position as the camera, we'll end up with a dodgy result from glm::project. Therefore we need to
            // manually handle this case by simply returning the center point of the viewport.
            if (Math::epsilon_equal(cameraNode->GetWorldPosition(), position))
            {
                return glm::vec3(this->width * 0.5f, this->height * 0.5f, 0.0f);
            }
            else
            {
                auto viewportCamera = cameraNode->GetComponent<GTEngine::CameraComponent>();
                assert(viewportCamera != nullptr);

                return glm::project(position, viewportCamera->GetViewMatrix(), viewportCamera->GetProjectionMatrix(), glm::uvec4(0, 0, this->width, this->height));
            }
        }

        return position;
    }

    glm::vec3 SceneViewport::Unproject(const glm::vec3 &position, int layer)
    {
        auto cameraNode = this->GetCameraNode(layer);
        if (cameraNode != nullptr)
        {
            auto viewportCamera = cameraNode->GetComponent<GTEngine::CameraComponent>();
            assert(viewportCamera != nullptr);

            return glm::unProject(position, viewportCamera->GetViewMatrix(), viewportCamera->GetProjectionMatrix(), glm::uvec4(0, 0, this->width, this->height));
        }

        return position;
    }

    glm::mat4 SceneViewport::Get2DProjectionMatrix(bool yDown) const
    {
        if (!yDown)
        {
            return glm::ortho(0.0f, static_cast<float>(this->width), 0.0f, static_cast<float>(this->height));
        }
        else
        {
            return glm::ortho(0.0f, static_cast<float>(this->width), static_cast<float>(this->height), 0.0f);
        }
    }

    glm::mat4 SceneViewport::GetMVPMatrix(int layer) const
    {
        auto cameraNode = this->GetCameraNode(layer);
        if (cameraNode != nullptr)
        {
            auto viewportCamera = cameraNode->GetComponent<GTEngine::CameraComponent>();
            if (viewportCamera != nullptr)
            {
                return viewportCamera->GetProjectionMatrix() * viewportCamera->GetViewMatrix();
            }
        }
        
        return glm::mat4();
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
