/**
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VIDEO_PROFILE_H_
#define VIDEO_PROFILE_H_

#include "VideoDef.h"
#include "MediaBaseProfile.h"

/**
 * VideoProfile is used to keep the SDP negotiation information for video like
 * SDP offer, answer and the negotiated media information.
 */
class VideoProfile : public MediaBaseProfile
{
public:
    /**
     * VideoFmtp attributes are used within the SDP to carry video parameters that provide
     * extra configurations for the specific video codecs described in the rtpmap.
     */
    class VideoFmtp : public BaseFmtp
    {
    public:
        explicit VideoFmtp(IN VideoFmtp* pFmtp = IMS_NULL) :
                m_eResolution(VIDEO_RESOLUTION_INVALID),
                m_nBitrate(0),
                m_nFrameRate(0),
                m_nAs(0),
                m_nProfile(AVC_PROFILE_NONE),
                m_nLevel(0),
                m_nPacketizationMode(1),
                m_strSpropParam(AString::ConstNull()),
                m_bShowPacketizationMode(IMS_FALSE),
                m_bShowSpropParam(IMS_FALSE)
        {
            if (pFmtp == IMS_NULL)
            {
                return;
            }

            m_eResolution = pFmtp->m_eResolution;
            m_nBitrate = pFmtp->m_nBitrate;
            m_nFrameRate = pFmtp->m_nFrameRate;
            m_nAs = pFmtp->m_nAs;
            m_nProfile = pFmtp->m_nProfile;
            m_nLevel = pFmtp->m_nLevel;
            m_nPacketizationMode = pFmtp->m_nPacketizationMode;
            m_strSpropParam = pFmtp->m_strSpropParam;
            m_bShowPacketizationMode = pFmtp->m_bShowPacketizationMode;
            m_bShowSpropParam = pFmtp->m_bShowSpropParam;
        };

        VideoFmtp(IN const VIDEO_RESOLUTION eResolution, IN const IMS_SINT32 nBitrate,
                IN const IMS_SINT32 nFrameRate, IN const IMS_SINT32 nAs,
                IN const IMS_UINT32 nProfile, IN const IMS_UINT32 nLevel,
                IN const IMS_UINT32 nPacketization, IN const AString strSprop) :
                m_eResolution(eResolution),
                m_nBitrate(nBitrate),
                m_nFrameRate(nFrameRate),
                m_nAs(nAs),
                m_nProfile(nProfile),
                m_nLevel(nLevel),
                m_nPacketizationMode(nPacketization),
                m_strSpropParam(strSprop),
                m_bShowPacketizationMode(IMS_FALSE),
                m_bShowSpropParam(IMS_FALSE){};

        virtual ~VideoFmtp(){};

        void SetResolution(IN const VIDEO_RESOLUTION eResolution) { m_eResolution = eResolution; }
        VIDEO_RESOLUTION GetResolution() { return m_eResolution; }
        void SetBitrate(IN const IMS_SINT32 nBitrate) { m_nBitrate = nBitrate; }
        IMS_SINT32 GetBitrate() { return m_nBitrate; }
        void SetFramerate(IN const IMS_SINT32 nFrameRate) { m_nFrameRate = nFrameRate; }
        IMS_SINT32 GetFramerate() { return m_nFrameRate; }
        void SetAs(IN const IMS_SINT32 nAs) { m_nAs = nAs; }
        IMS_SINT32 GetAs() { return m_nAs; }
        void SetProfile(IN const IMS_SINT32 nProfile) { m_nProfile = nProfile; }
        IMS_SINT32 GetProfile() { return m_nProfile; }
        void SetLevel(IN const IMS_SINT32 nLevel) { m_nLevel = nLevel; }
        IMS_SINT32 GetLevel() { return m_nLevel; }
        void SetPacketizationMode(IN const IMS_SINT32 nMode) { m_nPacketizationMode = nMode; }
        IMS_SINT32 GetPacketizationMode() { return m_nPacketizationMode; }
        void SetSpropParam(IN const AString strSprop) { m_strSpropParam = strSprop; }
        AString& GetSpropParam() { return m_strSpropParam; }
        void SetShowPacketizationMode(IN const IMS_BOOL nShow) { m_bShowPacketizationMode = nShow; }
        IMS_BOOL IsShowPacketizationModeEnabled() { return m_bShowPacketizationMode; }
        void SetShowSpropParam(IN const IMS_BOOL nShow) { m_bShowSpropParam = nShow; }
        IMS_BOOL IsShowSpropParamEnabled() { return m_bShowSpropParam; }

