/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20091208  toastops@                 Created
    </table>

    Description

*/

#ifndef _STREAM_MEDIA_PROPOSAL_H_
#define _STREAM_MEDIA_PROPOSAL_H_

#include "media/MediaProposal.h"



class StreamMediaProposal
    : public MediaProposal
{
public:
    StreamMediaProposal(IN ISDPOAState *piOAState_, IN IMS_SINT32 nStreamType_);
    virtual ~StreamMediaProposal();

private:
    StreamMediaProposal(IN CONST StreamMediaProposal &objRHS);
    StreamMediaProposal& operator=(IN CONST StreamMediaProposal &objRHS);

public:
    // MediaProposal class
    virtual IMS_SINT32 GetType() const;

    // IStreamMedia interface
    IMS_SINT32 GetStreamType() const;

private:
    IMS_SINT32 nStreamType;
};

#endif // _STREAM_MEDIA_PROPOSAL_H_
