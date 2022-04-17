/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20091208  toastops@                 Created
    </table>

    Description

*/

#ifndef _STREAM_MEDIA_H_
#define _STREAM_MEDIA_H_

#include "media/Media.h"

class StreamMediaImpl;



class StreamMedia
    : public Media
{
public:
    StreamMedia(IN Service *pService_, IN ISDPOAState *piOAState_);
    virtual ~StreamMedia();

private:
    StreamMedia(IN CONST StreamMedia &objRHS);
    StreamMedia& operator=(IN CONST StreamMedia &objRHS);

public:
    // IMedia interface
    virtual IMS_BOOL CanRead() const;
    virtual IMS_BOOL Exists() const;
    virtual IMS_SINT32 GetType() const;

    // IStreamMedia interface
    void* GetReceivingPlayer() const;
    void* GetSendingPlayer() const;
    IMS_SINT32 GetStreamType() const;
    IMS_RESULT SetPreferredQuality(IN IMS_SINT32 nQuality);
    IMS_RESULT SetSource(IN CONST AString &strSource);
    IMS_RESULT SetStreamType(IN IMS_SINT32 nType);

protected:
    virtual MediaProposal* CreateMediaProposal(IN ISDPOAState *piOAState);
    virtual IMS_BOOL PreviewInitInstance();
    virtual IMS_BOOL PostInitInstance();
    virtual void PreviewCleanupMedia();
    virtual void PostCleanupMedia();
    virtual void PreviewRemoveMedia();
    virtual void PostRemoveMedia();

public:
    // Stream types; Refer to IStreamMedia
    enum
    {
        STREAM_TYPE_AUDIO = 1,
        STREAM_TYPE_VIDEO,
        STREAM_TYPE_AUDIO_VIDEO
    };

    // Quality level; Refer to IStreamMedia
    enum
    {
        QUALITY_LOW = 1,
        QUALITY_MEDIUM,
        QUALITY_HIGH
    };

private:
    // Audio, Video, Audio/Video
    IMS_SINT32 nStreamType;
    // Low, Medium, High
    IMS_SINT32 nQualityLevel;
};

#endif // _STREAM_MEDIA_H_
