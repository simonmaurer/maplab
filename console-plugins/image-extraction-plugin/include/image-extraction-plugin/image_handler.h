//
// Created by Simon Maurer on 23.04.18.
//

#ifndef IMAGE_EXTRACTION_PLUGIN_IMAGE_HANDLER_H_
#define IMAGE_EXTRACTION_PLUGIN_IMAGE_HANDLER_H_

#include <string>
#include <vector>

#include <opencv2/core.hpp>

#include <map-manager/map-manager.h>
#include <vi-map/vi-map.h>

#include <boost/filesystem.hpp>

namespace image_extraction_plugin {

class ImageExtractor {
 protected:
  bool greyscale;
  cv::Size img_size;

 public:
  explicit ImageExtractor(
      const bool& greyscale = true,
      const cv::Size& img_size = cv::Size(-1, -1));
  virtual void extract(
      const vi_map::VIMapManager::MapReadAccess& map,
      const pose_graph::VertexIdList& vertex_idx,
      const std::string& out_path) = 0;

  virtual std::string getName() const;
  virtual std::string getFileEnding() const = 0;

  void resize(cv::Mat* image) const;
};

class PlainImageExtractor : public ImageExtractor {
 public:
  static const std::string MODE;

  explicit PlainImageExtractor(const bool& greyscale, const cv::Size& img_size);
  virtual void extract(
      const vi_map::VIMapManager::MapReadAccess& map,
      const pose_graph::VertexIdList& vertex_idx, const std::string& out_path);

  virtual std::string getName() const;
  virtual std::string getFileEnding() const;
};

class H5ImageExtractor : public ImageExtractor {
 public:
  static const std::string MODE;

  explicit H5ImageExtractor(const bool& greyscale, const cv::Size& img_size);
  virtual void extract(
      const vi_map::VIMapManager::MapReadAccess& map,
      const pose_graph::VertexIdList& vertex_idx, const std::string& out_path);

  virtual std::string getName() const;
  virtual std::string getFileEnding() const;
};

}  // namespace image_extraction_plugin

#endif  // IMAGE_EXTRACTION_PLUGIN_IMAGE_HANDLER_H_
