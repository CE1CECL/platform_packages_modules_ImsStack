/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100220  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_TRACE_OPTION_H_
#define _INTERFACE_IMS_TRACE_OPTION_H_

class ITraceOption
{
public:
    enum
    {
        // Option - filtering
        OPT_CAT_NONE = 0x00000000,
        OPT_CAT_D = 0x00000001,
        OPT_CAT_E = 0x00000002,
        OPT_CAT_I = 0x00000004,
        OPT_CAT_TEXT = 0x00000008,
        OPT_CAT_ALL = (OPT_CAT_D | OPT_CAT_E | OPT_CAT_I | OPT_CAT_TEXT),

        OPT_HIDE_PRIVACY = 0x00000100,

        // Option - logging medium
        OPT_MEDIUM_SERIAL = 0x00010000,

        OPT_MAX = 0x7FFFFFFF
    };
};

#endif // _INTERFACE_IMS_TRACE_OPTION_H_
