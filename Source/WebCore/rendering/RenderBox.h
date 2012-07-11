/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2006, 2007 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef RenderBox_h
#define RenderBox_h

#include "RenderBoxModelObject.h"
#include "RenderOverflow.h"
#include "ScrollTypes.h"

namespace WebCore {

class RenderBoxRegionInfo;
class RenderRegion;
struct PaintInfo;

enum SizeType { MainOrPreferredSize, MinSize, MaxSize };

enum OverlayScrollbarSizeRelevancy { IgnoreOverlayScrollbarSize, IncludeOverlayScrollbarSize };

class RenderBox : public RenderBoxModelObject {
public:
    RenderBox(Node*);
    virtual ~RenderBox();

    virtual bool requiresLayer() const OVERRIDE { return isRoot() || isOutOfFlowPositioned() || isRelPositioned() || isTransparent() || hasOverflowClip() || hasTransform() || hasHiddenBackface() || hasMask() || hasReflection() || hasFilter() || style()->specifiesColumns(); }

    // Use this with caution! No type checking is done!
    RenderBox* firstChildBox() const;
    RenderBox* lastChildBox() const;

    LayoutUnit x() const { return m_frameRect.x(); }
    LayoutUnit y() const { return m_frameRect.y(); }
    LayoutUnit width() const { return m_frameRect.width(); }
    LayoutUnit height() const { return m_frameRect.height(); }

    int pixelSnappedWidth() const { return m_frameRect.pixelSnappedWidth(); }
    int pixelSnappedHeight() const { return m_frameRect.pixelSnappedHeight(); }

    // These represent your location relative to your container as a physical offset.
    // In layout related methods you almost always want the logical location (e.g. x() and y()).
    LayoutUnit top() const { return topLeftLocation().y(); }
    LayoutUnit left() const { return topLeftLocation().x(); }

    void setX(LayoutUnit x) { m_frameRect.setX(x); }
    void setY(LayoutUnit y) { m_frameRect.setY(y); }
    void setWidth(LayoutUnit width) { m_frameRect.setWidth(width); }
    void setHeight(LayoutUnit height) { m_frameRect.setHeight(height); }

    LayoutUnit logicalLeft() const { return style()->isHorizontalWritingMode() ? x() : y(); }
    LayoutUnit logicalRight() const { return logicalLeft() + logicalWidth(); }
    LayoutUnit logicalTop() const { return style()->isHorizontalWritingMode() ? y() : x(); }
    LayoutUnit logicalBottom() const { return logicalTop() + logicalHeight(); }
    LayoutUnit logicalWidth() const { return style()->isHorizontalWritingMode() ? width() : height(); }
    LayoutUnit logicalHeight() const { return style()->isHorizontalWritingMode() ? height() : width(); }

    int pixelSnappedLogicalHeight() const { return style()->isHorizontalWritingMode() ? pixelSnappedHeight() : pixelSnappedWidth(); }
    int pixelSnappedLogicalWidth() const { return style()->isHorizontalWritingMode() ? pixelSnappedWidth() : pixelSnappedHeight(); }

    void setLogicalLeft(LayoutUnit left)
    {
        if (style()->isHorizontalWritingMode())
            setX(left);
        else
            setY(left);
    }
    void setLogicalTop(LayoutUnit top)
    {
        if (style()->isHorizontalWritingMode())
            setY(top);
        else
            setX(top);
    }
    void setLogicalLocation(const LayoutPoint& location)
    {
        if (style()->isHorizontalWritingMode())
            setLocation(location);
        else
            setLocation(location.transposedPoint());
    }
    void setLogicalWidth(LayoutUnit size)
    {
        if (style()->isHorizontalWritingMode())
            setWidth(size);
        else
            setHeight(size);
    }
    void setLogicalHeight(LayoutUnit size)
    {
        if (style()->isHorizontalWritingMode())
            setHeight(size);
        else
            setWidth(size);
    }
    void setLogicalSize(const LayoutSize& size)
    {
        if (style()->isHorizontalWritingMode())
            setSize(size);
        else
            setSize(size.transposedSize());
    }

    LayoutPoint location() const { return m_frameRect.location(); }
    LayoutSize locationOffset() const { return LayoutSize(x(), y()); }
    LayoutSize size() const { return m_frameRect.size(); }
    IntSize pixelSnappedSize() const { return m_frameRect.pixelSnappedSize(); }

    void setLocation(const LayoutPoint& location) { m_frameRect.setLocation(location); }
    
