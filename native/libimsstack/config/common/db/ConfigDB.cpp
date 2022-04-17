/*
    Author
    <table>
    date      author                    description
    --------  --------------            ----------
    20101010  hwangoo.park@             Created
    </table>

    Description

*/

#include "ServiceMemory.h"
#include "db/ConfigDB.h"

// DB field name : "id" - unique row identifier of db table
PUBLIC GLOBAL
const IMS_CHAR ConfigDB::FIELD_NAME_ID[] = "id";

// DB field name : "property" - ReadOnly : "0", ReadWrite : "1"
PUBLIC GLOBAL
const IMS_CHAR ConfigDB::FIELD_NAME_PROPERTY[] = "property";

// DB field name : "conf"
PUBLIC GLOBAL
const IMS_CHAR ConfigDB::FIELD_NAME_CONF[] = "conf";
