//
// Created by IDKTHIS on 16.07.2025.
//

#pragma once


#include "AActor.h"
#include "UBaseSystem.h"
#include <vector>
#include <memory>

class UWorld : public UObject {
    UCLASS(UWorld);
public:
    UWorld() = default;
    ~UWorld() = default;

    template<typename T, typename... Args>
     std::shared_ptr<T> SpawnActor(Args&&... args) {
        static_assert(std::is_base_of_v<AActor, T>, "T must inherit from AActor");

        auto actor = std::make_shared<T>(std::forward<Args>(args)...);
        m_actors.push_back(actor);
        return actor;
    }

    const std::vector<std::shared_ptr<AActor>>& GetActors() const {
        return m_actors;
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> AddSystem(Args&&... args) {
        auto system = std::make_shared<T>(std::forward<Args>(args)...);
        m_systems.push_back(system);
        return system;
    }

    void Update(float deltaTime) {
        for (auto& system : m_systems) {
            system->Update(deltaTime);
        }
        for (auto& actor : m_actors) {
            actor->Update(deltaTime);
        }
    }

private:
    std::vector<std::shared_ptr<AActor>> m_actors;
    std::vector<std::shared_ptr<UBaseSystem>> m_systems;
};