/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    </table>

    Description

*/

#ifndef _AOS_ENABLER_H_
#define _AOS_ENABLER_H_

#include "Enabler.h"

class AosEnabler
    : public Enabler
{
public:
    explicit AosEnabler(IN IMS_SINT32 nSlotId);
    virtual ~AosEnabler();

public:
    // IEnabler class
    virtual void Start();
    virtual void Stop();
};

#endif // _AOS_ENABLER_H_
