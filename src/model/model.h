#ifndef MODEL_H
#define MODEL_H

/**
 * @file model.h
 * @brief Модель данных для 3D Viewer приложения
 */

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "tranformation.h"

namespace s21 {

/**
 * @brief Коды ошибок для операций с моделью
 */
enum error_list {
  kNoError = 0,  ///< Операция выполнена успешно
  kFileWrongExtension = 1,  ///< Неверное расширение файла
  kFailedToOpen = 2,        ///< Не удалось открыть файл
  kIncorrectData = 3,  ///< Некорректные данные в файле
};

/**
 * @brief Основной класс модели для работы с 3D объектами
 *
 * Класс Model реализует паттерн Singleton и предоставляет функциональность
 * для загрузки, парсинга и трансформации 3D моделей из OBJ файлов.
 *
 * @details Модель поддерживает:
 * - Загрузку OBJ файлов с вершинами и гранями
 * - Аффинные преобразования (перемещение, поворот, масштабирование)
 * - Автоматическую нормализацию координат
 * - Обработку ошибок при загрузке
 *
 * @example
 * @code
 * Model& model = Model::GetInstance();
 * model.SetFileName("cube.obj");
 * model.Parser();
 * if (model.GetError() == kNoError) {
 *     model.Transform(kMove, 1.0, kX);
 * }
 * @endcode
 */
class Model {
 public:
  /**
   * @brief Парсит OBJ файл и загружает данные модели
   *
   * Читает файл построчно, извлекает вершины (v) и грани (f),
   * выполняет нормализацию координат при необходимости.
   *
   * @pre Имя файла должно быть установлено через SetFileName()
   * @post Данные модели загружены или установлен код ошибки
   */
  void Parser();

  /**
   * @brief Устанавливает имя файла для загрузки
   *
   * @param file_name Путь к OBJ файлу
   * @pre file_name должен иметь расширение .obj
   * @post Данные модели очищены, установлено новое имя файла
   */
  void SetFileName(const std::string& file_name);

  /**
   * @brief Выполняет аффинное преобразование модели
   *
   * @param strategy_type Тип преобразования (kMove, kRotate, kScale)
   * @param value Значение преобразования (смещение, угол, масштаб)
   * @param axis Ось преобразования (kX, kY, kZ)
   *
   * @pre Модель должна быть загружена
   * @post Координаты вершин изменены согласно преобразованию
   */
  void Transform(int strategy_type, double value, transformation_t axis);

  /**
   * @brief Возвращает код последней ошибки
   * @return Код ошибки из enum error_list
   * @retval kNoError Операция выполнена успешно
   * @retval kFileWrongExtension Неверное расширение файла
   * @retval kFailedToOpen Не удалось открыть файл
   * @retval kIncorrectData Некорректные данные в файле
   */
  int GetError() const noexcept;

  /**
   * @brief Возвращает константную ссылку на индексы вершин
   * @return Константная ссылка на вектор индексов рёбер
   */
  const std::vector<int>& GetVertexIndex() const noexcept;

  /**
   * @brief Возвращает константную ссылку на координаты вершин
   * @return Константная ссылка на вектор координат (x,y,z,x,y,z,...)
   */
  const std::vector<double>& GetVertexCoord() const noexcept;

  /**
   * @brief Возвращает неконстантную ссылку на индексы вершин
   * @return Ссылка на вектор индексов рёбер для модификации
   */
  std::vector<int>& GetVertexIndex() noexcept;

  /**
   * @brief Возвращает неконстантную ссылку на координаты вершин
   * @return Ссылка на вектор координат для модификации
   */
  std::vector<double>& GetVertexCoord() noexcept;

  /**
   * @brief Возвращает единственный экземпляр модели (Singleton)
   * @return Ссылка на экземпляр Model
   */
  static Model& GetInstance() noexcept;

  /**
   * @brief Возвращает количество вершин в модели
   * @return Количество вершин (размер координат / 3)
   */
  size_t GetVertexCount() const noexcept { return vertex_coord_.size() / 3; }

  /**
   * @brief Возвращает количество рёбер в модели
   * @return Количество рёбер (размер индексов / 2)
   */
  size_t GetEdgeCount() const noexcept { return vertex_index_.size() / 2; }

 private:
  /**
   * @brief Приватный конструктор для Singleton
   */
  Model() = default;

  /**
   * @brief Приватный деструктор для Singleton
   */
  ~Model() = default;

  /**
   * @brief Удалённый конструктор копирования
   */
  Model(const Model&) = delete;

  /**
   * @brief Удалённый оператор присваивания
   */
  Model& operator=(const Model&) = delete;

  /**
   * @brief Удалённый конструктор перемещения
   */
  Model(Model&&) = delete;

  /**
   * @brief Удалённый оператор присваивания перемещением
   */
  Model& operator=(Model&&) = delete;

  /**
   * @brief Парсит строку с вершиной
   * @param line Строка формата "v x y z"
   * @post Координаты добавлены в vertex_coord_ или установлена ошибка
   */
  void VertexParser_(const std::string& line);

  /**
   * @brief Парсит строку с гранью
   * @param line Строка с индексами вершин грани
   * @post Индексы рёбер добавлены в vertex_index_
   */
  void EdgesParser_(const std::string& line);

  /**
   * @brief Нормализует координаты модели
   * @post Максимальная координата не превышает 1.0
   */
  void Normalize_() noexcept;

  /**
   * @brief Проверяет корректность расширения файла
   * @param filename Имя файла для проверки
   * @return true если файл имеет расширение .obj
   */
  bool IsValidObjExtension_(const std::string& filename) const noexcept;

  /**
   * @brief Очищает все данные модели
   * @post Векторы координат и индексов очищены, код ошибки сброшен
   */
  void ClearData_() noexcept;

  std::string filename_;  ///< Имя загружаемого файла
  std::vector<double> vertex_coord_;  ///< Координаты вершин (x,y,z,...)
  std::vector<int> vertex_index_;  ///< Индексы рёбер
  int error_code_{kNoError};       ///< Код последней ошибки
  Strategy transformation_model_;  ///< Объект для выполнения трансформаций

  static constexpr double kNormalizationThreshold =
      10.0;  ///< Порог нормализации
  static constexpr size_t kMinObjFilenameLength =
      5;  ///< Минимальная длина имени OBJ файла
};

}  // namespace s21

#endif  // MODEL_H
