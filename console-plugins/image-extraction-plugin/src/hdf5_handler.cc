//
// Created by Simon Maurer on 25.04.18.
//

#include "image-extraction-plugin/hdf5_handler.h"

#include <opencv2/hdf.hpp>

// using namespace H5;

namespace image_extraction_plugin {

// H5Object
H5Object::H5Object(const std::string& dir_path, const int& split_size)
    : split_size(split_size) {
  this->dir_path = dir_path;
  if (this->dir_path.back() != '/') {
    this->dir_path += '/';
  }
}

void H5Object::add(const cv::Mat& mat) {
  this->all_data.push_back(mat);
}

void H5Object::reset() {
  this->all_data = cv::Mat();
  this->file_counter++;
}

// H5ImageObject
H5ImageObject::H5ImageObject(const std::string& dir_path, const int& split_size)
    : H5Object(dir_path, split_size) {}
bool H5ImageObject::write(const std::string& dir_path) {
  std::string output_pat = dir_path + "/" + std::to_string(this->file_counter);

  return true;
}

}  // namespace image_extraction_plugin
