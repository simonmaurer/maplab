//
// Created by Simon Maurer on 02.05.18.
//

#include "image-extraction-plugin/config.h"

#include <sstream>
#include <string>

#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

namespace image_extraction_plugin {

const std::string Config::training_dir = "training_dataset";
const std::string Config::validation_dir = "validation_dataset";

void Config::init(const std::string& output_dir) {
  this->work_path = fs::path(output_dir);
  this->work_path.append(generateWorkDir());
  if (!fs::exists(work_path)) {
    fs::create_directory(work_path);
  }

  this->training_path = fs::path(this->work_path);
  this->training_path.append(training_dir);
  if (!fs::exists(training_path)) {
    fs::create_directory(training_path);
  }
  this->validation_path = fs::path(this->work_path);
  this->validation_path.append(validation_dir);
  if (!fs::exists(validation_path)) {
    fs::create_directory(validation_path);
  }
}

std::string Config::generateWorkDir() {
  std::string work_dir;
  // boost::gregorian::date today = boost::gregorian::day_clock::local_day();
  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
  int hours = now.time_of_day().hours();
  int minutes = now.time_of_day().minutes();
  int seconds = now.time_of_day().seconds();
  work_dir += std::to_string(hours);
  work_dir += "-";
  work_dir += std::to_string(minutes);
  work_dir += "-";
  work_dir += std::to_string(seconds);
  /*boost::gregorian::date_facet *output_format = new
  boost::gregorian::date_facet("%d%m%Y_%H%M%S");

  std::ostringstream str_stream;
  str_stream.imbue(std::locale(str_stream.getloc(), output_format));
  str_stream << today << std::endl;

  delete output_format;
   work_dir = str_stream.str();
  */
  return work_dir;
}

const boost::filesystem::path& Config::getWorkPath() const {
  return work_path;
}

const boost::filesystem::path& Config::getTrainingPath() const {
  return training_path;
}

const boost::filesystem::path& Config::getValidationPath() const {
  return validation_path;
}

}  // namespace image_extraction_plugin
