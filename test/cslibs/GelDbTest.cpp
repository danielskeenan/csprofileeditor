/**
 * @file GelDbTest.cpp
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#include <gtest/gtest.h>
#include <cslibs/gel/GelDb.h>
#include <filesystem>

using namespace cslibs;

class GelDbTest : public ::testing::Test {
 private:
  static inline auto kDefsContents = R"EOF(
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
  std::filesystem::path defs_file_path_;
  std::filesystem::path db_path_;

  void SetUp() override {
    // Init defs file
    defs_file_path_ = std::filesystem::temp_directory_path() / std::tmpnam(nullptr);
    std::ofstream defs_file(defs_file_path_);
    defs_file << kDefsContents;
    defs_file.close();

    // Create db file
    db_path_ = std::filesystem::temp_directory_path() / std::tmpnam(nullptr);
  }

  void TearDown() override {
    std::filesystem::remove(defs_file_path_);
    std::filesystem::remove(db_path_);
  }

  gel::GelDb CreateDb(bool allow_writing = false) {
    return gel::GelDb(defs_file_path_.string(), db_path_.string(), allow_writing);
  }
};

TEST_F(GelDbTest, TestNew) {
  gel::GelDb gel_db = CreateDb(true);
  EXPECT_FALSE(gel_db.UpToDate());
  unsigned int last_progress = 0;
  gel_db.Update([&last_progress](unsigned long current, unsigned long total) {
    EXPECT_GT(current, last_progress);
    EXPECT_LE(current, total);
    last_progress = current;
  });
  EXPECT_TRUE(gel_db.UpToDate());
}

TEST_F(GelDbTest, TestGetManufacturers) {
  gel::GelDb gel_db = CreateDb(true);
  ASSERT_FALSE(gel_db.UpToDate());
  gel_db.Update();
  ASSERT_TRUE(gel_db.UpToDate());

  const std::vector<Manufacturer> expected_manufacturers{
      Manufacturer(1, "Apollo")
  };
  EXPECT_EQ(expected_manufacturers, gel_db.GetManufacturers());
}

TEST_F(GelDbTest, TestGetSeriesForManufacturer) {
  gel::GelDb gel_db = CreateDb(true);
  ASSERT_FALSE(gel_db.UpToDate());
  gel_db.Update();
  ASSERT_TRUE(gel_db.UpToDate());

  const std::vector<Manufacturer> expected_manufacturers{
      Manufacturer(1, "Apollo"),
  };
  EXPECT_EQ(expected_manufacturers, gel_db.GetManufacturers());

  const std::vector<Series> expected_series{
      Series(1, "Gel"),
  };
  EXPECT_EQ(expected_series, gel_db.GetSeriesForManufacturer(expected_manufacturers.front()));
}

TEST_F(GelDbTest, TestGetForSeries) {
  gel::GelDb gel_db = CreateDb(true);
  ASSERT_FALSE(gel_db.UpToDate());
  gel_db.Update();
  ASSERT_TRUE(gel_db.UpToDate());

  const std::vector<Manufacturer> expected_manufacturers{
      Manufacturer(1, "Apollo"),
  };
  EXPECT_EQ(expected_manufacturers, gel_db.GetManufacturers());

  const std::vector<Series> expected_series{
      Series(1, "Gel"),
  };
  EXPECT_EQ(expected_series, gel_db.GetSeriesForManufacturer(expected_manufacturers.front()));

  const std::vector<gel::Gel> expected_gels_code_order{
      gel::Gel("6356B5B5-0127-2D47-AC1C-5AD540D7D7D9",
               "1050",
               "Soft Diffusion",
               (0xFF << 24) | (254 << 16) | (255 << 8) | (251 << 0)),
      gel::Gel("4F5EC26C-D332-C146-8988-AEBA12B52916",
               "1100",
               "Hard Diffusion",
               (0xFF << 24) | (254 << 16) | (255 << 8) | (244 << 0)),
  };
  const std::vector<gel::Gel> expected_gels_name_order{
      gel::Gel("4F5EC26C-D332-C146-8988-AEBA12B52916",
               "1100",
               "Hard Diffusion",
               (0xFF << 24) | (254 << 16) | (255 << 8) | (244 << 0)),
      gel::Gel("6356B5B5-0127-2D47-AC1C-5AD540D7D7D9",
               "1050",
               "Soft Diffusion",
               (0xFF << 24) | (254 << 16) | (255 << 8) | (251 << 0)),
  };
  const std::vector<gel::Gel> expected_gels_color_order{
      gel::Gel("4F5EC26C-D332-C146-8988-AEBA12B52916",
               "1100",
               "Hard Diffusion",
               (0xFF << 24) | (254 << 16) | (255 << 8) | (244 << 0)),
      gel::Gel("6356B5B5-0127-2D47-AC1C-5AD540D7D7D9",
               "1050",
               "Soft Diffusion",
               (0xFF << 24) | (254 << 16) | (255 << 8) | (251 << 0)),
  };
  EXPECT_EQ(expected_gels_code_order, gel_db.GetGelForSeries(expected_series.front(), gel::GelDb::Sort::kCode));
  EXPECT_EQ(expected_gels_name_order, gel_db.GetGelForSeries(expected_series.front(), gel::GelDb::Sort::kName));
  EXPECT_EQ(expected_gels_color_order, gel_db.GetGelForSeries(expected_series.front(), gel::GelDb::Sort::kColor));
}
