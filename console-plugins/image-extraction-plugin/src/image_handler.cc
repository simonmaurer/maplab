//
// Created by Simon Maurer on 25.04.18.
//

#include "image-extraction-plugin/image_handler.h"

#include <string>
#include <vector>

ImageWriter::ImageWriter(const std::string &train_dir,
                         const std::string &validation_dir) :
    train_dir(train_dir), val_dir(validation_dir) {}

void ImageWriter::extract(const std::string &dir_path,
                          const std::vector<cv::Mat> &images) const {}
