/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20191018  sukhwan.mun@              Created
    </table>

    Description
*/


#ifndef _SERVICE_VONR_H_
#define _SERVICE_VONR_H_

#include "IMSMSG.h"

class IVoNR;
class VoNRServicePrivate;

class VoNRService
{
private:
    VoNRService();
    ~VoNRService();

private:
    VoNRService(IN CONST VoNRService &objRHS);
    VoNRService& operator=(IN CONST VoNRService &objRHS);

public:
    IVoNR* GetVoNR(IN IMS_SINT32 nSlotId);

    void DispatchServiceMessage(IN IMSMSG &objMSG);

    static VoNRService* GetVoNRService();

private:
    VoNRServicePrivate *pPrivate;
};

#endif // _SERVICE_VONR_H_
