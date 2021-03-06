// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/MaterialShaderCache.hpp>

namespace GT
{
    MaterialShaderCache::MaterialShaderCache()
        : shaders()
    {
    }

    MaterialShaderCache::~MaterialShaderCache()
    {
    }

    Shader* MaterialShaderCache::GetShader(const MaterialDefinition &material)
    {
        auto iShader = this->shaders.Find(&material);
        if (iShader != nullptr)
        {
            return iShader->value;
        }

        return nullptr;
    }

    void MaterialShaderCache::SetShader(const MaterialDefinition &material, Shader &shader)
    {
        this->shaders.Add(&material, &shader);
    }
}
