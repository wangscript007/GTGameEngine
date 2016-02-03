// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_AssetExplorer
#define GT_AssetExplorer

#include "EditorTool.hpp"
#include <dr_libs/wip/dr_gui_tree_view.h>
#include <dr_appkit/ak_menu.h>

namespace GT
{
    class Editor2;

    /// The asset explorer tool.
    ///
    /// This is implemented as an easy_appkit tool.
    class AssetExplorer : public EditorTool
    {
    public:

        /// Constructor.
        AssetExplorer(Editor2 &editor);

        /// Destructor.
        ~AssetExplorer();


        /// @copydoc EditorTool::Init()
        bool Init(const char* attributes);


        /// Retrieves a pointer to the internal tree-view control.
        drgui_element* GetTreeView();


        /// Called when an item is picked.
        void OnItemPicked(drgui_tree_view_item* pItem);


        /// Finds a tree-view item by it's path.
        //ak_tree_view_item* FindItemByAbsolutePath(const char* absolutePath);


        /// Inserts an item by it's path.
        ///
        /// @remarks
        ///     If the item already exists a pointer to the existing item will be returned.
        drgui_tree_view_item* InsertItem(const char* absolutePath);

        /// Removes an item by it's path.
        void RemoveItem(const char* absolutePath);

        /// Renames an item.
        void RenameItem(const char* absolutePathOld, const char* absolutePathNew);


        /// Retrieves the font to use for item text.
        drgui_font* GetItemFont() const;

        /// Retrieves the color to use for item text.
        drgui_color GetItemTextColor() const;

        /// Retrieves the height of a single item.
        float GetDefaultItemHeight() const;

        /// Retrieves the amount of padding to apply to each item in the explorer.
        float GetItemPadding() const;

        /// Retrieves the width of that arrow for each item.
        float GetItemArrowWidth() const;

        /// Retrieves the width of the icon for each item.
        float GetItemIconWidth() const;


        /// Retrieves the font to use for the arrow graphics.
        drgui_font* GetArrowFont() const;

        /// Retrieves the metrics of the arrow font.
        drgui_font_metrics GetArrowFontMetrics() const;

        /// Retrieves the glyph metrics of the arrow.
        drgui_glyph_metrics GetArrowGlyphMetrics() const;


        /// Retrieves the glyph metrics of the folder icon.
        drgui_glyph_metrics GetFolderIconGlyphMetrics() const;

        /// Retrieves the size of the folder icon when rendered as a string.
        void GetFolderIconStringSize(float* pWidthOut, float* pHeightOut);


        /// Retrieves the glyph metrics of the page icon.
        drgui_glyph_metrics GetPageIconGlyphMetrics() const;

        /// Retrieves the size of the folder icon when rendered as a string.
        void GetPageIconStringSize(float* pWidthOut, float* pHeightOut);


    private:

        /// Creates a child tree-view item.
        ///
        /// @remarks
        ///     This will only create a direct child.
        drgui_tree_view_item* InsertChildItem(drgui_tree_view_item* pParentItem, const char* relativePath);

        /// Recursively inserts a directory.
        void InsertDirectoryRecursive(const char* absolutePath);

        /// Helper for calculating the width of an item.
        float CalculateItemWidth(const char* text) const;


    private:

        /// The tree view control.
        drgui_element* m_pTV;

        /// The default font for items.
        drgui_font* m_pItemFont;

        /// The metrics of the default font.
        drgui_font_metrics m_itemFontMetrics;

        /// The font to use for the arrow graphic.
        drgui_font* m_pArrowFont;

        /// The metrics of the arrow font.
        drgui_font_metrics m_arrowFontMetrics;

        /// The glyph metrics of the arrow icon.
        drgui_glyph_metrics m_arrowGlyphMetrics;


        /// The glyph metrics of the folder icon.
        drgui_glyph_metrics m_folderIconGlyphMetrics;

        /// The width of the folder icon when drawn as a string.
        float m_folderIconStringWidth;

        /// The height of the folder icon when drawn as a string.
        float m_folderIconStringHeight;


        /// The glyph metrics of the page icon.
        drgui_glyph_metrics m_pageIconGlyphMetrics;

        /// The width of the folder icon when drawn as a string.
        float m_pageIconStringWidth;

        /// The height of the folder icon when drawn as a string.
        float m_pageIconStringHeight;




    public:

        /// Test menu.
        ak_window* m_pTestMenu;
    };
}

#endif
