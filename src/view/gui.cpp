/**
 * @file gui.cpp
 * @brief Реализация главного представления приложения 3D Viewer
 */

#include "gui.h"

#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>

#include "facade.h"
#include "ui_view.h"

namespace s21 {

View::View(QWidget* parent)
    : QWidget(parent), facade(nullptr), ui_(new Ui::View) {
  // Устанавливаем C локаль для корректного парсинга чисел с точкой
  setlocale(LC_NUMERIC, "C");

  // Инициализируем пользовательский интерфейс из .ui файла
  ui_->setupUi(this);

  // Создаем OpenGL виджет для отображения 3D моделей
  opengl_widget_ = new OpenGLWidget(this);

  // Заменяем placeholder в UI на реальный OpenGL виджет
  // Создаём layout с нулевыми отступами для точного позиционирования
  QVBoxLayout* layout = new QVBoxLayout(ui_->opengl_widget);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(opengl_widget_);

  // Применяем тёмную тему оформления
  LoadStyles_();

  // Подключаем все обработчики событий
  ConnectSlotSignals_();
}

View::~View() {
  delete ui_;
  // facade удален из конструктора, не нужно его удалять
}

void View::LoadStyles_() {
  // Загружаем таблицу стилей из ресурсов приложения
  QFile file(":/style.qss");
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    setStyleSheet(styleSheet);
    file.close();
  }
}

std::function<void(int)> View::CreateSliderHandler_(int transform_type,
                                                    int axis,
                                                    double scale_factor,
                                                    double& state_ref) {
  return [this, transform_type, axis, scale_factor, &state_ref](int value) {
    double new_value = value * scale_factor;

    // Специальная обработка для масштабирования
    if (transform_type == 2) {  // kScale
      if (state_ref == 0.0) state_ref = 1.0;
      double scale_factor_change = new_value / state_ref;
      if (std::abs(scale_factor_change - 1.0) > 0.001) {
        state_ref = new_value;
        emit TransformRequested(transform_type, scale_factor_change, axis);
      }
    } else {
      double delta = new_value - state_ref;
      if (std::abs(delta) > 0.001) {
        state_ref = new_value;
        emit TransformRequested(transform_type, delta, axis);
      }
    }
  };
}

void View::ConnectSlotSignals_() {
  // === Подключение кнопки выбора файла ===
  connect(ui_->pushButton_load_file, &QPushButton::clicked, [this]() {
    QString filepath = QFileDialog::getOpenFileName(
        this, tr("Выберите файл"), QDir::homePath(), tr("OBJ Files (*.obj)"));
    if (!filepath.isEmpty()) {
      emit SetModel(filepath);
      ui_->label_filename->setText(QFileInfo(filepath).fileName());
    }
  });

  // === Подключение слайдеров перемещения ===
  connect(ui_->horizontalSlider_move_x, &QSlider::valueChanged,
          CreateSliderHandler_(0, 0, 0.01, transform_state_.move_x));
  connect(ui_->horizontalSlider_move_y, &QSlider::valueChanged,
          CreateSliderHandler_(0, 1, 0.01, transform_state_.move_y));
  connect(ui_->horizontalSlider_move_z, &QSlider::valueChanged,
          CreateSliderHandler_(0, 2, 0.01, transform_state_.move_z));

  // === Подключение слайдеров поворота ===
  connect(ui_->horizontalSlider_rotate_x, &QSlider::valueChanged,
          CreateSliderHandler_(1, 0, 1.0, transform_state_.rotate_x));
  connect(ui_->horizontalSlider_rotate_y, &QSlider::valueChanged,
          CreateSliderHandler_(1, 1, 1.0, transform_state_.rotate_y));
  connect(ui_->horizontalSlider_rotate_z, &QSlider::valueChanged,
          CreateSliderHandler_(1, 2, 1.0, transform_state_.rotate_z));

  // === Подключение слайдера масштабирования ===
  connect(ui_->horizontalSlider_scale, &QSlider::valueChanged,
          CreateSliderHandler_(2, 0, 0.01, transform_state_.scale));

  // === Подключение drag&drop из OpenGL виджета ===
  connect(opengl_widget_, &OpenGLWidget::fileDropped,
          [this](const QString& filepath) {
            emit SetModel(filepath);
            ui_->label_filename->setText(QFileInfo(filepath).fileName());
          });
}

