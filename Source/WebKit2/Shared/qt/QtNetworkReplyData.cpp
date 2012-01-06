/*
 * Copyright (C) 2011 Zeno Albisser <zeno@webkit.org>
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

#include "config.h"
#include "QtNetworkReplyData.h"

#include "ArgumentCodersQt.h"
#include "KURL.h"
#include "Noncopyable.h"
#include "SharedMemory.h"
#include "WebCoreArgumentCoders.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <wtf/text/WTFString.h>

namespace WebKit {

QtNetworkReplyData::QtNetworkReplyData()
    : m_contentLength(0)
    , m_lastModified(0)
{ }

void QtNetworkReplyData::encode(CoreIPC::ArgumentEncoder* encoder) const
{
    encoder->encode(m_urlString);
    encoder->encodeEnum(m_operation);
    encoder->encode(m_contentDisposition);
    encoder->encode(m_contentType);
    encoder->encodeUInt64(m_contentLength);
    encoder->encode(m_location);
    encoder->encodeUInt64(m_lastModified);
    encoder->encode(m_cookie);
    encoder->encode(m_userAgent);
    encoder->encode(m_server);
    encoder->encode(m_replyUuid);
    encoder->encode(m_dataHandle);
}

bool QtNetworkReplyData::decode(CoreIPC::ArgumentDecoder* decoder, QtNetworkReplyData& destination)
{
    if (!decoder->decode(destination.m_urlString))
        return false;
    if (!decoder->decodeEnum(destination.m_operation))
        return false;
    if (!decoder->decode(destination.m_contentDisposition))
        return false;
    if (!decoder->decode(destination.m_contentType))
        return false;
    if (!decoder->decodeUInt64(destination.m_contentLength))
        return false;
    if (!decoder->decode(destination.m_location))
        return false;
    if (!decoder->decodeUInt64(destination.m_lastModified))
        return false;
    if (!decoder->decode(destination.m_cookie))
        return false;
    if (!decoder->decode(destination.m_userAgent))
        return false;
    if (!decoder->decode(destination.m_server))
        return false;
    if (!decoder->decode(destination.m_replyUuid))
        return false;
    if (!decoder->decode(destination.m_dataHandle))
        return false;
    return true;
}

} // namespace WebKit


