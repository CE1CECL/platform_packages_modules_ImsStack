
/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20100311  hwangoo.park@             Created
    </table>

    Description

*/

#ifndef _INTERFACE_IMS_TRACE_TAG_DECL_
#define _INTERFACE_IMS_TRACE_TAG_DECL_

////
// TRACE MODULE DEFINITIONS FOR FRAMEWORK & ENABLERS
////

//// IMS ("ims" package)
//// engine -> connection/service, etc...
#define __IMS_TRACE_TAG_IMS__                    __IMS_TRACE_TAG__(IMS)

//// IMS ("ims" package)
//// engine -> registration
#define __IMS_TRACE_TAG_REG__                    __IMS_TRACE_TAG__(REG)

//// CORE, CORE.MEDIA ("ims.core", "ims.core.media" package)
//// engine -> core, core/media
#define __IMS_TRACE_TAG_IMS_CORE__               __IMS_TRACE_TAG__(CORE)

//// PLATFORM
//// platform -> base
#define __IMS_TRACE_TAG_BASE__                   __IMS_TRACE_TAG__(BASE)

//// ADAPT
//// platform -> os
#define __IMS_TRACE_TAG_ADAPT__                  __IMS_TRACE_TAG__(ADAPT)

//// CONF
//// config
#define __IMS_TRACE_TAG_CONF__                   __IMS_TRACE_TAG__(CONF)

//// SIP
//// engine -> sipcore, protocol/sip -> sip
#define __IMS_TRACE_TAG_SIP__                    __IMS_TRACE_TAG__(SIP)

//// XML
//// protocol/xml -> xml
#define __IMS_TRACE_TAG_XML__                    __IMS_TRACE_TAG__(XML)

//// SDP
//// protocol/sdp -> sdp
#define __IMS_TRACE_TAG_SDP__                    __IMS_TRACE_TAG__(SDP)

//// FWK
//// platform -> base
#define __IMS_TRACE_TAG_FWK__                    __IMS_TRACE_TAG__(FWK)

//// AOS
//// enabler -> aos
#define __IMS_TRACE_TAG_AOS__                    __IMS_TRACE_TAG__(AOS)

//// MTC
//// enabler -> mtc
#define __IMS_TRACE_TAG_COM_UC__                 __IMS_TRACE_TAG__(MTC)
#define __IMS_TRACE_TAG_COM_MTC__                __IMS_TRACE_TAG__(MTC)
#define __IMS_TRACE_TAG_MTC__                    __IMS_TRACE_TAG__(MTC)

//// MTS
//// enabler -> mts
#define __IMS_TRACE_TAG_COM_SMS__                __IMS_TRACE_TAG__(MTS)
#define __IMS_TRACE_TAG_MTS__                    __IMS_TRACE_TAG__(MTS)

//// UCE
//// enabler -> uce
#define __IMS_TRACE_TAG_UCE__                    __IMS_TRACE_TAG__(UCE)

//// SIP_DELEGATE
//// enabler -> sipdelegate
#define __IMS_TRACE_TAG_SIP_DELEGATE__           __IMS_TRACE_TAG__(SIP_DELEGATE)

#endif // _INTERFACE_IMS_TRACE_TAG_DECL_
