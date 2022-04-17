/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100513  hwangoo.park@             Created
    </table>

    Description
     A listener type for receiving notification of when FramedMedia content is transferred.
    When content becomes available the application is notified by a call to the ContentReceived()
    method. The application can then retrieve the content by either using ReceiveBytes()
    or ReceiveFile() method on the FramedMedia class.
*/

#ifndef _INTERFACE_ON_FRAMED_MEDIA_LISTENER_H_
#define _INTERFACE_ON_FRAMED_MEDIA_LISTENER_H_

class FramedMedia;



class IOnFramedMediaListener
{
public:
    /*
     Notifies the application when an I/O error has occurred.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pMedia                  Concerned FramedMedia object
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void OnFramedMedia_ConnectionError(IN FramedMedia *pMedia) = 0;

    /*
     Notifies the application when the content is completely received. The content can now be
    retrieved by calling ReceiveBytes(...) or ReceiveFile(...) with the corresponding strMessageId
    on FramedMedia object.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pMedia                  FramedMedia object that received the content
    strMessageId            Identifier for the content that is ready for retrieval
    nSize                   Size of the content in bytes
    strFileName             File name or NULL if a file name is not associated
                            with the strMessageId
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void OnFramedMedia_ContentReceived(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId, IN IMS_SINT32 nSize,
            IN CONST AString &strFileName) = 0;

    /*
     Notifies the application that the content could not be received or that the content has been
    cancelled by the sending endpoint. The strMessageId is considered invalid after this callback
    and further use of this strMessageId is not possible.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pMedia                  FramedMedia object that could not receive the content
    strMessageId            Identifier for the content that could not be received
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void OnFramedMedia_ContentReceiveFailed(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId) = 0;

    /*
     Notifies the application when the content corresponding to the strMessageId has not been
    successfully delivered to the remote endpoint. The strMessageId is considered invalid
    after this callback and further use of this strMessageId is not possible.
     This method is invoked at the endpoint that sent the content.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pMedia                  FramedMedia object that sent the content
    strMessageId            Identifier for the content that could not be transferred
    nStatusCode             Status code why the transaction failed
    strReasonPhrase         Reason phrase
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void OnFramedMedia_DeliveryFailure(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId, IN IMS_SINT32 nStatusCode,
            IN CONST AString &strReasonPhrase) = 0;

    /*
     Notifies the application when the content corresponding to the strMessageId has been
    successfully delivered to the remote endpoint. The strMessageId is considered invalid
    after this callback and further use of this strMessageId is not possible.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pMedia                  FramedMedia object that sent the content
    strMessageId            Identifier for the content that has been tranferred
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void OnFramedMedia_DeliverySuccess(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId) = 0;

    /*
     Receives a file from the remote endpoint. This method will create a new file at the specified
    location. If the file already exists, it will be overwritten.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    pMedia                  FramedMedia object that received/sent the content
    strMessageId            Identifier for the content that is being transferred
    nTransferredBytes       Number of bytes that is transferred
    nTotalBytes             Number of total bytes in the content, or -1 if the size is not known
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void OnFramedMedia_TransferProgress(IN FramedMedia *pMedia,
            IN CONST AString &strMessageId, IN IMS_SINT32 nTransferredBytes,
            IN IMS_SINT32 nTotalBytes) = 0;
};

#endif // _INTERFACE_ON_FRAMED_MEDIA_LISTENER_H_
