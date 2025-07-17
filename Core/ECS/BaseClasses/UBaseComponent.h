//
// Created by IDKTHIS on 16.07.2025.
//

#pragma once
#include "UObject.h"
#include <memory>

class AActor; // forward

class UBaseComponent : public UObject {
    UCLASS(UBaseComponent);
public:
    UBaseComponent() = default;
    virtual ~UBaseComponent() = default;

    virtual void OnAttach(AActor* owner) { m_owner = owner; }
    virtual void OnDetach() {}

    virtual void Update(float deltaTime) {} // Optional for ticking systems

    AActor* GetOwner() const { return m_owner; }

protected:
    AActor* m_owner = nullptr;
};