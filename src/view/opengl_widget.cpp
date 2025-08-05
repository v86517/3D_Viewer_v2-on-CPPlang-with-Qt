/**
 * @file opengl_widget.cpp
 * @brief Реализация OpenGL виджета для отображения 3D моделей
 */

#include "opengl_widget.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QMimeData>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QPoint>
#include <QUrl>
#include <QWheelEvent>
#include <cmath>

namespace s21 {

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent),
      vertex_coord_(nullptr),
      vertex_index_(nullptr),
      count_vertex_coord_(0),
      count_vertex_index_(0),
      mouse_pressed_(false),
      rotation_x_(0.0f),
      rotation_y_(0.0f),
      rotation_z_(0.0f),
      scale_factor_(1.0f),
      translate_x_(0.0f),
      translate_y_(0.0f),
      translate_z_(0.0f) {
  setMinimumSize(800, 600);

  // Включаем поддержку drag&drop операций для загрузки файлов
  setAcceptDrops(true);
}

void OpenGLWidget::SetModelData(int* vertex_index, double* vertex_coord,
                                int count_vertex_index,
                                int count_vertex_coord) {
  // Сохраняем указатели на данные модели
  vertex_index_ = vertex_index;
  vertex_coord_ = vertex_coord;
  count_vertex_index_ = count_vertex_index;
  count_vertex_coord_ = count_vertex_coord;

  // Запрашиваем перерисовку для отображения новых данных
  update();
}

void OpenGLWidget::initializeGL() {
  // Инициализируем функции OpenGL для использования в коде
  initializeOpenGLFunctions();

  // Устанавливаем тёмно-серый цвет фона для контраста с белыми линиями
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  // Включаем тест глубины для корректного отображения 3D объектов
  glEnable(GL_DEPTH_TEST);
}

void OpenGLWidget::resizeGL(int w, int h) {
  /**
   * @brief Обновление области отображения при изменении размера
   *
   * Устанавливает viewport OpenGL в соответствии с новыми размерами
   * виджета. Это обеспечивает корректное отображение модели при
   * изменении размеров окна.
   */
  glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL() {
  // Очищаем буферы цвета и глубины для нового кадра
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Проверяем наличие валидных данных модели
  if (!vertex_coord_ || !vertex_index_ || count_vertex_coord_ == 0) {
    return;
  }

  /**
   * @brief Основной цикл рендеринга 3D модели
   *
   * Применяет последовательность трансформаций и отрисовывает
   * модель в каркасном режиме. Использует устаревший OpenGL API
   * для простоты и совместимости.
   */

  // Сбрасываем матрицу трансформации к единичной
  glLoadIdentity();

  // Применяем трансформации в правильном порядке:
  // 1. Смещение (translate) - позиционирование в пространстве
  glTranslatef(translate_x_, translate_y_, translate_z_);

  // 2. Повороты вокруг осей X, Y, Z
  glRotatef(rotation_x_, 1.0f, 0.0f, 0.0f);  // Поворот вокруг X
  glRotatef(rotation_y_, 0.0f, 1.0f, 0.0f);  // Поворот вокруг Y
  glRotatef(rotation_z_, 0.0f, 0.0f, 1.0f);  // Поворот вокруг Z

  // 3. Масштабирование - изменение размера модели
  glScalef(scale_factor_, scale_factor_, scale_factor_);

  // Устанавливаем белый цвет для линий каркасной модели
  glColor3f(1.0f, 1.0f, 1.0f);

  // Начинаем отрисовку линий (каждая пара вершин образует ребро)
  glBegin(GL_LINES);

  /**
   * @brief Отрисовка рёбер модели
   *
   * Проходим по массиву индексов рёбер попарно. Каждая пара индексов
   * определяет одно ребро модели. Для каждого ребра получаем координаты
   * вершин и отрисовываем линию между ними.
   */
  for (int i = 0; i < count_vertex_index_; i += 2) {
    // Получаем индексы первой и второй вершины ребра
    int idx1 = vertex_index_[i] * 3;  // Умножаем на 3 (x,y,z координаты)
    int idx2 = vertex_index_[i + 1] * 3;

    // Отрисовываем линию от первой вершины ко второй
    glVertex3d(vertex_coord_[idx1], vertex_coord_[idx1 + 1],
               vertex_coord_[idx1 + 2]);
    glVertex3d(vertex_coord_[idx2], vertex_coord_[idx2 + 1],
               vertex_coord_[idx2 + 2]);
  }

  // Завершаем отрисовку линий
  glEnd();
}

// === Публичные методы-обёртки для внешнего доступа ===

void OpenGLWidget::HandleMousePress(QMouseEvent* event) {
  mousePressEvent(event);
}

void OpenGLWidget::HandleMouseMove(QMouseEvent* event) {
  mouseMoveEvent(event);
}

void OpenGLWidget::HandleMouseRelease(QMouseEvent* event) {
  mouseReleaseEvent(event);
}

void OpenGLWidget::HandleWheel(QWheelEvent* event) { wheelEvent(event); }

void OpenGLWidget::HandleDrop(QDropEvent* event) { dropEvent(event); }

// === Protected методы обработки событий ===

void OpenGLWidget::mousePressEvent(QMouseEvent* event) {
  /**
   * @brief Начало интерактивного вращения модели
   *
   * При нажатии левой кнопки мыши запоминаем позицию для
   * последующего вычисления перемещения курсора.
   */
  if (event->button() == Qt::LeftButton) {
    mouse_pressed_ = true;
    last_mouse_position_ = event->pos();
  }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
  if (!mouse_pressed_ || !(event->buttons() & Qt::LeftButton)) {
    return;
  }

  QPoint delta = event->pos() - last_mouse_position_;

  // Обновляем углы поворота с нормализацией
  auto normalizeAngle = [](float& angle, float delta) {
    angle += delta * 0.5f;
    angle = std::fmod(angle + 360.0f, 360.0f);
    if (angle > 180.0f) angle -= 360.0f;
  };

  normalizeAngle(rotation_x_, delta.y());
  normalizeAngle(rotation_y_, delta.x());

  last_mouse_position_ = event->pos();
  update();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
  /**
   * @brief Завершение интерактивного вращения
   */
  if (event->button() == Qt::LeftButton) {
    mouse_pressed_ = false;
  }
}

void OpenGLWidget::wheelEvent(QWheelEvent* event) {
  constexpr float kScaleSensitivity = 1200.0f;
  constexpr float kMinScale = 0.1f;
  constexpr float kMaxScale = 10.0f;

  float scale_delta = event->angleDelta().y() / kScaleSensitivity;
  scale_factor_ = std::clamp(scale_factor_ + scale_delta, kMinScale, kMaxScale);

  update();
}

void OpenGLWidget::dropEvent(QDropEvent* event) {
  /**
   * @brief Обработка drag&drop для загрузки OBJ файлов
   *
   * Извлекает список перетащенных файлов, проверяет первый файл
   * на соответствие OBJ формату и испускает сигнал для загрузки.
   */

  const QMimeData* mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    QList<QUrl> urls = mimeData->urls();
    if (!urls.isEmpty()) {
      // Получаем путь к первому файлу
      QString filepath = urls.first().toLocalFile();

      // Проверяем расширение файла (регистронезависимо)
      if (filepath.endsWith(".obj", Qt::CaseInsensitive)) {
        // Испускаем сигнал для загрузки модели
        emit fileDropped(filepath);

        // Принимаем операцию drag&drop
        event->acceptProposedAction();
      }
    }
  }
}

}  // namespace s21
