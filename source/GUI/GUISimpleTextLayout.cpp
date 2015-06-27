// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUISimpleTextLayout.hpp>
#include <GTLib/Strings/LineIterator.hpp>
#include <GTLib/Strings/Tokenizer.hpp>

namespace GT
{
    GUISimpleTextLayout::GUISimpleTextLayout(GUIFontManager &fontManager)
        : GUITextLayout(fontManager),
          m_text(),
          m_boundsWidth(0), m_boundsHeight(0),
          m_offsetX(0), m_offsetY(0),
          m_tabSizeInSpaces(4),
          m_horizontalAlignment(GUITextLayoutHorizontalAlignment::Left), m_verticalAlignment(GUITextLayoutVerticalAlignment::Top),
          m_hFont(0),
          m_color(),
          m_lines(),
          m_textBoundsWidth(0), m_textBoundsHeight(0)
    {
    }

    GUISimpleTextLayout::~GUISimpleTextLayout()
    {
    }


    void GUISimpleTextLayout::SetText(const char* text)
    {
        m_text = text;

        this->RefreshLayout();
    }

    const char* GUISimpleTextLayout::GetText() const
    {
        return m_text.c_str();
    }


    void GUISimpleTextLayout::SetBounds(unsigned int width, unsigned int height)
    {
        m_boundsWidth  = width;
        m_boundsHeight = height;

        this->RefreshLayout();
    }

    void GUISimpleTextLayout::GetBounds(unsigned int &widthOut, unsigned int &heightOut) const
    {
        widthOut  = m_boundsWidth;
        heightOut = m_boundsHeight;
    }


    void GUISimpleTextLayout::SetOffset(int offsetX, int offsetY)
    {
        m_offsetX = offsetX;
        m_offsetY = offsetY;
    }

    void GUISimpleTextLayout::GetOffset(int &offsetXOut, int &offsetYOut) const
    {
        offsetXOut = m_offsetX;
        offsetYOut = m_offsetY;
    }


    int GUISimpleTextLayout::GetTextWidth() const
    {
        return m_textBoundsWidth;
    }

    int GUISimpleTextLayout::GetTextHeight() const
    {
        return m_textBoundsHeight;
    }


    void GUISimpleTextLayout::SetHorizontalAlignment(GUITextLayoutHorizontalAlignment alignment)
    {
        if (m_horizontalAlignment != alignment)
        {
            m_horizontalAlignment = alignment;

            this->RefreshAlignment();
        }
    }

    GUITextLayoutHorizontalAlignment GUISimpleTextLayout::GetHorizontalAlignment() const
    {
        return m_horizontalAlignment;
    }

    void GUISimpleTextLayout::SetVerticalAlignment(GUITextLayoutVerticalAlignment alignment)
    {
        if (m_verticalAlignment != alignment)
        {
            m_verticalAlignment = alignment;

            this->RefreshAlignment();
        }
    }

    GUITextLayoutVerticalAlignment GUISimpleTextLayout::GetVerticalAlignment() const
    {
        return m_verticalAlignment;
    }

    void GUISimpleTextLayout::SetAlignment(GUITextLayoutHorizontalAlignment horizontalAlignment, GUITextLayoutVerticalAlignment verticalAlignment)
    {
        if (m_horizontalAlignment != horizontalAlignment || m_verticalAlignment != verticalAlignment)
        {
            m_verticalAlignment   = verticalAlignment;
            m_horizontalAlignment = horizontalAlignment;

            this->RefreshAlignment();
        }
    }


    void GUISimpleTextLayout::SetDefaultFont(HGUIFont hFont)
    {
        m_hFont = hFont;

        this->RefreshLayout();
    }

    HGUIFont GUISimpleTextLayout::GetDefaultFont() const
    {
        return m_hFont;
    }


    void GUISimpleTextLayout::SetDefaultTextColor(const GTLib::Colour &color)
    {
        m_color = color;
    }

