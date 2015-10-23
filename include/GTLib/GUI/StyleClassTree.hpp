// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_StyleClassTree_hpp_
#define __GTLib_GUI_StyleClassTree_hpp_

#include "StyleClass.hpp"

namespace GTGUI
{
    /// Class representing a tree data structure for storing style classes.
    ///
    /// The requirements for this structure is fast searching by name.
    class StyleClassTree
    {
    public:
    
        /// Constructor.
        StyleClassTree();
        
        /// Destructor.
        ~StyleClassTree();
        
        
        /// Inserts a style class into the tree.
        ///
        /// @param styleClass [in] A reference to the style class to add to the list.
        void Insert(StyleClass &styleClass);
        void Insert(StyleClass* styleClass) { assert(styleClass != nullptr); this->Insert(*styleClass); }
        
        /// Removes a style class from the tree.
        ///
        /// @param styleClass [in] A reference to the style class to remove from the list.
        void Remove(StyleClass &styleClass);
        void Remove(StyleClass* styleClass) { assert(styleClass != nullptr); this->Remove(*styleClass); }
        
        
        /// Finds the style class by name.
        ///
        /// @param name           [in] The name of the style class to find.
        /// @param nameLengthInTs [in] The length of the name, or -1 if the name is null terminated.
        ///
        /// @return A pointer to the style class with the given name, or null if the style class does not exist.
        StyleClass* FindByName(const char* name, ptrdiff_t nameLengthInTs = -1);
    
    
        /// Retrieves a pointer to the root style class.
        StyleClass* GetRoot();
        
        
        /// Retrieves the number of items in the tree.
        ///
        /// @remarks
        ///     This runs in linear time.
        size_t GetCount() const;
        
    
    private:

        /// Performs an in-order successor removal of the given node.
        ///
        /// @param styleClass [in] A reference to the style class to remove.
        ///
        /// @remarks
        ///     This method asserts that the left and right children are not null.
        void RemoveByInOrderSuccessor(StyleClass &styleClass);

        /// Performs an in-order predecessor removal of the given node.
        ///
        /// @param styleClass [in] A reference to the style class to remove.
        ///
        /// @remarks
        ///     This method asserts that the left and right children are not null.
        void RemoveByInOrderPredecessor(StyleClass &styleClass);
        
        /// Replaces the left or right node of the parent of the given node with a new value.
        ///
        /// @param styleClass            [in] The style class whose parent is having it's left or right child switched.
        /// @param replacementStyleClass [in] The replacement style class.
        ///
        /// @remarks
        ///     This is used by Remove(). 'styleClass' will be the node being removed.
        void ReplaceStyleClassInParent(StyleClass &styleClass, StyleClass* replacementStyleClass);

        /// Finds the in-order predecessor of the given node.
        ///
        /// @param styleClass [in] The style class who's predecessor is being retrieved.
        StyleClass* FindInOrderPredecessor(StyleClass &styleClass);
        
        /// Finds the in-order successor of the given node.
        ///
        /// @param styleClass [in] The style class who's successor is being retrieved.
        StyleClass* FindInOrderSuccessor(StyleClass &styleClass);
    
        
        /// Recursive function for counting the number of items in the tree.
        size_t GetCount(StyleClass* root) const;
    

        /// Verifies the tree for correctness. This is only used for debugging and testing.
        bool Verify() const;
        bool Verify(StyleClass* root) const;
    
    private:
        
        /// A pointer to the root item. Starts out as null.
        StyleClass* m_root;

        /// Whether or not the next removal should be an in-order successor or predecessor type. This is toggled with each removal. The
        /// idea with this system is to try and keep the tree a bit more balanced when removing items. Not sure how well it'll work, though.
        bool m_removeBySuccessor;
        
        
    private:    // No copying.
        StyleClassTree(const StyleClassTree &);
        StyleClassTree & operator=(const StyleClassTree &);
    };
}

#endif
