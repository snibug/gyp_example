/*
 * Copyright 2014 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "lb_virtual_file_system.h"

#include "base/compiler_specific.h"
#include "external/chromium/testing/gtest/include/gtest/gtest.h"

#if defined(__LB_PS4__) || defined(__LB_XB360__)
class VirtualFileSystemTest : public testing::Test {
 protected:
  virtual void SetUp() OVERRIDE {
    vfs_ = new LBVirtualFileSystem();
  }

  virtual void TearDown() OVERRIDE {
    delete vfs_;
  }

  LBVirtualFileSystem *vfs_;
};

TEST_F(VirtualFileSystemTest, WriteAndRead) {
  LBVirtualFile *file = vfs_->Open("file1.tmp");
  ASSERT_TRUE(file != NULL);
  EXPECT_EQ(0, file->Size());

  int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  file->Write(expected, sizeof(expected), 0);

  char out_data[128];
  int bytes = file->Read(out_data, sizeof(out_data), 0);
  EXPECT_EQ(bytes, sizeof(expected));
  EXPECT_EQ(0, memcmp(expected, out_data, sizeof(expected)));
}

TEST_F(VirtualFileSystemTest, ReadWriteOffsets) {
  LBVirtualFile *file = vfs_->Open("file1.tmp");
  ASSERT_TRUE(file != NULL);
  EXPECT_EQ(0, file->Size());

  char expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  file->Write(expected, sizeof(expected), 0);

  // Seek to position 4 and read bytes.
  char out_data[128];
  int bytes = file->Read(out_data, sizeof(out_data), 4);

  const int kDataSize = 6;
  EXPECT_EQ(kDataSize, bytes);
  EXPECT_EQ(0, memcmp(&expected[4], out_data, kDataSize));

  // Write some bytes into the middle
  file->Write(expected, 3, 3);
  char expected1[] = { 0, 1, 2, 0, 1, 2, 6, 7, 8, 9 };
  EXPECT_EQ(sizeof(expected1), file->Size());

  bytes = file->Read(out_data, sizeof(out_data), 0);
  EXPECT_EQ(0, memcmp(expected1, out_data, bytes));

  // Try to read past the end.
  const int kOffsetPastEnd = 100000;
  bytes = file->Read(out_data, sizeof(out_data), kOffsetPastEnd);
  EXPECT_EQ(0, bytes);

  // Try to write past the end.
  file->Write(expected, sizeof(expected), kOffsetPastEnd);
  EXPECT_EQ(kOffsetPastEnd + sizeof(expected), file->Size());

  // Make sure the write past the end worked.
  bytes = file->Read(out_data, sizeof(out_data), kOffsetPastEnd);
  EXPECT_EQ(0, memcmp(expected, out_data, bytes));
}

TEST_F(VirtualFileSystemTest, Open) {
  // Create a few files and write some data
  LBVirtualFile *file = vfs_->Open("file1.tmp");
  ASSERT_TRUE(file != NULL);
  EXPECT_EQ(0, file->Size());

  // Write a few bytes of random data.
  file->Write("test", 4, 0);
  EXPECT_EQ(4, file->Size());

  // Open a new file
  file = vfs_->Open("file2.tmp");
  ASSERT_TRUE(file != NULL);
  EXPECT_EQ(0, file->Size());

  // Try to reopen the existing file.
  file = vfs_->Open("file1.tmp");
  ASSERT_TRUE(file != NULL);
  EXPECT_EQ(4, file->Size());
}

TEST_F(VirtualFileSystemTest, SerializeDeserialize) {
  // Create a few files and write some data
  LBVirtualFile *file = vfs_->Open("file1.tmp");
  EXPECT_TRUE(file != NULL);
  const char *data1 = "abc";
  int data1_size = 3;
  file->Write(data1, data1_size, 0);

  file = vfs_->Open("file2.tmp");
  EXPECT_TRUE(file != NULL);
  const char *data2 = "defg";
  int data2_size = 4;
  file->Write(data2, data2_size, 0);

  // First perform a dry run to figure out how much space we need.
  int bytes = vfs_->Serialize(NULL, true /*dry run*/);
  char *buffer = new char[bytes];

  // Now serialize and deserialize
  vfs_->Serialize(buffer, false /*dry run*/);

  // Deserialize the data into a new vfs
  LBVirtualFileSystem new_vfs;
  new_vfs.Deserialize(buffer);

  // Make sure the new vfs contains all the expected data.
  char file_contents[128];
  file = new_vfs.Open("file1.tmp");
  EXPECT_TRUE(file != NULL);
  bytes = file->Read(file_contents, sizeof(file_contents), 0);
  EXPECT_EQ(data1_size, bytes);
  EXPECT_EQ(0, memcmp(file_contents, data1, bytes));

  file = new_vfs.Open("file2.tmp");
  EXPECT_TRUE(file != NULL);
  bytes = file->Read(file_contents, sizeof(file_contents), 0);
  EXPECT_EQ(data2_size, bytes);
  EXPECT_EQ(0, memcmp(file_contents, data2, bytes));

  delete [] buffer;
}
#endif  // __LB_PS4__
