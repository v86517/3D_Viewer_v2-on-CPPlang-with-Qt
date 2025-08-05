/**
 * @file main.cpp
 * @brief Точка входа в приложение 3D Viewer v2.0
 *
 * Главный файл приложения, отвечающий за инициализацию Qt приложения,
 * создание компонентов MVC архитектуры и установку связей между ними.
 *
 * @details Файл реализует следующую функциональность:
 * - Инициализация Qt приложения и графической подсистемы
 * - Создание компонентов MVC: Model (Singleton), View, Controller
 * - Установка сигнально-слотовых соединений между компонентами
 * - Запуск главного цикла обработки событий Qt
 *
 * Архитектура приложения:
 * ```
 * ┌─────────┐    сигналы     ┌────────────┐    методы   ┌─────────────┐
 * │  View   │ ────────────>  │ Controller │ ──────────> │  Model      │
 * │ (GUI)   │                │  (тонкий)  │             │(Singleton)  │
 * └─────────┘ <────────────  └────────────┘ <────────── └─────────────┘
 *     ^          сигналы         ^             геттеры      │
 *     └──────────────────────────┴──────────────────── ─────┘
 * ```
 *
 * @see s21::View
 * @see s21::Controller
 * @see s21::Model
 * @see s21::Facade
 */

#include <QApplication>

#include "controller/controller.h"
#include "view/gui.h"

/**
 * @brief Главная функция приложения 3D Viewer v2.0
 *
 * Точка входа в программу, выполняющая полную инициализацию MVC архитектуры
 * и запуск графического интерфейса пользователя.
 *
 * @details Функция выполняет следующие шаги:
 * 1. **Настройка Qt**: Принудительно использует X11 платформу через xcb
 * 2. **Создание приложения**: Инициализирует QApplication с аргументами
 * командной строки
 * 3. **Создание компонентов MVC**:
 *    - View: Графический интерфейс с OpenGL виджетом
 *    - Controller: Тонкий контроллер для обработки команд
 *    - Model: Автоматически создается как Singleton при первом обращении
 * 4. **Установка соединений**:
 *    - Controller → View: Результаты загрузки и ошибки
 *    - View → Controller: Команды пользователя
 *    - Facade → Controller: Команды трансформации
 * 5. **Запуск интерфейса**: Показ окна и запуск event loop
 *
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 *
 * @return Код завершения приложения:
 *   - 0: Успешное завершение
 *   - !0: Ошибка выполнения или аварийное завершение
 *
 * @pre argc >= 1, argv[0] содержит имя исполняемого файла
 * @post Qt приложение инициализировано, MVC компоненты созданы и связаны
 *
 * @note Использует принудительную установку платформы xcb для совместимости с
 * X11
 *
 * @warning Функция блокирующая - выполнение завершается только при закрытии
 * приложения
 *
 * @example
 * Запуск приложения из командной строки:
 * @code{.sh}
 * ./3DViewer                    # Запуск без аргументов
 * ./3DViewer model.obj          # Запуск с файлом (не реализовано)
 * @endcode
 *
 * @see QApplication::exec()
 * @see s21::View::show()
 */
int main(int argc, char *argv[]) {
  // Принудительное использование X11 платформы для совместимости
  qputenv("QT_QPA_PLATFORM", "xcb");

  // Инициализация Qt приложения
  QApplication a(argc, argv);

  // Создание компонентов MVC архитектуры
  s21::View view;
  view.setWindowTitle("3D Viewer 2.0");

  s21::Controller controller;

  /**
   * @brief Установка соединений Controller → View
   *
   * Подключение сигналов контроллера к соответствующим слотам представления
   * для уведомления о результатах операций с моделью.
   */
  // Успешная загрузка модели: Controller::ModelLoaded →
  // View::HandleModelLoaded_
  QObject::connect(&controller, &s21::Controller::ModelLoaded, &view,
                   &s21::View::HandleModelLoaded_);

  // Ошибка загрузки модели: Controller::ModelLoadError →
  // View::HandleModelLoadError_
  QObject::connect(&controller, &s21::Controller::ModelLoadError, &view,
                   &s21::View::HandleModelLoadError_);

  // Модель трансформирована: Controller::ModelTransformed →
  // View::HandleModelTransformed_
  QObject::connect(&controller, &s21::Controller::ModelTransformed, &view,
                   &s21::View::HandleModelTransformed_);

  /**
   * @brief Установка соединений View → Controller
   *
   * Подключение сигналов представления к слотам контроллера
   * для передачи команд пользователя на обработку.
   */
  // Запрос загрузки модели: View::SetModel → Controller::LoadModel
  QObject::connect(&view, &s21::View::SetModel, &controller,
                   &s21::Controller::LoadModel);

  // Запрос трансформации: View::TransformRequested → Controller::TransformModel
  QObject::connect(&view, &s21::View::TransformRequested, &controller,
                   &s21::Controller::TransformModel);

  // Отображение главного окна приложения
  view.show();

  // Запуск главного цикла обработки событий Qt
  return QApplication::exec();
}
