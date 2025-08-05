#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

/**
 * @file tranformation.h
 * @brief Система трансформаций для 3D объектов
 */

#include <cmath>
#include <memory>
#include <vector>

namespace s21 {

/**
 * @brief Перечисление осей и типов трансформаций
 */
enum transformation_t {
  kX = 0,       ///< Ось X
  kY = 1,       ///< Ось Y
  kZ = 2,       ///< Ось Z
  kMove = 0,    ///< Тип трансформации: перемещение
  kRotate = 1,  ///< Тип трансформации: поворот
  kScale = 2  ///< Тип трансформации: масштабирование
};

/**
 * @brief Базовый класс стратегии трансформации
 *
 * Абстрактный класс, определяющий интерфейс для всех типов
 * трансформаций в рамках паттерна Strategy.
 *
 * @details Реализует чисто виртуальный метод Transform,
 * который должен быть переопределён в дочерних классах.
 */
class TransformationStrategy {
 public:
  /**
   * @brief Виртуальный деструктор
   */
  virtual ~TransformationStrategy() = default;

  /**
   * @brief Чисто виртуальный метод трансформации
   *
   * @param vertex_coord Вектор координат вершин для трансформации
   * @param value Значение трансформации (смещение/угол/масштаб)
   * @param axis Ось трансформации
   */
  virtual void Transform(std::vector<double>& vertex_coord, double value,
                         transformation_t axis) = 0;

 protected:
  /**
   * @brief Защищённый конструктор по умолчанию
   */
  TransformationStrategy() = default;

  /**
   * @brief Защищённый конструктор копирования
   */
  TransformationStrategy(const TransformationStrategy&) = default;

  /**
   * @brief Защищённый оператор присваивания
   */
  TransformationStrategy& operator=(const TransformationStrategy&) = default;

  /**
   * @brief Защищённый конструктор перемещения
   */
  TransformationStrategy(TransformationStrategy&&) = default;

  /**
   * @brief Защищённый оператор присваивания перемещением
   */
  TransformationStrategy& operator=(TransformationStrategy&&) = default;
};

/**
 * @brief Стратегия перемещения объекта
 *
 * Реализует линейное перемещение всех вершин объекта
 * вдоль выбранной оси на заданное расстояние.
 *
 * @example
 * @code
 * MoveStrategy move;
 * move.Transform(coords, 2.5, kX); // Перемещение на 2.5 по оси X
 * @endcode
 */
class MoveStrategy final : public TransformationStrategy {
 public:
  /**
   * @brief Выполняет перемещение вершин
   *
   * @param vertex_coord Координаты вершин для перемещения
   * @param step Величина смещения
   * @param axis Ось перемещения (kX, kY, kZ)
   *
   * @post Все координаты вдоль указанной оси увеличены на step
   */
  void Transform(std::vector<double>& vertex_coord, double step,
                 transformation_t axis) override;
};

/**
 * @brief Стратегия поворота объекта
 *
 * Реализует поворот всех вершин объекта вокруг выбранной оси
 * на заданный угол в градусах.
 *
 * @details Использует матрицы поворота:
 * - Вокруг X: y' = y*cos - z*sin, z' = y*sin + z*cos
 * - Вокруг Y: x' = x*cos + z*sin, z' = -x*sin + z*cos
 * - Вокруг Z: x' = x*cos + y*sin, y' = -x*sin + y*cos
 *
 * @example
 * @code
 * RotateStrategy rotate;
 * rotate.Transform(coords, 90.0, kZ); // Поворот на 90° вокруг Z
 * @endcode
 */
class RotateStrategy final : public TransformationStrategy {
 public:
  /**
   * @brief Выполняет поворот вершин
   *
   * @param vertex_coord Координаты вершин для поворота
   * @param angle Угол поворота в градусах
   * @param axis Ось поворота (kX, kY, kZ)
   *
   * @pre vertex_coord.size() >= 3
   * @post Координаты повёрнуты согласно матрице поворота
   */
  void Transform(std::vector<double>& vertex_coord, double angle,
                 transformation_t axis) override;

