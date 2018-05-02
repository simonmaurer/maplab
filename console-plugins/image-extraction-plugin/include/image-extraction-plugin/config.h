//
// Created by Simon Maurer on 01.05.18.
//

#ifndef IMAGE_EXTRACTION_PLUGIN_CONFIG_H_
#define IMAGE_EXTRACTION_PLUGIN_CONFIG_H_

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace image_extraction_plugin {

class Config {
  static const std::string training_dir;
  static const std::string validation_dir;

  boost::filesystem::path work_path;
  boost::filesystem::path training_path;
  boost::filesystem::path validation_path;

 public:
  void init(const std::string& output_dir);

  std::string generateWorkDir();

  // Getter
  const boost::filesystem::path& getWorkPath() const;

  const boost::filesystem::path& getTrainingPath() const;

  const boost::filesystem::path& getValidationPath() const;
};

}  // namespace image_extraction_plugin

#endif  // IMAGE_EXTRACTION_PLUGIN_CONFIG_H_
