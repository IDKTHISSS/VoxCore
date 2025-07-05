//
// Created by IDKTHIS on 02.07.2025.
//

#pragma once
#include "../Window/IWindow.h"




class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual bool Init(IWindow *window) = 0;
};
