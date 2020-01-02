// VnmCamera.cpp

#include "VnmCamera.h"
#include "glm/ext.hpp"
#include <cassert>

namespace Vnm
{
    static const glm::vec3 kUp(0.0f, 0.0f, -1.0f);

    void ThirdPersonCamera::SetPosition(const glm::vec3& position)
    {
        Camera::SetPosition(position);
    }

    void ThirdPersonCamera::SetTarget(const glm::vec3& target)
    {
        mTarget = target;
    }

    glm::vec3 ThirdPersonCamera::GetTarget() const
    {
        return mTarget;
    }

    glm::mat4 ThirdPersonCamera::GetLookAt() const
    {
        return glm::lookAt(mPosition, mTarget, kUp);
    }

    void ThirdPersonCamera::Move(const glm::vec3& delta)
    {
        mPosition += delta;
        mTarget += delta;
    }

    void ThirdPersonCamera::MoveForward(float distance)
    {
        glm::vec3 forward = mTarget - mPosition;
        forward = glm::normalize(forward);
        glm::vec3 scaledDirection = forward * distance;
        Move(scaledDirection);
    }

    void ThirdPersonCamera::MoveForwardConstrainHeight(float distance)
    {
        glm::vec3 forward = mTarget - mPosition;
        forward.z = 0.0f;
        forward = glm::normalize(forward);
        glm::vec3 scaledDirection = forward * distance;
        Move(scaledDirection);
    }

    void ThirdPersonCamera::MoveRightConstrainHeight(float distance)
    {
        glm::vec3 forward = mTarget - mPosition;
        glm::vec3 right = glm::cross(forward, kUp);
        right = glm::normalize(right);
        glm::vec3 scaledRight = right * distance;
        Move(scaledRight);
    }

    void ThirdPersonCamera::MoveUp(float distance)
    {
        glm::vec3 scaledUp = kUp * distance;
        Move(scaledUp);
    }

    void ThirdPersonCamera::Orbit(float hRadians, float vRadians)
    {
        glm::vec3 eye = mPosition - mTarget;
        glm::vec3 right = glm::cross(-1.0f * eye, kUp);
        right = glm::normalize(right);
        glm::mat3 hRotation = glm::rotate(glm::mat4(1.0f), hRadians, right);
        glm::mat3 vRotation = glm::rotate(glm::mat4(1.0f), vRadians, kUp);
        glm::mat3 rotation = vRotation * hRotation;
        glm::vec3 newEye = rotation * eye;
        SetPosition(mTarget + newEye);
    }

    void ThirdPersonCamera::Zoom(float signedDistance)
    {
        glm::vec3 forward = mTarget - mPosition;
        float distance = glm::length(forward);
        glm::vec3 normForward = glm::normalize(forward);
        signedDistance = distance - signedDistance < 1.0f ? distance - 1.0f : signedDistance;
        mPosition += normForward * signedDistance;
    }
}
