/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100424  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _PUBLICATION_REFRESH_HELPER_H_
#define _PUBLICATION_REFRESH_HELPER_H_

#include "util/RefreshHelper.h"

class PubState;



class PublicationRefreshHelper
    : public RefreshHelper
{
public:
    PublicationRefreshHelper(IN IRefreshable *piRefreshable_, IN CONST PubState *pPubState_);
    virtual ~PublicationRefreshHelper();

public:
    virtual IMS_BOOL AddSpecificHeader(IN ISIPConnection *piSC);
    virtual IMS_RESULT SendRefreshRequest(IN ISIPClientConnection *piSCC);
    virtual IMS_RESULT UpdateOnMessageReceived(IN CONST ISIPConnection *piSC);
    virtual IMS_RESULT UpdateOnMessageSent(IN CONST ISIPConnection *piSC);

protected:
    virtual void RefreshCompleted(IN ISIPClientConnection *piSCC, IN IMS_SINT32 nCode = 0);
    virtual void RefreshStarted();
    virtual void RefreshTerminated();

private:

public:

private:
    const PubState *pPubState;
};

#endif // _PUBLICATION_REFRESH_HELPER_H_
