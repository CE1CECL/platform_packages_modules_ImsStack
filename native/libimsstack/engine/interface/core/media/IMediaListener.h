/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100427  hwangoo.park@             Created
    </table>

    Description
     A listener type for receiving notification of when some mode property has changed
    on the media flow.
*/

#ifndef _INTERFACE_MEDIA_LISTENER_H_
#define _INTERFACE_MEDIA_LISTENER_H_

class IMedia;

class IMediaListener
{
public:
    /*
     The method is called when some media mode property has changed.

    Parameters
    <table>
    parameter               description
    ----------              ----------
    piMedia                 Concerned IMedia object
    </table>

    Returns
    <table>
    return                  description
    ----------              ----------
    </table>

    */
    virtual void Media_ModeChanged(IN IMedia *piMedia) = 0;
};

#endif // _INTERFACE_MEDIA_LISTENER_H_
