// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleSystemDefinition.hpp>
#include <GTEngine/Errors.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/IO.hpp>

namespace GTEngine
{
    ParticleSystemDefinition::ParticleSystemDefinition()
        : absolutePath(), relativePath(),
          emitters()
    {
    }

    ParticleSystemDefinition::~ParticleSystemDefinition()
    {
        this->Clear();
    }


    bool ParticleSystemDefinition::LoadFromFile(const char* fileNameIn, const char* relativePathIn)
    {
        GTCore::String newAbsolutePath;
        GTCore::String newRelativePath;


        if (GTCore::Path::IsAbsolute(fileNameIn))
        {
            newAbsolutePath = fileNameIn;

            if (relativePathIn != nullptr)
            {
                newRelativePath = relativePathIn;
            }
            else
            {
                GTEngine::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileNameIn);
                return false;
            }
        }
        else
        {
            newRelativePath = fileNameIn;

            if (!GTCore::IO::FindAbsolutePath(fileNameIn, newAbsolutePath))
            {
                return false;
            }
        }


        auto file = GTCore::IO::Open(newAbsolutePath.c_str(), GTCore::IO::OpenMode::Binary | GTCore::IO::OpenMode::Read);
        if (file != nullptr)
        {
            GTCore::FileDeserializer deserializer(file);
            this->Deserialize(deserializer);


            this->absolutePath = newAbsolutePath;
            this->relativePath = newRelativePath;

            GTCore::IO::Close(file);
            return true;
        }

        return false;
    }


    const char* ParticleSystemDefinition::GetAbsolutePath() const
    {
        return this->absolutePath.c_str();
    }

    const char* ParticleSystemDefinition::GetRelativePath() const
    {
        return this->relativePath.c_str();
    }




    size_t ParticleSystemDefinition::GetEmitterCount() const
    {
        return this->emitters.count;
    }


    ParticleEmitter* ParticleSystemDefinition::GetEmitter(size_t index)
    {
        return this->emitters[index];
    }

    const ParticleEmitter* ParticleSystemDefinition::GetEmitter(size_t index) const
    {
        return this->emitters[index];
    }


    ParticleEmitter* ParticleSystemDefinition::AppendNewEmitter()
    {
        auto newEmitter = new ParticleEmitter;
        this->emitters.PushBack(newEmitter);

        return newEmitter;
    }

    void ParticleSystemDefinition::DeleteEmitterByIndex(size_t index)
    {
        delete this->emitters[index];
        this->emitters.Remove(index);
    }




    //////////////////////////////////////
    // Serialization/Deserialization

    void ParticleSystemDefinition::Serialize(GTCore::Serializer &serializer)
    {
        GTCore::BasicSerializer intermediarySerializer;

        uint32_t emitterCount = static_cast<uint32_t>(this->emitters.count);
        intermediarySerializer.Write(emitterCount);


        // Emitters.
        for (uint32_t iEmitter = 0; iEmitter < emitterCount; ++iEmitter)
        {
            auto emitter = this->emitters[iEmitter];
            assert(emitter != nullptr);
            {
                emitter->Serialize(intermediarySerializer, false);      // <-- 'false' means that individual particle information will not be serialized (not needed for definitions).
            }
        }



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_ParticleSystemDefinition_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();
        
        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void ParticleSystemDefinition::Deserialize(GTCore::Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);

        if (header.id == Serialization::ChunkID_ParticleSystemDefinition_Main)
        {
            // We'll first want to clear the definition (reset it).
            this->Clear();

            uint32_t emitterCount;
            deserializer.Read(emitterCount);

            for (size_t iEmitter = 0; iEmitter < emitterCount; ++iEmitter)
            {
                auto emitter = this->AppendNewEmitter();
                assert(emitter != nullptr);
                {
                    emitter->Deserialize(deserializer);
                }
            }
        }
        else
        {
            GTEngine::PostError("Error deserializing particle system definition. Unknown chunk ID (%d).", header.id);
        }
    }



    //////////////////////////////////////
    // Private

    void ParticleSystemDefinition::Clear()
    {
        for (size_t i = 0; i < this->emitters.count; ++i)
        {
            delete this->emitters[i];
        }
        this->emitters.Clear();
    }
}