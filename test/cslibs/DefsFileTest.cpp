/**
 * @file DefsFileTest.cpp
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#include <gtest/gtest.h>
#include <cslibs/DefsFile.h>
#include <filesystem>

using namespace cslibs;

class DefsFileTest : public ::testing::Test {
 private:
  static inline auto kTestData = R"EOF(
IDENT 3:0
MANUFACTURER AVAB
CONSOLE PRONTO

$SOFTWAREVERSION V5.0 R0
! Gels File

CLEAR $GEL
$CARALLONVERSION 12.1.0
! 2016-05-17T16:19:28Z


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Gel Definitions
! $GEL Gel record start
! $$DCID Gel DCID
! $$GELMANUFACTURER Manufacturer,Range
! $$GELINFO IDNumber,Name,Red,Green,Blue

! Manufacturer Apollo
! Range apollo
! Range Gel

$GEL
$$DCID 6356B5B5-0127-2D47-AC1C-5AD540D7D7D9
$$GELMANUFACTURER Apollo,Gel
$$GELINFO 1050,Soft Diffusion,254,255,251

$GEL
$$DCID 4F5EC26C-D332-C146-8988-AEBA12B52916
$$GELMANUFACTURER Apollo,Gel
$$GELINFO 1100,Hard Diffusion,254,255,244

ENDDATA
  )EOF";

 protected:
  std::filesystem::path data_file_path_;

  void SetUp() override {
    const std::string file_name(std::tmpnam(nullptr));
    data_file_path_ = std::filesystem::temp_directory_path() / file_name;
    std::ofstream data_file(data_file_path_);
    data_file << kTestData;
    data_file.close();
  }

  void TearDown() override {
    std::filesystem::remove(data_file_path_);
  }
};

TEST_F(DefsFileTest, TestLoad) {
  DefsFile defs_file(data_file_path_);
  const DefsFile::Version expected_version(12, 1, 0);
  EXPECT_EQ(expected_version, defs_file.GetVersion());

  // Check records
  std::vector<DefsFile::Def> records;
  while (auto record = defs_file.GetNextRecord()) {
    records.push_back(record.value());
  }
  ASSERT_EQ(records.size(), 2);

  // AP1050
  EXPECT_EQ(records[0].record_name, "GEL");
  EXPECT_EQ(records[0].contents.size(), 3);
  EXPECT_EQ(records[0].contents["DCID"], "6356B5B5-0127-2D47-AC1C-5AD540D7D7D9");
  EXPECT_EQ(records[0].contents["GELMANUFACTURER"], "Apollo,Gel");
  EXPECT_EQ(records[0].contents["GELINFO"], "1050,Soft Diffusion,254,255,251");

  // AP1100
  EXPECT_EQ(records[1].record_name, "GEL");
  EXPECT_EQ(records[1].contents.size(), 3);
  EXPECT_EQ(records[1].contents["DCID"], "4F5EC26C-D332-C146-8988-AEBA12B52916");
  EXPECT_EQ(records[1].contents["GELMANUFACTURER"], "Apollo,Gel");
  EXPECT_EQ(records[1].contents["GELINFO"], "1100,Hard Diffusion,254,255,244");
}