    GTLib::Colour GUISimpleTextLayout::GetDefaultTextColor() const
    {
        return m_color;
    }




    //////////////////////////////////////////
    // Private

    void GUISimpleTextLayout::IterateVisibleTextRuns(std::function<void (const GUITextRunDesc &textRunDesc)> func) const
    {
        int lineHeight = this->GetFontManager().GetLineHeight(m_hFont);
        int topOffset  = 0; //this->GetFontManager().GetAscent(m_hFont); // - lineHeight;

        for (size_t iLine = 0; iLine < m_lines.GetCount(); ++iLine)
        {
            auto &line = m_lines[iLine];

            int lineTop    = line.height * static_cast<int>(iLine) + m_offsetY + topOffset;
            int lineBottom = lineTop + lineHeight;
            
            if (lineBottom > 0 && lineTop < static_cast<int>(m_boundsHeight))
            {
                int lineLeft  = line.alignmentOffsetX + m_offsetX;
                int lineRight = lineLeft + line.width;

                if (lineRight > 0 && lineLeft < static_cast<int>(m_boundsWidth))
                {
                    // The line is visible. Now iterate over each of it's runs.
                    for (size_t iRun = 0; iRun < line.runs.GetCount(); ++iRun)
                    {
                        auto &run = line.runs[iRun];

                        int runLeft  = lineLeft + run.xPos;
                        int runRight = runLeft + run.width;

                        if (runRight > 0 && runLeft < static_cast<int>(m_boundsWidth))
                        {
                            // The run is visible.
                            int runTop = run.yPos + line.alignmentOffsetY + topOffset;

                            GUITextRunDesc runDesc;
                            runDesc.text              = GTLib::String(run.textStart, run.textEnd - run.textStart);
                            runDesc.hFont             = m_hFont;
                            runDesc.xPos              = runLeft;
                            runDesc.yPos              = runTop;
                            runDesc.rotationInDegrees = 0;
                            runDesc.color             = m_color;
                            func(runDesc);
                        }
                    }
                }
            }
        }
    }


    void GUISimpleTextLayout::SetTabSizeInSpaces(int tabSizeInSpaces)
    {
        m_tabSizeInSpaces = tabSizeInSpaces;
    }
        
    int GUISimpleTextLayout::GetTabSizeInSpaces() const
    {
        return m_tabSizeInSpaces;
    }



    //////////////////////////////////////////
    // Private

