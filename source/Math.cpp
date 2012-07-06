
#include <GTEngine/Math.hpp>

namespace GTEngine
{
    namespace Math
    {
        void CalculateViewMatrix(const glm::vec3 &position, const glm::quat &orientation, glm::mat4 &result)
        {
            // orientation.
            result = glm::mat4_cast(glm::inverse(orientation));

            // position.
            result[3][0] = -position.x;
            result[3][1] = -position.y;
            result[3][2] = -position.z;
        }

        void CalculateTransformMatrix(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale, glm::mat4 &result)
        {
            // orientation.
            result = glm::mat4_cast(orientation);

            // position.
            result[3][0] = position.x;
            result[3][1] = position.y;
            result[3][2] = position.z;

            // scale.
            result[0] *= scale.x;
            result[1] *= scale.y;
            result[2] *= scale.z;
        }

        void CalculateFrustumPlanes(const glm::mat4 &mvp, Plane planes[6], bool normalize)
        {
            planes[0].a = mvp[0][3] - mvp[0][0]; planes[0].b = mvp[1][3] - mvp[1][0]; planes[0].c = mvp[2][3] - mvp[2][0]; planes[0].d = mvp[3][3] - mvp[3][0];
            planes[1].a = mvp[0][3] + mvp[0][0]; planes[1].b = mvp[1][3] + mvp[1][0]; planes[1].c = mvp[2][3] + mvp[2][0]; planes[1].d = mvp[3][3] + mvp[3][0];
            planes[2].a = mvp[0][3] - mvp[0][1]; planes[2].b = mvp[1][3] - mvp[1][1]; planes[2].c = mvp[2][3] - mvp[2][1]; planes[2].d = mvp[3][3] - mvp[3][1];
            planes[3].a = mvp[0][3] + mvp[0][1]; planes[3].b = mvp[1][3] + mvp[1][1]; planes[3].c = mvp[2][3] + mvp[2][1]; planes[3].d = mvp[3][3] + mvp[3][1];
            planes[4].a = mvp[0][3] - mvp[0][2]; planes[4].b = mvp[1][3] - mvp[1][2]; planes[4].c = mvp[2][3] - mvp[2][2]; planes[4].d = mvp[3][3] - mvp[3][2];
            planes[5].a = mvp[0][3] + mvp[0][2]; planes[5].b = mvp[1][3] + mvp[1][2]; planes[5].c = mvp[2][3] + mvp[2][2]; planes[5].d = mvp[3][3] + mvp[3][2];

            if (normalize)
            {
                planes[0].Normalize();
                planes[1].Normalize();
                planes[2].Normalize();
                planes[3].Normalize();
                planes[4].Normalize();
                planes[5].Normalize();
            }
        }
    }
}


// Trajectory.
namespace GTEngine
{
    namespace Math
    {
        namespace Trajectory
        {
            float LaunchAngle(float x, float y, float g, float v)
            {
                float gx =  g * x;
                float x2 =  x * x;
                float v2 =  v * v;
                float v4 = v2 * v2;

                float root = v4 - (g * ((g * x2) + (2.0f * y * v2)));
                if (root < 0.0f)
                {
                    root = 0.0f;
                }

                return glm::atan((v2 - glm::sqrt(root)) / gx);
            }

            float LaunchAngle(const glm::vec3 &start, const glm::vec3 &target, float g, float v)
            {
                float x = glm::min(glm::distance(start, target), MaxDistance(g, v));     // Range
                float y = target.y - start.y;

                return LaunchAngle(x, y, g, v);
            }


            float MaxDistance(float g, float v)
            {
                return (v * v) / g;
            }

            float HeightAtX(float x, float y0, float a, float g, float v)
            {
                float x2     = x * x;
                float vcosa  = v * glm::cos(a);
                float vcosa2 = vcosa * vcosa;

                return y0 + (x * glm::tan(a)) - ((g * x2) / (2.0f * vcosa2));
            }
        }
    }
}