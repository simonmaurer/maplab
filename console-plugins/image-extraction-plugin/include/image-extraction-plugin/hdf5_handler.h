//
// Created by Simon Maurer on 25.04.18.
//

#ifndef IMAGE_EXTRACTION_PLUGIN_HDF5_HANDLER_H_
#define IMAGE_EXTRACTION_PLUGIN_HDF5_HANDLER_H_

#include <string>
#include <vector>

#include <opencv2/core.hpp>

namespace image_extraction_plugin {

class H5Object {
 protected:
  // Data written to HDF5
  // std::vector <cv::Mat> data;
  cv::Mat all_data;

  // Internal variables
  std::string dir_path;
  int file_counter = 0;
  int split_size = -1;  // Default: dont split into ultiple .h5/.hdf5 files

 public:
  explicit H5Object(const std::string& dir_path, const int& split_size = -1);
  void add(const cv::Mat& mat);

  void reset();

  virtual void write(const std::string& dir_path) const = 0;
};

class H5ImageObject : public H5Object {
 public:
  explicit H5ImageObject(
      const std::string& dir_path, const int& split_size = -1);
  virtual bool write(const std::string& dir_path);
};

}  // namespace image_extraction_plugin

#endif  // IMAGE_EXTRACTION_PLUGIN_HDF5_HANDLER_H_
