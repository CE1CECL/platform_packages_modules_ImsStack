#include "FailReason.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(FailReasonTest, CopyConstructor)
{
    FailReason objReason(1, 1, AString("Extra Value"));
    FailReason objCopiedReason(objReason);

    EXPECT_EQ(objReason.nReason, objCopiedReason.nReason);
    EXPECT_EQ(objReason.nExtra, objCopiedReason.nExtra);
    EXPECT_STREQ(objReason.strExtra.GetStr(), objCopiedReason.strExtra.GetStr());
}

TEST(FailReasonTest, InitialValue)
{
    IMS_SINT32 nReason = 1;
    FailReason objReason(nReason);

    EXPECT_EQ(objReason.nReason, nReason);
    EXPECT_EQ(objReason.nExtra, -1);
    EXPECT_EQ(objReason.strExtra, AString::ConstNull());
}

TEST(FailReasonTest, AssignOperator)
{
    FailReason objAssignedReason(0);
    FailReason objReason(1, 1, AString("Extra Value"));

    objAssignedReason = objReason;

    EXPECT_EQ(objReason.nReason, objAssignedReason.nReason);
    EXPECT_EQ(objReason.nExtra, objAssignedReason.nExtra);
    EXPECT_STREQ(objReason.strExtra.GetStr(), objAssignedReason.strExtra.GetStr());
}
