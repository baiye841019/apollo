/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "modules/perception/integration_tests/perception_test_base.h"

#include <cstdlib>

#include "pcl/io/pcd_io.h"
#include "pcl_conversions/pcl_conversions.h"

#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "modules/common/vehicle_state/vehicle_state_provider.h"
#include "modules/perception/common/pcl_types.h"

namespace apollo {
namespace perception {

using common::adapter::AdapterManager;

DEFINE_string(test_data_dir, "", "the test data folder");
DEFINE_string(test_pointcloud_file, "", "The pointcloud file used in test");
DEFINE_string(test_localization_file, "", "The localization test file");
DEFINE_string(test_chassis_file, "", "The chassis test file");
DEFINE_string(perception_config_file, "", "The perception config file");

void PerceptionTestBase::SetUpTestCase() {
  FLAGS_perception_config_file =
      "modules/perception/integration_tests/testdata/conf/"
      "perception_config.pb.txt";
  FLAGS_perception_adapter_config_filename =
      "modules/perception/integration_tests/testdata/conf/adapter.conf";

  FLAGS_dag_config_path =
      "./integration_tests/testdata/conf/dag_streaming.config";

  FLAGS_test_pointcloud_file =
      "modules/perception/integration_tests/testdata/point_cloud_test_file.pcd";

  FLAGS_onboard_roi_filter = "HdmapROIFilter";
  FLAGS_onboard_segmentor = "CNNSegmentation";

  FLAGS_test_localization_file = "";
  FLAGS_test_chassis_file = "";
}

#define FEED_DATA_TO_ADAPTER(TYPE, DATA)                              \
  if (!AdapterManager::Get##TYPE()) {                                 \
    AERROR << #TYPE                                                   \
        " is not registered in adapter manager, check adapter file "; \
    return false;                                                     \
  }                                                                   \
  AdapterManager::Feed##TYPE##Data(DATA);

#define FEED_FILE_TO_ADAPTER(TYPE, FILENAME)                                   \
  if (!AdapterManager::Get##TYPE()) {                                          \
    AERROR << #TYPE                                                            \
        " is not registered in adapter manager, check adapter file "           \
           << FLAGS_perception_adapter_config_filename;                        \
    return false;                                                              \
  }                                                                            \
  if (!FILENAME.empty()) {                                                     \
    if (!AdapterManager::Feed##TYPE##File(FLAGS_test_data_dir + "/" +          \
                                          FILENAME)) {                         \
      AERROR << "Failed to feed " #TYPE " file " << FLAGS_test_data_dir << "/" \
             << FILENAME;                                                      \
      return false;                                                            \
    }                                                                          \
    AINFO << "Using " #TYPE << " provided by " << FLAGS_test_data_dir << "/"   \
          << FILENAME;                                                         \
  }

bool PerceptionTestBase::SetUpAdapters() {
  if (!AdapterManager::Initialized()) {
    AdapterManager::Init(FLAGS_perception_adapter_config_filename);
  }

  FEED_DATA_TO_ADAPTER(PointCloud, cloud_blob_);
  FEED_FILE_TO_ADAPTER(Localization, FLAGS_test_localization_file);
  FEED_FILE_TO_ADAPTER(Chassis, FLAGS_test_chassis_file);
  return true;
}

void PerceptionTestBase::SetUp() {
  SetUpTestCase();

  // load PCD file and transfer to point cloud
  pcl::PCLPointCloud2 pcl_pointcloud;
  CHECK_NE(-1, pcl::io::loadPCDFile(FLAGS_test_pointcloud_file, pcl_pointcloud))
      << FLAGS_test_pointcloud_file;

  pcl_conversions::fromPCL(pcl_pointcloud, cloud_blob_);

  CHECK(SetUpAdapters()) << "Failed to setup adapters";
  perception_.reset(new Perception);
  CHECK(perception_->Init().ok()) << "Failed to init perception module";
}

void PerceptionTestBase::UpdateData() {
  CHECK(SetUpAdapters()) << "Failed to setup adapters";
}

bool PerceptionTestBase::RunPerception(const std::string& test_case_name,
                                       int case_num) {
  if (perception_->Start() != common::Status::OK()) {
    AERROR << "Failed to start perception.";
    return false;
  }
  perception_->Stop();
  // TODO(All): Finish implementation of RunPerception here.
  return true;
}

}  // namespace perception
}  // namespace apollo