    void setSize(const LayoutSize& size) { m_frameRect.setSize(size); }
    void move(LayoutUnit dx, LayoutUnit dy) { m_frameRect.move(dx, dy); }

    LayoutRect frameRect() const { return m_frameRect; }
    IntRect pixelSnappedFrameRect() const { return pixelSnappedIntRect(m_frameRect); }
    void setFrameRect(const LayoutRect& rect) { m_frameRect = rect; }

    LayoutRect borderBoxRect() const { return LayoutRect(LayoutPoint(), size()); }
    IntRect pixelSnappedBorderBoxRect() const { return IntRect(IntPoint(), m_frameRect.pixelSnappedSize()); }
    virtual IntRect borderBoundingBox() const { return pixelSnappedBorderBoxRect(); } 

    // The content area of the box (excludes padding - and intrinsic padding for table cells, etc... - and border).
    LayoutRect contentBoxRect() const { return LayoutRect(borderLeft() + paddingLeft(), borderTop() + paddingTop(), contentWidth(), contentHeight()); }
    // The content box in absolute coords. Ignores transforms.
    IntRect absoluteContentBox() const;
    // The content box converted to absolute coords (taking transforms into account).
    FloatQuad absoluteContentQuad() const;

    // This returns the content area of the box (excluding padding and border). The only difference with contentBoxRect is that computedCSSContentBoxRect
    // does include the intrinsic padding in the content box as this is what some callers expect (like getComputedStyle).
    LayoutRect computedCSSContentBoxRect() const { return LayoutRect(borderLeft() + computedCSSPaddingLeft(), borderTop() + computedCSSPaddingTop(), clientWidth() - computedCSSPaddingLeft() - computedCSSPaddingRight(), clientHeight() - computedCSSPaddingTop() - computedCSSPaddingBottom()); }

    // Bounds of the outline box in absolute coords. Respects transforms
    virtual LayoutRect outlineBoundsForRepaint(RenderBoxModelObject* /*repaintContainer*/, LayoutPoint* cachedOffsetToRepaintContainer) const;
    virtual void addFocusRingRects(Vector<IntRect>&, const LayoutPoint&);

    // Use this with caution! No type checking is done!
    RenderBox* previousSiblingBox() const;
    RenderBox* nextSiblingBox() const;
    RenderBox* parentBox() const;

    // Visual and layout overflow are in the coordinate space of the box.  This means that they aren't purely physical directions.
    // For horizontal-tb and vertical-lr they will match physical directions, but for horizontal-bt and vertical-rl, the top/bottom and left/right
    // respectively are flipped when compared to their physical counterparts.  For example minX is on the left in vertical-lr,
    // but it is on the right in vertical-rl.
    LayoutRect layoutOverflowRect() const { return m_overflow ? m_overflow->layoutOverflowRect() : clientBoxRect(); }
    IntRect pixelSnappedLayoutOverflowRect() const { return pixelSnappedIntRect(layoutOverflowRect()); }
    LayoutSize maxLayoutOverflow() const { return LayoutSize(layoutOverflowRect().maxX(), layoutOverflowRect().maxY()); }
    LayoutUnit logicalLeftLayoutOverflow() const { return style()->isHorizontalWritingMode() ? layoutOverflowRect().x() : layoutOverflowRect().y(); }
    LayoutUnit logicalRightLayoutOverflow() const { return style()->isHorizontalWritingMode() ? layoutOverflowRect().maxX() : layoutOverflowRect().maxY(); }
    
    virtual LayoutRect visualOverflowRect() const { return m_overflow ? m_overflow->visualOverflowRect() : borderBoxRect(); }
    LayoutUnit logicalLeftVisualOverflow() const { return style()->isHorizontalWritingMode() ? visualOverflowRect().x() : visualOverflowRect().y(); }
    LayoutUnit logicalRightVisualOverflow() const { return style()->isHorizontalWritingMode() ? visualOverflowRect().maxX() : visualOverflowRect().maxY(); }
    
    void addLayoutOverflow(const LayoutRect&);
    void addVisualOverflow(const LayoutRect&);
    
    void addVisualEffectOverflow();
    void addOverflowFromChild(RenderBox* child) { addOverflowFromChild(child, child->locationOffset()); }
    void addOverflowFromChild(RenderBox* child, const LayoutSize& delta);
    void clearLayoutOverflow();
    
    void updateLayerTransform();

