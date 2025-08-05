#ifndef CONTROLLER_H
#define CONTROLLER_H

/**
 * @file controller.h
 * @brief Контроллер для 3D Viewer приложения в паттерне MVC
 */

#include <QObject>
#include <QString>
#include <vector>

#include "../model/model.h"

namespace s21 {

/**
 * @brief Контроллер в архитектуре MVC для управления 3D моделями
 *
 * Класс Controller служит посредником между представлением (View) и моделью
 * (Model) в соответствии с паттерном Model-View-Controller. Обеспечивает тонкую
 * прослойку между пользовательским интерфейсом и бизнес-логикой.
 *
 * @details Контроллер выполняет следующие функции:
 * - Обработка команд пользователя от представления
 * - Делегирование операций модели
 * - Преобразование типов данных между Qt и внутренними типами
 * - Обработка ошибок и их преобразование в сообщения
 * - Уведомление представления о изменениях через сигналы Qt
 *
 * Контроллер остается "тонким" - не содержит бизнес-логики, только координирует
 * взаимодействие между компонентами системы.
 *
 * @example
 * @code
 * Controller controller;
 *
 * // Подключение к представлению
 * connect(&view, &View::LoadModelRequested,
 *         &controller, &Controller::LoadModel);
 * connect(&controller, &Controller::ModelLoaded,
 *         &view, &View::HandleModelLoaded);
 *
 * // Загрузка модели
 * controller.LoadModel("/path/to/model.obj");
 * @endcode
 *
 * @see Model
 * @see View
 */
class Controller : public QObject {
  Q_OBJECT

 public:
  /**
   * @brief Конструктор контроллера
   *
   * Инициализирует контроллер и получает ссылку на единственный
   * экземпляр модели через паттерн Singleton.
   *
   * @param parent Родительский QObject для управления памятью Qt
   *
   * @post Контроллер готов к обработке команд
   */
  explicit Controller(QObject* parent = nullptr);

  /**
   * @brief Деструктор по умолчанию
   */
  ~Controller() = default;

  /**
   * @brief Загружает 3D модель из файла
   *
   * Принимает путь к OBJ файлу, делегирует загрузку модели и обрабатывает
   * результат. В случае успеха испускает сигнал ModelLoaded с данными модели.
   * При ошибке испускает сигнал ModelLoadError с описанием ошибки.
   *
   * @param file_path Путь к OBJ файлу для загрузки
   *
   * @pre file_path должен указывать на существующий OBJ файл
   * @post Модель загружена или установлен код ошибки
   *
   * @emit ModelLoaded При успешной загрузке модели
   * @emit ModelLoadError При ошибке загрузки
   *
   * @see Model::SetFileName()
   * @see Model::Parser()
   */
  void LoadModel(const QString& file_path);

  /**
   * @brief Выполняет трансформацию загруженной модели
   *
   * Принимает параметры трансформации, преобразует типы данных и
   * делегирует выполнение трансформации модели. После успешного
   * выполнения испускает сигнал ModelTransformed.
   *
   * @param strategy_type Тип трансформации:
   *   - kMove (0) - перемещение
   *   - kRotate (1) - поворот
   *   - kScale (2) - масштабирование
   * @param value Значение трансформации:
   *   - Для перемещения: расстояние смещения
   *   - Для поворота: угол в градусах
   *   - Для масштабирования: коэффициент масштаба
   * @param axis Ось трансформации:
   *   - 0 (kX) - ось X
   *   - 1 (kY) - ось Y
   *   - 2 (kZ) - ось Z
   *
   * @pre Модель должна быть успешно загружена
   * @post Координаты модели изменены согласно трансформации
   *
   * @emit ModelTransformed После успешного выполнения трансформации
   *
   * @see Model::Transform()
   */
  void TransformModel(int strategy_type, double value, int axis);

 signals:
  /**
   * @brief Сигнал об успешной загрузке модели
   *
   * Испускается после успешного парсинга OBJ файла.
   * Содержит все необходимые данные для отображения модели.
   *
   * @param vertex_index Вектор индексов вершин для рёбер
   * @param vertex_coord Вектор координат вершин (x,y,z,x,y,z,...)
   * @param filename Имя загруженного файла (без пути)
   * @param vertex_count Количество вершин в модели
   * @param edge_count Количество рёбер в модели
   *
   * @see LoadModel()
   */
  void ModelLoaded(const std::vector<int>& vertex_index,
                   const std::vector<double>& vertex_coord,
                   const QString& filename, int vertex_count, int edge_count);

  /**
   * @brief Сигнал об ошибке загрузки модели
   *
   * Испускается при возникновении ошибки во время загрузки или парсинга файла.
   * Содержит локализованное описание ошибки для пользователя.
   *
   * @param error_message Текстовое описание ошибки на русском языке
   *
   * @see LoadModel()
   * @see GetErrorMessage_()
   */
  void ModelLoadError(const QString& error_message);

  /**
   * @brief Сигнал об успешном выполнении трансформации
   *
   * Испускается после успешного применения любой трансформации к модели.
   * Уведомляет представление о необходимости обновления отображения с новыми
   * данными.
   *
   * @param vertex_index Обновленный вектор индексов вершин для рёбер
   * @param vertex_coord Обновленный вектор координат вершин (x,y,z,x,y,z,...)
   *
   * @see TransformModel()
   */
  void ModelTransformed(const std::vector<int>& vertex_index,
                        const std::vector<double>& vertex_coord);

 private:
  /**
   * @brief Преобразует код ошибки в пользовательское сообщение
   *
   * Конвертирует внутренние коды ошибок модели в локализованные
   * сообщения для отображения пользователю.
   *
   * @param error_code Код ошибки из enum error_list
   * @return Локализованное сообщение об ошибке
   *
   * @retval "Неверное расширение файла. Ожидается .obj" При kFileWrongExtension
   * @retval "Не удалось открыть файл" При kFailedToOpen
   * @retval "Некорректные данные в файле" При kIncorrectData
   * @retval "Неизвестная ошибка" При неопознанном коде ошибки
   */
  QString GetErrorMessage_(int error_code) const;

  /**
   * @brief Испускает сигнал ModelLoaded с данными модели
   *
   * Вспомогательный метод для формирования и отправки сигнала ModelLoaded.
   * Извлекает данные из модели, подсчитывает статистику и испускает сигнал.
   *
   * @param filename Имя файла для передачи в сигнале
   *
   * @pre Модель должна быть успешно загружена
   * @post Сигнал ModelLoaded испущен с актуальными данными
   *
   * @emit ModelLoaded
   */
  void EmitModelData_(const QString& filename);

  Model* model_;  ///< Указатель на единственный экземпляр модели (Singleton)
};

}  // namespace s21

#endif  // CONTROLLER_H
