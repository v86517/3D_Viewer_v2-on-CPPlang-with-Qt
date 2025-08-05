/**
 * @file controller.cpp
 * @brief Реализация контроллера для 3D Viewer приложения
 */

#include "controller.h"

#include <QFileInfo>

namespace s21 {

Controller::Controller(QObject* parent)
    : QObject(parent), model_(&Model::GetInstance()) {}

void Controller::LoadModel(const QString& file_path) {
  std::string std_file_path = file_path.toStdString();

  model_->SetFileName(std_file_path);
  model_->Parser();

  int error_code = model_->GetError();
  if (error_code != 0) {
    emit ModelLoadError(GetErrorMessage_(error_code));
  } else {
    EmitModelData_(QFileInfo(file_path).fileName());
  }
}

void Controller::TransformModel(int strategy_type, double value, int axis) {
  transformation_t transform_axis = static_cast<transformation_t>(axis);
  model_->Transform(strategy_type, value, transform_axis);

  const auto& vertex_index = model_->GetVertexIndex();
  const auto& vertex_coord = model_->GetVertexCoord();
  emit ModelTransformed(vertex_index, vertex_coord);
}

QString Controller::GetErrorMessage_(int error_code) const {
  switch (error_code) {
    case kFileWrongExtension:
      return "Неверное расширение файла. Ожидается .obj";
    case kFailedToOpen:
      return "Не удалось открыть файл";
    case kIncorrectData:
      return "Некорректные данные в файле";
    default:
      return "Неизвестная ошибка";
  }
}

void Controller::EmitModelData_(const QString& filename) {
  const auto& vertex_index = model_->GetVertexIndex();
  const auto& vertex_coord = model_->GetVertexCoord();

  int vertex_count = vertex_coord.size() / 3;
  int edge_count = vertex_index.size() / 2;

  emit ModelLoaded(vertex_index, vertex_coord, filename, vertex_count,
                   edge_count);
}

}  // namespace s21
