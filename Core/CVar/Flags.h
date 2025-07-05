//
// Created by IDKTHIS on 01.07.2025.
//

#ifndef FLAGS_H
#define FLAGS_H

enum CVarFlags {
    CVAR_NONE        = 0,
    CVAR_ARCHIVE     = 1 << 0, // сохранять в конфиг
    CVAR_READONLY    = 1 << 1, // нельзя изменять
    CVAR_CHEAT       = 1 << 2, // изменяем только если включены читы
    CVAR_RUNTIME_ONLY= 1 << 3  // не сохраняется в файл
};



#endif //FLAGS_H
