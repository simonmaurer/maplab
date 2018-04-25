//
// Created by similuke on 25.04.18.
//

#ifndef IMAGE_EXTRACTION_PLUGIN_HDF5_HANDLER_H_
#define IMAGE_EXTRACTION_PLUGIN_HDF5_HANDLER_H_

#include <string>

class HDF5Handler {
 private:
 public:
  virtual void extract(const std::string& dir_path) const = 0;
};

class HDF5PatchWriter {
 private:
 public:
  bool write(const std::string& dir_path);
};

#endif  // IMAGE_EXTRACTION_PLUGIN_HDF5_HANDLER_H_
