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

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
// #include <hdf5>
// #include "H5Cpp.h"

namespace fs = boost::filesystem;

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
    "keep original image size = -1, 1 - std::numeric_limits<int>::max()");
DEFINE_int32(
    ie_num_images, -1,
    "Number of images to extract per map"
    "Supported commands: "
    "extract_images "
    "Supported number of images: "
    "extract all images = -1, 1 - <number of vertices per map>");

// only supported by extract_patches
DEFINE_int32(
    ie_patchsize, 64,
    "Patch size [px]"
    "Supported commands: "
    "extract_patches "
    "Supported patch sizes: "
    "1 - 1024");
DEFINE_uint64(
    ie_num_landmarks_per_map, 100,
    "Number of landmarks/3d points per map to extract corresponding image "
    "patches from"
    "Supported commands: "
    "extract_patches "
    "Supported number of landmarks: "
    "1 - <number of landmarks per map>");
DEFINE_int32(
    ie_num_samples_per_landmark, 8,
    "Number of patch pairs/triplets per observed landmark"
    "Supported commands: "
    "extract_patches "
    "Supported number of samples per landmark: "
    "1 - std::numeric_limits<int>::max()");

namespace image_extraction_plugin {

ImageExtractionPlugin::ImageExtractionPlugin(common::Console* console)
    : common::ConsolePluginBase(console) {
  addCommand(
      {"extract_images", "ei"}, [this]() -> int { return extractImages(); },
      "This command extracts images or patches of matching/non-matching "
      "keypoint "
      "pairs/triplets corresponding to same 3d points of a loaded map. "
      "Parameters "
      "are: --mode/-M {images, pairs, triplets}, --output_dir/-O, "
      "--patch_size/-P",
      common::Processing::Sync);
  addCommand(
      {"extract_patches", "ep"}, [this]() -> int { return extractPatches(); },
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
  vi_map::VIMapManager map_manager;
  std::string selected_map_key;
  if (!getSelectedMapKeyIfSet(&selected_map_key)) {
    return common::kStupidUserError;
  }
  if (!validateGeneralFlags(map_manager, selected_map_key)) {
    return common::kStupidUserError;
  }
  if (!validateImageFlags()) {
    return common::kStupidUserError;
  }
  std::cout << "Image extraction in progress.." << std::endl;

  vi_map::VIMapManager::MapReadAccess map =
      map_manager.getMapReadAccess(selected_map_key);
  // processPatches(map);  //???

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
  for (const pose_graph::VertexId& id : vertex_idx_extracted) {
    const vi_map::Vertex& vertex = map->getVertex(id);
    cv::Mat image;
    if (FLAGS_ie_greyscale) {
      // frame_id
      map->getRawImage(vertex, frame_id, &image);
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
  vi_map::VIMapManager map_manager;
  std::string selected_map_key;
  if (!getSelectedMapKeyIfSet(&selected_map_key)) {
    return common::kStupidUserError;
  }
  // H5::H5File hh("lol", H5F_ACC_RDWR);

  if (!validateGeneralFlags(map_manager, selected_map_key)) {
    return common::kStupidUserError;
  }
  if (!validatePatchFlags()) {
    return common::kStupidUserError;
  }

  std::cout << "Patch extraction in progress.." << std::endl;

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

// Validate input flags (value range)
bool ImageExtractionPlugin::validateGeneralFlags(
    const vi_map::VIMapManager& map_manager, const std::string& map_key) const {
  if (FLAGS_ie_trainval_ratio < 0.0 || FLAGS_ie_trainval_ratio > 1.0) {
    LOG(ERROR) << "Invalid value for parameter, please use values"
                  "in range [0.0, 1.0]";
    return false;
  }

  std::string output_path;
  if (FLAGS_ie_output_dir == "") {
    map_manager.getMapFolder(map_key, &output_path);
  } else {
    output_path = FLAGS_ie_output_dir;
  }
  const fs::path output_dir(output_path);
  /*if (!fs::is_directory(output_dir)) {
    // LOG(ERROR) << "Invalid value for parameter --ie_output_dir, "
              //    << output_path << " is not a valid directory.";
    return false;
  }*/
  std::cout << "output directory: " << FLAGS_ie_output_dir << std::endl;
  std::cout << "extract greyscale: " << std::boolalpha << FLAGS_ie_greyscale
            << std::endl;
  std::cout << "training/validation set ratio: " << FLAGS_ie_trainval_ratio
            << std::endl;

  return true;
}

bool ImageExtractionPlugin::validateImageFlags() const {
  if (FLAGS_ie_num_images < -1 || FLAGS_ie_num_images == 0 ||
      FLAGS_ie_num_images > std::numeric_limits<int>::max()) {
    LOG(ERROR)
        << "Invalid value for parameter, please use -1 to extract all"
           "images or parameter range: [1, std::numeric_limits<int>::max()]";
    return false;
  }
  if (FLAGS_ie_imagesize < -1 || FLAGS_ie_num_images == 0 ||
      FLAGS_ie_imagesize > std::numeric_limits<int>::max()) {
    LOG(ERROR) << "Invalid value for parameter, please use -1 to keep original"
                  " size or parameter range [std::numeric_limits<int>::max()]"
                  " for resizing to square image, e.g. 224x224";
    return false;
  }

  if (FLAGS_ie_num_images == -1) {
    std::cout << "number of images to extract: all" << std::endl;
  } else {
    std::cout << "number of images to extract: " << FLAGS_ie_num_images
              << std::endl;
  }
  if (FLAGS_ie_imagesize == -1) {
    std::cout << "target image size: keep original size" << std::endl;
  } else {
    std::cout << "target image size: " << FLAGS_ie_imagesize << "x"
              << FLAGS_ie_imagesize << std::endl;
  }

  return true;
}

bool ImageExtractionPlugin::validatePatchFlags() const {
  if (FLAGS_ie_patchsize < 1 || FLAGS_ie_patchsize > 1024) {
    LOG(ERROR) << "Invalid value for parameter, please use a value from"
                  "parameter range: [1, 1024]";
    return false;
  }
  if (FLAGS_ie_num_landmarks_per_map < 1 ||
      FLAGS_ie_num_landmarks_per_map > std::numeric_limits<uint64_t>::max()) {
    LOG(ERROR) << "Invalid value for parameter, please use a value from"
                  " parameter range [1, std::numeric_limits<uint64_t>::max()]";
    return false;
  }
  if (FLAGS_ie_num_samples_per_landmark < 1 ||
      FLAGS_ie_num_samples_per_landmark > std::numeric_limits<int>::max()) {
    LOG(ERROR) << "Invalid value for parameter, please use a value from"
                  " parameter range [1, std::numeric_limits<int>::max()]";
    return false;
  }

  std::cout << "target patch size: " << FLAGS_ie_patchsize << std::endl;
  std::cout << "number of landmarks to extract patches from: "
            << FLAGS_ie_num_landmarks_per_map << std::endl;
  std::cout << "minimum number of samples a landmark has been observed from: "
            << FLAGS_ie_num_samples_per_landmark << std::endl;

  return true;
}

// Delegation of image/patch extraction
bool ImageExtractionPlugin::processPatches(
    const vi_map::VIMapManager::MapReadAccess& map) const {
  vi_map::LandmarkIdList landmark_ids;
  map->getAllLandmarkIds(&landmark_ids);
  const size_t num_map_landmarks = landmark_ids.size();
  std::cout << num_map_landmarks << std::endl;
  if (FLAGS_ie_num_landmarks_per_map > num_map_landmarks) {
    LOG(ERROR)
        << "--ie_num_landmarks_per_map, the specified number of landmarks"
           " exceeds the number of landmarks of the map!";
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
