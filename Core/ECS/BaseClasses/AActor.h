//
// Created by IDKTHIS on 16.07.2025.
//

#pragma once


#include "UObject.h"
#include "UBaseComponent.h"
#include <vector>
#include <memory>

class AActor : public UObject {
    UCLASS(AActor);
public:
    AActor() = default;
    virtual ~AActor() = default;

    void AddComponent(std::shared_ptr<UBaseComponent> component) {
        component->OnAttach(this);
        m_components.push_back(component);
    }

    template<typename T>
    T* GetComponent() {
        for (auto& comp : m_components) {
            if (auto ptr = dynamic_cast<T*>(comp.get())) {
                return ptr;
            }
        }
        return nullptr;
    }

    void Update(float deltaTime) {
        for (auto& comp : m_components) {
            comp->Update(deltaTime);
        }
    }

protected:
    std::vector<std::shared_ptr<UBaseComponent>> m_components;
};