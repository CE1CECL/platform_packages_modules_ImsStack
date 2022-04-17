/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20091208  toastops@                 Created
    </table>

    Description
     The IStreamMedia represents a standardized application-independent streaming media that
    can be rendered in real-time. The typical use is to stream audio or video content between
    endpoints in real-time, in for example telephony applications.
     The IMS engine manages the media formats, codecs and protocols used in the transmission
    according to relevant standard. It is possible to stream audio and video separately,
    as well as combined audio and video.
     By default, the IStreamMedia object supports audio streaming, which is mandatory for all
    devices claiming streaming support. The data is sourced from a microphone of the device.
     The originating endpoint may call IStreamMedia::SetStreamType() to explicitly set
    the media type for streaming, regardless of whether it is going to send or receive media.
    This is not possible for the terminating endpoint.
     For video (moving pictures), the camera is used to source video content. Both peripherals are
    used for the combined audio/video media. If the terminal has several microphones, or cameras,
    the choice is implementation-specific.
     It is possible for an originating endpoint to change the data source to something
    else if needed. This is not available to the terminating endpoint, thus limiting it to stream
    only from microphone and/or camera.
*/

#ifndef _INTERFACE_STREAM_MEDIA_H_
#define _INTERFACE_STREAM_MEDIA_H_

#include "media/IMedia.h"

/*

StreamMedia interface

Example

See Also
IMedia

*/
class IStreamMedia
    : public IMedia
{
public:
    /*
     Returns a IPlayer initiated to render the receiving part of this IMedia.
    The returned IPlayer is in the REALIZED state.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    void*                   IPlayer object
    </table>

    */
    virtual void* GetReceivingPlayer() const = 0;

    /*
     Returns a IPlayer initiated to source the sending part of this IMedia.
    The returned IPlayer is in the REALIZED state.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    void*                   IPlayer object
    </table>

    */
    virtual void* GetSendingPlayer() const = 0;

    /*
     Returns the stream type for the IMedia.
    If the stream type has not been set, it will default to STREAM_TYPE_AUDIO.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    IMS_SINT32              Stream type
    </table>

    */
    virtual IMS_SINT32 GetStreamType() const = 0;

    /*
     Sets a preferred quality of the media stream. This method does not guarantee
    the desired quality but enables the platform during the media negotiation to select
    an appropriate quality of service if possible.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    nQuality                Preferred quality
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    void (J281)
    IMS_RESULT (IMS)        IMS_SUCCESS / IMS_FAILURE
    </table>

    */
    virtual IMS_RESULT SetPreferredQuality(IN IMS_SINT32 nQuality) = 0;

    /*
     Sets the source to capture media from. If this method is not called, then the audio
    is sourced from a default microphone of the device, and video is sourced
    from a default camera.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    strSource               Source locator URI of media to be streamed
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    void (J281)
    IMS_RESULT (IMS)        IMS_SUCCESS / IMS_FAILURE
    </table>

    */
    virtual IMS_RESULT SetSource(IN CONST AString &strSource) = 0;

    /*
     Sets the stream type for the IMedia. This is used as an indication for the IMS engine
    regarding which stream type to use for this media.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    nType                   Stream type; AUDIO or VIDEO
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    void (J281)
    IMS_RESULT (IMS)        IMS_SUCCESS / IMS_FAILURE
    </table>

    */
    virtual IMS_RESULT SetStreamType(IN IMS_SINT32 nType) = 0;

    //// IMS extensions

public:
    // Stream types
    enum
    {
        STREAM_TYPE_AUDIO = 1,
        STREAM_TYPE_VIDEO,
        STREAM_TYPE_AUDIO_VIDEO
    };

    // Quality level
    enum
    {
        QUALITY_LOW = 1,
        QUALITY_MEDIUM,
        QUALITY_HIGH
    };
};

#endif // _INTERFACE_STREAM_MEDIA_H_
