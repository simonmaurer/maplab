//
// Created by similuke on 23.04.18.
//

#ifndef IMAGE_EXTRACTION_PLUGIN_EXTRACTOR_H_
#define IMAGE_EXTRACTION_PLUGIN_EXTRACTOR_H_

#include <string>

class Extractor {
 public:
  virtual void extract(const std::string& dir_path) const = 0;
};

#endif  // IMAGE_EXTRACTION_PLUGIN_EXTRACTOR_H_
