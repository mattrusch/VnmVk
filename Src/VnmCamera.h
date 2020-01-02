// VnmCamera.h

#pragma once

#include "glm/glm.hpp"

namespace Vnm
{
    class Camera
    {
    public:
        glm::mat4 GetLookAt() const;
        void Move(const glm::vec3& delta);

        void SetPosition(const glm::vec3& position) { mPosition = position; }
        glm::vec3 GetPosition() const { return mPosition; }

    protected:
        Camera() : mPosition(0.0f, 0.0f, 0.0f) {}
        ~Camera() {}

        glm::vec3 mPosition;
    };

    class ThirdPersonCamera : public Camera
    {
    public:
        ThirdPersonCamera() : Camera(), mTarget(0.0f, 0.0f, 1.0f) {}
        ~ThirdPersonCamera() {}

        glm::mat4 GetLookAt() const;
        void Move(const glm::vec3& delta);
        void MoveForward(float distance);
        void MoveForwardConstrainHeight(float distance);
        void MoveRightConstrainHeight(float distance);
        void MoveUp(float distance);

        void Orbit(float hRadians, float vRadians);
        void Zoom(float signedDistance);

        void SetPosition(const glm::vec3& position);
        void SetTarget(const glm::vec3& target);
        void SetTargetConstrainDistance(const glm::vec3& target); // TODO
        glm::vec3 GetTarget() const;

    protected:
        glm::vec3 mTarget;
    };
}