 private:
  static constexpr double kRadianToDegree =
      M_PI / 180.0;  ///< Коэффициент перевода градусов в радианы

  /**
   * @brief Поворот вокруг оси X
   * @param vertex_coord Координаты вершин
   * @param cos_val Предвычисленный косинус угла
   * @param sin_val Предвычисленный синус угла
   */
  void RotateAroundX_(std::vector<double>& vertex_coord, double cos_val,
                      double sin_val);

  /**
   * @brief Поворот вокруг оси Y
   * @param vertex_coord Координаты вершин
   * @param cos_val Предвычисленный косинус угла
   * @param sin_val Предвычисленный синус угла
   */
  void RotateAroundY_(std::vector<double>& vertex_coord, double cos_val,
                      double sin_val);

  /**
   * @brief Поворот вокруг оси Z
   * @param vertex_coord Координаты вершин
   * @param cos_val Предвычисленный косинус угла
   * @param sin_val Предвычисленный синус угла
   */
  void RotateAroundZ_(std::vector<double>& vertex_coord, double cos_val,
                      double sin_val);
};

/**
 * @brief Стратегия масштабирования объекта
 *
 * Реализует изменение размера объекта путём умножения
 * всех координат на коэффициент масштабирования.
 *
 * @example
 * @code
 * ScaleStrategy scale;
 * scale.Transform(coords, 2.0, kX); // Увеличение в 2 раза
 * scale.Transform(coords, 0.5, kX); // Уменьшение в 2 раза
 * @endcode
 */
class ScaleStrategy final : public TransformationStrategy {
 public:
  /**
   * @brief Выполняет масштабирование вершин
   *
   * @param vertex_coord Координаты вершин для масштабирования
   * @param scale Коэффициент масштабирования
   * @param axis Не используется (масштабирование применяется ко всем осям)
   *
   * @pre scale > 0.0
   * @post Все координаты умножены на scale
   */
  void Transform(std::vector<double>& vertex_coord, double scale,
                 transformation_t axis) override;
};

/**
 * @brief Контекст стратегии для выполнения трансформаций
 *
 * Класс-контекст паттерна Strategy, который управляет
 * выбором и выполнением различных типов трансформаций.
 *
 * @details Использует std::unique_ptr для автоматического
 * управления памятью стратегий.
 *
 * @example
 * @code
 * Strategy context;
 * context.SetStrategy(std::make_unique<MoveStrategy>());
 * context.PerformTransformation(coords, 1.0, kX);
 * @endcode
 */
class Strategy {
 public:
  /**
   * @brief Конструктор по умолчанию
   */
  Strategy() = default;

  /**
   * @brief Деструктор по умолчанию
   */
  ~Strategy() = default;

  /**
   * @brief Удалённый конструктор копирования
   */
  Strategy(const Strategy&) = delete;

  /**
   * @brief Удалённый оператор присваивания
   */
  Strategy& operator=(const Strategy&) = delete;

  /**
   * @brief Конструктор перемещения по умолчанию
   */
  Strategy(Strategy&&) = default;

  /**
   * @brief Оператор присваивания перемещением по умолчанию
   */
  Strategy& operator=(Strategy&&) = default;

  /**
   * @brief Устанавливает стратегию трансформации
   *
   * @param strategy Умный указатель на стратегию трансформации
   *
   * @post Предыдущая стратегия автоматически удалена,
   *       установлена новая стратегия
   */
  void SetStrategy(std::unique_ptr<TransformationStrategy> strategy) noexcept;

  /**
   * @brief Выполняет трансформацию с использованием текущей стратегии
   *
   * @param vertex_coord Координаты вершин для трансформации
   * @param value Параметр трансформации
   * @param axis Ось трансформации
   *
   * @pre Стратегия должна быть установлена через SetStrategy()
   * @post Трансформация выполнена согласно текущей стратегии
   */
  void PerformTransformation(std::vector<double>& vertex_coord, double value,
                             transformation_t axis);

 private:
  std::unique_ptr<TransformationStrategy>
      strategy_;  ///< Текущая стратегия трансформации
};

}  // namespace s21

#endif  // TRANSFORMATION_H