    LayoutUnit contentWidth() const { return clientWidth() - paddingLeft() - paddingRight(); }
    LayoutUnit contentHeight() const { return clientHeight() - paddingTop() - paddingBottom(); }
    LayoutUnit contentLogicalWidth() const { return style()->isHorizontalWritingMode() ? contentWidth() : contentHeight(); }
    LayoutUnit contentLogicalHeight() const { return style()->isHorizontalWritingMode() ? contentHeight() : contentWidth(); }

    // IE extensions. Used to calculate offsetWidth/Height.  Overridden by inlines (RenderFlow)
    // to return the remaining width on a given line (and the height of a single line).
    virtual LayoutUnit offsetWidth() const { return width(); }
    virtual LayoutUnit offsetHeight() const { return height(); }

    // FIXME: The implementation for these functions will change once we move to subpixel layout. See bug 60318.
    virtual int pixelSnappedOffsetWidth() const { return pixelSnappedWidth(); }
    virtual int pixelSnappedOffsetHeight() const { return pixelSnappedHeight(); }

    // More IE extensions.  clientWidth and clientHeight represent the interior of an object
    // excluding border and scrollbar.  clientLeft/Top are just the borderLeftWidth and borderTopWidth.
    LayoutUnit clientLeft() const { return borderLeft(); }
    LayoutUnit clientTop() const { return borderTop(); }
    LayoutUnit clientWidth() const;
    LayoutUnit clientHeight() const;
    LayoutUnit clientLogicalWidth() const { return style()->isHorizontalWritingMode() ? pixelSnappedClientWidth() : pixelSnappedClientHeight(); }
    LayoutUnit clientLogicalHeight() const { return style()->isHorizontalWritingMode() ? pixelSnappedClientHeight() : pixelSnappedClientWidth(); }
    LayoutUnit clientLogicalBottom() const { return borderBefore() + clientLogicalHeight(); }
    LayoutRect clientBoxRect() const { return LayoutRect(clientLeft(), clientTop(), clientWidth(), clientHeight()); }

    int pixelSnappedClientWidth() const;
    int pixelSnappedClientHeight() const;

    // scrollWidth/scrollHeight will be the same as clientWidth/clientHeight unless the
    // object has overflow:hidden/scroll/auto specified and also has overflow.
    // scrollLeft/Top return the current scroll position.  These methods are virtual so that objects like
    // textareas can scroll shadow content (but pretend that they are the objects that are
    // scrolling).
    virtual int scrollLeft() const;
    virtual int scrollTop() const;
    virtual int scrollWidth() const;
    virtual int scrollHeight() const;
    virtual void setScrollLeft(int);
    virtual void setScrollTop(int);

    virtual LayoutUnit marginTop() const OVERRIDE { return m_marginBox.top(); }
    virtual LayoutUnit marginBottom() const OVERRIDE { return m_marginBox.bottom(); }
    virtual LayoutUnit marginLeft() const OVERRIDE { return m_marginBox.left(); }
    virtual LayoutUnit marginRight() const OVERRIDE { return m_marginBox.right(); }
    void setMarginTop(LayoutUnit margin) { m_marginBox.setTop(margin); }
    void setMarginBottom(LayoutUnit margin) { m_marginBox.setBottom(margin); }
    void setMarginLeft(LayoutUnit margin) { m_marginBox.setLeft(margin); }
    void setMarginRight(LayoutUnit margin) { m_marginBox.setRight(margin); }

    virtual LayoutUnit marginLogicalLeft() const { return m_marginBox.logicalLeft(style()); }
    virtual LayoutUnit marginLogicalRight() const { return m_marginBox.logicalRight(style()); }
    
    virtual LayoutUnit marginBefore(const RenderStyle* overrideStyle = 0) const OVERRIDE { return m_marginBox.before(overrideStyle ? overrideStyle : style()); }
    virtual LayoutUnit marginAfter(const RenderStyle* overrideStyle = 0) const OVERRIDE { return m_marginBox.after(overrideStyle ? overrideStyle : style()); }
    virtual LayoutUnit marginStart(const RenderStyle* overrideStyle = 0) const OVERRIDE { return m_marginBox.start(overrideStyle ? overrideStyle : style()); }
    virtual LayoutUnit marginEnd(const RenderStyle* overrideStyle = 0) const OVERRIDE { return m_marginBox.end(overrideStyle ? overrideStyle : style()); }
    void setMarginBefore(LayoutUnit value, const RenderStyle* overrideStyle = 0) { m_marginBox.setBefore(overrideStyle ? overrideStyle : style(), value); }
    void setMarginAfter(LayoutUnit value, const RenderStyle* overrideStyle = 0) { m_marginBox.setAfter(overrideStyle ? overrideStyle : style(), value); }
    void setMarginStart(LayoutUnit value, const RenderStyle* overrideStyle = 0) { m_marginBox.setStart(overrideStyle ? overrideStyle : style(), value); }
    void setMarginEnd(LayoutUnit value, const RenderStyle* overrideStyle = 0) { m_marginBox.setEnd(overrideStyle ? overrideStyle : style(), value); }

