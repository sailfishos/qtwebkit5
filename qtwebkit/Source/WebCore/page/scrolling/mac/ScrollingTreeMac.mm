/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "config.h"
#import "ScrollingTree.h"

#if ENABLE(THREADED_SCROLLING)

#import "AutodrainedPool.h"
#import "ScrollingTreeScrollingNodeMac.h"
#import <QuartzCore/CATextLayer.h>

namespace WebCore {

void ScrollingTree::setDebugRootLayer(CALayer *rootLayer)
{
    AutodrainedPool pool;

    [m_debugInfoLayer.get() removeFromSuperlayer];

    if (!rootLayer)
        return;

    m_debugInfoLayer = [CALayer layer];
    [rootLayer addSublayer:m_debugInfoLayer.get()];

    [m_debugInfoLayer.get() setBorderWidth:1];
    [m_debugInfoLayer.get() setBorderColor:CGColorGetConstantColor(kCGColorBlack)];
    
    [m_debugInfoLayer.get() setFrame:CGRectMake(0, 0, 25, 25)];
    updateDebugRootLayer();
}

void ScrollingTree::updateDebugRootLayer()
{
    if (!m_debugInfoLayer)
        return;

    AutodrainedPool pool;

    RetainPtr<CGColorRef> backgroundColor;

    if (m_rootNode->shouldUpdateScrollLayerPositionOnMainThread())
        backgroundColor = adoptCF(CGColorCreateGenericRGB(1, 0, 0, .7));

    {
        MutexLocker lock(m_mutex);
        if (m_hasWheelEventHandlers) {
            if (!backgroundColor)
                backgroundColor = adoptCF(CGColorCreateGenericRGB(1, 1, 0, .7));
        }
    }

    if (!backgroundColor)
        backgroundColor = adoptCF(CGColorCreateGenericRGB(0, 1, 0, .7));

    [m_debugInfoLayer.get() setBackgroundColor:backgroundColor.get()];
}

} // namespace WebCore

#endif // ENABLE(THREADED_SCROLLING)
