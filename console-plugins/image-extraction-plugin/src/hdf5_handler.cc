//
// Created by Simon Maurer on 25.04.18.
//

#include "image-extraction-plugin/hdf5_handler.h"

// using namespace H5;

HDF5PatchWriter::HDF5PatchWriter(const std::string& filename)
    : filename(filename) {}

bool HDF5PatchWriter::write(const std::string& dir_path) {
  const std::string output_pat = dir_path + "/" + filename;

  // H5::H5File file(FILE_NAME, H5F_ACC_RDWR);
  return true;
}
