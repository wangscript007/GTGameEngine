
#include <GTEngine/Rendering/RCQueue.hpp>
#include <GTEngine/Rendering/RenderCommand.hpp>

namespace GTEngine
{
    RCQueue::RCQueue()
        : commands()
    {
    }
    
    void RCQueue::Append(RenderCommand &cmd)
    {
        this->commands.PushBack(&cmd);
    }

    void RCQueue::Execute()
    {
        for (size_t i = 0; i < this->commands.count; ++i)
        {
            this->commands.buffer[i]->Execute();
        }
    }

    void RCQueue::Clear()
    {
        // Just to make sure we don't cycle over and attempt to call destructors, we'll simply set the count to 0
        // which is enough to clear the buffer. We would otherwise use this->commands.Clear();
        this->commands.count = 0;
    }
}
