//
// Created by similuke on 23.04.18.
//

#ifndef IMAGE_EXTRACTION_PLUGIN_IMAGE_EXTRACTION_PLUGIN_H_
#define IMAGE_EXTRACTION_PLUGIN_IMAGE_EXTRACTION_PLUGIN_H_

#include <string>

#include <console-common/console-plugin-base.h>
#include <map-manager/map-manager.h>
#include <vi-map/vi-map.h>

namespace image_extraction_plugin {
class ImageExtractionPlugin : public common::ConsolePluginBase {
 public:
  explicit ImageExtractionPlugin(common::Console* console);
  virtual std::string getPluginId() const;

 private:
  int extractImages() const;
  int extractPatches() const;
  bool checkImageFlags() const;
  bool checkPatchFlags() const;
  bool processPatches(const vi_map::VIMapManager::MapReadAccess map);
};

}  // namespace image_extraction_plugin

#endif  // IMAGE_EXTRACTION_PLUGIN_IMAGE_EXTRACTION_PLUGIN_H_
