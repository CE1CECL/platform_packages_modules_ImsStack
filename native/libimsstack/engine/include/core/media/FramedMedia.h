/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100503  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _FRAMED_MEDIA_H_
#define _FRAMED_MEDIA_H_

#include "IMSMap.h"
#include "AStringArray.h"
#include "ByteArray.h"
#include "media/Media.h"

class IOnFramedMediaListener;



class FramedMedia
    : public Media
{
public:
    FramedMedia(IN Service *pService_, IN ISDPOAState *piOAState_);
    virtual ~FramedMedia();

private:
    FramedMedia(IN CONST FramedMedia &objRHS);
    FramedMedia& operator=(IN CONST FramedMedia &objRHS);

public:
    // Media class
    virtual IMS_BOOL CanRead() const;
    virtual IMS_BOOL Exists() const;
    virtual IMS_SINT32 GetType() const;

    // IFramedMedia interface
    void Cancel(IN CONST AString &strMessageId);
    const AStringArray& GetAcceptedContentTypes() const;
    const AString& GetContentType(IN CONST AString &strMessageId) const;
    AString GetHeader(IN CONST AString &strMessageId, IN CONST AString &strName) const;
    const ByteArray& ReceiveBytes(IN CONST AString &strMessageId) const;
    IMS_RESULT ReceiveFile(IN CONST AString &strMessageId, IN CONST AString &strLocator);
    AString SendBytes(IN CONST ByteArray &objContent, IN CONST AString &strContentType,
            IN CONST IMSMap<AString, AStringArray> &objHeaders);
    AString SendFile(IN CONST AString &strLocator, IN CONST AString &strContentType,
            IN CONST IMSMap<AString, AStringArray> &objHeaders);
    IMS_RESULT SetAcceptedContentTypes(IN CONST AStringArray &objAcceptedContentTypes);
    void SetListener(IN IOnFramedMediaListener *piListener);

protected:
    // Media class
    virtual MediaProposal* CreateMediaProposal(IN ISDPOAState *piOAState);
    virtual IMS_BOOL PreviewInitInstance();
    virtual IMS_BOOL PostInitInstance();
    virtual void PreviewCleanupMedia();
    virtual void PostCleanupMedia();
    virtual void PreviewRemoveMedia();
    virtual void PostRemoveMedia();

private:
    AStringArray objAcceptedContentTypes;

    IOnFramedMediaListener *piListener;
};

#endif // _FRAMED_MEDIA_H_
