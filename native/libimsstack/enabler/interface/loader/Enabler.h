/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170221  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _ENABLER_H_
#define _ENABLER_H_

#include "IEnabler.h"

class Enabler
    : public IEnabler
{
public:
    explicit Enabler(IN IMS_SINT32 nSlotId_);
    virtual ~Enabler();

public:
    inline IMS_SINT32 GetSlotId() const
    { return nSlotId; }

    // IEnabler class
    virtual void Start();
    virtual void Stop();

    virtual void Destroy();

private:
    IMS_SINT32 nSlotId;
};

#endif // _ENABLER_H_
