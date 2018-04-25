//
// Created by Simon Maurer on 19.04.18.
//

#include "image-extraction-plugin/image-extraction-plugin.h"

#include <iostream>
#include <string>

#include <algorithm>
#include <iterator>
#include <random>

#include <console-common/console-plugin-base.h>
#include <map-manager/map-manager.h>
#include <vi-map/vi-map.h>

#include <opencv2/core.hpp>
// #include <hdf5>
// #include "H5Cpp.h"

//// Definition of flags for commands
// global
DEFINE_string(
    ie_output_dir, "",
    "Path to the output directory where the images or .hdf5 files shall be "
    "exported. Defaults to input map directory");

DEFINE_bool(
    ie_greyscale, false,
    "Whether to extract images/patches as RGB or greyscale");

DEFINE_double(
    ie_trainval_ratio, 1.0,
    "Training vs validation data ratio for the dataset split."
    "Supported range: "
    "[0.0, 1.0], a value of 1.0 corresponds to outputting data to the "
    "training set only");

// only supported by extract_images
DEFINE_int32(
    ie_imagesize, -1,
    "Image size [px]"
    "Supported commands: "
    "extract_images "
    "Supported patch sizes: "
    "keep original image size = -1, 0-std::numeric_limits<int>::max()");
DEFINE_int32(
    ie_num_images, 200,
    "Number of images to extract per map"
    "Supported commands: "
    "extract_images "
    "Supported number of images: "
    "1 - number of vertices per map");

// only supported by extract_patches
DEFINE_int32(
    ie_patchsize, 64,
    "Patch size [px]"
    "Supported commands: "
    "extract_patches "
    "Supported patch sizes: "
    "0-std::numeric_limits<int>::max()");
DEFINE_int64(
    ie_num_landmarks_per_map, 100,
    "Number of landmarks/3d points per map to extract corresponding image "
    "patches from"
    "Supported commands: "
    "extract_patches "
    "Supported number of landmarks: "
    "1 - number of landmarks per map");
DEFINE_int32(
    ie_num_samples_per_landmark, 8,
    "Number of patch pairs/triplets per observed landmark"
    "Supported commands: "
    "extract_patches "
    "Supported number of samples per landmark: "
    "1 - number of samples the landmark has been observed");

namespace image_extraction_plugin {

ImageExtractionPlugin::ImageExtractionPlugin(common::Console* console)
    : common::ConsolePluginBase(console) {
  addCommand(
      {"extract_images"}, [this]() -> int { return extractImages(); },
      "This command extracts images or patches of matching/non-matching "
      "keypoint "
      "pairs/triplets corresponding to same 3d points of a loaded map. "
      "Parameters "
      "are: --mode/-M {images, pairs, triplets}, --output_dir/-O, "
      "--patch_size/-P",
      common::Processing::Sync);
  addCommand(
      {"extract_patches"}, [this]() -> int { return extractPatches(); },
      "This command extracts images or patches of matching/non-matching "
      "keypoint "
      "pairs/triplets corresponding to same 3d points of a loaded map. "
      "Parameters "
      "are: --mode/-M {images, pairs, triplets}, --output_dir/-O, "
      "--patch_size/-P",
      common::Processing::Sync);
}

std::string ImageExtractionPlugin::getPluginId() const {
  return "image_extraction_plugin";
}

int ImageExtractionPlugin::extractImages() const {
  std::string selected_map_key;
  if (!getSelectedMapKeyIfSet(&selected_map_key)) {
    return common::kStupidUserError;
  }

  std::cout << "Image extraction in progress.." << std::endl;

  vi_map::VIMapManager map_manager;
  vi_map::VIMapManager::MapReadAccess map =
      map_manager.getMapReadAccess(selected_map_key);

  // ToDo load associated images of map (greyscale/RGB) and start
  // extraction
  vi_map::LandmarkIdList landmark_ids;
  map->getAllLandmarkIds(&landmark_ids);
  std::cout << "# landmarks in total: " << landmark_ids.size() << std::endl;
  std::vector<std::string> map_keys;
  map_manager.getAllMapKeys(&map_keys);
  for (const std::string map_key : map_keys) {
    std::cout << "map key: " << map_key << std::endl;
  }

  std::string map_path;
  map_manager.getMapFolder(selected_map_key, &map_path);
  std::cout << "map path: " << map_path << std::endl;

  int num_landmarks_per_mape = 0;
  std::cout << "num_landmarks_per_map FLAG: " << FLAGS_ie_num_landmarks_per_map
            << std::endl;

  // Other commonly used return values are common::kUnknownError and
  // common::kStupidUserError.
  return common::kSuccess;
}

int ImageExtractionPlugin::extractPatches() const {
  std::string selected_map_key;
  if (!getSelectedMapKeyIfSet(&selected_map_key)) {
    return common::kStupidUserError;
  }

  if (!checkPatchFlags()) {
    return common::kStupidUserError;
  }
  std::cout << "Patch extraction in progress.." << std::endl;

  vi_map::VIMapManager map_manager;
  const vi_map::VIMapManager::MapReadAccess map =
      map_manager.getMapReadAccess(selected_map_key);
  // processPatches(map);

  // Shuffle

  /*
          // ToDo load associated images of map (greyscale/RGB) and start
          // extraction
          std::cout << map->numMissions() << std::endl;

          std::string map_path;
          map_manager.getMapFolder(selected_map_key, &map_path);
          std::cout << "map path: " << map_path << std::endl;
  */
  // Other commonly used return values are common::kUnknownError and
  // common::kStupidUserError.
  return common::kSuccess;
}

bool ImageExtractionPlugin::checkImageFlags() const {
  return true;
}

bool ImageExtractionPlugin::checkPatchFlags() const {
  return true;
}

bool ImageExtractionPlugin::processPatches(
    const vi_map::VIMapManager::MapReadAccess map) {
  vi_map::LandmarkIdList landmark_ids;
  map->getAllLandmarkIds(&landmark_ids);
  const size_t num_map_landmarks = landmark_ids.size();
  std::cout << num_map_landmarks << std::endl;
  if (FLAGS_ie_num_landmarks_per_map > num_map_landmarks)
    return false;

  std::random_device rd;
  std::mt19937 generator(
      rd());  // which is why we define the method as non-const
  std::shuffle(landmark_ids.begin(), landmark_ids.end(), generator);
  vi_map::LandmarkIdList landmark_ids_extracted;
  const vi_map::LandmarkIdList::iterator copy_iterator =
      landmark_ids.begin() + FLAGS_ie_num_landmarks_per_map;
  // std::copy(landmark_ids.begin(), copy_iterator, landmark_ids_extracted);
  for (auto it = landmark_ids.begin(); it != copy_iterator; ++it) {
    landmark_ids_extracted.push_back(*it);
  }

  const size_t split_pos =
      FLAGS_ie_trainval_ratio * landmark_ids_extracted.size();
  vi_map::LandmarkIdList train_ids;
  vi_map::LandmarkIdList validation_ids;
  const vi_map::LandmarkIdList::iterator split_iterator =
      landmark_ids_extracted.begin() + split_pos;
  for (auto it = landmark_ids_extracted.begin();
       it != landmark_ids_extracted.end(); ++it) {
    if (std::distance(it, split_iterator) > 0) {
      train_ids.push_back(*it);
    } else {
      validation_ids.push_back(*it);
    }
  }

  return true;
}

}  // namespace image_extraction_plugin

MAPLAB_CREATE_CONSOLE_PLUGIN(image_extraction_plugin::ImageExtractionPlugin);
