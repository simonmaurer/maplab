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
  int num_data = 0;
  int img_channels = 1;
  int img_rows = 0;
  int img_cols = 0;
  const int NUM_DATA_DIMS = 4;

  // Internal variables
  std::string dir_path;
  std::string current_output_path;
  std::string file_ending;
  int file_counter = 0;
  int split_size = -1;  // Default: dont split into multiple .h5/.hdf5 files

  void setOutputPath();
  void writeHeader() const;

 public:
  // Serialization definition
  static const std::string DIMS;
  static const std::string NUM_DATA;
  static const std::string DATA;

  explicit H5Object(
      const std::string& dir_path, const int& split_size = -1,
      const std::string& file_ending = ".h5");
  bool add(const cv::Mat& mat);

  void reset();

  virtual bool write() const = 0;
};

class H5ImageObject : public H5Object {
 protected:
  std::vector<std::vector<cv::KeyPoint>> all_keypoints;

 public:
  static const std::string KEYPOINTS;

  explicit H5ImageObject(
      const std::string& dir_path, const int& split_size = -1,
      const std::string& file_ending = ".h5");
  void addKeypoints(const std::vector<cv::KeyPoint>& keypoints);
  virtual bool write() const;
};

}  // namespace image_extraction_plugin

#endif  // IMAGE_EXTRACTION_PLUGIN_HDF5_HANDLER_H_
