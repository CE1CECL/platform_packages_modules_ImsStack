/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20091208  toastops@                 Created
    </table>

    Description

*/

#ifndef _STREAM_MEDIA_IMPL_H_
#define _STREAM_MEDIA_IMPL_H_

#include "media/MediaImpl.h"
#include "media/IStreamMedia.h"
#include "media/IOnMediaListener.h"
#include "media/StreamMedia.h"

class StreamMediaProposalImpl;



class StreamMediaImpl
    : public MediaImpl
    , public IStreamMedia
    , public IOnMediaListener
{
public:
    StreamMediaImpl(IN StreamMedia *pStreamMedia_);
    virtual ~StreamMediaImpl();

private:
    StreamMediaImpl(IN CONST StreamMediaImpl &objRHS);
    StreamMediaImpl& operator=(IN CONST StreamMediaImpl &objRHS);

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

    // IStreamMedia interface
    virtual void* GetReceivingPlayer() const;
    virtual void* GetSendingPlayer() const;
    virtual IMS_SINT32 GetStreamType() const;
    virtual IMS_RESULT SetPreferredQuality(IN IMS_SINT32 nQuality);
    virtual IMS_RESULT SetSource(IN CONST AString &strSource);
    virtual IMS_RESULT SetStreamType(IN IMS_SINT32 nType);

private:
    IMediaListener *piMediaListener;
    StreamMediaProposalImpl *pStreamMediaProposal;

    StreamMedia *pStreamMedia;
};

#endif // _STREAM_MEDIA_IMPL_H_
