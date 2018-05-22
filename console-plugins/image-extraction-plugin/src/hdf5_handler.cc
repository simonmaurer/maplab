//
// Created by Simon Maurer on 25.04.18.
//
/*
 * This header defines the object serialization and datasets,
 * please see its Python equivalent hdf5_handler.py in
 * project binary__nn__descriptors
 */

#include "image-extraction-plugin/hdf5_handler.h"

#include <iostream>

#include <opencv2/hdf.hpp>

// using namespace H5;

namespace image_extraction_plugin {

//// H5Object
// Serialization definition
const std::string H5Object::DIMS = "dims";
const std::string H5Object::NUM_DATA = "num_data";
const std::string H5Object::DATA = "/data";

const std::string H5ImageObject::KEYPOINTS =
    "/keypoints";  // used with keypointsX

H5Object::H5Object(
    const std::string& dir_path, const int& split_size,
    const std::string& file_ending)
    : split_size(split_size), file_ending(file_ending) {
  this->dir_path = dir_path;
  if (this->dir_path.back() != '/') {
    this->dir_path += '/';
  }

  setOutputPath();
}

void H5Object::setOutputPath() {
  if (split_size <= 0) {
    this->current_output_path = this->dir_path + "dataset" + this->file_ending;
  } else {
    this->current_output_path = this->dir_path + "dataset_" +
                                std::to_string(this->file_counter) +
                                this->file_ending;
  }
}

bool H5Object::add(const cv::Mat& mat) {
  // Assigning dimensions from first added cv::Mat
  if (this->num_data == 0) {
    this->img_channels = mat.channels();
    this->img_rows = mat.rows;
    this->img_cols = mat.cols;
    std::cout << "Dimension of data: " << this->img_channels << "x"
              << this->img_rows << "x" << this->img_cols << std::endl;
    // check if dimensions are equal for all data
  } else {
    if (this->img_channels != mat.channels() || this->img_rows != mat.rows ||
        this->img_cols != mat.cols) {
      std::cout << "Different dimensions in data!" << std::endl;
      return false;
    }
  }

  // this->data.push_back(mat);
  this->all_data.push_back(mat);
  this->num_data++;

  return true;
}

void H5Object::reset() {
  this->all_data = cv::Mat();
  this->file_counter++;
  setOutputPath();
}

void H5Object::writeHeader() const {
  cv::Ptr<cv::hdf::HDF5> h5_file = cv::hdf::open(this->current_output_path);

  // Header: dimensions
  cv::Mat dims;
  dims =
      (cv::Mat_<int>(1, 3) << this->img_channels, this->img_rows,
       this->img_cols);
  // std::cout << dims.at<int>(0,0) << std::endl;
  // std::cout << dims.at<int>(0,1) << std::endl;
  h5_file->dscreate(dims.rows, dims.cols, dims.type(), H5Object::DIMS);
  h5_file->dswrite(dims, H5Object::DIMS);
  // Header: sample information
  cv::Mat num_data;
  num_data = (cv::Mat_<int>(1, 1) << this->num_data);
  h5_file->dscreate(1, 1, CV_32S, H5Object::NUM_DATA);
  h5_file->dswrite(num_data, H5Object::NUM_DATA);
  h5_file->close();

  /*H5::H5File *f = new H5::H5File(this->current_output_path, H5F_ACC_TRUNC);

  hsize_t dim[1];
  dim[0] = 1;
  H5::DataSpace space(1, dim);
  H5::DataSet *dataset = new H5::DataSet(f->createDataSet(H5Object::NUM_DATA,
  H5::PredType::NATIVE_INT, space));
  int num_samples[1];
  num_samples[0] = this->data_counter;
  dataset->write(num_samples, H5::PredType::NATIVE_INT);
  delete dataset;
  delete f;
*/
  // hsize_t rank = 1;
}

// H5ImageObject
H5ImageObject::H5ImageObject(
    const std::string& dir_path, const int& split_size,
    const std::string& file_ending)
    : H5Object(dir_path, split_size, file_ending) {}
void H5ImageObject::addKeypoints(const std::vector<cv::KeyPoint>& keypoints) {
  this->all_keypoints.push_back(keypoints);
}
bool H5ImageObject::write() const {
  writeHeader();
  cv::Ptr<cv::hdf::HDF5> h5_file = cv::hdf::open(this->current_output_path);

  // Group for data
  if (!h5_file->hlexists(DATA))
    h5_file->grcreate(DATA);
  /*h5_file->dscreate(
      this->all_data.rows, this->all_data.cols, this->all_data.type(),
      H5Object::DATA + H5Object::DATA);
  h5_file->dsinsert(this->all_data, H5Object::DATA + H5Object::DATA);*/
  const int datadims[NUM_DATA_DIMS] = {this->num_data, this->img_rows,
                                       this->img_cols, this->img_channels};
  const cv::Mat batch = this->all_data.reshape(1, NUM_DATA_DIMS, datadims);
  h5_file->dscreate(
      NUM_DATA_DIMS, datadims, batch.type(), H5Object::DATA + H5Object::DATA);
  h5_file->dsinsert(batch, H5Object::DATA + H5Object::DATA);

  // Group for keypoints if available
  if (this->all_keypoints.size() > 0) {
    if (!h5_file->hlexists(KEYPOINTS)) {
      h5_file->grcreate(KEYPOINTS);
    }

    for (size_t i = 0; i < all_keypoints.size(); i++) {
      const int num_keypoints = all_keypoints[i].size();
      h5_file->kpcreate(
          num_keypoints, H5ImageObject::KEYPOINTS + H5ImageObject::KEYPOINTS +
                             std::to_string(i));
      h5_file->kpinsert(
          all_keypoints[i], H5ImageObject::KEYPOINTS +
                                H5ImageObject::KEYPOINTS + std::to_string(i));
    }
  }

  h5_file->close();

  return true;
}

}  // namespace image_extraction_plugin
