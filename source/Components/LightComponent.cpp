
#include <GTEngine/Components/LightComponent.hpp>

// PointLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(PointLightComponent, "PointLight");

    PointLightComponent::PointLightComponent(SceneNode &node)
        : Component(node), colour(1.0f, 1.0f, 1.0f), constantAttenuation(1.0f), linearAttenuation(0.0f), quadraticAttenuation(0.0666f)
    {
    }

    void PointLightComponent::Initialise(const glm::vec3 &colour, float constant, float linear, float quadratic)
    {
        this->colour               = colour;
        this->constantAttenuation  = constant;
        this->linearAttenuation    = linear;
        this->quadraticAttenuation = quadratic;
    }

    void PointLightComponent::SetAttenuation(float constant, float linear, float quadratic)
    {
        this->constantAttenuation  = constant;
        this->linearAttenuation    = linear;
        this->quadraticAttenuation = quadratic;
    }

    float PointLightComponent::GetApproximateRadius() const
    {
        double c = this->constantAttenuation;
        double l = this->linearAttenuation;
        double q = this->quadraticAttenuation;

        return static_cast<float>((-l + sqrt(l * l - 4.0 * (c - 100.0) * q)) / (2.0 * q));      // <-- <c - 100.0f> was previously <c - 1000.0f>. Might need to keep experimenting here.
    }
}

// SpotLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(SpotLightComponent, "SpotLight");

    SpotLightComponent::SpotLightComponent(SceneNode &node)
        : Component(node), innerAngle(40.0f), outerAngle(45.0f), colour(1.0f, 1.0f, 1.0f), constantAttenuation(1.0f), linearAttenuation(0.0f), quadraticAttenuation(0.0666f)
    {
    }

    void SpotLightComponent::SetAttenuation(float constant, float linear, float quadratic)
    {
        this->constantAttenuation  = constant;
        this->linearAttenuation    = linear;
        this->quadraticAttenuation = quadratic;
    }

    float SpotLightComponent::GetApproximateLength() const
    {
        double c = this->constantAttenuation;
        double l = this->linearAttenuation;
        double q = this->quadraticAttenuation;

        return static_cast<float>((-l + sqrt(l * l - 4.0 * (c - 100.0) * q)) / (2.0 * q));      // <-- <c - 100.0f> was previously <c - 1000.0f>. Might need to keep experimenting here.
    }    
}



// DirectionalLight
namespace GTEngine
{
    GTENGINE_IMPL_COMPONENT(DirectionalLightComponent, "DirectionalLight");

    DirectionalLightComponent::DirectionalLightComponent(SceneNode &node)
        : Component(node)
    {
    }

    void DirectionalLightComponent::Initialise(const glm::vec3 &colour)
    {
        this->colour = colour;
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
