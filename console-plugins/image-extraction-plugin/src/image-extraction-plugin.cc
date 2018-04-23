//
// Created by Simon Maurer on 19.04.18.
//

#include "image-extraction-plugin/image-extraction-plugin.h"

#include <iostream>
#include <string>

#include <console-common/console-plugin-base.h>
#include <map-manager/map-manager.h>
#include <vi-map/vi-map.h>

#include <opencv2/core.hpp>

//// Definition of flags for commands
// global
DEFINE_string(
    image_extraction_output_dir, "",
    "Path to the output directory where the images or .hdf5 files shall be "
    "exported. Defaults to input map directory");

DEFINE_bool(
    image_extraction_greyscale, false,
    "Whether to extract images/patches as RGB or greyscale");

DEFINE_double(
    image_extraction_trainval_ratio, 1.0,
    "Training vs validation data ratio for the dataset split."
    "Supported range: "
    "[0.0, 1.0], a value of 1.0 corresponds to outputting data to the "
    "training set only");

// only supported by extract_images
DEFINE_int32(
    image_extraction_imagesize, -1,
    "Image size [px]"
    "Supported commands: "
    "extract_images "
    "Supported patch sizes: "
    "keep original image size = -1, 0-std::numeric_limits<int>::max()");
DEFINE_int32(
    image_extraction_num_images, 200,
    "Number of images to extract per map"
    "Supported commands: "
    "extract_images "
    "Supported number of images: "
    "1 - number of vertices per map");

// only supported by extract_patches
DEFINE_int32(
    image_extraction_patchsize, 64,
    "Patch size [px]"
    "Supported commands: "
    "extract_patches "
    "Supported patch sizes: "
    "0-std::numeric_limits<int>::max()");
DEFINE_int32(
    image_extraction_num_landmarks_per_map, 64,
    "Number of landmarks/3d points per map to extract corresponding image "
    "patches from"
    "Supported commands: "
    "extract_patches "
    "Supported number of landmarks: "
    "1 - number of landmarks per map");
DEFINE_int32(
    image_extraction_num_samples_per_landmark, 8,
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
  // This function will write the name of the selected map key into
  // selected_map_key. The function will return false and print an error
  // message if no map key is selected.
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

  // Other commonly used return values are common::kUnknownError and
  // common::kStupidUserError.
  return common::kSuccess;
}

int ImageExtractionPlugin::extractPatches() const {
  std::string selected_map_key;
  // This function will write the name of the selected map key into
  // selected_map_key. The function will return false and print an error
  // message if no map key is selected.
  if (!getSelectedMapKeyIfSet(&selected_map_key)) {
    return common::kStupidUserError;
  }

  std::cout << "Patch extraction in progress.." << std::endl;

  vi_map::VIMapManager map_manager;
  vi_map::VIMapManager::MapReadAccess map =
      map_manager.getMapReadAccess(selected_map_key);
  // ToDo load associated images of map (greyscale/RGB) and start
  // extraction
  std::cout << map->numMissions() << std::endl;

  std::string map_path;
  map_manager.getMapFolder(selected_map_key, &map_path);
  std::cout << "map path: " << map_path << std::endl;

  // Other commonly used return values are common::kUnknownError and
  // common::kStupidUserError.
  return common::kSuccess;
}

checkPreconditions() const {}

}  // namespace image_extraction_plugin

MAPLAB_CREATE_CONSOLE_PLUGIN(image_extraction_plugin::ImageExtractionPlugin);
