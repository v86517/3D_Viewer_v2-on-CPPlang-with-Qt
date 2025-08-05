/**
 * @file model.cpp
 * @brief Реализация класса Model для работы с 3D объектами
 */

#include "model.h"

#include <algorithm>
#include <cmath>

namespace s21 {

void Model::Parser() {
  if (error_code_ != kNoError) {
    return;
  }

  std::ifstream file(filename_);
  if (!file.is_open()) {
    error_code_ = kFailedToOpen;
    return;
  }

  vertex_coord_.reserve(1000);
  vertex_index_.reserve(2000);

  std::string line;
  line.reserve(256);

  while (error_code_ == kNoError && std::getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    if (line.size() >= 2) {
      if (line[0] == 'v' && line[1] == ' ') {
        VertexParser_(line);
      } else if (line[0] == 'f' && line[1] == ' ') {
        EdgesParser_(std::string(line.begin() + 2, line.end()));
      }
    }
  }

  if (error_code_ == kNoError) {
    Normalize_();
  }
}

void Model::VertexParser_(const std::string& line) {
  double x = 0.0, y = 0.0, z = 0.0;
  char dummy = 0;

  int parsed = std::sscanf(line.c_str(), "%c %lf %lf %lf", &dummy, &x, &y, &z);

  if (parsed == 4 && dummy == 'v') {
    vertex_coord_.insert(vertex_coord_.end(), {x, y, z});
  } else {
    error_code_ = kIncorrectData;
  }
}

void Model::EdgesParser_(const std::string& line) {
  std::istringstream iss(line);
  std::string token;
  std::vector<int> face_indices;
  face_indices.reserve(10);

  while (std::getline(iss, token, ' ')) {
    if (token.empty()) continue;

    int index = 0;
    if (std::sscanf(token.c_str(), "%d", &index) == 1 && index > 0) {
      face_indices.push_back(index - 1);
    }
  }

  // Создаем рёбра для грани
  if (face_indices.size() >= 2) {
    for (size_t i = 0; i < face_indices.size(); ++i) {
      size_t next = (i + 1) % face_indices.size();
      vertex_index_.push_back(face_indices[i]);
      vertex_index_.push_back(face_indices[next]);
    }
  }
}

void Model::SetFileName(const std::string& file_name) {
  ClearData_();

  if (!IsValidObjExtension_(file_name)) {
    error_code_ = kFileWrongExtension;
  } else {
    filename_ = file_name;
    error_code_ = kNoError;
  }
}

bool Model::IsValidObjExtension_(const std::string& filename) const noexcept {
  if (filename.size() < kMinObjFilenameLength) {
    return false;
  }

  return filename.size() >= 4 &&
         filename.compare(filename.size() - 4, 4, ".obj") == 0;
}

void Model::ClearData_() noexcept {
  vertex_coord_.clear();
  vertex_index_.clear();
  error_code_ = kNoError;
}

int Model::GetError() const noexcept { return error_code_; }

const std::vector<int>& Model::GetVertexIndex() const noexcept {
  return vertex_index_;
}

const std::vector<double>& Model::GetVertexCoord() const noexcept {
  return vertex_coord_;
}

std::vector<int>& Model::GetVertexIndex() noexcept { return vertex_index_; }

std::vector<double>& Model::GetVertexCoord() noexcept { return vertex_coord_; }

void Model::Transform(int strategy_type, double value, transformation_t axis) {
  if (vertex_coord_.empty()) {
    return;
  }

  switch (strategy_type) {
    case kMove:
      transformation_model_.SetStrategy(std::make_unique<MoveStrategy>());
      break;
    case kRotate:
      transformation_model_.SetStrategy(std::make_unique<RotateStrategy>());
      break;
    case kScale:
      transformation_model_.SetStrategy(std::make_unique<ScaleStrategy>());
      break;
    default:
      return;
  }

  transformation_model_.PerformTransformation(vertex_coord_, value, axis);
}

void Model::Normalize_() noexcept {
  if (vertex_coord_.empty()) {
    return;
  }

  double max_abs_value = 0.0;
  for (double coord : vertex_coord_) {
    double abs_coord = std::abs(coord);
    if (abs_coord > max_abs_value) {
      max_abs_value = abs_coord;
    }
  }

  if (max_abs_value > kNormalizationThreshold) {
    double scale_factor = 1.0 / max_abs_value;
    std::for_each(vertex_coord_.begin(), vertex_coord_.end(),
                  [scale_factor](double& coord) { coord *= scale_factor; });
  }
}

Model& Model::GetInstance() noexcept {
  static Model instance;
  return instance;
}

}  // namespace s21