void View::HandleModelLoaded_(const std::vector<int>& vertex_index,
                              const std::vector<double>& vertex_coord,
                              const QString& filename, int vertex_count,
                              int edge_count) {
  // Создаём статические копии данных для обеспечения времени жизни
  // Статические переменные сохраняют данные между вызовами функции
  static std::vector<int> vertex_index_copy;
  static std::vector<double> vertex_coord_copy;

  // Копируем входные данные
  vertex_index_copy = vertex_index;
  vertex_coord_copy = vertex_coord;

  // Получаем указатели на данные для OpenGL
  vertex_index_ = vertex_index_copy.data();
  vertex_coord_ = vertex_coord_copy.data();
  count_vertex_index_ = static_cast<int>(vertex_index_copy.size());
  count_vertex_coord_ = static_cast<int>(vertex_coord_copy.size());

  // Передаём данные в OpenGL виджет для отрисовки
  opengl_widget_->SetModelData(vertex_index_, vertex_coord_,
                               count_vertex_index_, count_vertex_coord_);

  // Обновляем информацию в пользовательском интерфейсе
  ui_->label_filename->setText(filename);
  ui_->label_file_info->setText(
      QString("Вершин: %1, Рёбер: %2").arg(vertex_count).arg(edge_count));

  // Сбрасываем все слайдеры при загрузке новой модели
  ClearSliders_();
}

void View::HandleModelLoadError_(const QString& error_message) {
  // Отображаем модальное окно с ошибкой
  QMessageBox::warning(this, "Ошибка загрузки", error_message);
}

void View::ClearSliders_() {
  // Временно отключаем сигналы для предотвращения лишних вызовов
  ui_->horizontalSlider_move_x->blockSignals(true);
  ui_->horizontalSlider_move_y->blockSignals(true);
  ui_->horizontalSlider_move_z->blockSignals(true);
  ui_->horizontalSlider_rotate_x->blockSignals(true);
  ui_->horizontalSlider_rotate_y->blockSignals(true);
  ui_->horizontalSlider_rotate_z->blockSignals(true);
  ui_->horizontalSlider_scale->blockSignals(true);

  // Сбрасываем значения слайдеров
  ui_->horizontalSlider_move_x->setValue(0);
  ui_->horizontalSlider_move_y->setValue(0);
  ui_->horizontalSlider_move_z->setValue(0);
  ui_->horizontalSlider_rotate_x->setValue(0);
  ui_->horizontalSlider_rotate_y->setValue(0);
  ui_->horizontalSlider_rotate_z->setValue(0);
  ui_->horizontalSlider_scale->setValue(100);

  // Включаем сигналы обратно
  ui_->horizontalSlider_move_x->blockSignals(false);
  ui_->horizontalSlider_move_y->blockSignals(false);
  ui_->horizontalSlider_move_z->blockSignals(false);
  ui_->horizontalSlider_rotate_x->blockSignals(false);
  ui_->horizontalSlider_rotate_y->blockSignals(false);
  ui_->horizontalSlider_rotate_z->blockSignals(false);
  ui_->horizontalSlider_scale->blockSignals(false);

  // Сбрасываем внутреннее состояние трансформаций
  transform_state_.move_x = 0.0;
  transform_state_.move_y = 0.0;
  transform_state_.move_z = 0.0;
  transform_state_.rotate_x = 0.0;
  transform_state_.rotate_y = 0.0;
  transform_state_.rotate_z = 0.0;
  transform_state_.scale = 1.0;
}

// === Обработчики событий мыши ===
// Все события мыши перенаправляются в OpenGL виджет для обработки

void View::mouseMoveEvent(QMouseEvent* event) {
  if (opengl_widget_) {
    opengl_widget_->HandleMouseMove(event);
  }
}

void View::mousePressEvent(QMouseEvent* event) {
  if (opengl_widget_) {
    opengl_widget_->HandleMousePress(event);
  }
}

void View::mouseReleaseEvent(QMouseEvent* event) {
  if (opengl_widget_) {
    opengl_widget_->HandleMouseRelease(event);
  }
}

void View::wheelEvent(QWheelEvent* event) {
  if (opengl_widget_) {
    opengl_widget_->HandleWheel(event);
  }
}

void View::HandleModelTransformed_(const std::vector<int>& vertex_index,
                                   const std::vector<double>& vertex_coord) {
  // Обновляем статические копии данных новыми трансформированными значениями
  static std::vector<int> vertex_index_copy;
  static std::vector<double> vertex_coord_copy;

  vertex_index_copy = vertex_index;
  vertex_coord_copy = vertex_coord;

  // Обновляем указатели на данные
  vertex_index_ = vertex_index_copy.data();
  vertex_coord_ = vertex_coord_copy.data();
  count_vertex_index_ = static_cast<int>(vertex_index_copy.size());
  count_vertex_coord_ = static_cast<int>(vertex_coord_copy.size());

  // Передаём обновлённые данные в OpenGL виджет
  if (opengl_widget_) {
    opengl_widget_->SetModelData(vertex_index_, vertex_coord_,
                                 count_vertex_index_, count_vertex_coord_);
  }
}

}  // namespace s21
