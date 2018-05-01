//
// Created by Simon Maurer on 19.04.18.
//

#include "image-extraction-plugin/image-extraction-plugin.h"

#include <iostream>
#include <limits>
#include <string>

#include <algorithm>
#include <iterator>
#include <random>

#include <console-common/console-plugin-base.h>
#include <map-manager/map-manager.h>
#include <vi-map/vertex.h>
#include <vi-map/vi-map.h>
// #include <vi-map/vertex-inl.h>

#include <opencv2/core.hpp>
// #include <hdf5>
// #include "H5Cpp.h"

//// Definition of flags for commands
// supported by extract_images/extract_patches
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
    "keep original image size = -1, 1-std::numeric_limits<int>::max()");
DEFINE_int32(
    ie_num_images, -1,
    "Number of images to extract per map"
    "Supported commands: "
    "extract_images "
    "Supported number of images: "
    "extract all images = -1, 1-number of vertices per map");

// only supported by extract_patches
DEFINE_int32(
    ie_patchsize, 64,
    "Patch size [px]"
    "Supported commands: "
    "extract_patches "
    "Supported patch sizes: "
    "0-std::numeric_limits<int>::max()");
DEFINE_uint64(
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
  if (!validateGeneralFlags()) {
    return common::kStupidUserError;
  }
  if (!validateImageFlags()) {
    return common::kStupidUserError;
  }
  std::cout << "Image extraction in progress.." << std::endl;

  vi_map::VIMapManager map_manager;
  vi_map::VIMapManager::MapReadAccess map =
      map_manager.getMapReadAccess(selected_map_key);
  processPatches(map);  //???

  pose_graph::VertexIdList vertex_idx;
  map->getAllVertexIds(&vertex_idx);
  const int num_vertices = vertex_idx.size();
  if (FLAGS_ie_num_images > num_vertices) {
    LOG(ERROR) << "--ie_num_images, the number of images"
                  " specified exceeds the number of vertices!";
    return common::kStupidUserError;
  }

  pose_graph::VertexIdList vertex_idx_extracted;
  int num_images_to_extract = FLAGS_ie_num_images;
  if (FLAGS_ie_num_images == -1) {
    num_images_to_extract = num_vertices;
  }
  const pose_graph::VertexIdList::iterator copy_iterator =
      vertex_idx.begin() + num_images_to_extract;
  for (auto it = vertex_idx.begin(); it != copy_iterator; ++it) {
    vertex_idx_extracted.push_back(*it);
  }

  std::vector<cv::Mat> images;
  const unsigned int frame_id = 0;  // id of camera frame
  for (const pose_graph::VertexId id : vertex_idx_extracted) {
    const vi_map::Vertex& vertex = map->getVertex(id);
    cv::Mat image;
    cv::Mat_<float> l;
    if (FLAGS_ie_greyscale) {
      // frame_id
      map->getRawImage(vertex, frame_id, &l);
    } else {
      map->getRawColorImage(vertex, frame_id, &image);
    }
    images.push_back(image);
  }
  std::cout << "Extracted " << images.size()
            << " images from map: " << selected_map_key << std::endl;

  // ToDo load associated images of map (greyscale/RGB) and start
  /*
  vi_map::LandmarkIdList landmark_ids;
  map->getAllLandmarkIds(&landmark_ids);
  std::cout << "# total landmarks in total: " << landmark_ids.size() <<
  std::endl;
  std::vector<std::string> map_keys;
  map_manager.getAllMapKeys(&map_keys);
  for (const std::string map_key : map_keys) {
      std::cout << "map key: " << map_key << std::endl;
  }*/

  std::string map_path;
  map_manager.getMapFolder(selected_map_key, &map_path);
  std::cout << "Saved images to folder: " << map_path << std::endl;

  // Other commonly used return values are common::kUnknownError and
  // common::kStupidUserError.
  return common::kSuccess;
}

int ImageExtractionPlugin::extractPatches() const {
  std::string selected_map_key;
  if (!getSelectedMapKeyIfSet(&selected_map_key)) {
    return common::kStupidUserError;
  }
  // H5::H5File hh("lol", H5F_ACC_RDWR);

  if (!validateGeneralFlags()) {
    return common::kStupidUserError;
  }
  if (!validatePatchFlags()) {
    return common::kStupidUserError;
  }
  std::cout << "Patch extraction in progress.." << std::endl;

  vi_map::VIMapManager map_manager;
  const vi_map::VIMapManager::MapReadAccess map =
      map_manager.getMapReadAccess(selected_map_key);
  // processPatches(map);
  // vi_map_helpers::VIMapQueries queries(*map);
  vi_map::Vertex v;
  vi_map::Landmark l;

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

bool ImageExtractionPlugin::validateGeneralFlags() const {
  if (FLAGS_ie_trainval_ratio < 0.0 || FLAGS_ie_trainval_ratio > 1.0) {
    LOG(ERROR) << "Invalid value for parameter, please use values"
                  "in range [0.0, 1.0]";
    return false;
  }
  return true;
}

bool ImageExtractionPlugin::validateImageFlags() const {
  if (FLAGS_ie_num_images < -1 ||
      FLAGS_ie_num_images > std::numeric_limits<int>::max()) {
    LOG(ERROR) << "Invalid value for parameter, please use -1 to extract all"
                  "images or parameter range[std::numeric_limits<int>::max()]";
    return false;
  }
  if (FLAGS_ie_imagesize < -1 ||
      FLAGS_ie_imagesize > std::numeric_limits<int>::max()) {
    LOG(ERROR) << "Invalid value for parameter, please use -1 to keep original"
                  " size or parameter range [std::numeric_limits<int>::max()]"
                  " for resizing to square image, e.g. 224x224";
    return false;
  }

  return true;
}

bool ImageExtractionPlugin::validatePatchFlags() const {
  return true;
}

bool ImageExtractionPlugin::processPatches(
    const vi_map::VIMapManager::MapReadAccess& map) const {
  vi_map::LandmarkIdList landmark_ids;
  map->getAllLandmarkIds(&landmark_ids);
  const size_t num_map_landmarks = landmark_ids.size();
  std::cout << num_map_landmarks << std::endl;
  if (FLAGS_ie_num_landmarks_per_map > num_map_landmarks) {
    LOG(ERROR) << "--ie_num_landmarks_per_map, the number of landmarks per map"
                  " specified exceeds the number of landmarks of the map!";
    return false;
  }

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
