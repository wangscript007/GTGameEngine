// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GUIContextBase_hpp_
#define __GT_GUIContextBase_hpp_

#include "GUIElement.hpp"
#include "GUISurface.hpp"
#include "GUIFontManager.hpp"


// TODO:
//      - Replace hSurface with pSurface
//      - Replace hElement with pElement

namespace GT
{
    /// Base class for the GUI context.
    ///
    /// This class should only be used internally.
    ///
    /// The GUI context is built with a layered architectured. This class represents the lowest level layer. This layer works
    /// directly on pointers. Higher level layers abstract pointers into handles.
    ///
    /// This layer does not store a list of surfaces and elements. That needs to be handled at a higher level layer. Higher
    /// level layers need to implement iteration functions.
    class GUIContextBase
    {
    protected:

        /// Constructor.
        GUIContextBase();

        /// Destructor
        virtual ~GUIContextBase();


        /////////////////////////////////////////////////////////////////
        // Virtual Methods
        //
        // These must be implemented at a higher level by a derived class.

        /// Creates a new surface.
        ///
        /// @return A pointer to the new surface.
        ///
        /// @remarks
        ///     The initial size will be 0,0
        virtual GUISurface* CreateSurfacePtr() = 0;

        /// Deletes the given surface.
        ///
        /// @param pSurface [in] A pointer to the surface to delete.
        virtual void DeleteSurfacePtr(GUISurface* pSurface) = 0;


        /// Creates a new GUI element.
        ///
        /// @return A pointer to the new GUI element.
        virtual GUIElement* CreateElementPtr() = 0;

        /// Deletes a GUI element that was created with CreateGUIElement().
        ///
        /// @param pElement [in] The GUI element to delete.
        virtual void DeleteElementPtr(GUIElement* pElement) = 0;
        

        /// Iterates over each surface.
        ///
        /// @param handler [in] The function to call for each surface.
        virtual void IterateSurfaces(std::function<bool (GUISurface* pSurface)> handler) const = 0;

        /// Iterates over each element.
        ///
        /// @param handler [in] The function to call for each element.
        virtual void IterateElements(std::function<bool (GUIElement* pElement)> handler) const = 0;


        /// Posts the OnSize event.
        ///
        /// @param element [in] A reference to the element receiving the event.
        /// @param width   [in] The new width of the element.
        /// @param height  [in] The new height of the element.
        virtual void PostEvent_OnSize(GUIElement* pElement, unsigned int width, unsigned int height) = 0;

        /// Posts the OnMove event.
        ///
        /// @param element [in] A reference to the element receiving the event.
        /// @param x       [in] The new position of the element on the x axis, relative to the top left corner of the parent.
        /// @param y       [in] The new position of the element on the y axis, relative to the top left corner of the parent.
        virtual void PostEvent_OnMove(GUIElement* pElement, int x, int y) = 0;

        /// Posts the OnSize and OnMove event.
        ///
        /// @param element [in] A reference to the element receiving the event.
        /// @param width   [in] The new width of the element.
        /// @param height  [in] The new height of the element.
        /// @param x       [in] The new position of the element on the x axis, relative to the top left corner of the parent.
        /// @param y       [in] The new position of the element on the y axis, relative to the top left corner of the parent.
        virtual void PostEvent_OnSizeAndOnMove(GUIElement* pElement, unsigned int width, unsigned height, int x, int y) = 0;

        /// Posts the OnMouseEnter event.
        ///
        /// @param element [in] A reference to the element receiving the event.
        virtual void PostEvent_OnMouseEnter(GUIElement* pElement) = 0;

        /// Posts the OnMouseLeave event.
        ///
        /// @param element [in] A reference to the element receiving the event.
        virtual void PostEvent_OnMouseLeave(GUIElement* pElement) = 0;

        /// Posts the OnMouseMove event.
        ///
        /// @param element   [in] A reference to the element receiving the event.
        /// @param mousePosX [in] The position of the mouse on the x axis relative to the top left corner of the element.
        /// @param mousePosY [in] The position of the mouse on the y axis relative to the top left corner of the element.
        virtual void PostEvent_OnMouseMove(GUIElement* pElement, int mousePosX, int mousePosY) = 0;

        /// Posts the OnPaint event.
        ///
        /// @param hSurface [in] A handle to the surface that has been painted.
        /// @param rect     [in] The rectangle that will just painted.
        //virtual void PostEvent_OnPaint(GUISurface* pSurface, const GTLib::Rect<int> &rect) = 0;

        /// Posts the OnSurfaceNeedsRepaid event.
        ///
        /// @param hSurface [in] A handle to the surface that needs to be repainted.
        /// @param rect     [in] The rectangle region that needs to be repainted.
        ///
        /// @remarks
        ///     Applications should call PaintSurface() in response to this event. This will then route all of the rendering commands
        ///     to the rendering event handler.
        virtual void PostEvent_OnSurfaceNeedsRepaint(GUISurface* pSurface, const GTLib::Rect<int> &rect) = 0;


        /// Begins painting the given surface.
        ///
        /// @param pSurface [in] A pointer to the surface being painted.
        virtual void Renderer_BeginPaintSurface(GUISurface* pSurface, void* pInputData) = 0;

        /// Ends painting the given surface.
        virtual void Renderer_EndPaintSurface() = 0;

        /// Clears the given rectangle region for the current surface.
        ///
        /// @param rect [in] The rectangle to clear.
        virtual void Renderer_Clear(const GTLib::Rect<int> &rect) = 0;

        /// Draws a rectangle.
        ///
        /// @param rect  [in] The rectangle to draw.
        /// @param color [in] The colour to draw the rectangle.
        virtual void Renderer_DrawRectangle(GTLib::Rect<int> rect, GTLib::Colour colour) = 0;

        /// Sets the clipping rectangle for rendering.
        ///
        /// @param rect [in] The clipping rectangle.
        virtual void Renderer_SetClippingRect(GTLib::Rect<int> clippingRect) = 0;

        /// Determines whether or not the current renderer can draw the given font.
        ///
        /// @parma hFont [in] A handle to the font.
        ///
        /// @return True if the current renderer can draw text with the given font.
        virtual bool Renderer_CanDrawText(HGUIFont hFont) = 0;

        /// Draws the given string using the given font using the current renderer.
        ///
        /// @param hFont   [in] A handle to the font.
        /// @param text    [in] The text to draw.
        /// @param options [in] The settings to use when drawing the text.
        virtual void Renderer_DrawText(HGUIFont hFont, const char* text, const GUITextRenderingOptions &options) = 0;




        /////////////////////////////////////////////////////////////////
        // Surfaces

        /// Creates a new surface.
        ///
        /// @remarks
        ///     The initial size will be 0,0
        GUISurface* CreateSurface();

        /// Deletes a surface.
        ///
        /// @param pSurface [in] A handle to the surface to delete.
        void DeleteSurface(GUISurface* pSurface);

        /// Retrieves a handle to a surface by it's ID.
        ///
        /// @param id [in] The ID of the surface to retrieve.
        ///
        /// @return A pointer to the surface with the given ID, or null if a surface of the given ID could not be found.
        ///
        /// @remarks
        ///     This runs in linear time.
        GUISurface* FindSurfaceByID(const char* id) const;

        /// Sets the ID of the given surface.
        ///
        /// @param pSurface [in] A pointer to the surface to delete.
        /// @param newID    [in] The new ID of the surface.
        void SetSurfaceID(GUISurface* pSurface, const char* id);

        /// Retrieves the ID of the given surface.
        ///
        /// @param pSurface [in] The surface whose ID is being retrieved.
        const char* GetSurfaceID(GUISurface* pSurface) const;

        /// Sets the size of a surface.
        ///
        /// @param pSurface  [in] A pointer to the surface whose size is being set.
        /// @param newWidth  [in] The new width of the surface.
        /// @param newHeight [in] The new height of the surface.
        ///
        /// @remarks
        ///     The relevant GUI elements will be resized and repositioned in response to a change in the size of the surface.
        void SetSurfaceSize(GUISurface* pSurface, unsigned int newWidth, unsigned int newHeight);

        /// Retrieves the size of a surface.
        ///
        /// @param hSurface  [in]  The handle to the surface whose size is being retrieved.
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        void GetSurfaceSize(GUISurface* pSurface, unsigned int &widthOut, unsigned int &heightOut) const;

        /// Determines whether or not the given surface contains the given element.
        ///
        /// @param hSurface [in] The surface to query.
        /// @param hElement [in] The element to look for.
        ///
        /// @return True if the given element is on the given surface.
        ///
        /// @remarks
        ///     This runs in linear time.
        bool DoesSurfaceContainGUIElement(GUISurface* pSurface, GUIElement* pElement) const;

        /// Iterates over each element that is attached to the given surface.
        ///
        /// @param surface [in] A reference to the surface whose elements are being iterated.
        /// @param handler [in] The function to call for each element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateSurfaceElements(GUISurface* pSurface, std::function<bool (GUIElement* pElement)> handler);


        /// Sets the given surface's aux. data pointer.
        ///
        /// @param hElement [in] A handle to the surface.
        /// @param auxData  [in] A pointer to the aux. data.
        ///
        /// @remarks
        ///     The aux. data can be retrieved with GetSurfaceAuxData().
        void SetSurfaceAuxData(GUISurface* pSurface, void* auxData)
        {
            if (pSurface != nullptr)
            {
                pSurface->auxData = auxData;
            }
        }

        /// Retrieve's the given surface's aux. data pointer.
        ///
        /// @param hElement [in] A handle to the surface.
        ///
        /// @return A pointer to the aux. data.
        template <typename T>
        T* GetSurfaceAuxData(GUISurface* pSurface)
        {
            if (pSurface != nullptr)
            {
                return reinterpret_cast<T*>(pSurface->auxData);
            }

            return reinterpret_cast<T*>(nullptr);
        }



