// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/GUI/Caret.hpp>
#include <GTLib/GUI/Server.hpp>

namespace GTGUI
{
    Caret::Caret()
        : owner(nullptr), xPos(0), yPos(0), width(1), height(20), blinkInterval(0.5), blinkTime(0.0), isOn(true),
          isRenderingDataValid(false), mesh(nullptr)
    {
    }

    Caret::~Caret()
    {
        delete this->mesh;
    }

    void Caret::Show(Element *owner)
    {
        if (this->owner != owner)
        {
            this->owner = owner;
            this->InvalidateRenderingData();
        }
    }
    
    void Caret::Hide()
    {
        if (this->owner != nullptr)
        {
            this->owner = nullptr;
            this->InvalidateRenderingData();
        }
    }

    void Caret::GetPosition(int &x, int &y)
    {
        if (this->owner != nullptr)
        {
            x = this->xPos;
            y = this->yPos;
        }
    }
    
    void Caret::SetPosition(int x, int y)
    {
        if (this->owner != nullptr)
        {
            this->xPos = x;
            this->yPos = y;

            // When the caret is moved, we reset it's timer.
            this->isOn      = true;
            this->blinkTime = 0.0f;

            this->InvalidateRenderingData();
        }
    }

    void Caret::GetSize(unsigned int &width, unsigned int &height)
    {
        if (this->owner != nullptr)
        {
            width  = this->width;
            height = this->height;
        }
    }

    void Caret::SetSize(unsigned int width, unsigned int height)
    {
        if (this->owner != nullptr)
        {
            this->width  = width;
            this->height = height;

            this->InvalidateRenderingData();
        }
    }
    
    void Caret::SetBlinkInterval(double blinkIntervalSeconds)
    {
        if (this->owner != nullptr)
        {
            this->blinkInterval = blinkIntervalSeconds;
        }
    }

    bool Caret::IsVisible() const
    {
        return this->owner != nullptr && this->isOn;
    }


    void Caret::ValidateRenderingData()
    {
        if (!this->isRenderingDataValid && this->owner != nullptr)
        {
            if (this->mesh != nullptr)
            {
                this->owner->server.GetGarbageCollector().MarkForCollection(*this->mesh);
                this->mesh = nullptr;           // Not needed, but I like to do it for sanity.
            }


            // Now we build a pretty standard quad mesh.
            float left   = static_cast<float>(this->xPos);
            float top    = static_cast<float>(this->yPos);
            float right  = left + static_cast<float>(this->width);
            float bottom = top  + static_cast<float>(this->height);

            auto &colour  = this->owner->style.textCursorColour->value;
            auto  opacity = this->owner->GetAbsoluteOpacity();


            MeshVertex vertices[4];

            // Left/Bottom
            vertices[0].positionX = left;     vertices[0].positionY = bottom;
            vertices[0].texCoordX = 0.0f;     vertices[0].texCoordY = 0.0f;
            vertices[0].colourR   = colour.r; vertices[0].colourG   = colour.g; vertices[0].colourB = colour.b; vertices[0].colourA = opacity;

            // Right/Bottom
            vertices[1].positionX = right;    vertices[1].positionY = bottom;
            vertices[1].texCoordX = 1.0f;     vertices[1].texCoordY = 0.0f;
            vertices[1].colourR   = colour.r; vertices[1].colourG   = colour.g; vertices[1].colourB = colour.b; vertices[1].colourA = opacity;

            // Right/Top
            vertices[2].positionX = right;    vertices[2].positionY = top;
            vertices[2].texCoordX = 1.0f;     vertices[2].texCoordY = 1.0f;
            vertices[2].colourR   = colour.r; vertices[2].colourG   = colour.g; vertices[2].colourB = colour.b; vertices[2].colourA = opacity;

            // Left/Top
            vertices[3].positionX = left;     vertices[3].positionY = top;
            vertices[3].texCoordX = 0.0f;     vertices[3].texCoordY = 1.0f;
            vertices[3].colourR   = colour.r; vertices[3].colourG   = colour.g; vertices[3].colourB = colour.b; vertices[3].colourA = opacity;


            unsigned int indices[6] =
            {
                0, 1, 2,
                2, 3, 0
            };

            this->mesh = new Mesh(vertices, 4, indices, 6);

            this->isRenderingDataValid = true;
        }
    }

    void Caret::InvalidateRenderingData()
    {
        this->isRenderingDataValid = false;
    }
    

    void Caret::Update(double dt)
    {
        if (this->owner != nullptr)
        {
            this->blinkTime += dt;
            
            // If the caret has been in it's current blink state for long enough, we can swap it.
            if (this->blinkTime > this->blinkInterval)
            {
                this->isOn      = !this->isOn;
                this->blinkTime = 0.0f;
            }
        }
    }
}
