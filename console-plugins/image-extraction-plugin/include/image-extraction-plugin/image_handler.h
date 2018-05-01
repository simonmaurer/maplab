//
// Created by Simon Maurer on 23.04.18.
//

#ifndef IMAGE_EXTRACTION_PLUGIN_IMAGE_HANDLER_H_
#define IMAGE_EXTRACTION_PLUGIN_IMAGE_HANDLER_H_

#include <string>
#include <vector>

#include <opencv2/core.hpp>

#include <boost/filesystem.hpp>

class ImageWriter {
 private:
  const std::string train_dir;
  const std::string val_dir;

 public:
  explicit ImageWriter(
      const std::string& train_dir = "training_set",
      const std::string& val_dir = "validation_set");

  virtual void extract(
      const std::string& dir_path,
      const std::vector<cv::Mat>& images) const = 0;
};

#endif  // IMAGE_EXTRACTION_PLUGIN_IMAGE_HANDLER_H_