    // The following five functions are used to implement collapsing margins.
    // All objects know their maximal positive and negative margins.  The
    // formula for computing a collapsed margin is |maxPosMargin| - |maxNegmargin|.
    // For a non-collapsing box, such as a leaf element, this formula will simply return
    // the margin of the element.  Blocks override the maxMarginBefore and maxMarginAfter
    // methods.
    enum MarginSign { PositiveMargin, NegativeMargin };
    virtual bool isSelfCollapsingBlock() const { return false; }
    virtual LayoutUnit collapsedMarginBefore() const { return marginBefore(); }
    virtual LayoutUnit collapsedMarginAfter() const { return marginAfter(); }

    virtual void absoluteRects(Vector<IntRect>&, const LayoutPoint& accumulatedOffset) const;
    virtual void absoluteQuads(Vector<FloatQuad>&, bool* wasFixed) const;
    
    LayoutRect reflectionBox() const;
    int reflectionOffset() const;
    // Given a rect in the object's coordinate space, returns the corresponding rect in the reflection.
    LayoutRect reflectedRect(const LayoutRect&) const;

    virtual void layout();
    virtual void paint(PaintInfo&, const LayoutPoint&);
    virtual bool nodeAtPoint(const HitTestRequest&, HitTestResult&, const HitTestPoint& pointInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) OVERRIDE;

    virtual LayoutUnit minPreferredLogicalWidth() const;
    virtual LayoutUnit maxPreferredLogicalWidth() const;

    LayoutUnit overrideLogicalContentWidth() const;
    LayoutUnit overrideLogicalContentHeight() const;
    bool hasOverrideHeight() const;
    bool hasOverrideWidth() const;
    void setOverrideLogicalContentHeight(LayoutUnit);
    void setOverrideLogicalContentWidth(LayoutUnit);
    void clearOverrideSize();

    virtual LayoutSize offsetFromContainer(RenderObject*, const LayoutPoint&, bool* offsetDependsOnPoint = 0) const;
    
    LayoutUnit computeBorderBoxLogicalWidth(LayoutUnit width) const;
    LayoutUnit computeBorderBoxLogicalHeight(LayoutUnit height) const;
    LayoutUnit computeContentBoxLogicalWidth(LayoutUnit width) const;
    LayoutUnit computeContentBoxLogicalHeight(LayoutUnit height) const;

    virtual void borderFitAdjust(LayoutRect&) const { } // Shrink the box in which the border paints if border-fit is set.

    // Resolve auto margins in the inline direction of the containing block so that objects can be pushed to the start, middle or end
    // of the containing block.
    void computeInlineDirectionMargins(RenderBlock* containingBlock, LayoutUnit containerWidth, LayoutUnit childWidth);

    // Used to resolve margins in the containing block's block-flow direction.
    void computeBlockDirectionMargins(RenderBlock* containingBlock);

    enum RenderBoxRegionInfoFlags { CacheRenderBoxRegionInfo, DoNotCacheRenderBoxRegionInfo };
    LayoutRect borderBoxRectInRegion(RenderRegion*, LayoutUnit offsetFromLogicalTopOfFirstPage = 0, RenderBoxRegionInfoFlags = CacheRenderBoxRegionInfo) const;
    void clearRenderBoxRegionInfo();
    
    void positionLineBox(InlineBox*);

    virtual InlineBox* createInlineBox();
    void dirtyLineBoxes(bool fullLayout);

    // For inline replaced elements, this function returns the inline box that owns us.  Enables
    // the replaced RenderObject to quickly determine what line it is contained on and to easily
    // iterate over structures on the line.
    InlineBox* inlineBoxWrapper() const { return m_inlineBoxWrapper; }
    void setInlineBoxWrapper(InlineBox* boxWrapper) { m_inlineBoxWrapper = boxWrapper; }
    void deleteLineBoxWrapper();

