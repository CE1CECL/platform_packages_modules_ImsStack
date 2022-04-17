/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100507  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _FRAMED_MEDIA_PROPOSAL_H_
#define _FRAMED_MEDIA_PROPOSAL_H_

#include "AStringArray.h"
#include "media/MediaProposal.h"



class FramedMediaProposal
    : public MediaProposal
{
public:
    FramedMediaProposal(IN ISDPOAState *piOAState_,
            IN CONST AStringArray &objAcceptedContentTypes_);
    virtual ~FramedMediaProposal();

private:
    FramedMediaProposal(IN CONST FramedMediaProposal &objRHS);
    FramedMediaProposal& operator=(IN CONST FramedMediaProposal &objRHS);

public:
    // MediaProposal class
    virtual IMS_SINT32 GetType() const;

    // IFramedMedia interface
    const AStringArray& GetAcceptedContentTypes() const;

private:
    AStringArray objAcceptedContentTypes;
};

#endif // _FRAMED_MEDIA_PROPOSAL_H_