        /////////////////////////////////////////////////////////////////
        // Elements

        /// Creates a new GUI element.
        ///
        /// @return A handle to the new GUI element.
        GUIElement* CreateElement();

        /// Deletes a GUI element that was created with CreateGUIElement().
        ///
        /// @param hElement [in] The GUI element to delete.
        void DeleteElement(GUIElement* pElement);

        /// Sets the ID of the given element.
        ///
        /// @param pElement [in] The GUI element whose having it's ID changed.
        /// @param newID    [in] The new ID of the GUI element.
        void SetElementID(GUIElement* pElement, const char* id);

        /// Retrieves the ID of the given element.
        ///
        /// @param pElement [in] The GUI element whose ID is being retrieved.
        ///
        /// @return The ID of the given element.
        const char* GetElementID(GUIElement* pElement) const;

        /// Finds an element by it's ID.
        ///
        /// @param elementID [in] The ID of the element to search for.
        ///
        /// @return
        ///     A pointer to the first element with the given ID, or 0 if an element could not be found.
        GUIElement* FindElementByID(const char* elementID) const;


        /// Retrieves the parent element of the given element.
        ///
        /// @param pChildElement [in] The GUI element whose parent is being retrieved.
        ///
        /// @return A handle to the parent GUI element, or null if it does not have one.
        GUIElement* GetElementParent(GUIElement* pChildElement) const;

        /// Assigns the element to a different parent.
        ///
        /// @param pChildElement  [in] The GUI element that is being reparented.
        /// @param pParentElement [in] The new parent GUI element.
        ///
        /// @remarks
        ///     When pParentElement is not null, this is equivalent to AppendChildGUIElement(pParentElement, hElement). When hParent is null, the element
        ///     will be detached from it's parent.
        void SetElementParent(GUIElement* pChildElement, GUIElement* pParentElement);

        /// Detaches an element from it's parent.
        ///
        /// @param pChildElement [in] The GUI element that is being detached from it's parent.
        ///
        /// @remarks
        ///     This is equivalent to this->RemoveChildGUIElement(pElement->pParent, pElement). If the element does not have a parent, this does
        ///     nothing.
        void DetachElementFromParent(GUIElement* pChildElement);

        /// Detaches an element from it's parent and siblings.
        ///
        /// @param pChildElement [in] The GUI element that is being detached from it's siblings.
        ///
        /// @remarks
        ///     If the element has a parent, it will be detached from that, too. The difference between this and DetachGUIElementFromParent() is that
        ///     that the later will not detach from siblings if it does not have a parent.
        void DetachElementFromParentAndSiblings(GUIElement* pChildElement);

        /// Appends a child GUI element.
        ///
        /// @param pParentElement [in] The GUI element that is having a child element appended.
        /// @param pChildElement  [in] The child GUI element to append.
        ///
        /// @remarks
        ///     An element can be prepended with PrependChildGUIElement().
        void AppendChildElement(GUIElement* pParentElement, GUIElement* pChildElement);

        /// Prepends a child GUI element.
        ///
        /// @param pParentElement [in] The GUI element that is having a child element prepended.
        /// @param pChildElement  [in] The child GUI element to append.
        ///
        /// @remarks
        ///     An element can be appended to the end using AppendChildGUIElement().
        void PrependChildElement(GUIElement* pParentElement, GUIElement* pChildElement);

        /// Appends an element to the given sibling.
        ///
        /// @param pElementToAppendTo [in] A pointer to the element being appended to.
        /// @param pElementToAppend   [in] A pointer to the element to append.
        ///
        /// @remarks
        ///     If the elements have different parents, the element to insert will be detached from it's current parent first.
        void AppendSiblingElement(GUIElement* pElementToAppendTo, GUIElement* pElementToAppend);

        /// Prepends an element to the given sibling.
        ///
        /// @param pElementToPrependTo [in] A pointer to the element being prepended to.
        /// @param pElementToPrepend   [in] A pointer to the element to prepend.
        ///
        /// @remarks
        ///     If the elements have different parents, the element to insert will be detached from it's current parent first.
        void PrependSiblingElement(GUIElement* pElementToPrependTo, GUIElement* pElementToPrepend);

        /// Retrieves a handle to the given element's next sibling.
        ///
        /// @param hElement [in] The GUI element whose next sibling is being retrieved.
        ///
        /// @return A handle to the GUI element of the given element's next sibling, or 0 if the element does not have a sibling or is the last one.
        GUIElement* GetElementNextSibling(GUIElement* pElement) const;

        /// Retrieves a handle to the given element's previous sibling.
        ///
        /// @param hElement [in] The GUI element whose previous sibling is being retrieved.
        ///
        /// @return A handle to the GUI element whose previous sibling is being retrieved.
        GUIElement* GetElementPreviousSibling(GUIElement* pElement) const;


