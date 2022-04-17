/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100427  hwangoo.park@             Created
    </table>

    Description
     The IFramedMedia represents a media connection on which content is delivered in packets.
    It can be used for instant messages, serialized data objects or files.
    The media is transported with the MSRP, according to RFC 4975.
    There are two different ways to access the send and receive methods:
    with content as byte arrays or as files.
    Large contents may be divided into multiple transactions. The different parts will be merged
    together in the remote terminal. It is possible to send
    and receive multiple messages simultaneous using the same IFramedMedia.
*/

#ifndef _INTERFACE_FRAMED_MEDIA_H_
#define _INTERFACE_FRAMED_MEDIA_H_

#include "IMSMap.h"
#include "AStringArray.h"
#include "ByteArray.h"
#include "media/IMedia.h"

class IFramedMediaListener;

/*

FramedMedia interface

Example

See Also
IMedia

*/
class IFramedMedia
    : public IMedia
{
public:
    /*
     Cancels the ongoing transfer.
    This method can be called for outgoing and incoming content.
    If no matching strMessageId is found or if the content is already transferred,
    this method will not do anything.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strMessageId            Identifier corresponding to the content to be cancelled
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void Cancel(IN CONST AString &strMessageId) = 0;

    /*
     Returns the accepted content type(s) of this media.
    If the accepted content type(s) has not been set, it will default to "*" and
    indicate that any content type may be transferred.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AStringArray            The accepted content type(s) of this media
    </table>

    */
    virtual const AStringArray& GetAcceptedContentTypes() const = 0;

    /*
     Returns the content type of the content that is identified by the strMessageId parameter.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strMessageId            Identifier for the content
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AString                 Content type
    </table>

    */
    virtual const AString& GetContentType(IN CONST AString &strMessageId) const = 0;

    /*
     Returns the header value from the content that is identified by the strMessageId parameter.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strMessageId            Identifier for the content
    strName                 Header name
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AString                 String containing the header value
                            Null string if the header does not exist
    </table>

    */
    virtual AString GetHeader(IN CONST AString &strMessageId, IN CONST AString &strName) const = 0;

    /*
     Receives content from the remote endpoint.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strMessageId            Identifier which content to receive
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    ByteArray               Byte array containing the received content
    </table>

    */
    virtual const ByteArray& ReceiveBytes(IN CONST AString &strMessageId) const = 0;

    /*
     Receives a file from the remote endpoint. This method will create a new file
    at the specified location. If the file already exists, it will be overwritten.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strMessageId            Identifier which file to receive
    strLocator              Location where the file will be stored
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    void (J281)
    IMS_RESULT (IMS)        IMS_SUCCESS / IMS_FAILURE
    </table>

    */
    virtual IMS_RESULT ReceiveFile(IN CONST AString &strMessageId,
            IN CONST AString &strLocator) = 0;

    /*
     Sends the content to the remote endpoint over a reliable connection.
    This method is asynchronous.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    objContent              Byte array containing the content to be sent
    strContentType          Content MIME type
    objHeaders              Headers to be set
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AString                 Identifier for the content sent
    </table>

    */
    virtual AString SendBytes(IN CONST ByteArray &objContent, IN CONST AString &strContentType,
            IN CONST IMSMap<AString, AStringArray> &objHeaders) = 0;

    /*
     Sends a file to the remote endpoint over a reliable connection.
    This method is asynchronous.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    objContent              Location of the file to be sent
    strContentType          Content MIME type
    objHeaders              Headers to be set
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    AString                 Identifier for the content sent
    </table>

    */
    virtual AString SendFile(IN CONST AString &strLocator, IN CONST AString &strContentType,
            IN CONST IMSMap<AString, AStringArray> &objHeaders) = 0;

    /*
     Sets the accepted content type(s) of this media.
    If the accepted content type(s) has not been set, it will default to "*" and
    indicate that any content type may be transferred.

    Parameters
    <table>
    parameter                   description
    ----------                  ----------
    objAcceptedContentTypes     Array of the content types accepted in this media
    </table>

    Returns
    <table>
    return                      description
    ----------                  ----------
    void (J281)
    IMS_RESULT (IMS)            IMS_SUCCESS / IMS_FAILURE
    </table>

    */
    virtual IMS_RESULT SetAcceptedContentTypes(
            IN CONST AStringArray &objAcceptedContentTypes) = 0;

    /*
     Sets a listener for this IFramedMedia, replacing any previous IFramedMediaListener.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    piListener              Listener to be set, or NULL
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void SetListener(IN IFramedMediaListener *piListener) = 0;
};

#endif // _INTERFACE_FRAMED_MEDIA_H_
