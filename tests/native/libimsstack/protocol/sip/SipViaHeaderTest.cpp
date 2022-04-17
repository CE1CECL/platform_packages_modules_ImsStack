/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "msg/SipViaHeader.h"

namespace android {

class SipViaHeaderTest : public ::testing::Test {

public:
    SipViaHeader *pVia;

protected:
    virtual void SetUp() override {
        pVia = new SipViaHeader();
        ASSERT_TRUE(pVia != nullptr);
    }

    virtual void TearDown() override {
        if(pVia){
            delete pVia;
        }
    }
};

TEST_F(SipViaHeaderTest, createViaHdr) {
    EXPECT_EQ(SIP_TRUE,pVia->SetProtocolName((SIP_CHAR*)"SIP"));
    EXPECT_EQ(SIP_TRUE,pVia->SetProtocolVer((SIP_CHAR*)"2.0"));
    EXPECT_EQ(SIP_TRUE,pVia->SetTransport((SIP_CHAR*)"TCP"));
    EXPECT_EQ(SIP_TRUE,pVia->SetHost((SIP_CHAR*)"2409:4031:241d:5ff5:b54d:c29a:ecea:88b8"));
    EXPECT_EQ(SIP_TRUE,pVia->SetPortNum((SIP_UINT16)39002));
    EXPECT_EQ(SIP_TRUE,pVia->SetBranchParam((SIP_CHAR*)"z9hG4bK1422bd448-755bfe94"));
}

TEST_F(SipViaHeaderTest, createAndEncodeViaHdr) {
    // Max buffer size
    enum { MAX_BUFFER_SIZE = 65535 };
    SIP_CHAR BUFFER[MAX_BUFFER_SIZE];
    SIP_CHAR *pBuff =  &BUFFER[0];

    EXPECT_EQ(SIP_TRUE,pVia->SetProtocolName((SIP_CHAR*)"SIP"));
    EXPECT_EQ(SIP_TRUE,pVia->SetProtocolVer((SIP_CHAR*)"2.0"));
    EXPECT_EQ(SIP_TRUE,pVia->SetTransport((SIP_CHAR*)"TCP"));
    EXPECT_EQ(SIP_TRUE,pVia->SetHost((SIP_CHAR*)"2409:4031:241d:5ff5:b54d:c29a:ecea:88b8"));
    EXPECT_EQ(SIP_TRUE,pVia->SetPortNum((SIP_UINT16)39002));
    EXPECT_EQ(SIP_TRUE,pVia->SetBranchParam((SIP_CHAR*)"z9hG4bK1422bd448-755bfe94"));
    EXPECT_EQ(SIP_TRUE,pVia->EncodeHdr(&pBuff));
}

TEST_F(SipViaHeaderTest, createViaHdr_Negative) {
    EXPECT_EQ(SIP_FALSE,pVia->SetProtocolName((SIP_CHAR*)SIP_NULL));
    EXPECT_EQ(SIP_FALSE,pVia->SetProtocolVer((SIP_CHAR*)SIP_NULL));
    EXPECT_EQ(SIP_FALSE,pVia->SetTransport((SIP_CHAR*)SIP_NULL));
    EXPECT_EQ(SIP_FALSE,pVia->SetHost((SIP_CHAR*)SIP_NULL));
    EXPECT_EQ(SIP_FALSE,pVia->SetBranchParam((SIP_CHAR*)SIP_NULL));
}

} //namespace android
