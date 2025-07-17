//
// Created by IDKTHIS on 16.07.2025.
//

#include "VulkanShader.h"

#include <fstream>
#include <sstream>

#include "Core/Log/Logger.h"
#include "Platform/Renderer/Vulkan/Devices/LogicalDevice.h"


VulkanShader::VulkanShader(LogicalDevice* logicalDevice)
    : m_logicalDevice(logicalDevice) {}

VulkanShader::~VulkanShader() {
    for (auto& [_, module] : m_shaderModules) {
        m_logicalDevice->GetHandle().destroyShaderModule(module);
    }
}

std::unordered_map<std::string, ShaderStage> VulkanShader::StringToShaderStage() {
    return {
            {"vertex", ShaderStage::Vertex},
            {"fragment", ShaderStage::Fragment},
            {"compute", ShaderStage::Compute}
    };
}

shaderc_shader_kind VulkanShader::GetShadercKind(ShaderStage stage) {
    switch (stage) {
        case ShaderStage::Vertex:   return shaderc_glsl_vertex_shader;
        case ShaderStage::Fragment: return shaderc_glsl_fragment_shader;
        case ShaderStage::Compute:  return shaderc_glsl_compute_shader;
        default: throw std::runtime_error("Unsupported shader stage");
    }
}

std::vector<uint32_t> VulkanShader::CompileGLSL(const std::string& source, shaderc_shader_kind kind) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
    options.SetTargetSpirv(shaderc_spirv_version_1_6);
    options.SetSourceLanguage(shaderc_source_language_glsl);
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetWarningsAsErrors();
    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
        source,
        kind,
        "shader",
        options
    );

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        LOGF_ERROR("Shader compilation failed: %s\n", result.GetErrorMessage().c_str());
        throw std::runtime_error("Shader compilation failed: " + result.GetErrorMessage());
    }
    return {result.cbegin(), result.cend()};
}

void VulkanShader::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        LOGF_ERROR("failed to open shader file: %s\n", filepath.c_str());
        throw std::runtime_error("Failed to open shader file: " + filepath);
    }

    std::unordered_map<std::string, std::string> sources;
    std::string line;
    std::stringstream current;
    std::string type;

    while (std::getline(file, line)) {
        if (line.find("#type") == 0) {
            if (!type.empty()) {
                sources[type] = current.str();
                current.str("");
                current.clear();
            }
            type = line.substr(6);
        } else {
            current << line << '\n';
        }
    }
    if (!type.empty()) {
        sources[type] = current.str();
    }

    auto stageMap = StringToShaderStage();
    for (const auto& [typeStr, source] : sources) {
        ShaderStage stage = stageMap.at(typeStr);
        auto spirv = CompileGLSL(source, GetShadercKind(stage));

        vk::ShaderModuleCreateInfo info{};
        info.codeSize = spirv.size() * sizeof(uint32_t);
        info.pCode = spirv.data();

        vk::ShaderModule module = m_logicalDevice->GetHandle().createShaderModule(info);

        m_spirvCode[stage] = spirv;
        m_shaderModules[stage] = module;
    }
}

const std::unordered_map<ShaderStage, std::vector<uint32_t>>& VulkanShader::GetBinaryData() const {
    return m_spirvCode;
}

const std::unordered_map<ShaderStage, vk::ShaderModule>& VulkanShader::GetShaderModules() const {
    return m_shaderModules;
}

vk::ShaderModule& VulkanShader::GetShaderModule(const std::string &stageName) {
    auto map = StringToShaderStage();
    auto it = map.find(stageName);
    if (it == map.end()) {
        throw std::runtime_error("Unknown shader stage: " + stageName);
    }

    ShaderStage stage = it->second;
    auto moduleIt = m_shaderModules.find(stage);
    if (moduleIt == m_shaderModules.end()) {
        throw std::runtime_error("Shader module for stage '" + stageName + "' not found");
    }

    return moduleIt->second;
}
