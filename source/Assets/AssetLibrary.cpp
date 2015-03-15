// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/AssetLibrary.hpp>
#include <GTGameEngine/Assets/Asset.hpp>
#include <GTGameEngine/Assets/AssetAllocator.hpp>
#include <GTGameEngine/FileSystem.hpp>
#include <GTLib/String.hpp>
#include <GTLib/IO.hpp>

namespace GT
{
    AssetLibrary::AssetLibrary(FileSystem &fileSystem)
        : m_fileSystem(fileSystem),
          m_allocators(),
          m_extensionTypeMapping(),
          m_loadedAssets()
    {
    }

    AssetLibrary::~AssetLibrary()
    {
    }


    bool AssetLibrary::Startup()
    {
        return true;
    }

    void AssetLibrary::Shutdown()
    {
        for (size_t iAsset = 0; iAsset < m_loadedAssets.count; ++iAsset)
        {
            auto pAsset = m_loadedAssets.buffer[iAsset]->value;
            assert(pAsset != nullptr);
            {
                auto pAllocator = this->FindAllocatorByType(pAsset->GetType());
                assert(pAllocator != nullptr);
                {
                    pAllocator->DeleteAsset(pAsset);
                }
            }
        }
    }


    Asset* AssetLibrary::Load(const char* filePathOrIdentifier)
    {
        // When an asset is cached, the absolute path is used to retrieve the cached object. It is possible, however, for an asset to not actually
        // be loaded from a file, in which case filePathOrIdentifier is used as the unique identifier without any modification.

        GTLib::String absolutePathOrIdentifier;
        if (!m_fileSystem.FindAbsolutePath(filePathOrIdentifier, absolutePathOrIdentifier))
        {
            // The file could not be found, but the asset loader might be using it as a unique token, so we just assume use it as-is for the absolute path in this case.
            absolutePathOrIdentifier = filePathOrIdentifier;
        }


        auto iExistingAsset = m_loadedAssets.Find(absolutePathOrIdentifier.c_str());
        if (iExistingAsset == nullptr)
        {
            AssetType assetType = this->FindTypeByExtension(GTLib::IO::GetExtension(filePathOrIdentifier));

            auto pAllocator = this->FindAllocatorByType(assetType);
            if (pAllocator != nullptr)
            {
                auto pAsset = pAllocator->CreateAsset(assetType);
                if (pAsset != nullptr)
                {
                    if (pAsset->Load(absolutePathOrIdentifier.c_str(), m_fileSystem))
                    {
                        m_loadedAssets.Add(absolutePathOrIdentifier.c_str(), pAsset);
                        return pAsset;
                    }
                    else
                    {
                        // Failed to load the asset.
                        pAllocator->DeleteAsset(pAsset);
                        return nullptr;
                    }
                }
                else
                {
                    // Failed to create an asset of the given type.
                    return nullptr;
                }
            }
            else
            {
                // Could not find a supported allocator.
                return nullptr;
            }
        }
        else
        {
            auto pExistingAsset = iExistingAsset->value;
            assert(pExistingAsset != nullptr);
            {
                pExistingAsset->IncrementReferenceCount();
            }

            return iExistingAsset->value;
        }
    }

    Asset* AssetLibrary::Load(Asset* pAsset)
    {
        if (pAsset != nullptr)
        {
            pAsset->IncrementReferenceCount();
        }

        return pAsset;
    }

    void AssetLibrary::Unload(Asset* pAsset)
    {
        if (pAsset != nullptr)
        {
            if (pAsset->DecrementReferenceCount() == 0)
            {
                for (size_t iAsset = 0; iAsset < m_loadedAssets.count; ++iAsset)
                {
                    if (m_loadedAssets.buffer[iAsset]->value == pAsset)
                    {
                        // Remove from the cache.
                        m_loadedAssets.RemoveByIndex(iAsset);

                        
                        // Delete the asset object.
                        auto pAllocator = this->FindAllocatorByType(pAsset->GetType());
                        assert(pAllocator != nullptr);
                        {
                            pAllocator->DeleteAsset(pAsset);
                        }


                        break;
                    }
                }
            }
        }
    }

    void AssetLibrary::Reload(const char* filePath)
    {
        GTLib::String absolutePath;
        if (m_fileSystem.FindAbsolutePath(filePath, absolutePath))
        {
            auto iAsset = m_loadedAssets.Find(absolutePath.c_str());
            if (iAsset != nullptr)
            {
                auto pAsset = iAsset->value;
                assert(pAsset != nullptr);
                {
                    if (pAsset->Load(absolutePath.c_str(), m_fileSystem))
                    {
                    }
                    else
                    {
                        // Failed to reload asset.
                    }
                }
            }
        }
    }


    void AssetLibrary::RegisterAllocator(AssetAllocator &allocator)
    {
        // It is considered an error if the allocator already exists.
        assert(m_allocators.Exists(&allocator) == false);
        {
            m_allocators.PushBack(&allocator);
        }
    }

    void AssetLibrary::RegisterExtensions(AssetExtensionDesc* extensions, size_t extensionsCount)
    {
        for (size_t iExtension = 0; iExtension < extensionsCount; ++iExtension)
        {
            AssetExtensionDesc &extension = extensions[iExtension];

            m_extensionTypeMapping.Add(extension.extension, ((static_cast<uint64_t>(extension.classification) << 32) | static_cast<uint64_t>(extension.type)));
        }
    }



    ////////////////////////////////////
    // Private

    AssetAllocator* AssetLibrary::FindAllocatorByExtension(const char* extension)
    {
        return this->FindAllocatorByType(this->FindTypeByExtension(extension));
    }

    AssetAllocator* AssetLibrary::FindAllocatorByType(AssetType type)
    {
        for (size_t iAllocator = 0; iAllocator < m_allocators.GetCount(); ++iAllocator)
        {
            auto pAllocator = m_allocators[iAllocator];
            assert(pAllocator != nullptr);
            {
                if (pAllocator->IsAssetTypeSupported(type))
                {
                    return pAllocator;
                }
            }
        }

        return nullptr;
    }

    AssetType AssetLibrary::FindTypeByExtension(const char* extension)
    {
        auto iAssetType = m_extensionTypeMapping.Find(extension);
        if (iAssetType != nullptr)
        {
            return static_cast<AssetType>(iAssetType->value & 0x00000000FFFFFFFF);
        }

        return AssetType_Unknown;
    }

    AssetClass AssetLibrary::FindClassByExtension(const char* extension)
    {
        auto iAssetType = m_extensionTypeMapping.Find(extension);
        if (iAssetType != nullptr)
        {
            return static_cast<AssetClass>((iAssetType->value & 0xFFFFFFFF00000000) >> 32);
        }

        return AssetType_Unknown;
    }
}
