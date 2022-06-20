/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20170510  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _ENABLER_LOADER_H_
#define _ENABLER_LOADER_H_

#include "IMSMap.h"
#include "ISystemConfigListener.h"

class EnablerFactory;
class EnablerThread;

class EnablerLoader
    : public ISystemConfigListener
{
private:
    EnablerLoader();
    virtual ~EnablerLoader();

    EnablerLoader(IN const EnablerLoader& objRHS);
    EnablerLoader& operator=(IN const EnablerLoader& objRHS);

public:
    void Init();

    static void CreateInstance();
    static void DestroyInstance();
    static EnablerLoader* GetInstance();

protected:
    // ISystemConfigListener class
    virtual void SystemConfig_ConfigurationChanged(
            IN IMS_SINT32 nEvent, IN IMS_SINT32 nSlotId = IMS_SLOT_ANY);

private:
    void CreateAndAddThread(IN IMS_SINT32 nSlotId);
    EnablerThread* GetEnablerThread(IN IMS_SINT32 nSlotId) const;

    void ControlEnablers(IN IMS_SINT32 nSlotId);

    static ImsAppThread* CreateThread(IN void* pvParam);

private:
    class EnablerThreadParam
    {
    public:
        inline EnablerThreadParam(IN EnablerFactory *pEnablerFactory_, IN IMS_SINT32 nSlotId_)
            : pEnablerFactory(pEnablerFactory_)
            , nSlotId(nSlotId_)
        {}
        inline ~EnablerThreadParam()
        {}

    public:
        EnablerFactory *pEnablerFactory;
        IMS_SINT32 nSlotId;
    };

    static EnablerLoader *pEnablerLoader;

    EnablerFactory *pEnablerFactory;

    // <slot-id, enabler-thread>
    IMSMap<IMS_SINT32, EnablerThread*> objEnablerThreads;
};

#endif // _ENABLER_LOADER_H_
