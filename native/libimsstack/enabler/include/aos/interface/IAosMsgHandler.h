#ifndef _INTERFACE_AOS_MESSAGE_HANDLER_H_
#define _INTERFACE_AOS_MESSAGE_HANDLER_H_

class IAosMsgHandlerListener;

class IAosMsgHandler
{
public:
    /*
        * Sends a Message to be delivered after the specified amount of time elapses.
        *
        * Remarks
        * Parameters
        * <table>
        * parameter            description
        * ----------          ----------
        * piListener             entity which will receive the message.
        * nMessage             message to be delivered.
        * nDuration             time duration in milli-seconds.
        * </table>
        *
        * Returns
        * <table>
        * return                   description
        * ----------           ----------
        * IMS_TRUE              the message is successfully placed in to the queue.
        * IMS_FALSE             failure. usually because the message is existing in the queue.
        * </table>
        */
    virtual IMS_BOOL SendEmptyMessageDelayed(IN CONST IAosMsgHandlerListener* piListener,
        IN IMS_SINT32 nMessage, IN IMS_SINT32 nDuration) = 0;

    /*
        * Remove any pending posts of messages in the message queue.
        *
        * Remarks
        * Parameters
        * <table>
        * parameter            description
        * ----------          ----------
        * piListener             entity which is registered to receive the message.
        * nMessage             message to be delivered.
        * </table>
        *
        * Returns
        * <table>
        * return                   description
        * ----------           ----------
        * </table>
        */
    virtual void RemoveMessages(IN CONST IAosMsgHandlerListener* piListener,
        IN IMS_SINT32 nMessage) = 0;

    /*
        * Check if there are any pending posts of messages in the message queue.
        *
        * Remarks
        * Parameters
        * <table>
        * parameter            description
        * ----------          ----------
        * piListener             entity which is registered to receive the message.
        * nMessage             message to be delivered.
        * </table>
        *
        * Returns
        * <table>
        * return                   description
        * ----------           ----------
        * IMS_TRUE              the message is existing in the message queue.
        * IMS_FALSE             the message is not existing in the message queue.
        * </table>
        */
    virtual IMS_BOOL HasMessages(IN CONST IAosMsgHandlerListener* piListener,
        IN IMS_SINT32 nMessage) = 0;
};

#endif //_INTERFACE_AOS_MESSAGE_HANDLER_H_
