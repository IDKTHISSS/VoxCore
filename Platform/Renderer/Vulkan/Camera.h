#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(glm::vec3 position, float yaw, float pitch, float fov, float aspect, float nearPlane, float farPlane);

    void Move(const glm::vec3& offset);
    void Rotate(float yawOffset, float pitchOffset);
    void UpdateView();

    glm::mat4 GetViewMatrix() const { return m_view; }
    glm::mat4 GetProjectionMatrix() const { return m_projection; }
    glm::vec3 GetPosition() const { return m_position; }

    void SetAspect(float aspect);

private:
    glm::vec3 m_position;
    float m_yaw;
    float m_pitch;
    float m_fov;
    float m_aspect;
    float m_near;
    float m_far;
    glm::mat4 m_view;
    glm::mat4 m_projection;
    void UpdateProjection();
};

