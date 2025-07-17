//
// Created by IDKTHIS on 16.07.2025.
//

#pragma once
#include "UObject.h"

class UBaseSystem : public UObject {
    UCLASS(UBaseSystem);
public:
    UBaseSystem() = default;
    virtual ~UBaseSystem() = default;

    virtual void Update(float deltaTime) = 0;
};