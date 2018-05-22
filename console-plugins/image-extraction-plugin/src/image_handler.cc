//
// Created by Simon Maurer on 25.04.18.
//

#include "image-extraction-plugin/image_handler.h"

#include <string>
#include <vector>

#include <opencv2/highgui.hpp>
#include <opencv2/hdf.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <image-extraction-plugin/hdf5_handler.h>

namespace fs = boost::filesystem;

namespace image_extraction_plugin {

// static const variables for MODE
const std::string PlainImageExtractor::MODE = "plain";
const std::string H5ImageExtractor::MODE = "hdf5";

// ImageExtractor
ImageExtractor::ImageExtractor(const bool& greyscale, const cv::Size& img_size)
    : greyscale(greyscale), img_size(img_size) {}

std::string ImageExtractor::getName() const {
  return "image_extraction_";
}

void ImageExtractor::resize(cv::Mat* image) const {
  if (this->img_size.height > -1 && this->img_size.width > -1) {
    cv::resize(*image, *image, this->img_size);
  }
}

// PlainImageExtractor
PlainImageExtractor::PlainImageExtractor(
    const bool& greyscale, const cv::Size& img_size)
    : ImageExtractor(greyscale, img_size) {}

void PlainImageExtractor::extract(
    const vi_map::VIMapManager::MapReadAccess& map,
    const pose_graph::VertexIdList& vertex_idx, const std::string& out_path) {
  bool success = false;
  fs::path parent_dir(out_path);

  const unsigned int frame_id = 0;  // id of camera frame
  int img_id = 0;
  for (const pose_graph::VertexId& id : vertex_idx) {
    const vi_map::Vertex& vertex = map->getVertex(id);
    cv::Mat image;
    if (this->greyscale) {
      // frame_id
      map->getRawImage(vertex, frame_id, &image);
    } else {
      map->getRawColorImage(vertex, frame_id, &image);
    }
    if (image.rows <= 0) {
      LOG(ERROR) << "Could not extract image of vertex"
                    " with id "
                 << id << ", resources missing? - "
                          "check with res_stats!";
      // return false;
      continue;
    }
    this->resize(&image);

    fs::path out_image(out_path);
    out_image.append("img_" + std::to_string(img_id) + this->getFileEnding());
    cv::imwrite(out_image.string(), image);
    img_id++;
    std::cout << "Processing vertex id: " << id << std::endl;
  }
  std::cout << "Extracted " << img_id << " images to: " << parent_dir.string()
            << std::endl;
  success = true;

  // return success;
}

std::string PlainImageExtractor::getName() const {
  return ImageExtractor::getName() + "plain_";
}
std::string PlainImageExtractor::getFileEnding() const {
  return ".jpg";
}

// H5ImageExtractor
H5ImageExtractor::H5ImageExtractor(
    const bool& greyscale, const cv::Size& img_size)
    : ImageExtractor(greyscale, img_size) {
  this->detector = cv::FastFeatureDetector::create(20, true);
}

void H5ImageExtractor::extract(
    const vi_map::VIMapManager::MapReadAccess& map,
    const pose_graph::VertexIdList& vertex_idx, const std::string& out_path) {
  bool success = false;
  fs::path parent_dir(out_path);

  H5ImageObject h5(parent_dir.string());

  const unsigned int frame_id = 0;  // id of camera frame
  int img_id = 0;
  for (const pose_graph::VertexId& id : vertex_idx) {
    const vi_map::Vertex& vertex = map->getVertex(id);
    cv::Mat image;
    if (this->greyscale) {
      // frame_id
      map->getRawImage(vertex, frame_id, &image);
    } else {
      map->getRawColorImage(vertex, frame_id, &image);
    }
    if (image.rows <= 0) {
      LOG(ERROR) << "Could not extract image of vertex"
                    " with id " << id << ", resources missing? - "
                                         "check with res_stats!";
      // return false;
      continue;
    }
    this->resize(&image);

    if (img_id < 5) {
      h5.add(image);
      std::vector<cv::KeyPoint> kpts;
      detector->detect(image, kpts);
      h5.addKeypoints(kpts);
    }
    /*h5.add(image);
    std::vector<cv::KeyPoint> keypoints;
    this->detector->detect(image, keypoints);
    h5.addKeypoints(keypoints);*/

    img_id++;
    std::cout << "Processing vertex id: " << id << std::endl;
  }
  h5.write();

  std::cout << "Extracted images to: " << parent_dir.string() << std::endl;

  success = true;
  // return success;
}

std::string H5ImageExtractor::getName() const {
  return ImageExtractor::getName() + "hdf5_";
}
std::string H5ImageExtractor::getFileEnding() const {
  return ".h5";
}

}  // namespace image_extraction_plugin