    private:
        VIDEO_RESOLUTION m_eResolution;
        IMS_SINT32 m_nBitrate;
        IMS_SINT32 m_nFrameRate;
        IMS_SINT32 m_nAs;
        IMS_UINT32 m_nProfile;
        IMS_UINT32 m_nLevel;
        IMS_SINT32 m_nPacketizationMode;
        AString m_strSpropParam;
        IMS_BOOL m_bShowPacketizationMode;
        IMS_BOOL m_bShowSpropParam;
    };

    /**
     * AvcFmtp attributes are used within the SDP to carry AVC parameters that provide
     * extra configurations for the specific video codecs described in the rtpmap.
     */
    class AvcFmtp : public VideoFmtp
    {
    public:
        AString strProfileLevelId;
        IMS_BOOL bShow_ProfileLevelId;

    public:
        explicit AvcFmtp(IN AvcFmtp* pFmtp = IMS_NULL) :
                VideoFmtp(pFmtp),
                strProfileLevelId(AString::ConstNull()),
                bShow_ProfileLevelId(IMS_FALSE)
        {
            if (pFmtp == IMS_NULL)
            {
                return;
            }
            strProfileLevelId = pFmtp->strProfileLevelId;
            bShow_ProfileLevelId = pFmtp->bShow_ProfileLevelId;
        };

        AvcFmtp(IN const VIDEO_RESOLUTION resol, IN const IMS_UINT32 bitrate,
                IN const IMS_UINT32 frameRate, IN const IMS_UINT32 as, IN const IMS_UINT32 profile,
                IN const IMS_UINT32 level, IN const AString profileLevelID,
                IN const IMS_UINT32 packetization, IN const AString sprop) :
                VideoFmtp(resol, bitrate, frameRate, as, profile, level, packetization, sprop),
                strProfileLevelId(profileLevelID),
                bShow_ProfileLevelId(IMS_FALSE){};

        virtual ~AvcFmtp(){};
    };

    /**
     * HevcFmtp attributes are used within the SDP to carry HEVC parameters that provide
     * extra configurations for the specific video codecs described in the rtpmap.
     */
    class HevcFmtp : public VideoFmtp
    {
    public:
        IMS_BOOL bShow_Profile;
        IMS_BOOL bShow_Level;

    public:
        explicit HevcFmtp(IN HevcFmtp* pFmtp = IMS_NULL) :
                VideoFmtp(pFmtp),
                bShow_Profile(IMS_FALSE),
                bShow_Level(IMS_FALSE)
        {
            if (pFmtp == IMS_NULL)
            {
                return;
            }

            bShow_Profile = pFmtp->bShow_Profile;
            bShow_Level = pFmtp->bShow_Level;
        };

        HevcFmtp(IN const VIDEO_RESOLUTION resol, IN const IMS_UINT32 bitrate,
                IN const IMS_UINT32 frameRate, IN const IMS_UINT32 as,
                IN const VIDEO_PROFILE_HEVC profile, IN const IMS_UINT32 level,
                IN const IMS_UINT32 packetization, IN const AString& sprop) :
                VideoFmtp(resol, bitrate, frameRate, as, profile, level, packetization, sprop),
                bShow_Profile(IMS_FALSE),
                bShow_Level(IMS_FALSE){};

        virtual ~HevcFmtp(){};
    };

public:
    class RtcpFbAttributes
    {
    public:
        IMS_BOOL bTrrSupported;
        IMS_SINT32 nTrrInt;
        IMS_BOOL bNackSupported;
        IMS_BOOL bTmmbrSupported;
        IMS_SINT32 nTmmbrSmaxPr;
        IMS_BOOL bPliSupported;
        IMS_BOOL bFirSupported;

    public:
        RtcpFbAttributes() :
                bTrrSupported(IMS_FALSE),
                nTrrInt(0),
                bNackSupported(IMS_FALSE),
                bTmmbrSupported(IMS_FALSE),
                nTmmbrSmaxPr(-1),
                bPliSupported(IMS_FALSE),
                bFirSupported(IMS_FALSE){};

        RtcpFbAttributes& operator=(IN const RtcpFbAttributes& obj)
        {
            if (this != &obj)
            {
                bTrrSupported = obj.bTrrSupported;
                nTrrInt = obj.nTrrInt;
                bNackSupported = obj.bNackSupported;
                bTmmbrSupported = obj.bTmmbrSupported;
                nTmmbrSmaxPr = obj.nTmmbrSmaxPr;
                bPliSupported = obj.bPliSupported;
                bFirSupported = obj.bFirSupported;
            }
            return (*this);
        }
    };

public:
    /**
     * Payload for video is the actual video data transported by RTP in a packet.
     */
    class Payload : public BasePayload
    {
    public:
        IMS_BOOL bIncludeImageAttr;
        IMS_BOOL bIncludeFrameSize;
        AString strImageAttr;
        RtcpFbAttributes objRtcpFbAttr;

