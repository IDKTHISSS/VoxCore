// Created by GitHub Copilot on 17.07.2025
#pragma once
#include "Core/ECS/BaseClasses/UBaseComponent.h"
#include <vector>
#include <random>
#include <glm/glm.hpp>

class UMeshComponent : public UBaseComponent {
public:
    UCLASS(UMeshComponent);
    UMeshComponent() = default;
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    std::vector<glm::vec3> colors;

    void SetCubeMesh() {
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned>(std::time(nullptr)));
            seeded = true;
        }

        // Рандомный размер куба от 0.2 до 1.0
        float size = 0.2f + static_cast<float>(std::rand()) / RAND_MAX * 0.8f;

        vertices = {
            { -0.5f, -0.5f, -0.5f },
            {  0.5f, -0.5f, -0.5f },
            {  0.5f,  0.5f, -0.5f },
            { -0.5f,  0.5f, -0.5f },
            { -0.5f, -0.5f,  0.5f },
            {  0.5f, -0.5f,  0.5f },
            {  0.5f,  0.5f,  0.5f },
            { -0.5f,  0.5f,  0.5f }
        };

        // Умножаем каждую вершину на size
        for (auto& v : vertices) {
            v *= size;
        }

        indices = {
            0,2,1, 0,3,2, // -Z
            4,5,6, 4,6,7, // +Z
            0,1,5, 0,5,4, // -Y
            2,3,7, 2,7,6, // +Y
            0,4,7, 0,7,3, // -X
            1,2,6, 1,6,5  // +X
        };

        auto randColor = []() -> glm::vec3 {
            return glm::vec3(
                static_cast<float>(std::rand()) / RAND_MAX,
                static_cast<float>(std::rand()) / RAND_MAX,
                static_cast<float>(std::rand()) / RAND_MAX
            );
        };

        colors = {
            randColor(), // -Z
            randColor(), // +Z
            randColor(), // -Y
            randColor(), // +Y
            randColor(), // -X
            randColor()  // +X
        };
    }
    void SetWeirdMesh() {
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned>(std::time(nullptr)));
            seeded = true;
        }

        float size = 0.2f + static_cast<float>(std::rand()) / RAND_MAX * 0.8f;

        // 8 "деформированных" вершин
        vertices = {
            { -0.5f, -0.5f, -0.5f },
            {  0.6f, -0.3f, -0.4f },
            {  0.4f,  0.6f, -0.6f },
            { -0.6f,  0.5f, -0.3f },
            { -0.4f, -0.6f,  0.7f },
            {  0.5f, -0.4f,  0.6f },
            {  0.7f,  0.5f,  0.5f },
            { -0.5f,  0.7f,  0.6f }
        };

        for (auto& v : vertices) {
            v *= size;
        }

        // Та же топология (12 треугольников, как у куба)
        indices = {
            0,2,1, 0,3,2, // сторона 1
            4,5,6, 4,6,7, // сторона 2
            0,1,5, 0,5,4, // сторона 3
            2,3,7, 2,7,6, // сторона 4
            0,4,7, 0,7,3, // сторона 5
            1,2,6, 1,6,5  // сторона 6
        };

        auto randColor = []() -> glm::vec3 {
            return glm::vec3(
                static_cast<float>(std::rand()) / RAND_MAX,
                static_cast<float>(std::rand()) / RAND_MAX,
                static_cast<float>(std::rand()) / RAND_MAX
            );
        };

        colors = {
            randColor(),
            randColor(),
            randColor(),
            randColor(),
            randColor(),
            randColor()
        };
    }
    void SetDetailedWeirdMesh() {
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned>(std::time(nullptr)));
            seeded = true;
        }

        float size = 0.3f + static_cast<float>(std::rand()) / RAND_MAX * 0.7f;

        auto randVec3 = [](float scale = 1.0f) {
            return glm::vec3(
                (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * scale,
                (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * scale,
                (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * scale
            );
        };

        // Базовые вершины искажённой сферы (24 вершины)
        vertices.clear();
        for (int i = 0; i < 24; ++i) {
            // Распределим их по сфере и добавим рандом
            float theta = static_cast<float>(i) / 24.0f * glm::two_pi<float>();
            float phi = static_cast<float>(i % 12) / 12.0f * glm::pi<float>();
            glm::vec3 p = glm::vec3(
                std::sin(phi) * std::cos(theta),
                std::cos(phi),
                std::sin(phi) * std::sin(theta)
            );
            p += randVec3(0.3f); // шум
            vertices.push_back(p * size);
        }

        // Рандомно сгенерированные треугольники (например, 44)
        indices.clear();
        for (int i = 0; i < 44; ++i) {
            int i0 = std::rand() % 24;
            int i1 = std::rand() % 24;
            int i2 = std::rand() % 24;
            if (i0 != i1 && i1 != i2 && i0 != i2) {
                indices.push_back(i0);
                indices.push_back(i1);
                indices.push_back(i2);
            }
        }

        // Цвет на каждый треугольник (по количеству треугольников)
        colors.clear();
        for (int i = 0; i < indices.size() / 3; ++i) {
            colors.push_back(glm::vec3(
                static_cast<float>(std::rand()) / RAND_MAX,
                static_cast<float>(std::rand()) / RAND_MAX,
                static_cast<float>(std::rand()) / RAND_MAX
            ));
        }
    }
};

