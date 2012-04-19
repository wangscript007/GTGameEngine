
#include <GTEngine/MeshBuilder.hpp>

namespace GTEngine
{
    MeshBuilder::MeshBuilder(size_t vertexSizeInFloats)
        : vertexSizeInFloats(vertexSizeInFloats), vertexBuffer(), indexBuffer()
    {
    }

    MeshBuilder::~MeshBuilder()
    {
    }

    void MeshBuilder::EmitVertex(const float* vertexData)
    {
        unsigned int index;
        if (!this->Find(vertexData, index))
        {
            index = static_cast<unsigned int>(this->vertexBuffer.count) / vertexSizeInFloats;

            // Here is where we append the input vertex data to the mesh builder's vertex data buffer.
            for (auto i = 0U; i < vertexSizeInFloats; ++i)
            {
                this->vertexBuffer.PushBack(vertexData[i]);
            }
        }
        
        // Now we just add the index to the end.
        this->indexBuffer.PushBack(index);
    }

    bool MeshBuilder::Find(const float* vertexData, unsigned int &indexOut)
    {
        size_t vertexCount = this->vertexBuffer.count / vertexSizeInFloats;

        for (size_t i = 0; i < vertexCount; ++i)
        {
            if (this->Equal(this->vertexBuffer.buffer + (i * vertexSizeInFloats), vertexData))
            {
                indexOut = static_cast<unsigned int>(indexOut);
                return true;
            }
        }

        return false;
    }

    bool MeshBuilder::Equal(const float* vertexData0, const float* vertexData1)
    {
        // TODO: Look into SSE instrinsics for an optimization for appropriately aligned formats. P3T2N3 should benefit from this...
        for (size_t i = 0; i < vertexSizeInFloats; ++i)
        {
            if (vertexData0[i] != vertexData1[i])
            {
                return false;
            }
        }

        // If we make it here, it means we are equal.
        return true;
    }
}