    public:
        Payload() :
                BasePayload(),
                bIncludeImageAttr(IMS_FALSE),
                bIncludeFrameSize(IMS_FALSE),
                strImageAttr(AString::ConstNull()),
                objRtcpFbAttr(){};

        Payload(IN const Payload& obj) :
                BasePayload(obj),
                bIncludeImageAttr(obj.bIncludeImageAttr),
                bIncludeFrameSize(obj.bIncludeFrameSize),
                strImageAttr(obj.strImageAttr),
                objRtcpFbAttr(obj.objRtcpFbAttr)
        {
            if (objRtpMap.GetPayloadType().EqualsIgnoreCase("H264"))
            {
                pFmtp = new VideoProfile::AvcFmtp(static_cast<VideoProfile::AvcFmtp*>(obj.pFmtp));
            }
            else if (objRtpMap.GetPayloadType().EqualsIgnoreCase("H265"))
            {
                pFmtp = new VideoProfile::HevcFmtp(static_cast<VideoProfile::HevcFmtp*>(obj.pFmtp));
            }
        }

        virtual ~Payload() {}

        Payload& operator=(IN const Payload& obj)
        {
            if (this != &obj)
            {
                BasePayload::operator=(obj);

                if (objRtpMap.GetPayloadType().EqualsIgnoreCase("H264"))
                {
                    pFmtp = new VideoProfile::AvcFmtp(
                            static_cast<VideoProfile::AvcFmtp*>(obj.pFmtp));
                }
                else if (objRtpMap.GetPayloadType().EqualsIgnoreCase("H265"))
                {
                    pFmtp = new VideoProfile::HevcFmtp(
                            static_cast<VideoProfile::HevcFmtp*>(obj.pFmtp));
                }

                bIncludeImageAttr = obj.bIncludeImageAttr;
                bIncludeFrameSize = obj.bIncludeFrameSize;
                strImageAttr = obj.strImageAttr;
                objRtcpFbAttr = obj.objRtcpFbAttr;
            }

            return (*this);
        }
    };

public:
    IMS_SINT32 nFrameRate;
    IMS_BOOL bSupportAvpf;
    IMS_SINT32 nCvoId;
    IMS_BOOL bSupportCapaNegoForAvpf;

public:
    VideoProfile() :
            MediaBaseProfile(
                    IpAddress::IPv6NONE, 0, 0, "RTP/AVPF", 0, 0, 0, 0, MEDIA_DIRECTION_INVALID),
            nFrameRate(0),
            bSupportAvpf(IMS_FALSE),
            nCvoId(-1),
            bSupportCapaNegoForAvpf(IMS_FALSE){};

    virtual ~VideoProfile() {}

    VideoProfile(IN VideoProfile* profile) :
            MediaBaseProfile(profile)
    {
        if (profile == nullptr)
        {
            return;
        }
        nFrameRate = profile->nFrameRate;
        bSupportAvpf = profile->bSupportAvpf;
        nCvoId = profile->nCvoId;
        bSupportCapaNegoForAvpf = profile->bSupportCapaNegoForAvpf;
    }

    VideoProfile(IN const VideoProfile& obj) :
            MediaBaseProfile(obj)
    {
        nFrameRate = obj.nFrameRate;
        bSupportAvpf = obj.bSupportAvpf;
        nCvoId = obj.nCvoId;
        bSupportCapaNegoForAvpf = obj.bSupportCapaNegoForAvpf;
    }

    VideoProfile& operator=(IN const VideoProfile& obj)
    {
        if (this != &obj)
        {
            MediaBaseProfile::operator=(obj);
            nFrameRate = obj.nFrameRate;
            bSupportAvpf = obj.bSupportAvpf;
            nCvoId = obj.nCvoId;
            bSupportCapaNegoForAvpf = obj.bSupportCapaNegoForAvpf;
        }

        return (*this);
    }

    Payload* GetPayloadAt(IN IMS_UINT32 nIndex) override
    {
        BasePayload* pPayload = MediaBaseProfile::GetPayloadAt(nIndex);
        return (pPayload != IMS_NULL) ? static_cast<Payload*>(pPayload) : IMS_NULL;
    }
};

#endif
