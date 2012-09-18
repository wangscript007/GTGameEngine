
#include <GTEngine/Components/LightComponent.hpp>
#include <GTEngine/Scene.hpp>

// Lighting utils.
namespace GTEngine
{
    float GetApproximateAttenuationRadius(double c, double l, double q)
    {
        // If 'q' is 0.0, we'll end up with a division by 0 bug. In this case, we'll replace it with a tiny value for approximation.
        if (q == 0.0)
        {
            q = 0.000001;
        }

        return static_cast<float>((-l + sqrt(l * l - 4.0 * (c - 1000.0) * q)) / (2.0 * q));      // <-- <c - 100.0f> was previously <c - 1000.0f>. Might need to keep experimenting here.
    }
}

// PointLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(PointLightComponent, "PointLight");

    PointLightComponent::PointLightComponent(SceneNode &node)
        : Component(node), colour(1.0f, 1.0f, 1.0f), constantAttenuation(1.0f), linearAttenuation(0.0f), quadraticAttenuation(0.0666f),
          castShadows(false)
    {
    }

    void PointLightComponent::SetAttenuation(float constant, float linear, float quadratic)
    {
        this->constantAttenuation  = constant;
        this->linearAttenuation    = linear;
        this->quadraticAttenuation = quadratic;

        // The scene will need to update things like culling information. We need to let it know about this.
        auto scene = this->node.GetScene();
        if (scene != nullptr)
        {
            scene->OnSceneNodeComponentChanged(this->node, *this);
        }
    }

    float PointLightComponent::GetApproximateRadius() const
    {
        return GetApproximateAttenuationRadius(this->constantAttenuation, this->linearAttenuation, this->quadraticAttenuation);
    }

    void PointLightComponent::EnableShadowCasting()
    {
        this->castShadows = true;
    }

    void PointLightComponent::DisableShadowCasting()
    {
        this->castShadows = false;
    }
}

// SpotLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(SpotLightComponent, "SpotLight");

    SpotLightComponent::SpotLightComponent(SceneNode &node)
        : Component(node), innerAngle(40.0f), outerAngle(45.0f), colour(1.0f, 1.0f, 1.0f), constantAttenuation(1.0f), linearAttenuation(0.0f), quadraticAttenuation(0.0666f),
          castShadows(false)
    {
    }

    void SpotLightComponent::SetAttenuation(float constant, float linear, float quadratic)
    {
        this->constantAttenuation  = constant;
        this->linearAttenuation    = linear;
        this->quadraticAttenuation = quadratic;

        // The scene will need to update things like culling information. We need to let it know about this.
        auto scene = this->node.GetScene();
        if (scene != nullptr)
        {
            scene->OnSceneNodeComponentChanged(this->node, *this);
        }
    }

    float SpotLightComponent::GetApproximateLength() const
    {
        double c = this->constantAttenuation;
        double l = this->linearAttenuation;
        double q = this->quadraticAttenuation;

        return static_cast<float>((-l + sqrt(l * l - 4.0 * (c - 1000.0) * q)) / (2.0 * q));      // <-- <c - 100.0f> was previously <c - 1000.0f>. Might need to keep experimenting here.
    }

    void SpotLightComponent::EnableShadowCasting()
    {
        this->castShadows = true;
    }

    void SpotLightComponent::DisableShadowCasting()
    {
        this->castShadows = false;
    }
}



// DirectionalLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(DirectionalLightComponent, "DirectionalLight");

    DirectionalLightComponent::DirectionalLightComponent(SceneNode &node)
        : Component(node),
          castShadows(false)
    {
    }

    void DirectionalLightComponent::Initialise(const glm::vec3 &colour)
    {
        this->colour = colour;
    }

    void DirectionalLightComponent::EnableShadowCasting()
    {
        this->castShadows = true;
    }

    void DirectionalLightComponent::DisableShadowCasting()
    {
        this->castShadows = false;
    }
}

// AmbientLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(AmbientLightComponent, "AmbientLight");

    AmbientLightComponent::AmbientLightComponent(SceneNode &node)
        : Component(node)
    {
    }

    void AmbientLightComponent::Initialise(const glm::vec3 &colour)
    {
        this->colour = colour;
    }
}