    void GUISimpleTextLayout::RefreshLayout()
    {
        // The previous runs need to be removed.
        m_lines.Clear();


        GUIFontManager &fontManager = this->GetFontManager();

        int lineHeight = fontManager.GetLineHeight(m_hFont);
        int tabWidth = 0;

        GUIGlyphMetrics spaceMetrics;
        if (fontManager.GetGlyphMetrics(m_hFont, ' ', spaceMetrics))
        {
            tabWidth = spaceMetrics.advance * this->GetTabSizeInSpaces();
        }

        int textBoundsWidth  = 0;
        int textBoundsHeight = 0;

        // We split by lines and then again by tabs.
        int lineCount = 0;
        GTLib::Strings::LineIterator iLine(m_text.c_str());
        while (iLine)
        {
            TextLine newLine;
            newLine.width  = 0;
            newLine.height = lineHeight;
            newLine.alignmentOffsetX = 0;
            newLine.alignmentOffsetY = 0;

            TextRun currentRun;
            currentRun.textStart      = nullptr;
            currentRun.textEnd        = nullptr;
            currentRun.characterCount = 0;
            currentRun.width          = 0;
            currentRun.xPos           = 0;
            currentRun.yPos           = 0;

            // We need to split the string based on tabs.
            GTLib::Strings::Iterator<char> c(iLine.start, iLine.end - iLine.start);
            while (c)
            {
                if (c.character == '\t')
                {
                    // If we are in the middle of a run it needs to be ended.
                    if (currentRun.textStart != nullptr)
                    {
                        // End the run and add it to the list of runs.
                        {
                            currentRun.textEnd = c.str;
                            currentRun.xPos    = newLine.width;
                            currentRun.yPos    = newLine.height * lineCount;

                            int runWidth  = 0;
                            int runHeight = 0;
                            if (fontManager.MeasureString(m_hFont, currentRun.textStart, currentRun.characterCount, runWidth, runHeight))
                            {
                                currentRun.width = runWidth;
                                newLine.width += runWidth;
                            }


                            newLine.runs.PushBack(currentRun);
                        }
                        

                        // Reset the run.
                        {
                            currentRun.textStart      = nullptr;
                            currentRun.textEnd        = nullptr;
                            currentRun.characterCount = 0;
                            currentRun.width          = 0;
                            currentRun.xPos           = 0;
                            currentRun.yPos           = 0;
                        }
                    }

                    
                    // Increment the tab size.
                    newLine.width += (tabWidth - (newLine.width % tabWidth));
                }
                else
                {
                    // It's a normal character. If we are in the middle of a run we just continue iterating. Otherwise we begin a new one.
                    if (currentRun.textStart == nullptr)
                    {
                        currentRun.textStart = c.str;
                    }

                    currentRun.characterCount += 1;
                }

                ++c;
            }

            // There might be a run that needs to be added.
            if (currentRun.textStart != nullptr)
            {
                currentRun.textEnd = c.str;
                currentRun.xPos    = newLine.width;
                currentRun.yPos    = newLine.height * lineCount;

                int runWidth  = 0;
                int runHeight = 0;
                if (fontManager.MeasureString(m_hFont, currentRun.textStart, currentRun.characterCount, runWidth, runHeight))
                {
                    currentRun.width = runWidth;
                    newLine.width += runWidth;
                }


                newLine.runs.PushBack(currentRun);
            }



            if (textBoundsWidth < newLine.width)
            {
                textBoundsWidth = newLine.width;
            }

            textBoundsHeight += newLine.height;



            m_lines.PushBack(newLine);

            lineCount += 1;
            ++iLine;
        }


        m_textBoundsWidth  = textBoundsWidth;
        m_textBoundsHeight = textBoundsHeight;


        // If we were to return now the text would be alignment top/left. If the alignment is not top/left we need to refresh the layout.
        if (m_horizontalAlignment != GUITextLayoutHorizontalAlignment::Left || m_verticalAlignment != GUITextLayoutVerticalAlignment::Top)
        {
            this->RefreshAlignment();
        }
    }


    void GUISimpleTextLayout::RefreshAlignment()
    {
        for (size_t iLine = 0; iLine < m_lines.GetCount(); ++iLine)
        {
            auto &line = m_lines[iLine];

            switch (m_horizontalAlignment)
            {
            case GUITextLayoutHorizontalAlignment::Right:
                {
                    line.alignmentOffsetX = m_boundsWidth - line.width;
                    break;
                }

            case GUITextLayoutHorizontalAlignment::Center:
                {
                    line.alignmentOffsetX = (m_boundsWidth - line.width) / 2;
                    break;
                }

            case GUITextLayoutHorizontalAlignment::Left:
            default:
                {
                    line.alignmentOffsetX = 0;
                    break;
                }
            }


            switch (m_verticalAlignment)
            {
            case GUITextLayoutVerticalAlignment::Bottom:
                {
                    line.alignmentOffsetY = m_boundsHeight - m_textBoundsHeight;
                    break;
                }

            case GUITextLayoutVerticalAlignment::Center:
                {
                    line.alignmentOffsetY = (m_boundsHeight - m_textBoundsHeight) / 2;
                    break;
                }

            case GUITextLayoutVerticalAlignment::Top:
            default:
                {
                    line.alignmentOffsetY = 0;
                    break;
                }
            }
        }
    }
}