    virtual LayoutRect clippedOverflowRectForRepaint(RenderBoxModelObject* repaintContainer) const;
    virtual void computeRectForRepaint(RenderBoxModelObject* repaintContainer, LayoutRect&, bool fixed = false) const;

    virtual void repaintDuringLayoutIfMoved(const LayoutRect&);

    virtual LayoutUnit containingBlockLogicalWidthForContent() const;
    LayoutUnit containingBlockLogicalWidthForContentInRegion(RenderRegion*, LayoutUnit offsetFromLogicalTopOfFirstPage) const;
    LayoutUnit containingBlockAvailableLineWidthInRegion(RenderRegion*, LayoutUnit offsetFromLogicalTopOfFirstPage) const;
    LayoutUnit perpendicularContainingBlockLogicalHeight() const;
    
    virtual void computeLogicalWidth();
    virtual void computeLogicalHeight();

    RenderBoxRegionInfo* renderBoxRegionInfo(RenderRegion*, LayoutUnit offsetFromLogicalTopOfFirstPage, RenderBoxRegionInfoFlags = CacheRenderBoxRegionInfo) const;
    void computeLogicalWidthInRegion(RenderRegion* = 0, LayoutUnit offsetFromLogicalTopOfFirstPage = ZERO_LAYOUT_UNIT);

    bool stretchesToViewport() const
    {
        return document()->inQuirksMode() && style()->logicalHeight().isAuto() && !isFloatingOrOutOfFlowPositioned() && (isRoot() || isBody()) && !document()->shouldDisplaySeamlesslyWithParent();
    }

    virtual IntSize intrinsicSize() const { return IntSize(); }
    LayoutUnit intrinsicLogicalWidth() const { return style()->isHorizontalWritingMode() ? intrinsicSize().width() : intrinsicSize().height(); }
    LayoutUnit intrinsicLogicalHeight() const { return style()->isHorizontalWritingMode() ? intrinsicSize().height() : intrinsicSize().width(); }

    // Whether or not the element shrinks to its intrinsic width (rather than filling the width
    // of a containing block).  HTML4 buttons, <select>s, <input>s, legends, and floating/compact elements do this.
    bool sizesLogicalWidthToFitContent(SizeType) const;
    virtual bool stretchesToMinIntrinsicLogicalWidth() const { return false; }

    LayoutUnit shrinkLogicalWidthToAvoidFloats(LayoutUnit childMarginStart, LayoutUnit childMarginEnd, const RenderBlock* cb, RenderRegion*, LayoutUnit offsetFromLogicalTopOfFirstPage);

    LayoutUnit computeLogicalWidthInRegionUsing(SizeType, LayoutUnit availableLogicalWidth, const RenderBlock* containingBlock, RenderRegion*, LayoutUnit offsetFromLogicalTopOfFirstPage);
    LayoutUnit computeLogicalHeightUsing(SizeType, const Length& height);
    LayoutUnit computeContentLogicalHeightUsing(SizeType, const Length& height);
    LayoutUnit computeReplacedLogicalWidthUsing(SizeType, Length width) const;
    LayoutUnit computeReplacedLogicalWidthRespectingMinMaxWidth(LayoutUnit logicalWidth, bool includeMaxWidth = true) const;
    LayoutUnit computeReplacedLogicalHeightUsing(SizeType, Length height) const;
    LayoutUnit computeReplacedLogicalHeightRespectingMinMaxHeight(LayoutUnit logicalHeight) const;

    virtual LayoutUnit computeReplacedLogicalWidth(bool includeMaxWidth = true) const;
    virtual LayoutUnit computeReplacedLogicalHeight() const;

    LayoutUnit computePercentageLogicalHeight(const Length& height);

    // Block flows subclass availableWidth to handle multi column layout (shrinking the width available to children when laying out.)
    virtual LayoutUnit availableLogicalWidth() const { return contentLogicalWidth(); }
    LayoutUnit availableLogicalHeight() const;
    LayoutUnit availableLogicalHeightUsing(const Length&) const;
    
    // There are a few cases where we need to refer specifically to the available physical width and available physical height.
    // Relative positioning is one of those cases, since left/top offsets are physical.
    LayoutUnit availableWidth() const { return style()->isHorizontalWritingMode() ? availableLogicalWidth() : availableLogicalHeight(); }
    LayoutUnit availableHeight() const { return style()->isHorizontalWritingMode() ? availableLogicalHeight() : availableLogicalWidth(); }

