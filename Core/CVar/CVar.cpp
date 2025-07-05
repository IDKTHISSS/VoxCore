//
// Created by IDKTHIS on 01.07.2025.
//

#include "CVar.h"

ConVar::ConVar(const std::string& name, CVarValue defaultValue, const std::string& description, int flags)
    : name(name), description(description), value(defaultValue), flags(flags)
{
    Register(this);
}

void ConVar::Register(ConVar* var) {
    CVarManager::Instance().Register(var);
}

CVarManager& CVarManager::Instance() {
    static CVarManager inst;
    return inst;
}

void CVarManager::Register(ConVar* var) {
    std::lock_guard<std::mutex> lock(mutex_);
    vars[var->name] = var;
}

ConVar* CVarManager::Get(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = vars.find(name);
    return (it != vars.end()) ? it->second : nullptr;
}

void CVarManager::Set(const std::string& name, CVarValue val) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto* var = Get(name);
    if (!var) return;

    if (var->flags & CVAR_READONLY) {
        std::cerr << "[CVar] Нельзя изменить READONLY переменную: " << name << "\n";
        return;
    }
    if ((var->flags & CVAR_CHEAT) /* && !cheats_enabled */) {
        std::cerr << "[CVar] " << name << " можно изменить только при включенных читах\n";
        return;
    }

    var->value = val;
}

bool CVarManager::IsReadOnly(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto* var = Get(name);
    return var && (var->flags & CVAR_READONLY);
}
