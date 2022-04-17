/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100503  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _FRAMED_MEDIA_IMPL_H_
#define _FRAMED_MEDIA_IMPL_H_

#include "media/MediaImpl.h"
#include "media/IFramedMedia.h"
#include "media/IOnMediaListener.h"
#include "media/IOnFramedMediaListener.h"
#include "media/FramedMedia.h"

class FramedMediaProposalImpl;



class FramedMediaImpl
    : public MediaImpl
    , public IFramedMedia
    , public IOnMediaListener
    , public IOnFramedMediaListener
{
public:
    FramedMediaImpl(IN FramedMedia *pFramedMedia_);
    virtual ~FramedMediaImpl();

private:
    FramedMediaImpl(IN CONST FramedMediaImpl &objRHS);
    FramedMediaImpl& operator=(IN CONST FramedMediaImpl &objRHS);

private:
    // MediaImpl class
    virtual IMS_BOOL Equals(IN CONST IMedia *piMedia) const;
    virtual IMedia* GetInterface();
    virtual Media* GetMedia() const;

    // IMedia interface
    virtual IMS_BOOL CanRead() const;
    virtual IMS_BOOL CanWrite() const;
    virtual IMS_BOOL Exists() const;
    virtual IMS_SINT32 GetDirection() const;
    virtual IMSList<IMediaDescriptor*> GetMediaDescriptors() const;
    virtual IMedia* GetProposal(IN IMS_BOOL bIMSExtension = IMS_TRUE) const;
    virtual IMS_SINT32 GetState() const;
    virtual IMS_SINT32 GetUpdateState() const;
    virtual IMS_RESULT SetDirection(IN IMS_SINT32 nDirection);
    virtual void SetMediaListener(IN IMediaListener *piListener);

    //// IMS extensions
    virtual IMediaDescriptor* GetMediaDescriptor() const;
    virtual IMS_SINT32 GetType() const;
    virtual void RemoveMediaDescriptor(IN IMS_UINT32 nPosition);

    virtual void OnMedia_FictitiousMediaCreated(IN Media *pMedia);
    virtual void OnMedia_FictitiousMediaDestroyed(IN Media *pMedia);
    virtual void OnMedia_ModeChanged(IN Media *pMedia);

    // IFramedMedia interface
    virtual void Cancel(IN CONST AString &strMessageId);
    virtual const AStringArray& GetAcceptedContentTypes() const;
    virtual const AString& GetContentType(IN CONST AString &strMessageId) const;
    virtual AString GetHeader(IN CONST AString &strMessageId, IN CONST AString &strName) const;
    virtual const ByteArray& ReceiveBytes(IN CONST AString &strMessageId) const;
    virtual IMS_RESULT ReceiveFile(IN CONST AString &strMessageId, IN CONST AString &strLocator);
    virtual AString SendBytes(IN CONST ByteArray &objContent, IN CONST AString &strContentType,
            IN CONST IMSMap<AString, AStringArray> &objHeaders);
    virtual AString SendFile(IN CONST AString &strLocator, IN CONST AString &strContentType,
            IN CONST IMSMap<AString, AStringArray> &objHeaders);
    virtual IMS_RESULT SetAcceptedContentTypes(IN CONST AStringArray &objAcceptedContentTypes);
    virtual void SetListener(IN IFramedMediaListener *piListener);

    // IOnFramedMediaListener interface
    virtual void OnFramedMedia_ConnectionError(IN FramedMedia *pMedia);
    virtual void OnFramedMedia_ContentReceived(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId, IN IMS_SINT32 nSize, IN CONST AString &strFileName);
    virtual void OnFramedMedia_ContentReceiveFailed(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId);
    virtual void OnFramedMedia_DeliveryFailure(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId, IN IMS_SINT32 nStatusCode,
            IN CONST AString &strReasonPhrase);
    virtual void OnFramedMedia_DeliverySuccess(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId);
    virtual void OnFramedMedia_TransferProgress(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId, IN IMS_SINT32 nTransferredBytes,
            IN IMS_SINT32 nTotalBytes);

private:
    IMediaListener *piMediaListener;
    IFramedMediaListener *piFramedMediaListener;
    FramedMediaProposalImpl *pFramedMediaProposal;

    FramedMedia *pFramedMedia;
};

#endif // _FRAMED_MEDIA_IMPL_H_