    virtual int verticalScrollbarWidth() const;
    int horizontalScrollbarHeight() const;
    int scrollbarLogicalHeight() const { return style()->isHorizontalWritingMode() ? horizontalScrollbarHeight() : verticalScrollbarWidth(); }
    virtual bool scroll(ScrollDirection, ScrollGranularity, float multiplier = 1, Node** stopNode = 0);
    virtual bool logicalScroll(ScrollLogicalDirection, ScrollGranularity, float multiplier = 1, Node** stopNode = 0);
    bool canBeScrolledAndHasScrollableArea() const;
    virtual bool canBeProgramaticallyScrolled() const;
    virtual void autoscroll();
    virtual void stopAutoscroll() { }
    virtual void panScroll(const IntPoint&);
    bool hasAutoVerticalScrollbar() const { return hasOverflowClip() && (style()->overflowY() == OAUTO || style()->overflowY() == OOVERLAY); }
    bool hasAutoHorizontalScrollbar() const { return hasOverflowClip() && (style()->overflowX() == OAUTO || style()->overflowX() == OOVERLAY); }
    bool scrollsOverflow() const { return scrollsOverflowX() || scrollsOverflowY(); }
    bool scrollsOverflowX() const { return hasOverflowClip() && (style()->overflowX() == OSCROLL || hasAutoHorizontalScrollbar()); }
    bool scrollsOverflowY() const { return hasOverflowClip() && (style()->overflowY() == OSCROLL || hasAutoVerticalScrollbar()); }
    
    bool hasUnsplittableScrollingOverflow() const;
    bool isUnsplittableForPagination() const;

    virtual LayoutRect localCaretRect(InlineBox*, int caretOffset, LayoutUnit* extraWidthToEndOfLine = 0);

    virtual LayoutRect overflowClipRect(const LayoutPoint& location, RenderRegion*, OverlayScrollbarSizeRelevancy = IgnoreOverlayScrollbarSize);
    LayoutRect clipRect(const LayoutPoint& location, RenderRegion*);
    virtual bool hasControlClip() const { return false; }
    virtual LayoutRect controlClipRect(const LayoutPoint&) const { return LayoutRect(); }
    bool pushContentsClip(PaintInfo&, const LayoutPoint& accumulatedOffset);
    void popContentsClip(PaintInfo&, PaintPhase originalPhase, const LayoutPoint& accumulatedOffset);

    virtual void paintObject(PaintInfo&, const LayoutPoint&) { ASSERT_NOT_REACHED(); }
    virtual void paintBoxDecorations(PaintInfo&, const LayoutPoint&);
    virtual void paintMask(PaintInfo&, const LayoutPoint&);
    virtual void imageChanged(WrappedImagePtr, const IntRect* = 0);

    // Called when a positioned object moves but doesn't necessarily change size.  A simplified layout is attempted
    // that just updates the object's position. If the size does change, the object remains dirty.
    bool tryLayoutDoingPositionedMovementOnly()
    {
        LayoutUnit oldWidth = width();
        computeLogicalWidth();
        // If we shrink to fit our width may have changed, so we still need full layout.
        if (oldWidth != width())
            return false;
        computeLogicalHeight();
        return true;
    }

    LayoutRect maskClipRect();

    virtual VisiblePosition positionForPoint(const LayoutPoint&);

    void removeFloatingOrPositionedChildFromBlockLists();
    
    RenderLayer* enclosingFloatPaintingLayer() const;
    
    virtual LayoutUnit firstLineBoxBaseline() const { return -1; }
    virtual LayoutUnit lastLineBoxBaseline() const { return -1; }

    bool shrinkToAvoidFloats() const;
    virtual bool avoidsFloats() const;

    virtual void markForPaginationRelayoutIfNeeded() { }

    bool isWritingModeRoot() const { return !parent() || parent()->style()->writingMode() != style()->writingMode(); }

    bool isDeprecatedFlexItem() const { return !isInline() && !isFloatingOrOutOfFlowPositioned() && parent() && parent()->isDeprecatedFlexibleBox(); }
    
