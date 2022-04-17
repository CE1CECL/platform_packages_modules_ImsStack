/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100506  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _FRAMED_MEDIA_PROPOSAL_IMPL_H_
#define _FRAMED_MEDIA_PROPOSAL_IMPL_H_

#include "media/IFramedMedia.h"
#include "media/FramedMediaProposal.h"



class FramedMediaProposalImpl
    : public IFramedMedia
{
public:
    FramedMediaProposalImpl(IN FramedMediaProposal *pMediaProposal_);
    virtual ~FramedMediaProposalImpl();

private:
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

private:
    FramedMediaProposal *pMediaProposal;
};

#endif // _FRAMED_MEDIA_PROPOSAL_IMPL_H_
