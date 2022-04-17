#ifndef _INTERFACE_AOS_MESSAGE_HANDLER_LISTENER_H_
#define _INTERFACE_AOS_MESSAGE_HANDLER_LISTENER_H_

class IAosMsgHandlerListener
{
public:
    /*
        * Subclasses must implement this to receive messages.
        *
        * Remarks
        * Parameters
        * <table>
        * parameter            description
        * ----------          ----------
        * nMessage             message to be delivered.
        * </table>
        *
        * Returns
        * <table>
        * return                   description
        * ----------           ----------
        * </table>
        */
    virtual void HandleMessage(IN IMS_SINT32 nMessage) = 0;
};

#endif // _INTERFACE_AOS_MESSAGE_HANDLER_LISTENER_H_
