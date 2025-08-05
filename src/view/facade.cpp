/**
 * @file facade.cpp
 * @brief Реализация фасада для упрощения работы с трансформациями 3D модели
 */

#include "facade.h"

#include <QSlider>
#include <map>

#include "ui_view.h"

namespace s21 {

Facade::Facade(Ui::View *ui, QObject *parent) : QObject(parent), ui_(ui) {}

void Facade::MoveModel(QString &slider_name) {
  static const std::map<QString, int> axis_map = {
      {"horizontalSlider_move_x", 0},
      {"horizontalSlider_move_y", 1},
      {"horizontalSlider_move_z", 2}};

  auto it = axis_map.find(slider_name);
  if (it == axis_map.end()) return;

  int axis = it->second;
  double *current_value = nullptr;
  QSlider *slider = nullptr;

  switch (axis) {
    case 0:
      current_value = &transform_info_.move_x;
      slider = ui_->horizontalSlider_move_x;
      break;
    case 1:
      current_value = &transform_info_.move_y;
      slider = ui_->horizontalSlider_move_y;
      break;
    case 2:
      current_value = &transform_info_.move_z;
      slider = ui_->horizontalSlider_move_z;
      break;
  }

  double rate = slider->value() / 30.0;
  emit SetTransform(0, rate - *current_value, axis);
  *current_value = rate;
}

void Facade::RotateModel(QString &slider_name) {
  static const std::map<QString, int> axis_map = {
      {"horizontalSlider_rotate_x", 0},
      {"horizontalSlider_rotate_y", 1},
      {"horizontalSlider_rotate_z", 2}};

  auto it = axis_map.find(slider_name);
  if (it == axis_map.end()) return;

  int axis = it->second;
  double *current_value = nullptr;
  QSlider *slider = nullptr;

  switch (axis) {
    case 0:
      current_value = &transform_info_.rotate_x;
      slider = ui_->horizontalSlider_rotate_x;
      break;
    case 1:
      current_value = &transform_info_.rotate_y;
      slider = ui_->horizontalSlider_rotate_y;
      break;
    case 2:
      current_value = &transform_info_.rotate_z;
      slider = ui_->horizontalSlider_rotate_z;
      break;
  }

  double delta = slider->value() - *current_value;
  emit SetTransform(1, delta, axis);
  *current_value = slider->value();
}

void Facade::ScaleModel(QString &slider_name) {
  if (slider_name == "horizontalSlider_scale") {
    /**
     * @brief Экспоненциальное преобразование значения слайдера масштаба
     *
     * Применяем формулу: rate = 10^(slider_value/60) / 100
     *
     * Это даёт логарифмическое распределение масштаба:
     * - При slider_value = 0: rate ≈ 0.01 (1% от исходного размера)
     * - При slider_value = 60: rate = 0.1 (10% от исходного размера)
     * - При slider_value = 120: rate = 1.0 (100% - исходный размер)
     * - При slider_value = 180: rate = 10.0 (1000% от исходного размера)
     *
     * Такое распределение обеспечивает равномерную чувствительность
     * во всём диапазоне масштабирования.
     */
    double rate = pow(10, ui_->horizontalSlider_scale->value() / 60.0) / 100;

    // Инициализация при первом использовании
    if (transform_info_.scale == 0) transform_info_.scale = rate;

    // Вычисляем коэффициент изменения масштаба относительно текущего состояния
    // Это позволяет применять масштабирование инкрементально
    emit SetTransform(2, rate / transform_info_.scale, 0);

    // Сохраняем новое абсолютное значение масштаба
    transform_info_.scale = rate;
  }
}

void Facade::Transform(QSlider *slider) {
  QString slider_name = slider->objectName();

  // Используем структурированное определение типа трансформации
  if (slider_name.contains("move")) {
    MoveModel(slider_name);
  } else if (slider_name.contains("rotate")) {
    RotateModel(slider_name);
  } else if (slider_name.contains("scale")) {
    ScaleModel(slider_name);
  }
}

}  // namespace s21