        /// Iterates over the previous siblings of the given element in forward order.
        ///
        /// @param pElement [in] The GUI element whose siblings are being iterated.
        /// @param handler  [in] The function to call for each element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementPrevSiblings(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the previous siblings of the given element in reverse order.
        ///
        /// @param pElement [in] The GUI element whose siblings are being iterated.
        /// @param handler  [in] The function to call for each element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementPrevSiblingsReverse(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the next siblings of the given element.
        ///
        /// @param pElement [in] The GUI element whose siblings are being iterated.
        /// @param handler  [in] The function to call for each element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementNextSiblings(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the next siblings of the given element in reverse order.
        ///
        /// @param pElement [in] The GUI element whose siblings are being iterated.
        /// @param handler  [in] The function to call for each element.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementNextSiblingsReverse(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the siblings of the given element, not including itself.
        ///
        /// @param pElement [in] The GUI element whose siblings are being iterated.
        /// @param handler  [in] The function to call for each sibling.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementSiblings(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;
        bool IterateElementSiblingsAndNotSelf(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const { return this->IterateElementSiblings(pElement, handler); }

        /// Iterates over the siblings of the given element, including itself.
        ///
        /// @param pElement [in] The GUI element whose siblings are being iterated.
        /// @param handler  [in] The function to call for each sibling.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        bool IterateElementSiblingsAndSelf(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the children of the given element.
        ///
        /// @param pElement [in] The element whose children are being iterated.
        /// @param handler  [in] The function to call for each child.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        ///
        /// @remarks
        ///     This is not recursive. Use IterateElementDescendants() to iterate recursively.
        bool IterateElementChildren(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the descendants of the given element.
        ///
        /// @param pElement [in] The element whose descendants are being iterated.
        /// @param handler  [in] The function to call for each descendant.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        ///
        /// @remarks
        ///     This is recursive.
        bool IterateElementDescendants(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;

        /// Iterates over the ancestor of the given element.
        ///
        /// @param pElement [in] The element whose ancestors are being iterated.
        /// @param handler  [in] The function to call for each ancestory.
        ///
        /// @return False if the iteration was terminated early as a result of handler() returning false. True if everything was iterated.
        ///
        /// @remarks
        ///     This is recursive.
        bool IterateElementAncestors(GUIElement* pElement, std::function<bool (GUIElement*)> handler) const;


        /// Determines if the given element is a direct child of the given element.
        ///
        /// @param pParentElement [in] The parent GUI element.
        /// @param pChildElement  [in] The child GUI element.
        ///
        /// @return True of the given element is a direct child of the other element.
        ///
        /// @remarks
        ///     This is not recursive. To recursively test if the element is a descendant, use IsElementDescendant().
        bool IsElementChild(GUIElement* pParentElement, GUIElement* pChildElement) const;

        /// Determines if the given element is the parent of the other given element.
        ///
        /// @param pChildElement  [in] The child GUI element.
        /// @param pParentElement [in] The parent GUI element.
        ///
        /// @return True if the given element is the parent of the other given element.
        ///
        /// @remarks
        ///     This is not recursive. To recursively test if the element is an ancestor, use IsElementAncestor().
        bool IsElementParent(GUIElement* pChildElement, GUIElement* pParentElement) const;

        /// Determines if the given element is a descendant of the given parent.
        ///
        /// @param pParentElement [in] The parent GUI element.
        /// @param pChildElement  [in] The child GUI element.
        ///
        /// @return True if the given child is a descendant of the given element.
        bool IsElementDescendant(GUIElement* pParentElement, GUIElement* pChildElement) const;

        /// Determines if the given element is an ancestor of the given element.
        ///
        /// @param pChildElement    [in] The parent GUI element.
        /// @param pAncestorElement [in] The child GUI element.
        ///
        /// @return True if the given ancestor element is an ancestor of the given element.
        bool IsElementAncestor(GUIElement* pChildElement, GUIElement* pAncestorElement) const;


        /// Sets the relative position of the given element, in DPI-aware points.
        ///
        /// @param pElement [in] The element whose position is being set.
        /// @param xPos     [in] The X position.
        /// @param yPos     [in] The Y position.
        ///
        /// @remarks
        ///     The will set the positioning to relative and give the left/top corder priority.
        void SetElementPosition(GUIElement* pElement, float xPos, float yPos);
        void SetElementPosition(GUIElement* pElement, int xPos, int yPos);

        /// Sets the left style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose left position is being set.
        /// @param left     [in] The new left position.
        ///
        /// @remarks
        ///     This will give the left position priority over right.
        void SetElementLeftPosition(GUIElement* pElement, int left);

        /// Sets the top style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose top position is being set.
        /// @param top      [in] The new top position.
        ///
        /// @remarks
        ///     This will give the top position priority over bottom.
        void SetElementTopPosition(GUIElement* pElement, int top);

        /// Sets the right style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose right position is being set.
        /// @param right    [in] The new right position.
        ///
        /// @remarks
        ///     This will give the right position priority over left.
        void SetElementRightPosition(GUIElement* pElement, int right);

        /// Sets the bottom style property of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose bottom position is being set.
        /// @param bottom   [in] The new bottom position.
        ///
        /// @remarks
        ///     This will give the bottom position priority over top.
        void SetElementBottomPosition(GUIElement* pElement, int bottom);


        /// Sets whether the element is positioned relative to the inside of the padding, border, or ouside border of the parent element.
        ///
        /// @param hElement [in] The GUI element whose position origin is being set.
        /// @param origin   [in] The new position origin.
        ///
        /// @remarks
        ///     This is only used when the position is relative.
        void SetElementRelativePositionOrigin(GUIElement* pElement, PositionOrigin origin);

        /// Determines whether the element is positioned relative to the inside of the padding, border, or ouside border of the parent element.
        ///
        /// @param hElement [in] The GUI elemetn whose position origin is being retrieved.
        ///
        /// @return The position origin.
        ///
        /// @remarks
        ///     This is only relevant when the position is relative.
        PositionOrigin GetElementRelativePositionOrigin(GUIElement* pElement) const;


        /// Retrieves the actual relative position of the given element.
        ///
        /// @param hElement [in]  The GUI element whose position is being retrieved.
        /// @param xPosOut  [out] A reference to the variable that will receive the X position.
        /// @param yPosOut  [out] A reference to the variable that will receive the Y position.
        void GetElementPosition(GUIElement* pElement, float &xPosOut, float &yPosOut) const;
        void GetElementPosition(GUIElement* pElement, int &xPosOut, int &yPosOut) const;

        /// Retrieves the absolute position of the given element.
        ///
        /// @param hElement [in] The GUI element whose absolute position is being retrieved.
        /// @param xPosOut  [out] A reference to the variable that will receive the absolute X position.
        /// @param yPosOut  [out] A reference to the variable that will receive the absolute Y position.
        ///
        /// @remarks
        ///     This is a simple retrieval function. This will not dynamically compute the position.
        void GetElementAbsolutePosition(GUIElement* pElement, float &xPosOut, float &yPosOut) const;
        void GetElementAbsolutePosition(GUIElement* pElement, int &xPosOut, int &yPosOut) const;

        /// Sets the size of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose size is being set.
        /// @param width    [in] The width of the element.
        /// @param height   [in] The height of the element.
        void SetElementSize(GUIElement* pElement, float width, float height);
        void SetElementSize(GUIElement* pElement, unsigned int width, unsigned int height);

        /// Sets the size of the given element to a ratio of it's parent's size.
        ///
        /// @param hElement    [in] The GUI element whose size is being set.
        /// @param widthRatio  [in] The new width ratio of the element.
        /// @param heightRatio [in] The new height ratio of the element.
        void SetElementSizeRatio(GUIElement* pElement, float widthRatio, float heightRatio);

        /// Sets the size of the given element to tightly wrap it's children.
        ///
        /// @param hElement [in] The GUI element whose size is being set.
        void SetElementSizeToChildren(GUIElement* pElement);



        /// Sets the width of the element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose width is being set.
        /// @param width    [in] The new width of the element.
        ///
        /// @remarks
        ///     If both the width and height needs to be changed use SetGUIElementSize instead.
        void SetElementWidth(GUIElement* pElement, float width);
        void SetElementWidth(GUIElement* pElement, unsigned int width);

        /// Sets the width of the element as a ratio of the parent's width.
        ///
        /// @param hElement   [in] The GUI element whose width is being set.
        /// @param widthRatio [in] The new width ratio of the element.
        ///
        /// @remarks
        ///     If both the width and height needs to be changed, and they are both ratios, use SetGUIElementSizeRatio().
        ///     @par
        ///     The ratio cannot be more than 2 decimal places.
        void SetElementWidthRatio(GUIElement* pElement, float widthRatio);

        /// Sets the width of the element so that it is sized based on it's children.
        ///
        /// @param hElement [in] The GUI element whose width is being set.
        void SetElementWidthToChildren(GUIElement* pElement);

        /// Sets the minimum width of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose min width is being set.
        /// @param minWidth [in] The min width of the element.
        void SetElementMinWidth(GUIElement* pElement, float minWidth);
        void SetElementMinWidth(GUIElement* pElement, unsigned int minWidth);

        /// Sets the minimum width of the given element to a ratio of the parent's width.
        ///
        /// @param hElement      [in] The GUI element whose min width is being set.
        /// @param minWidthRatio [in] The new min width ratio.
        ///
        /// @remarks
        ///     If both the min width and height needs to be changed, and they're both ratios, use SetGUIElementMinSizeRatio().
        ///     @par
        ///     The ratio cannot be to more than 2 decimal places.
        void SetElementMinWidthRatio(GUIElement* pElement, float minWidthRatio);

        /// Sets the min width of the given element to that of it's children.
        ///
        /// @param hElement [in] The GUI element whose having minimum width set.
        void SetElementMinWidthToChildren(GUIElement* pElement);

        /// Sets the min width of the given element to it's default.
        ///
        /// @param hElement [in] The GUI element whose min width is being reset.
        ///
        /// @remarks
        ///     This is equivalent to setting the min width to 0.
        void SetElementMinWidthToDefault(GUIElement* pElement) { this->SetElementMinWidth(pElement, 0U); }

        /// Sets the maxium width of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose max width is being set.
        /// @param maxWidth [in] The max width of the element.
        void SetElementMaxWidth(GUIElement* pElement, float maxWidth);
        void SetElementMaxWidth(GUIElement* pElement, unsigned int maxWidth);

        /// Sets the minimum width of the given element to a ratio of the parent's width.
        ///
        /// @param hElement      [in] The GUI element whose min width is being set.
        /// @param minWidthRatio [in] The new min width ratio.
        ///
        /// @remarks
        ///     If both the min width and height needs to be changed, and they're both ratios, use SetGUIElementMinSizeRatio().
        ///     @par
        ///     The ratio cannot be to more than 2 decimal places.
        void SetElementMaxWidthRatio(GUIElement* pElement, float maxWidthRatio);

        /// Sets the min width of the given element to that of it's children.
        ///
        /// @param hElement [in] The GUI element whose having minimum width set.
        void SetElementMaxWidthToChildren(GUIElement* pElement);

        /// Sets the min width of the given element to it's default.
        ///
        /// @param hElement [in] The GUI element whose min width is being reset.
        ///
        /// @remarks
        ///     This is equivalent to setting the min width to 0.
        void SetElementMaxWidthToDefault(GUIElement* pElement) { this->SetElementMinWidth(pElement, GUIStyleNumber_MaxUnsigned); }



        /// Sets the height of the element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose height is being set.
        /// @param height   [in] The new height of the element.
        ///
        /// @remarks
        ///     If both the width and height needs to be changed use SetGUIElementSize instead.
        void SetElementHeight(GUIElement* pElement, float height);
        void SetElementHeight(GUIElement* pElement, unsigned int height);

        /// Sets the height of the element as a ratio of the parent's height.
        ///
        /// @param hElement [in] The GUI element whose height is being set.
        /// @param height   [in] The new height ratio of the element.
        ///
        /// @remarks
        ///     If both the width and height needs to be changed, and they are both ratios, use SetGUIElementSizeRatio().
        ///     @par
        ///     The ratio cannot be more than 2 decimal places.
        void SetElementHeightRatio(GUIElement* pElement, float heightRatio);

        /// Sets the height of the element so that it is sized based on it's children.
        ///
        /// @param hElement [in] The GUI element whose height is being set.
        void SetElementHeightToChildren(GUIElement* pElement);

        /// Sets the minimum height of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The GUI element whose min height is being set.
        /// @param minHeight [in] The min height of the element.
        void SetElementMinHeight(GUIElement* pElement, float minHeight);
        void SetElementMinHeight(GUIElement* pElement, unsigned int minHeight);

        /// Sets the minimum height of the given element to a ratio of the parent's height.
        ///
        /// @param hElement       [in] The GUI element whose min height is being set.
        /// @param minHeightRatio [in] The new min height ratio.
        ///
        /// @remarks
        ///     If both the min height and height needs to be changed, and they're both ratios, use SetGUIElementMinSizeRatio().
        ///     @par
        ///     The ratio cannot be to more than 2 decimal places.
        void SetElementMinHeightRatio(GUIElement* pElement, float minHeightRatio);

        /// Sets the min height of the given element to that of it's children.
        ///
        /// @param hElement [in] The GUI element whose having minimum height set.
        void SetElementMinHeightToChildren(GUIElement* pElement);

        /// Sets the min height of the given element to it's default.
        ///
        /// @param hElement [in] The GUI element whose min height is being reset.
        ///
        /// @remarks
        ///     This is equivalent to setting the min height to 0.
        void SetElementMinHeightToDefault(GUIElement* pElement) { this->SetElementMinHeight(pElement, 0U); }

        /// Sets the maxium height of the given element, in DPI-aware points.
        ///
        /// @param hElement  [in] The GUI element whose max height is being set.
        /// @param maxHeight [in] The max height of the element.
        void SetElementMaxHeight(GUIElement* pElement, float maxHeight);
        void SetElementMaxHeight(GUIElement* pElement, unsigned int maxHeight);

        /// Sets the minimum height of the given element to a ratio of the parent's height.
        ///
        /// @param hElement       [in] The GUI element whose min height is being set.
        /// @param minHeightRatio [in] The new min height ratio.
        ///
        /// @remarks
        ///     If both the min height and height needs to be changed, and they're both ratios, use SetGUIElementMinSizeRatio().
        ///     @par
        ///     The ratio cannot be to more than 2 decimal places.
        void SetElementMaxHeightRatio(GUIElement* pElement, float maxHeightRatio);

        /// Sets the min height of the given element to that of it's children.
        ///
        /// @param hElement [in] The GUI element whose having minimum height set.
        void SetElementMaxHeightToChildren(GUIElement* pElement);

        /// Sets the min height of the given element to it's default.
        ///
        /// @param hElement [in] The GUI element whose min height is being reset.
        ///
        /// @remarks
        ///     This is equivalent to setting the min height to 0.
        void SetElementMaxHeightToDefault(GUIElement* pElement) { this->SetElementMinHeight(pElement, GUIStyleNumber_MaxUnsigned); }


        /// Retrieves the actual width of the element.
        ///
        /// @param hElement [in] The GUI element whose actual width is being retrieved.
        ///
        /// @return The actual width of the element.
        float GetElementWidth(GUIElement* pElement) const;

        /// Retrieves the actual height of the element.
        ///
        /// @param hElement [in] The GUI element whose actual height is being retrieved.
        ///
        /// @return The actual height of the element.
        float GetElementHeight(GUIElement* pElement) const;


        /// Retrieves the absolute rectangle of the given element.
        ///
        /// @param hElement [in] The GUI element whose absolute rectangle is being retrieved.
        /// @param rectOut  [out] A reference to the structure that will receive the absolute rectangle of the given element.
        void GetElementAbsoluteRect(GUIElement* pElement, GTLib::Rect<float> &rectOut) const;
        void GetElementAbsoluteRect(GUIElement* pElement, GTLib::Rect<int> &rectOut) const;


        /// Sets the positiong of the given element.
        ///
        /// @param hElement    [in] The element whose positioning is changing.
        /// @param positioning [in] The new position to use with the element.
        ///
        /// @remarks
        ///     Use this to change between absolute, relative and auto-positioning.
        void SetElementPositioning(GUIElement* pElement, Positioning positioning);

        /// Retrieves the positioning of the given element.
        ///
        /// @param hElement [in] The element whose positioning is being retrieved.
        ///
        /// @return The positioning of the element.
        Positioning GetElementPositioning(GUIElement* pElement) const;



        /// Sets the background colour of the given element.
        ///
        /// @param hElement [in] The GUI element whose background colour is being changed.
        /// @param colour   [in] The new background colour.
        void SetElementBackgroundColour(GUIElement* pElement, const GTLib::Colour &colour);

        /// Retrieves the background colour of the given element.
        ///
        /// @param hElement [in] The GUI element whose background colour is being retrieved.
        ///
        /// @return The background colour of the given element.
        GTLib::Colour GetElementBackgroundColour(GUIElement* pElement) const;


        /// Sets the width of the left border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderLeftWidth(GUIElement* pElement, uint32_t width);

        /// Sets the width of the top border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderTopWidth(GUIElement* pElement, uint32_t width);

        /// Sets the width of the right border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderRightWidth(GUIElement* pElement, uint32_t width);

        /// Sets the width of the bottom border, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderBottomWidth(GUIElement* pElement, uint32_t width);

        /// Sets the width of the border of the given element, in DPI-aware points.
        ///
        /// @param hElement    [in] The element whose border width is being set.
        /// @param borderWidth [in] The border width.
        void SetElementBorderWidth(GUIElement* pElement, uint32_t width);


        /// Sets the colour of the left border of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderLeftColor(GUIElement* pElement, const GTLib::Colour &borderColor);

        /// Sets the colour of the top border of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderTopColor(GUIElement* pElement, const GTLib::Colour &borderColor);

        /// Sets the colour of the right border of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderRightColor(GUIElement* pElement, const GTLib::Colour &borderColor);

        /// Sets the colour of the bottom border of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderBottomColor(GUIElement* pElement, const GTLib::Colour &borderColor);

        /// Sets the colour of all four borders of the given element.
        ///
        /// @param hElement    [in] The element whose border colour is being set.
        /// @param borderColor [in] The border colour.
        void SetElementBorderColor(GUIElement* pElement, const GTLib::Colour &borderColor);


        /// Sets the width and color of the left border.
        ///
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorderLeft(GUIElement* pElement, uint32_t width, const GTLib::Colour &borderColor);

        /// Sets the width and color of the top border.
        ///
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorderTop(GUIElement* pElement, uint32_t width, const GTLib::Colour &borderColor);

        /// Sets the width and color of the right border.
        ///
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorderRight(GUIElement* pElement, uint32_t width, const GTLib::Colour &borderColor);

        /// Sets the width and color of the bottom border.
        ///
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorderBottom(GUIElement* pElement, uint32_t width, const GTLib::Colour &borderColor);

        /// Sets the border of the given element.
        ///
        /// @param hElement [in] The element whose border is being set.
        /// @param width    [in] The width of the border, in DPI-aware points.
        /// @param color    [in] The colour of the border.
        void SetElementBorder(GUIElement* pElement, uint32_t width, const GTLib::Colour &colour);


        /// Sets the left padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The left padding.
        void SetElementPaddingLeft(GUIElement* pElement, uint32_t padding);
        
        /// Sets the top padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The top padding.
        void SetElementPaddingTop(GUIElement* pElement, uint32_t padding);

        /// Sets the right padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The right padding.
        void SetElementPaddingRight(GUIElement* pElement, uint32_t padding);

        /// Sets the bottom padding of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The bottom padding.
        void SetElementPaddingBottom(GUIElement* pElement, uint32_t padding);
        
        /// Sets the padding of all sides of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose padding is being set.
        /// @param padding  [in] The padding that will be applied to all sides.
        ///
        /// @remarks
        ///     This will set the padding to all sides.
        void SetElementPadding(GUIElement* pElement, uint32_t padding);


        /// Sets the left margin of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose left margin is being set.
        /// @param margin   [in] The left margin.
        void SetElementMarginLeft(GUIElement* pElement, uint32_t margin);

        /// Sets the top margin of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose top margin is being set.
        /// @param margin   [in] The top margin.
        void SetElementMarginTop(GUIElement* pElement, uint32_t margin);

        /// Sets the right margin of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose right margin is being set.
        /// @param margin   [in] The right margin.
        void SetElementMarginRight(GUIElement* pElement, uint32_t margin);

        /// Sets the bottom margin of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose bottom margin is being set.
        /// @param margin   [in] The bottom margin.
        void SetElementMarginBottom(GUIElement* pElement, uint32_t margin);

        /// Sets the margin of all sides of the given element, in DPI-aware points.
        ///
        /// @param hElement [in] The element whose margin is being set.
        /// @param margin   [in] The margin that will be applied to all sides.
        ///
        /// @param 
        ///     This will set th emargin to all sides.
        void SetElementMargin(GUIElement* pElement, uint32_t margin);


        /// Sets the axis the children of the given element run along (vertical or horizontal)
        ///
        /// @param hElement [in] The GUI element whose child axis is changing.
        /// @param axis     [in] The new axis.
        void SetElementChildAxis(GUIElement* pElement, ChildAxis axis);

        /// Retrieves the axis the children of thi given element run along (vertical or horizontal)
        ///
        /// @param hElement [in] The GUI element whose child axis is being retrieved.
        ///
        /// @return The axis the children of the given element run along (vertical or horizontal)
        ChildAxis GetElementChildAxis(GUIElement* pElement) const;


        /// Sets the horizontal alignment of the children.
        ///
        /// @param hElement [in] The GUI element whose horizontal alignment is being set.
        /// @param align    [in] The new horizontal alignment for children.
        void SetElementHorizontalAlign(GUIElement* pElement, HorizontalAlign align);

        /// Retrieves the horizontal alignment of children of the given element.
        ///
        /// @param hElement [in] The GUI element whose horizontal alignment is being retrieved.
        ///
        /// @return The horizontal alignment of children of the given element.
        HorizontalAlign GetElementHorizontalAlign(GUIElement* pElement) const;

        /// Sets the vertical alignment of the children.
        ///
        /// @param hElement [in] The GUI element whose vertical alignment is being set.
        /// @param align    [in] The new vertical alignment for children.
        void SetElementVerticalAlign(GUIElement* pElement, VerticalAlign align);

        /// Retrieves the vertical alignment of children of the given element.
        ///
        /// @param hElement [in] The GUI element whose vertical alignment is being retrieved.
        ///
        /// @return The vertical alignment of children of the given element.
        VerticalAlign GetElementVerticalAlign(GUIElement* pElement) const;


        /// Sets the boundary for the widths of child elements with relative widths.
        ///
        /// @param hElement [in] The GUI element whose width boundary is being changed.
        /// @param boundary [in] The new boundary mode.
        void SetElementChildrenWidthBoundary(GUIElement* pElement, ChildrenSizeBoundary boundary);

        /// Retrieves the boundary for the widths of child elements with relative widths.
        ChildrenSizeBoundary GetElementChildrenWidthBoundary(GUIElement* pElement) const;

        /// Sets the boundary for the heights of child elements with relative heights.
        ///
        /// @param hElement [in] The GUI element whose height boundary is being changed.
        /// @param boundary [in] The new boundary mode.
        void SetElementChildrenHeightBoundary(GUIElement* pElement, ChildrenSizeBoundary boundary);

        /// Retrieves the boundary for the heights of child elements with relative heights.
        ChildrenSizeBoundary GetElementChildrenHeightBoundary(GUIElement* pElement) const;

        /// Helper function for setting the boundary for the sizes of children elements with relative sizes.
        ///
        /// @param hElement [in] The GUI element whose size boundary is being changed.
        /// @param boundary [in] The new boundary mode.
        void SetElementChildrenSizeBoundary(GUIElement* pElement, ChildrenSizeBoundary boundary);


        /// Sets the clipping boundary for child elements.
        ///
        /// @param hElement [in] A reference to the element whose clipping boundary is being set.
        /// @param boundary [in] The new clipping boundary.
        ///
        /// @remarks
        ///     This method controls whether or not children are clipped against the elements padding, border, or outer area.
        void SetElementClippingBoundary(GUIElement* pElement, ClippingBoundary boundary);

        /// Retrieves the clipping boundary used with the given element.
        ///
        /// @param hElement [in] A reference to the element whose clipping boundary is being retrieved.
        ClippingBoundary GetElementClippingBoundary(GUIElement* pElement) const;


        /// Sets the clipping mode of the given element.
        ///
        /// @param hElement [in] A handle to the element whose clipping mode is being set.
        /// @param clippingMode [in] The new clipping mode.
        ///
        /// @remarks
        ///     To revert back to defaults, use ClippingMode_Auto or ClippingMode_Default.
        ///     @par
        ///     Use this to change the clipping mode from the defaults.
        void SetElementClippingMode(GUIElement* pElement, ClippingMode clippingMode);

        /// Retrieves the clipping mode of the given element.
        ///
        /// @param hElement [in] A handle to the element whose clipping mode is being retrieved.
        ///
        /// @return The clipping mode of the given element.
        ClippingMode GetElementClippingMode(GUIElement* pElement) const;


        /// Sets whether or not the element is visible.
        ///
        /// @param hElement [in] The GUI element whose visibility state is being set.
        /// @param visible  [in] Whether or not the element should be made visible.
        void SetElementVisibility(GUIElement* pElement, bool visible);

        /// Hides the given element.
        ///
        /// @param hElement [in] The GUI element that's being hiden.
        void HideElement(GUIElement* pElement) { this->SetElementVisibility(pElement, false); }

        /// Shows the given element.
        void ShowElement(GUIElement* pElement) { this->SetElementVisibility(pElement, true); }

        /// Determines whether or not the element is visible.
        ///
        /// @param hElement [in] The GUI element whose visibility state is being determined.
        ///
        /// @return True if the given element is visible; false otherwise.
        ///
        /// @remarks
        ///     Because an invisible element will render it's descendants invisible, this will be called recursively on ancestors.
        bool IsElementVisible(GUIElement* pElement) const;


        /// Enables flexing of the width's of percentage-sized child elements.
        ///
        /// @param hElement [in] The GUI element that child flexing is being enabled on.
        void EnableElementChildWidthFlexing(GUIElement* pElement);

        /// Disbles flexing of the width's of percentage-sized child elements.
        ///
        /// @param hElement [in] The GUI element that child flexing is being disabled on.
        void DisableElementChildWidthFlexing(GUIElement* pElement);

        /// Determines whether or not flexing is enabled on the children of the given element.
        ///
        /// @param hElement [in] The GUI element whose child flexing state is being retrieved.
        ///
        /// @return True if flexing is enabled on the width's of the children of the given element.
        bool IsElementChildWidthFlexingEnabled(GUIElement* pElement);


        /// Enables flexing of the width's of percentage-sized child elements.
        ///
        /// @param hElement [in] The GUI element that child flexing is being enabled on.
        void EnableElementChildHeightFlexing(GUIElement* pElement);

        /// Disbles flexing of the heights's of percentage-sized child elements.
        ///
        /// @param hElement [in] The GUI element that child flexing is being disabled on.
        void DisableElementChildHeightFlexing(GUIElement* pElement);

        /// Determines whether or not flexing is enabled on the children of the given element.
        ///
        /// @param hElement [in] The GUI element whose child flexing state is being retrieved.
        ///
        /// @return True if flexing is enabled on the width's of the children of the given element.
        bool IsElementChildHeightFlexingEnabled(GUIElement* pElement);


        /// Sets the text of the given element.
        ///
        /// @param hElement [in] A handle to the element whose text is being set.
        /// @param text     [in] The element's text.
        void SetElementText(GUIElement* pElement, const char* text);

        /// Retrieves the text of the given element.
        ///
        /// @param hElement [in] A handle to the element whose text is being retrieved.
        ///
        /// @return A pointer to the buffer containing the element's text. This will be null-terminated.
        const char* GetElementText(GUIElement* pElement) const;

        /// Deterimes whether or not the given element has text.
        ///
        /// @param hElement [in] A handle to the element whose to check.
        ///
        /// @return True if the given element has any text.
        bool DoesElementHaveText(GUIElement* pElement) const;

        /// Sets the font of the given element.
        ///
        /// @param hElement [in] A handle to the GUI element whose font is being set.
        /// @param family   [in] The font family ('Arial', 'Courier', etc.)
        /// @param weight   [in] The font's weight (bold, medium, etc.)
        /// @param slant    [in] The font's slant (none, italic, oblique)
        /// @param size     [in] The size of the font.
        /// @param sizeType [in] Specifies whether or not the size is in points or pixels. Pixels by default.
        ///
        /// @return A pointer to the font object that is assigned to the element.
        HGUIFont SetElementFont(GUIElement* pElement, const char* family, FontWeight weight, FontSlant slant, uint32_t size, uint32_t sizeType = NumberType_Pixels);

        /// Retrieves the current font of the given element.
        ///
        /// @param hElement [in] A handle to the GUI element whose font is being retrieved.
        ///
        /// @return A pointer to the font object.
        HGUIFont GetElementFont(GUIElement* pElement) const;


        /// Sets the colour of the element's text.
        ///
        /// @param hElement [in] A handle to the GUI element whose text colour is being set.
        /// @param color    [in] The new color of the element's text.
        void SetElementTextColor(GUIElement* pElement, const GTLib::Colour &color);

        /// Retrieves the colour of the given element's text.
        ///
        /// @param hElement [in] A handle to the GUI element whose text colour is being retrieved.
        ///
        /// @return The colour of the text.
        GTLib::Colour GetElementTextColor(GUIElement* pElement) const;



        /// Attaches the given element to the given surface.
        ///
        /// @param hElement [in] A handle to the element whose surface is changing.
        /// @param hSurface [in] A handle to the new surface.
        ///
        /// @return True if the surface is changed; false otherwise.
        ///
        /// @remarks
        ///     The surface of a child element can not be changed - it is always set the surface of it's ancestors.
        ///     @par
        ///     This will recursively change the surface of the children.
        bool AttachElementToSurface(GUIElement* pElement, GUISurface* pSurface);

        /// Detaches the given element from the given surface.
        ///
        /// @param hElement [in] A handle to the element being detached.
        ///
        /// @return True if the surface is changed; false otherwise.
        ///
        /// @remarks
        ///     The surface of a child element can not be changed - it is always set the surface of it's ancestors.
        ///     @par
        ///     This will recursively change the surface of the children.
        ///     @par
        ///     This will return false if hElement is not a root level element.
        bool DetachElementFromSurface(GUIElement* pElement);

        /// Retrieves the surface the element is currently attached to.
        ///
        /// @param hElement [in] A handle to the element whose surface is being retrieved.
        GUISurface* GetElementSurface(GUIElement* pElement) const;



        ////////////////////////////////////////////////////////////////
        // Batching

        /// Begins a batch operator.
        ///
        /// @remarks
        ///     Batched operations allow the context to more efficeintly perform bulk operations. The main thing here is batching
        ///     layout and painting operations to avoid redundancy.
        ///     @par
        ///     Begin/end batching pairs can be called within each other, but they should always be paired together correctly.
        void BeginBatch();

        /// Ends a batch operations, and performs all of the required operations to get the relevant state up to date.
        ///
        /// @remarks
        ///     This should be paired with a called to BeginBatch(). This is where repainting will occur for surfaces whose painting
        ///     mode is not deferred.
        void EndBatch();


        ////////////////////////////////////////////////////////////////
        // Painting

        /// Invalidates a rectangular region of the given surface.
        ///
        /// @param hSurface [in] The surface whose rectangle region is being invalidated.
        /// @param rect     [in] The rectangle to invalidate.
        void InvalidateSurfaceRect(GUISurface* pSurface, const GTLib::Rect<int> &rect);

        /// Paints a portion of the given surface.
        ///
        /// @param hSurface   [in] The surface to paint.
        /// @param rect       [in] The rectangle region to paint.
        /// @param pInputData [in] A pointer that will be passed to the BeginPaintSurface() method of the renderer.
        void PaintSurface(GUISurface* pSurface, const GTLib::Rect<int> &rect, void* pInputData = nullptr);

        /// Paints the entire region of the given surface.
        ///
        /// @param hSurface   [in] A handle to the surface to paint.
        /// @param pInputData [in] A pointer that will be passed to the BeginPaintSurface() method of the renderer.
        void PaintSurface(GUISurface* pSurface, void* pInputData = nullptr);


        ////////////////////////////////////////////////////////////////
        // Inbound Events
        //
        // These methods are called by the host application in response to certain events such as when the mouse moves within a surface.

        /// Called by the host application when the mouse moves within the given surface.
        ///
        /// @param hSurface  [in] A handle to the surface the mouse was moved within.
        /// @param mousePosX [in] The x position of the cursor on the surface relative to the top left corner.
        /// @param mousePosY [in] The y position of the cursor on the surface relative to the top left corner.
        void OnMouseMove(GUISurface* pSurface, int mousePosX, int mousePosY);



        ////////////////////////////////////////////////////////////////
        // Finding/Gathering

        /// Finds the element sitting directly under the given point on the given surface.
        ///
        /// @param hSurface [in] A handle to the surface to search for.
        /// @param x        [in] The position of the point on the x axis relative to the top left corner.
        /// @param y        [in] The position of the point on the y axis relative to the top left corner.
        ///
        /// @return A handle to the element sitting directly under the point, or 0 if no element is sitting under the point.
        ///
        /// @remarks
        ///     This will take clipping into account.
        GUIElement* FindElementUnderPoint(GUISurface* pSurface, int x, int y);



        ////////////////////////////////////////////////////////////////
        // DPI / Scaling

        /// Retrieves the base DPI on the X axis.
        unsigned int GetBaseXDPI() const;

        /// Retrieves the base DPI on the Y axis.
        unsigned int GetBaseYDPI() const;

        /// Retrieves the base DPI on both the X and Y axis.
        ///
        /// @param xBaseDPIOut [out] A reference to the variable that will receive the base DPI on the X axis.
        /// @param yBaseDPIOut [out] A reference to the variable that will receive the base DPI on the Y axis.
        void GetBaseDPI(unsigned int &xBaseDPIOut, unsigned int &yBaseDPIOut) const;

        /// Sets the base DPI on both the X and Y axis.
        ///
        /// @param xBaseDPI [in] The new base DPI on the X axis.
        /// @param yBaseDPI [in] The new base DPI on the Y axis.
        ///
        /// @remarks
        ///     This will cause a re-validation of the layout and will redraw every surface.
        void SetBaseDPI(unsigned int xBaseDPI, unsigned int yBaseDPI);


        /// Retrieves the DPI on the X axis.
        unsigned int GetXDPI(GUISurface* pSurface) const;

        /// Retrieves the DPI on the Y axis.
        unsigned int GetYDPI(GUISurface* pSurface) const;

        /// Retrieves the DPI on both the X and Y axis.
        ///
        /// @param xDPIOut [out] A reference to the variable that will receive the DPI on the X axis.
        /// @param yDPIOut [out] A reference to the variable that will receive the DPI on the Y axis.
        void GetDPI(GUISurface* pSurface, unsigned int &xDPIOut, unsigned int &yDPIOut);

        /// Sets the DPI on both the X and Y axis.
        ///
        /// @param xDPI [in] The new DPI on the X axis.
        /// @param yDPI [in] The new DPI on the Y axis.
        ///
        /// @remarks
        ///     This will cause a re-validation of the layout and will redraw every surface.
        void SetDPI(GUISurface* pSurface, unsigned int xDPI, unsigned int yDPI);


        /// Retrieves the DPI scaling factor for the X axis based on the given surface's DPI.
        ///
        /// @param hSurface [in] The surface whose DPI is being used to calculate the scaling factor.
        ///
        /// @return A float equal to the current DPI divided by the base DPI for the X axis.
        float GetXDPIScalingFactor(GUISurface* pSurface) const;

        /// Retrieves the DPI scaling factor for the Y axis based on the given surface's DPI.
        ///
        /// @param hSurface [in] The surface whose DPI is being used to calculate the scaling factor.
        ///
        /// @return a float equal to the current DPI divided by the base DPI for the Y axis.
        float GetYDPIScalingFactor(GUISurface* pSurface) const;


        ////////////////////////////////////////////////////////////////
        // Utilities

        /// Converts a point from absolute to relative coordinates based on the given element.
        ///
        /// @param hElement [in]  A handle to the element to make the point relative to.
        /// @param xIn      [in]  The absolute position on the x axis to convert.
        /// @param yIn      [in]  The absolute position on the y axis to convert.
        /// @param xOut     [out] A reference to the variable that will receive the relative position on the x axis.
        /// @param yOut     [out] A reference to the variable that will receive the relative position on the y axis.
        void AbsoluteToRelative(GUIElement* pElement, int xIn, int yIn, int &xOut, int &yOut) const;


        ////////////////////////////////////////////////////////////////
        // Miscellaneous

        /// Retrieves a pointer to the font manager.
        ///
        /// @return A pointer to the font manager.
        GUIFontManager* GetFontManager();


    private:

        /// Recursively sets the surface for the given element and it's children.
        ///
        /// @param element  [in] A reference to the element whose surface is being set.
        /// @param hSurface [in] A handle to the surface.
        void SetElementSurfaceRecursive(GUIElement* pElement, GUISurface* pSurface);


        /// Retrieves the rectangle children of the given element will be clipped against.
        ///
        /// @param hElement [in]  The GUI element whose clipping rectangle is being retrieved.
        /// @param rectOut  [out] A reference to the object that will receive the children clipping rectangle.
        void GetElementChildrenClippingRect(GUIElement* pElement, GTLib::Rect<float> &rectOut) const;
        void GetElementChildrenClippingRect(GUIElement* pElement, GTLib::Rect<int> &rectOut) const;


        /// Determines whether or not the given element is currently configured to be clipped against it's parent.
        ///
        /// @param element [in] A reference to the element whose clipping state is being retrieved.
        bool IsElementClippedAgainstParent(GUIElement* pElement) const;


        /// Performs a clipped traversals of the children of the given element.
        ///
        /// @param element      [in] A reference the root element.
        /// @param clippingRect [in] The rectangle to clip against.
        /// @param func         [in] The function to call for the input element and each of it's children.
        ///
        /// @remarks
        ///     This will do a pre-order depth-first traversal, and will include the root element.
        ///     @par
        ///     The delegate function will not be called if the child is completely clipped.
        void ClippedTraversal(GUIElement* pElement, const GTLib::Rect<float> &clippingRect, std::function<void (GUIElement*, const GTLib::Rect<int> &)> func);


        /// Updates the given element's font based on it's current style.
        ///
        /// @param element [in] A reference to the element whose having it's font updated.
        ///
        /// @return A pointer to the element's new font.
        HGUIFont UpdateElementFontFromStyle(GUIElement* pElement);



        ////////////////////////////////////////////////////////////////
        // DPI / Scaling

        /// Updates every element as required as a result of a change in DPI.
        ///
        /// @param surface [in] A reference to the surface whose DPI has changed.
        ///
        /// @remarks
        ///     This will update the layout and font of every element.
        void UpdateAllElementsOnDPIChange(GUISurface* pSurface);
        void UpdateAllElementsOnDPIChange();

        /// Updates the size of each border of the given element based on their style and current DPI scaling.
        ///
        /// @param element [in] A reference to the element whose border sizes are being updated.
        ///
        /// @remarks
        ///     This does not invalidate the layout.
        void UpdateElementBorderSizes(GUIElement* pElement);

        /// Updates the size of the margins of the given element based on their style and current DPI scaling.
        ///
        /// @param element [in] A reference to the element whose margin sizes are being updated.
        ///
        /// @remarks
        ///     This does not invalidate the layout.
        void UpdateElementMarginSizes(GUIElement* pElement);

        /// Updates the size of the padding of the given element based on their style and current DPI scaling.
        ///
        /// @param element [in] A reference to the element whose padding sizes are being updated.
        ///
        /// @remarks
        ///     This does not invalidate the layout.
        void UpdateElementPaddingSizes(GUIElement* pElement);


        //////////////////////////////////////////////////
        // Painting

        /// Marks the given rectangle as invalid which will cause a repaint.
        ///
        /// @param surface [in] The surface whose rectangle region is being invalidated.
        /// @param rect    [in] The rectangle to invalidate.
        ///
        /// @remarks
        ///     Invalidating a rectangle does not redraw it. It will instead be redrawn when the host application requests it with Paint().
        void Painting_InvalidateRect(GUISurface* pSurface, const GTLib::Rect<int> &rect);

        /// Invalidates the rectangle of the given element.
        ///
        /// @param element [in] A reference to the element whose rectangle is being invalidated.
        void Painting_InvalidateElementRect(GUIElement* pElement);

        /// Paints the invalid rectangles of every surface that has an invalid region.
        ///
        /// @remarks
        ///     This does not paint surfaces whose painting modes are set to GUIPaintingMode::Deferred.
        ///     @par
        ///     This will mark every invalid rectangle as valid.
        void Painting_PaintAndValidateSurfaceRects();

        /// Paints the given element and it's descendents if they fall into the given rectangle.
        ///
        /// @param surface      [in] The surface being drawn to.
        /// @param element      [in] A reference to the element to draw.
        /// @param clippingRect [in] The rectangle to clip against.
        ///
        /// @remarks
        ///     If the element falls outside the given rectangle, it will ignored, as will it's descendents that are clipped against it.
        void Painting_PaintElement(GUISurface* pSurface, GUIElement* pElement, const GTLib::Rect<int> &clippingRect);

        /// Optimized function for setting the clipping and drawing the rectangle at the same time.
        ///
        /// @param surface [in] The surface to draw on.
        /// @param rect    [in] The clipping rectangle, and the rectangle to draw.
        /// @param color   [in] The color to draw the clipping rectangle.
        ///
        /// @remarks
        ///     This function simply combines SetClippingRect and DrawRectangle into a single optimized call.
        void Painting_DrawAndSetClippingRect(GUISurface* pSurface, const GTLib::Rect<int> &rect, const GTLib::Colour &color);




        //////////////////////////////////////////////////
        // Layout

        /// Helper method for setting the size of the given element.
        ///
        /// @param hElement   [in] A handle to the element whose size is being set.
        /// @param width      [in] The width of the element as a 32-bit unsigned int, the value of which is determined by widthType
        /// @param widthType  [in] The type of number the width is (actual, percentage, etc.)
        /// @param height     [in] The height of the element as a 32-bit unsigned int, the value of which is determined by heightType
        /// @param heightType [in] The type of number the height is (actual, precentage, etc.)
        ///
        /// @remarks
        ///     widthType and heightType can be one of the NumberType_* contants.
        void Layout_SetElementSize(GUIElement* pElement, uint32_t width, uint32_t widthType, uint32_t height, uint32_t heightType);

        /// Helper method for setting the width of the given element.
        ///
        /// @param hElement   [in] A handle to the element whose size is being set.
        /// @param width      [in] The width of the element as a 32-bit unsigned int, the value of which is determined by widthType
        /// @param widthType  [in] The type of number the width is (actual, percentage, etc.)
        ///
        /// @remarks
        ///     widthType can be one of the NumberType_* contants.
        void Layout_SetElementWidth(GUIElement* pElement, uint32_t width, uint32_t widthType);

        /// Helper method for setting the min width of the given element.
        ///
        /// @param hElement  [in] A handle to the element whose min size is being set.
        /// @param minWidth  [in] The min width of the element as a 32-bit unsigned int, the value of which is determined by widthType.
        /// @param widthType [in] The type of number the width is (actual, percentage, etc.)
        void Layout_SetElementMinWidth(GUIElement* pElement, uint32_t minWidth, uint32_t widthType);

        /// Helper method for setting the max width of the given element.
        ///
        /// @param hElement  [in] A handle to the element whose min size is being set.
        /// @param minWidth  [in] The ax width of the element as a 32-bit unsigned int, the value of which is determined by widthType.
        /// @param widthType [in] The type of number the width is (actual, percentage, etc.)
        void Layout_SetElementMaxWidth(GUIElement* pElement, uint32_t maxWidth, uint32_t widthType);

        /// Helper method for setting the height of the given element.
        ///
        /// @param hElement   [in] A handle to the element whose size is being set.
        /// @param height     [in] The height of the element as a 32-bit unsigned int, the value of which is determined by heightType
        /// @param heightType [in] The type of number the height is (actual, precentage, etc.)
        ///
        /// @remarks
        ///     heightType can be one of the NumberType_* contants.
        void Layout_SetElementHeight(GUIElement* pElement, uint32_t height, uint32_t heightType);

        /// Helper method for setting the min height of the given element.
        ///
        /// @param hElement  [in] A handle to the element whose min size is being set.
        /// @param minWidth  [in] The min height of the element as a 32-bit unsigned int, the value of which is determined by widthType.
        /// @param widthType [in] The type of number the height is (actual, percentage, etc.)
        void Layout_SetElementMinHeight(GUIElement* pElement, uint32_t minHeight, uint32_t heightType);

        /// Helper method for setting the max height of the given element.
        ///
        /// @param hElement  [in] A handle to the element whose min size is being set.
        /// @param minWidth  [in] The ax height of the element as a 32-bit unsigned int, the value of which is determined by widthType.
        /// @param widthType [in] The type of number the height is (actual, percentage, etc.)
        void Layout_SetElementMaxHeight(GUIElement* pElement, uint32_t maxHeight, uint32_t heightType);


        /// Invalidates the layout of the given element.
        ///
        /// @param hElement [in] A reference to the element whose layout is being invaliated.
        /// @param flags    [in] The flags specifying which layout properties have changed.
        ///
        /// @remarks
        ///     When a change is made to an element such that it's layout needs updating, this will need to be called on that element.
        ///     @par
        ///     Basically, this function just marks the layout as invalid - it does not actually update the layout. The layout will be
        ///     updated when ValidateGUIElementLayouts() is called.
        void Layout_InvalidateElementLayout(GUIElement* pElement, unsigned int flags);

        /// Invalidates the applicable layouts of the applicable elements based on when the inner width of the given element changes.
        ///
        /// @param element [in] A reference to the element whose inner width has changed.
        void Layout_InvalidateElementLayoutsOnInnerWidthChange(GUIElement* pElement);

        /// Invalidates the applicable layouts of the applicable elements based on when the inner height of the given element changes.
        ///
        /// @param element [in] A reference to the element whose inner height has changed.
        void Layout_InvalidateElementLayoutsOnInnerHeightChange(GUIElement* pElement);

        /// Invalidates the applicable layouts of the applicable elements based on when the inner size (width and height) of the given element changes.
        ///
        /// @param element [in] A reference to the element whose inner size has changed.
        void Layout_InvalidateElementLayoutsOnInnerSizeChange(GUIElement* pElement);


        /// Invalidates the applicable layouts of the applicable elements based on when the child axis of the given element changes.
        ///
        /// @param element [in] A reference to the element whose child axis has changed.
        void Layout_InvalidateElementLayoutsOnChildAxisChange(GUIElement* pElement);


        /// Invalidates the applicable layouts of the applicable elements based on when the horizontal align of the given element changes.
        ///
        /// @param element [in] A reference to the element whose horizontal alignment has changed.
        void Layout_InvalidateElementLayoutsOnHorizontalAlignChange(GUIElement* pElement);

        /// Invalidates the applicable layouts of the applicable elements based on when the vertical align of the given element changes.
        ///
        /// @param element [in] A reference to the element whose vertical alignment has changed.
        void Layout_InvalidateElementLayoutsOnVerticalAlignChange(GUIElement* pElement);



        /// Validates the layouts of every element whose layout is invalid.
        ///
        /// @remarks
        ///     This is where the layouts of elements are actually updated. By the time this method returns, all elements should have valid layouts.
        void Layout_ValidateElementLayouts();

        /// Validates the layout of the given element.
        ///
        /// @param element [in] The element whose layout is being validated.
        void Layout_ValidateElementLayout(GUIElement* pElement);

        /// Validates the width of the given element.
        ///
        /// @param element [in] The element whose width is being validated.
        void Layout_ValidateElementWidth(GUIElement* pElement);

        /// Validates the height of the given element.
        ///
        /// @param element [in] The element whose hieght is being validated.
        void Layout_ValidateElementHeight(GUIElement* pElement);

        /// Validates the position of the given element.
        ///
        /// @param element               [in] The element whose position is being validated.
        ///
        /// @remarks
        ///     The will validate both the X and Y position.
        void Layout_ValidateElementPosition(GUIElement* pElement);

        /// Validates the position of the given element based on absolute positioning.
        ///
        /// @param element               [in] The element whose position is being validated.
        ///
        /// @remarks
        ///     This should only be called when the positioning of the given element is absolute.
        void Layout_ValidateElementPosition_Absolute(GUIElement* pElement);

        /// Validates the position of the given element based on relative positioning.
        ///
        /// @param element [in] The element whose position is being validated.
        ///
        /// @remarks
        ///     This should only be called when the positioning of the given element is relative.
        void Layout_ValidateElementPosition_Relative(GUIElement* pElement);

        /// Validates the position of the given element based on automatic positioning.
        ///
        /// @param element [in] The element whose position is being validated.
        ///
        /// @remarks
        ///     This should only be called when the positioning of the given element is automatic.
        void Layout_ValidateElementPosition_Auto(GUIElement* pElement);

        /// Validates the text layout of the given element.
        ///
        /// @param element [in] The element whose text layout is being validated.
        void Layout_ValidateElementText(GUIElement* pElement);


        /// Marks the width of the givn element as valid.
        ///
        /// @param element [in] A reference to the element whose width is being marked as valid.
        void Layout_MarkElementWidthAsValid(GUIElement* pElement);

        /// Marks the height of the givn element as valid.
        ///
        /// @param element [in] A reference to the element whose height is being marked as valid.
        void Layout_MarkElementHeightAsValid(GUIElement* pElement);

        /// Marks the position of the given element as valid.
        ///
        /// @param element [in] A reference to the element whose position is being marked as valid.
        void Layout_MarkElementPositionAsValid(GUIElement* pElement);

        /// Marks the layout of the text of the given element as valid.
        ///
        /// @param element [in] A reference to the element whose text layout is being marked as valid.
        void Layout_MarkElementTextAsValid(GUIElement* pElement);


        /// Updates the width of the given element, returning it's new outer width (width + horizontal margins).
        ///
        /// @param element [in] The element whose width is being updated.
        float Layout_UpdateElementWidth(GUIElement* pElement);

        /// Updastes the height of the given element, returning it's new outer height (width + vertical margins).
        ///
        /// @param element [in] The element whose height is being updated.
        float Layout_UpdateElementHeight(GUIElement* pElement);


        /// Calculates the width of the given element based on the given width style attribute.
        ///
        /// @param element         [in] The element whose new width is being calculated.
        /// @param width           [in] The width style value.
        /// @param widthType       [in] The width style value type (%, absolute, etc).
        /// @param calculateFlexed [in] Whether or not flexing should be calculated.
        ///
        /// @remarks
        ///     This does not set the new width.
        float Layout_CalculateElementWidth(GUIElement* pElement, uint32_t width, uint32_t widthType, bool calculateFlexed);

        /// Calculates the height of the given element based on the given height style attribute.
        ///
        /// @param element         [in] The element whose height is being calculated.
        /// @param height          [in] The height style value.
        /// @param heightType      [in] The height style value type (%, absolute, etc).
        /// @param calculateFlexed [in] Whether or not flexing should be calculated.
        ///
        /// @remarks
        ///     This does not set the new width.
        float Layout_CalculateElementHeight(GUIElement* pElement, uint32_t height, uint32_t heightType, bool calculateFlexed);


        /// Calculates the width of the children of the given element for when the parent is auto-sized based on the width of it's children.
        ///
        /// @param element [in] The element whose children width is being calculated.
        ///
        /// @return The width of the children of the given element for when it is auto-sized based on the width of the it's children.
        ///
        /// @remarks
        ///     This will only include children whose size would actually affect the parent. This only considers auto-positioned children. Absolute
        ///     and relative positioned elements are ignored.
        float Layout_CalculateElementChildrenWidthForAutoSize(GUIElement* pElement) const;

        /// Calculates the height of the children of the given element for when the parent is auto-sized based on the height of it's children.
        ///
        /// @param element [in] The element whose children height is being calculated.
        ///
        /// @return The height of the children of the given element for when it is auto-sized based on the height of the it's children.
        ///
        /// @remarks
        ///     This will only include children whose size would actually affect the parent. This only considers auto-positioned children. Absolute
        ///     and relative positioned elements are ignored.
        float Layout_CalculateElementChildrenHeightForAutoSize(GUIElement* pElement) const;

        /// Retrieves the total width of the children of the given element that would contribute to it's width when auto-sized.
        ///
        /// @param element [in] A reference to the element whose childrens' size is being retrieved.
        ///
        /// @remarks
        ///     This will not include invisible children, not those that would not contribute to the size of the element if it were sized based on the children.
        float Layout_CalculateElementChildrenWidth(GUIElement* pElement);

        /// Retrieves the total height of the children of the given element that would contribute to it's width when auto-sized.
        ///
        /// @param element [in] A reference to the element whose childrens' size is being retrieved.
        ///
        /// @remarks
        ///     This will not include invisible children, not those that would not contribute to the size of the element if it were sized based on the children.
        float Layout_CalculateElementChildrenHeight(GUIElement* pElement);


        /// Retrieves the total width of the given element and it's visible, auto-positioned siblings.
        ///
        /// @param element [in] A reference to the element whose siblings size is being retrieved.
        ///
        /// @remarks
        ///     This will not include invisible siblings, nor those that are not auto-positioned.
        float Layout_CalculateTotalVisibleAutoPositionedSiblingsWidth(GUIElement* pElement);

        /// Retrieves the total height of the given element and it's visible, auto-positioned siblings.
        ///
        /// @param element [in] A reference to the element whose siblings size is being retrieved.
        ///
        /// @remarks
        ///     This will not include invisible siblings, nor those that are not auto-positioned.
        float Layout_CalculateTotalVisibleAutoPositionedSiblingsHeight(GUIElement* pElement);


        /// Finds the closest neighboring auto-positioned sibling of the given element.
        ///
        /// @param element [in] The element whose neighboring sibling is being retrieved.
        ///
        /// @remarks
        ///     This is used when needing to update the position of auto-positioned sibling elements because they all depend on each other.
        GUIElement* Layout_GetNeighboringAutoPositionedSibling(GUIElement* pElement);

        /// Finds the first auto-positioned child of the given element.
        ///
        /// @param element [in] The element whose first auto-positioned child is being retrieved.
        ///
        /// @remarks
        ///     This is used when needing to update the position of auto-positioned children.
        GUIElement* Layout_GetFirstAutoPositionedChild(GUIElement* pElement);



        /// Marks the position of the given element as changed.
        ///
        /// @param element [in] A reference to the element whose position is being marked as changed.
        ///
        /// @remarks
        ///     At the end of validation, all elements whose position has changed will have move events posted.
        void Layout_MarkElementPositionAsChanged(GUIElement* pElement);

        /// Marks the size of the given element as changed.
        ///
        /// @param element [in] A reference to the element whose size is being marked as changed.
        ///
        /// @remarks
        ///     At the end of layout validation, all elements whose size is marked as changed will have size events posted.
        void Layout_MarkElementSizeAsChanged(GUIElement* pElement);


        /// Recursively determines whether or not the absolute position of the given element has changed.
        ///
        /// @param element [in] A reference to the element whose absolute position change state is being checked.
        bool Layout_HasElementAbsolutePositionChanged(GUIElement* pElement);


        /// Updates the absolute positions of the elements whose absolute positions are currently invalid.
        ///
        /// @remarks
        ///     This is called at the end of a layout validation process.
        void Layout_UpdateElementAbsolutePositions();

        /// Updates the absolute position of the given element, and then recursively does the same thing for it's children.
        ///
        /// @param element [in] The element whose position is being updated.
        void Layout_UpdateElementAbsolutePosition(GUIElement* pElement);


        /// Posts OnSize and OnMove events to every element whose size and/or position has changed.
        ///
        /// @remarks
        ///     This is called at the end of a layout validation process.
        void Layout_InvalidateRectsAndPostEventsToElements();



        ////////////////////////////
        // Static Helpers
        
        /// Retrieves the horizontal padding (left padding + right padding) of the given element.
        ///
        /// @param element [in] A reference to the element whose horizontal padding is being retrieved.
        static float Layout_GetElementHorizontalPadding(GUIElement* pElement);

        /// Retrieves the vertical padding (top padding + bottom padding) of the given element.
        ///
        /// @param element [in] A reference to the element whose vertical padding is being retrieved.
        static float Layout_GetElementVerticalPadding(GUIElement* pElement);

        /// Retrieves the horizontal margin (left margin + right margin) of the given element.
        ///
        /// @param element [in] A reference to the element whose horizontal margin is being retrieved.
        static float Layout_GetElementHorizontalMargin(GUIElement* pElement);

        /// Retrieves the vertical margin (top margin + bottom margin) of the given element.
        ///
        /// @param element [in] A reference to the element whose vertical margin is being retrieved.
        static float Layout_GetElementVerticalMargin(GUIElement* pElement);

        /// Retrieves the size of the horizontal borders (left and right borders) of the given element.
        ///
        /// @param element [in] A reference to the element whose horizontal border size is being retrieved.
        static float Layout_GetElementHorizontalBorderSize(GUIElement* pElement);

        /// Retrieves the size of the vertical borders (top and bottom borders) of the given element.
        ///
        /// @param element [in] A reference to the element whose vertical border size is being retrieved.
        static float Layout_GetElementVerticalBorderSize(GUIElement* pElement);

        /// Retrieves the width + left margin + right margin of the given element.
        ///
        /// @param element [in] A reference to the element whose outer width is being retrieved.
        ///
        /// @remarks
        ///     The outer width is the width of the element plus the left and right margins.
        static float Layout_GetElementOuterWidth(GUIElement* pElement);

        /// Retrieves the height + top margin + bottom margin of the given element.
        ///
        /// @param element [in] A reference to the element whose outer height is being retrieved.
        ///
        /// @remarks
        ///     The outer height is the height of the element plus the top and bottom margins.
        static float Layout_GetElementOuterHeight(GUIElement* pElement);

        /// Retrieves the width minus the left and right padding and borders of the given element.
        ///
        /// @param element [in] A reference to the element whose inner width is being retrieved.
        ///
        /// @remarks
        ///     The inner width is the width of the element minus the left and right padding and borders.
        static float Layout_GetElementInnerWidth(GUIElement* pElement);

        /// Retrieves the height minus the top and bottom padding and borders of the given element.
        ///
        /// @param element [in] A reference to the element whose inner height is being retrieved.
        ///
        /// @remarks
        ///     The inner height is the height of the element minus the top and bottom padding and borders.
        static float Layout_GetElementInnerHeight(GUIElement* pElement);

        /// Retrieves the width minus the left and right borders of the given element.
        ///
        /// @param element [in] A reference to the element whose inner border width is being retrieved.
        ///
        /// @remarks
        ///     The inner-border width is the width of the element minus the left and right borders, but including the padding.
        static float Layout_GetElementInnerBorderWidth(GUIElement* pElement);

        /// Retrieves the height minus the top and bottom borders of the given element.
        ///
        /// @param element [in] A reference to the element whose inner-border height is being retrieved.
        ///
        /// @remarks
        ///     The inner-border height is the height of the element minus the top and bottom borders, but including the padding.
        static float Layout_GetElementInnerBorderHeight(GUIElement* pElement);

        /// Retrieves the width of the given element that should be used when calculating the relative width of a child.
        ///
        /// @param element [in] A reference to the element whose width is being calculated.
        static float Layout_GetElementWidthForRelativeSizing(GUIElement* pElement);

        /// Retrieves the height of the given element that should be used when calculating the relative height of a child.
        ///
        /// @param element [in] A reference to the element whose height is being calculated.
        static float Layout_GetElementHeightForRelativeSizing(GUIElement* pElement);

        /// Determines whether or not the width of the given element is invalid.
        ///
        /// @param element [in] A reference to the element whose width validation state is being determined.
        ///
        /// @return True if the width of the given element is invalid; false otherwise.
        static bool Layout_IsElementWidthInvalid(GUIElement* pElement);

        /// Determines whether or not the height of the given element is invalid.
        ///
        /// @param element [in] A reference to the element whose height validation state is being determined.
        ///
        /// @return True if the height of the given element is invalid; false otherwise.
        static bool Layout_IsElementHeightInvalid(GUIElement* pElement);

        /// Determines whether or not the position of the given element is invalid.
        ///
        /// @param element [in] A reference to the element whose position validation state is being determined.
        ///
        /// @return True if the position of the given element is invalid; false otherwise.
        static bool Layout_IsElementPositionInvalid(GUIElement* pElement);
        


    private:

        /// The font manager for working with fonts. This depends on m_imageManager, so must be declared and initialized afterwards.
        //GUIFontManagerOld m_fontManager;

        /// A pointer to the font manager.
        GUIFontManager* m_pFontManager;


        /// The base DPI on the X axis for calculating the scaling factor. Defaults to 96.
        unsigned int m_xBaseDPI;

        /// The base DPI on yhe Y axis for calculating the scaling factor. Defaults to 96.
        unsigned int m_yBaseDPI;


        /// Object containing the necessary variables for handling layout updates.
        struct GUILayoutContext
        {
            GUILayoutContext()
                : invalidElements(),
                  validatedElements(),
                  elementsWithInvalidAbsolutePositions()
            {
            }

            /// The elements that have an invalid layout property. We need fast insertions and removals here, so we'll use a linked list.
            GTLib::List<GUIElement*> invalidElements;

            /// The elements whose layout has changed. We use this during the post-process stage of layout validation so we can know which elements to post events to.
            GTLib::Vector<GUIElement*> validatedElements;

            /// The elements that need to have their absolute positions updated.
            GTLib::Vector<GUIElement*> elementsWithInvalidAbsolutePositions;

        } m_layoutContext;


        /// The batch operation lock counter. When this is >0, any operation that is controlled by batching will be locked.
        uint32_t m_batchLockCounter;
    };
}

#endif
