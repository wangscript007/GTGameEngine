// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/PrefabLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/Dictionary.hpp>
#include <utility>


namespace GTEngine
{
    //////////////////////////////////////
    // Globals.

    typedef std::pair<Prefab*, size_t> PrefabReference;

    /// The list of loaded classes, indexed by the absolute path.
    static GTCore::Dictionary<PrefabReference> LoadedPrefabs;




    //////////////////////////////////////
    // Startup/Shutdown.

    bool PrefabLibrary::Startup()
    {
        return true;
    }

    void PrefabLibrary::Shutdown()
    {
        for (size_t i = 0; i < LoadedPrefabs.count; ++i)
        {
            delete LoadedPrefabs.buffer[i]->value.first;
        }
        LoadedPrefabs.Clear();
    }



    //////////////////////////////////////
    // Acquire/Unacquire

    Prefab* PrefabLibrary::Acquire(const char* fileName, const char* makeRelativeTo)
    {
        GTCore::String relativePath(fileName);

        if (GTCore::Path::IsAbsolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                relativePath = GTCore::IO::ToRelativePath(fileName, makeRelativeTo);
            }
            else
            {
                GTEngine::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        GTCore::String absolutePath;
        if (GTCore::IO::FindAbsolutePath(fileName, absolutePath))
        {
            auto iLoadedPrefab = LoadedPrefabs.Find(absolutePath.c_str());
            if (iLoadedPrefab == nullptr)
            {
                // Does not exist. Needs to be loaded.
                auto file = GTCore::IO::Open(absolutePath.c_str(), GTCore::IO::OpenMode::Read);
                if (file != nullptr)
                {
                    // We use a file deserializer for this.
                    GTCore::FileDeserializer deserializer(file);
                    
                    auto newPrefab = new Prefab(absolutePath.c_str(), relativePath.c_str());
                    newPrefab->Deserialize(deserializer);

                    LoadedPrefabs.Add(absolutePath.c_str(), PrefabReference(newPrefab, 1));

                    // Can't forget to close the file.
                    GTCore::IO::Close(file);

                    return newPrefab;
                }
                else
                {
                    GTEngine::PostError("Can not find file: %s", fileName);
                }
            }
            else
            {
                // Already exists, so we just increment the counter.
                ++iLoadedPrefab->value.second;

                return iLoadedPrefab->value.first;
            }
        }
        else
        {
            GTEngine::PostError("Can not find file: %s", fileName);
        }


        return nullptr;
    }

    void PrefabLibrary::Unacquire(const Prefab* prefabToUnacquire)
    {
        if (prefabToUnacquire != nullptr)
        {
            // We need to search by value.
            for (size_t i = 0; i < LoadedPrefabs.count; ++i)
            {
                auto &value = LoadedPrefabs.buffer[i]->value;
                if (value.first == prefabToUnacquire)
                {
                    assert(value.second > 0);
                    {
                        --value.second;

                        if (value.second == 0)
                        {
                            delete value.first;
                            LoadedPrefabs.RemoveByIndex(i);
                        }

                        break;
                    }
                }
            }
        }
    }
}