    virtual LayoutUnit lineHeight(bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const;
    virtual LayoutUnit baselinePosition(FontBaseline, bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const;

    virtual LayoutUnit offsetLeft() const OVERRIDE;
    virtual LayoutUnit offsetTop() const OVERRIDE;

    LayoutPoint flipForWritingModeForChild(const RenderBox* child, const LayoutPoint&) const;
    LayoutUnit flipForWritingMode(LayoutUnit position) const; // The offset is in the block direction (y for horizontal writing modes, x for vertical writing modes).
    LayoutPoint flipForWritingMode(const LayoutPoint&) const;
    LayoutPoint flipForWritingModeIncludingColumns(const LayoutPoint&) const;
    LayoutSize flipForWritingMode(const LayoutSize&) const;
    void flipForWritingMode(LayoutRect&) const;
    FloatPoint flipForWritingMode(const FloatPoint&) const;
    void flipForWritingMode(FloatRect&) const;
    // These represent your location relative to your container as a physical offset.
    // In layout related methods you almost always want the logical location (e.g. x() and y()).
    LayoutPoint topLeftLocation() const;
    LayoutSize topLeftLocationOffset() const;

    LayoutRect logicalVisualOverflowRectForPropagation(RenderStyle*) const;
    LayoutRect visualOverflowRectForPropagation(RenderStyle*) const;
    LayoutRect logicalLayoutOverflowRectForPropagation(RenderStyle*) const;
    LayoutRect layoutOverflowRectForPropagation(RenderStyle*) const;

    RenderOverflow* hasRenderOverflow() const { return m_overflow.get(); }    
    bool hasVisualOverflow() const { return m_overflow && !borderBoxRect().contains(m_overflow->visualOverflowRect()); }

    virtual bool needsPreferredWidthsRecalculation() const;
    virtual void computeIntrinsicRatioInformation(FloatSize& /* intrinsicSize */, double& /* intrinsicRatio */, bool& /* isPercentageIntrinsicSize */) const { }

    IntSize scrolledContentOffset() const;
    LayoutSize cachedSizeForOverflowClip() const;

    virtual bool hasRelativeDimensions() const;
    virtual bool hasRelativeLogicalHeight() const;

    bool hasHorizontalLayoutOverflow() const
    {
        if (RenderOverflow* overflow = hasRenderOverflow()) {
            LayoutRect layoutOverflowRect = overflow->layoutOverflowRect();
            flipForWritingMode(layoutOverflowRect);
            return layoutOverflowRect.x() < x() || layoutOverflowRect.maxX() > x() + logicalWidth();
        }

        return false;
    }

    bool hasVerticalLayoutOverflow() const
    {
        if (RenderOverflow* overflow = hasRenderOverflow()) {
            LayoutRect layoutOverflowRect = overflow->layoutOverflowRect();
            flipForWritingMode(layoutOverflowRect);
            return layoutOverflowRect.y() < y() || layoutOverflowRect.maxY() > y() + logicalHeight();
        }

        return false;
    }

    virtual RenderBox* createAnonymousBoxWithSameTypeAs(const RenderObject*) const
    {
        ASSERT_NOT_REACHED();
        return 0;
    }

protected:
    virtual void willBeDestroyed();

    virtual void styleWillChange(StyleDifference, const RenderStyle* newStyle);
    virtual void styleDidChange(StyleDifference, const RenderStyle* oldStyle);
    virtual void updateBoxModelInfoFromStyle();

    virtual bool backgroundIsObscured() const { return false; }
    void paintBackground(const PaintInfo&, const LayoutRect&, BackgroundBleedAvoidance = BackgroundBleedNone);
    
    void paintFillLayer(const PaintInfo&, const Color&, const FillLayer*, const LayoutRect&, BackgroundBleedAvoidance, CompositeOperator, RenderObject* backgroundObject);
    void paintFillLayers(const PaintInfo&, const Color&, const FillLayer*, const LayoutRect&, BackgroundBleedAvoidance = BackgroundBleedNone, CompositeOperator = CompositeSourceOver, RenderObject* backgroundObject = 0);

    void paintMaskImages(const PaintInfo&, const LayoutRect&);

    BackgroundBleedAvoidance determineBackgroundBleedAvoidance(GraphicsContext*) const;

#if PLATFORM(MAC)
    void paintCustomHighlight(const LayoutPoint&, const AtomicString& type, bool behindText);
#endif

    void computePositionedLogicalWidth(RenderRegion* = 0, LayoutUnit offsetFromLogicalTopOfFirstPage = 0);
    
    virtual bool shouldComputeSizeAsReplaced() const { return isReplaced() && !isInlineBlockOrInlineTable(); }

    virtual void mapLocalToContainer(RenderBoxModelObject* repaintContainer, bool fixed, bool useTransforms, TransformState&, ApplyContainerFlipOrNot = ApplyContainerFlip, bool* wasFixed = 0) const;
    virtual const RenderObject* pushMappingToContainer(const RenderBoxModelObject*, RenderGeometryMap&) const;
    virtual void mapAbsoluteToLocalPoint(bool fixed, bool useTransforms, TransformState&) const;

    void paintRootBoxFillLayers(const PaintInfo&);

    RenderObject* splitAnonymousBoxesAroundChild(RenderObject* beforeChild);
 
private:
    bool fixedElementLaysOutRelativeToFrame(Frame*, FrameView*) const;

    bool includeVerticalScrollbarSize() const;
    bool includeHorizontalScrollbarSize() const;

    // Returns true if we did a full repaint
    bool repaintLayerRectsForImage(WrappedImagePtr image, const FillLayer* layers, bool drawingBackground);
   
    LayoutUnit containingBlockLogicalWidthForPositioned(const RenderBoxModelObject* containingBlock, RenderRegion* = 0,
        LayoutUnit offsetFromLogicalTopOfFirstPage = 0, bool checkForPerpendicularWritingMode = true) const;
    LayoutUnit containingBlockLogicalHeightForPositioned(const RenderBoxModelObject* containingBlock, bool checkForPerpendicularWritingMode = true) const;

    void computePositionedLogicalHeight();
    void computePositionedLogicalWidthUsing(SizeType, Length logicalWidth, const RenderBoxModelObject* containerBlock, TextDirection containerDirection,
                                            LayoutUnit containerLogicalWidth, LayoutUnit bordersPlusPadding,
                                            Length logicalLeft, Length logicalRight, Length marginLogicalLeft, Length marginLogicalRight,
                                            LayoutUnit& logicalWidthValue, LayoutUnit& marginLogicalLeftValue, LayoutUnit& marginLogicalRightValue, LayoutUnit& logicalLeftPos);
    void computePositionedLogicalHeightUsing(SizeType, Length logicalHeight, const RenderBoxModelObject* containerBlock,
                                             LayoutUnit containerLogicalHeight, LayoutUnit bordersPlusPadding,
                                             Length logicalTop, Length logicalBottom, Length marginLogicalTop, Length marginLogicalBottom,
                                             LayoutUnit& logicalHeightValue, LayoutUnit& marginLogicalTopValue, LayoutUnit& marginLogicalBottomValue, LayoutUnit& logicalTopPos);

    void computePositionedLogicalHeightReplaced();
    void computePositionedLogicalWidthReplaced();

    // This function calculates the minimum and maximum preferred widths for an object.
    // These values are used in shrink-to-fit layout systems.
    // These include tables, positioned objects, floats and flexible boxes.
    virtual void computePreferredLogicalWidths() { setPreferredLogicalWidthsDirty(false); }

private:
    // The width/height of the contents + borders + padding.  The x/y location is relative to our container (which is not always our parent).
    LayoutRect m_frameRect;

protected:
    LayoutBoxExtent m_marginBox;

    // The preferred logical width of the element if it were to break its lines at every possible opportunity.
    LayoutUnit m_minPreferredLogicalWidth;
    
    // The preferred logical width of the element if it never breaks any lines at all.
    LayoutUnit m_maxPreferredLogicalWidth;

    // For inline replaced elements, the inline box that owns us.
    InlineBox* m_inlineBoxWrapper;

    // Our overflow information.
    OwnPtr<RenderOverflow> m_overflow;

private:
    // Used to store state between styleWillChange and styleDidChange
    static bool s_hadOverflowClip;
};

inline RenderBox* toRenderBox(RenderObject* object)
{ 
    ASSERT(!object || object->isBox());
    return static_cast<RenderBox*>(object);
}

inline const RenderBox* toRenderBox(const RenderObject* object)
{ 
    ASSERT(!object || object->isBox());
    return static_cast<const RenderBox*>(object);
}

// This will catch anyone doing an unnecessary cast.
void toRenderBox(const RenderBox*);

inline RenderBox* RenderBox::previousSiblingBox() const
{
    return toRenderBox(previousSibling());
}

inline RenderBox* RenderBox::nextSiblingBox() const
{ 
    return toRenderBox(nextSibling());
}

inline RenderBox* RenderBox::parentBox() const
{
    return toRenderBox(parent());
}

inline RenderBox* RenderBox::firstChildBox() const
{
    return toRenderBox(firstChild());
}

inline RenderBox* RenderBox::lastChildBox() const
{
    return toRenderBox(lastChild());
}

} // namespace WebCore

#endif // RenderBox_h
