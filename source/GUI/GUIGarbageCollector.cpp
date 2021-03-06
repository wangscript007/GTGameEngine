// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/GUI/GUIGarbageCollector.hpp>
#include <GTGE/GUI/GUIElement.hpp>
#include <GTGE/GUI/GUIServer.hpp>
#include <GTGE/GUI/Rendering/GUIMesh.hpp>

namespace GT
{
    GUIGarbageCollector::GUIGarbageCollector()
        : garbageElements(), garbageMeshes()
    {
    }
    
    GUIGarbageCollector::~GUIGarbageCollector()
    {
        while (this->garbageElements.root != nullptr)
        {
            delete this->garbageElements.root->value.object;
            this->garbageElements.RemoveRoot();
        }

        while (this->garbageMeshes.root != nullptr)
        {
            delete this->garbageMeshes.root->value.object;
            this->garbageMeshes.RemoveRoot();
        }
    }

    void GUIGarbageCollector::Collect(bool force)
    {
        for (auto i = this->garbageElements.root; i != nullptr; )
        {
            auto &gcitem = i->value;
            if (gcitem.counter == 0 || force)
            {
                delete gcitem.object;

                auto next = i->next;
                this->garbageElements.Remove(i);

                i = next;
            }
            else
            {
                i = i->next;
                --gcitem.counter;
            }
        }

        for (auto i = this->garbageMeshes.root; i != nullptr; )
        {
            auto &gcitem = i->value;
            if (gcitem.counter == 0 || force)
            {
                delete gcitem.object;

                auto next = i->next;
                this->garbageMeshes.Remove(i);

                i = next;
            }
            else
            {
                i = i->next;
                --gcitem.counter;
            }
        }
    }

    void GUIGarbageCollector::MarkForCollection(GUIElement &element, int counter)
    {
        // The primary style class should be deleted immediately so that it is never referenced while the element is in the garbage.
        element.DeletePrimaryStyleClass();
        this->garbageElements.Append(GCItem<GUIElement>(element, counter));
    }

    void GUIGarbageCollector::MarkForCollection(GUIMesh &mesh, int counter)
    {
        this->garbageMeshes.Append(GCItem<GUIMesh>(mesh, counter));
    }
}
