//
// Created by Simon Maurer on 19.04.18.
//

#include "image-extraction-plugin/image-extraction-plugin.h"

#include <iostream>
#include <string>

#include <console-common/console-plugin-base.h>
#include <map-manager/map-manager.h>
#include <vi-map/vi-map.h>

#include <opencv2/core.hpp>

namespace image_extraction_plugin {

ImageExtractionPlugin::ImageExtractionPlugin(common::Console* console)
    : common::ConsolePluginBase(console) {
  addCommand(
      {"extract_patches"}, [this]() -> int { return extract(); },
      "This command extracts images or patches of matching/non-matching "
      "keypoint "
      "pairs/triplets corresponding to same 3d points of a loaded map. "
      "Parameters "
      "are: --mode/-M {images, pairs, triplets}, --output_dir/-O, "
      "--patch_size/-P",
      common::Processing::Sync);
}

std::string ImageExtractionPlugin::getPluginId() const {
  return "image_extraction_plugin";
}

int ImageExtractionPlugin::extract() const {
  std::cout << "Patch extraction in progress.." << std::endl;

  std::string selected_map_key;
  // This function will write the name of the selected map key into
  // selected_map_key. The function will return false and print an error
  // message if no map key is selected.
  if (!getSelectedMapKeyIfSet(&selected_map_key)) {
    return common::kStupidUserError;
  }

  vi_map::VIMapManager map_manager;
  vi_map::VIMapManager::MapReadAccess map =
      map_manager.getMapReadAccess(selected_map_key);
  // ToDo load associated images of map (greyscale/RGB) and start
  // extraction
  std::cout << map->numMissions() << std::endl;

  std::string map_path;
  map_manager.getMapFolder(selected_map_key, &map_path);
  std::cout << "map path: " << map_path << std::endl;

  // Every console command returns an integer, you can take one from
  // the CommandStatus enum. kSuccess returns everything is fine.
  // Other commonly used return values are common::kUnknownError and
  // common::kStupidUserError.
  return common::kSuccess;
}

}  // namespace image_extraction_plugin

MAPLAB_CREATE_CONSOLE_PLUGIN(image_extraction_plugin::ImageExtractionPlugin);
