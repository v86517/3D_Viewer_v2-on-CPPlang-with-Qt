/**
 * @file tranformation.cpp
 * @brief Реализация системы трансформаций для 3D объектов
 */

#include "tranformation.h"

#include <algorithm>

namespace s21 {

void MoveStrategy::Transform(std::vector<double>& vertex_coord, double step,
                             transformation_t axis) {
  const size_t axis_offset = static_cast<size_t>(axis);

  for (size_t i = axis_offset; i < vertex_coord.size(); i += 3) {
    vertex_coord[i] += step;
  }
}

void RotateStrategy::Transform(std::vector<double>& vertex_coord, double angle,
                               transformation_t axis) {
  if (vertex_coord.size() < 3) {
    return;
  }

  const double rad_angle = angle * kRadianToDegree;
  const double cos_val = std::cos(rad_angle);
  const double sin_val = std::sin(rad_angle);

  switch (axis) {
    case kX:
      RotateAroundX_(vertex_coord, cos_val, sin_val);
      break;
    case kY:
      RotateAroundY_(vertex_coord, cos_val, sin_val);
      break;
    case kZ:
      RotateAroundZ_(vertex_coord, cos_val, sin_val);
      break;
  }
}

void RotateStrategy::RotateAroundX_(std::vector<double>& vertex_coord,
                                    double cos_val, double sin_val) {
  for (size_t i = 1; i < vertex_coord.size(); i += 3) {
    const double y = vertex_coord[i];
    const double z = vertex_coord[i + 1];

    vertex_coord[i] = cos_val * y - sin_val * z;
    vertex_coord[i + 1] = sin_val * y + cos_val * z;
  }
}

void RotateStrategy::RotateAroundY_(std::vector<double>& vertex_coord,
                                    double cos_val, double sin_val) {
  for (size_t i = 0; i < vertex_coord.size(); i += 3) {
    const double x = vertex_coord[i];
    const double z = vertex_coord[i + 2];

    vertex_coord[i] = cos_val * x + sin_val * z;
    vertex_coord[i + 2] = -sin_val * x + cos_val * z;
  }
}

void RotateStrategy::RotateAroundZ_(std::vector<double>& vertex_coord,
                                    double cos_val, double sin_val) {
  for (size_t i = 0; i < vertex_coord.size(); i += 3) {
    const double x = vertex_coord[i];
    const double y = vertex_coord[i + 1];

    vertex_coord[i] = cos_val * x + sin_val * y;
    vertex_coord[i + 1] = -sin_val * x + cos_val * y;
  }
}

void ScaleStrategy::Transform(std::vector<double>& vertex_coord, double scale,
                              transformation_t) {
  if (scale <= 0.0) {
    return;
  }

  std::transform(vertex_coord.begin(), vertex_coord.end(), vertex_coord.begin(),
                 [scale](double coord) { return coord * scale; });
}

void Strategy::SetStrategy(
    std::unique_ptr<TransformationStrategy> strategy) noexcept {
  strategy_ = std::move(strategy);
}

void Strategy::PerformTransformation(std::vector<double>& vertex_coord,
                                     double value, transformation_t axis) {
  if (strategy_) {
    strategy_->Transform(vertex_coord, value, axis);
  }
}

}  // namespace s21
