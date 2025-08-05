#include <gtest/gtest.h>

#include <cmath>
#include <fstream>

#include "../model/model.h"

using namespace s21;

class ModelTest : public ::testing::Test {
 protected:
  void SetUp() override {
    model_ = &Model::GetInstance();
    // Очищаем состояние модели перед каждым тестом
    model_->SetFileName("");
  }

  void TearDown() override {
    // Удаляем тестовые файлы после тестов
    std::remove("test_valid.obj");
    std::remove("test_invalid.obj");
    std::remove("test_empty.obj");
    std::remove("test_wrong_extension.txt");
  }

  // Создаём тестовый .obj файл с валидными данными
  void CreateValidObjFile() {
    std::ofstream file("test_valid.obj");
    file << "# Test OBJ file\n";
    file << "v 0.0 0.0 0.0\n";
    file << "v 1.0 0.0 0.0\n";
    file << "v 1.0 1.0 0.0\n";
    file << "v 0.0 1.0 0.0\n";
    file << "f 1 2 3\n";
    file << "f 1 3 4\n";
    file.close();
  }

  // Создаём тестовый файл с некорректными данными
  void CreateInvalidObjFile() {
    std::ofstream file("test_invalid.obj");
    file << "v invalid data\n";
    file << "f abc def\n";
    file.close();
  }

  // Создаём пустой .obj файл
  void CreateEmptyObjFile() {
    std::ofstream file("test_empty.obj");
    file.close();
  }

  Model* model_;
};

// Тесты для установки имени файла
TEST_F(ModelTest, SetFileName_ValidObjFile_NoError) {
  model_->SetFileName("test.obj");
  EXPECT_EQ(model_->GetError(), 0);
}

TEST_F(ModelTest, SetFileName_WrongExtension_Error) {
  model_->SetFileName("test.txt");
  EXPECT_EQ(model_->GetError(), kFileWrongExtension);
}

TEST_F(ModelTest, SetFileName_ShortName_Error) {
  model_->SetFileName("a.o");
  EXPECT_EQ(model_->GetError(), kFileWrongExtension);
}

TEST_F(ModelTest, SetFileName_EmptyString_Error) {
  model_->SetFileName("");
  EXPECT_EQ(model_->GetError(), kFileWrongExtension);
}

// Тесты для парсинга
TEST_F(ModelTest, Parser_ValidFile_Success) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  EXPECT_EQ(model_->GetError(), 0);

  auto& vertex_coord = model_->GetVertexCoord();
  auto& vertex_index = model_->GetVertexIndex();

  // Проверяем количество вершин (4 вершины * 3 координаты = 12)
  EXPECT_EQ(vertex_coord.size(), 12);

  // Проверяем координаты первой вершины
  EXPECT_DOUBLE_EQ(vertex_coord[0], 0.0);
  EXPECT_DOUBLE_EQ(vertex_coord[1], 0.0);
  EXPECT_DOUBLE_EQ(vertex_coord[2], 0.0);

  // Проверяем координаты второй вершины
  EXPECT_DOUBLE_EQ(vertex_coord[3], 1.0);
  EXPECT_DOUBLE_EQ(vertex_coord[4], 0.0);
  EXPECT_DOUBLE_EQ(vertex_coord[5], 0.0);

  // Проверяем индексы (2 треугольника = 6 линий = 12 индексов)
  EXPECT_EQ(vertex_index.size(), 12);
}

TEST_F(ModelTest, Parser_NonExistentFile_FailedToOpen) {
  model_->SetFileName("nonexistent.obj");
  model_->Parser();

  EXPECT_EQ(model_->GetError(), kFailedToOpen);
}

TEST_F(ModelTest, Parser_InvalidData_IncorrectDataError) {
  CreateInvalidObjFile();
  model_->SetFileName("test_invalid.obj");
  model_->Parser();

  EXPECT_EQ(model_->GetError(), kIncorrectData);
}

TEST_F(ModelTest, Parser_EmptyFile_Success) {
  CreateEmptyObjFile();
  model_->SetFileName("test_empty.obj");
  model_->Parser();

  EXPECT_EQ(model_->GetError(), 0);
  EXPECT_EQ(model_->GetVertexCoord().size(), 0);
  EXPECT_EQ(model_->GetVertexIndex().size(), 0);
}

// Тесты для трансформаций через публичный интерфейс модели
TEST_F(ModelTest, Transform_MoveX_Success) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  auto& vertex_coord = model_->GetVertexCoord();
  double original_x = vertex_coord[0];

  int strategy_type = kMove;
  double value = 1.5;
  transformation_t axis = kX;

  model_->Transform(strategy_type, value, axis);

  // Проверяем, что координата X изменилась на значение value
  EXPECT_DOUBLE_EQ(vertex_coord[0], original_x + value);
}

TEST_F(ModelTest, Transform_MoveY_Success) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  auto& vertex_coord = model_->GetVertexCoord();
  double original_y = vertex_coord[1];

  int strategy_type = kMove;
  double value = 2.0;
  transformation_t axis = kY;

  model_->Transform(strategy_type, value, axis);

  // Проверяем, что координата Y изменилась на значение value
  EXPECT_DOUBLE_EQ(vertex_coord[1], original_y + value);
}

TEST_F(ModelTest, Transform_MoveZ_Success) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  auto& vertex_coord = model_->GetVertexCoord();
  double original_z = vertex_coord[2];

  int strategy_type = kMove;
  double value = -1.0;
  transformation_t axis = kZ;

  model_->Transform(strategy_type, value, axis);

  // Проверяем, что координата Z изменилась на значение value
  EXPECT_DOUBLE_EQ(vertex_coord[2], original_z + value);
}

TEST_F(ModelTest, Transform_RotateX_Success) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  auto& vertex_coord = model_->GetVertexCoord();
  double original_y = vertex_coord[4];
  double original_z = vertex_coord[5];

  int strategy_type = kRotate;
  double angle = 90.0;
  transformation_t axis = kX;

  model_->Transform(strategy_type, angle, axis);

  // После поворота на 90 градусов вокруг X: y' = -z, z' = y
  EXPECT_NEAR(vertex_coord[4], -original_z, 1e-10);
  EXPECT_NEAR(vertex_coord[5], original_y, 1e-10);
}

TEST_F(ModelTest, Transform_RotateY_Success) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  auto& vertex_coord = model_->GetVertexCoord();
  double original_x = vertex_coord[3];
  double original_z = vertex_coord[5];

  int strategy_type = kRotate;
  double angle = 90.0;
  transformation_t axis = kY;

  model_->Transform(strategy_type, angle, axis);

  // После поворота на 90 градусов вокруг Y: x' = z, z' = -x
  EXPECT_NEAR(vertex_coord[3], original_z, 1e-10);
  EXPECT_NEAR(vertex_coord[5], -original_x, 1e-10);
}

TEST_F(ModelTest, Transform_RotateZ_Success) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  auto& vertex_coord = model_->GetVertexCoord();
  double original_x = vertex_coord[3];
  double original_y = vertex_coord[4];

  int strategy_type = kRotate;
  double angle = 90.0;
  transformation_t axis = kZ;

  model_->Transform(strategy_type, angle, axis);

  // После поворота на 90 градусов вокруг Z: x' = y, y' = -x
  EXPECT_NEAR(vertex_coord[3], original_y, 1e-10);
  EXPECT_NEAR(vertex_coord[4], -original_x, 1e-10);
}

TEST_F(ModelTest, Transform_Scale_Success) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  auto& vertex_coord = model_->GetVertexCoord();
  std::vector<double> original_coords = vertex_coord;

  int strategy_type = kScale;
  double scale_factor = 2.0;
  transformation_t axis = kX;

  model_->Transform(strategy_type, scale_factor, axis);

  // Проверяем, что все координаты увеличились в scale_factor раз
  for (size_t i = 0; i < vertex_coord.size(); ++i) {
    EXPECT_DOUBLE_EQ(vertex_coord[i], original_coords[i] * scale_factor);
  }
}

// Тесты для нормализации
TEST_F(ModelTest, Normalize_LargeCoordinates_Normalized) {
  // Создаём файл с большими координатами
  std::ofstream file("test_large.obj");
  file << "v 100.0 200.0 300.0\n";
  file << "v 50.0 100.0 150.0\n";
  file.close();

  model_->SetFileName("test_large.obj");
  model_->Parser();

  auto& vertex_coord = model_->GetVertexCoord();

  // Все координаты должны быть меньше или равны 1.0
  for (double coord : vertex_coord) {
    EXPECT_LE(coord, 1.0);
  }

  std::remove("test_large.obj");
}

// Тесты для Singleton паттерна
TEST_F(ModelTest, Singleton_SameInstance) {
  Model& instance1 = Model::GetInstance();
  Model& instance2 = Model::GetInstance();

  EXPECT_EQ(&instance1, &instance2);
}

// Тесты для парсинга граней с различными форматами
TEST_F(ModelTest, Parser_ComplexFaces_Success) {
  std::ofstream file("test_complex.obj");
  file << "v 0.0 0.0 0.0\n";
  file << "v 1.0 0.0 0.0\n";
  file << "v 1.0 1.0 0.0\n";
  file << "v 0.0 1.0 0.0\n";
  file << "f 1 2 3 4\n";  // Четырёхугольник
  file.close();

  model_->SetFileName("test_complex.obj");
  model_->Parser();

  EXPECT_EQ(model_->GetError(), 0);

  auto& vertex_index = model_->GetVertexIndex();
  // Четырёхугольник: 1-2, 2-3, 3-4, 4-1 = 8 индексов
  EXPECT_EQ(vertex_index.size(), 8);

  std::remove("test_complex.obj");
}

// Тест для проверки очистки данных при установке нового файла
TEST_F(ModelTest, SetFileName_ClearsData) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  // Проверяем, что данные загружены
  EXPECT_GT(model_->GetVertexCoord().size(), 0);
  EXPECT_GT(model_->GetVertexIndex().size(), 0);

  // Устанавливаем новый файл
  model_->SetFileName("new_file.obj");

  // Данные должны быть очищены
  EXPECT_EQ(model_->GetVertexCoord().size(), 0);
  EXPECT_EQ(model_->GetVertexIndex().size(), 0);
}

// Тесты для различных комбинаций трансформаций
TEST_F(ModelTest, Transform_Multiple_Operations_Success) {
  CreateValidObjFile();
  model_->SetFileName("test_valid.obj");
  model_->Parser();

  auto& vertex_coord = model_->GetVertexCoord();
  double original_x = vertex_coord[0];

  // Последовательно применяем несколько трансформаций
  int move_type = kMove;
  double move_value = 1.0;
  transformation_t x_axis = kX;

  int scale_type = kScale;
  double scale_value = 2.0;

  int rotate_type = kRotate;
  double rotate_value = 45.0;

  model_->Transform(move_type, move_value, x_axis);
  model_->Transform(scale_type, scale_value, x_axis);
  model_->Transform(rotate_type, rotate_value, x_axis);

  // Проверяем, что координаты изменились
  EXPECT_NE(vertex_coord[0], original_x);
}

// Тест для проверки парсинга файла с комментариями
TEST_F(ModelTest, Parser_FileWithComments_Success) {
  std::ofstream file("test_comments.obj");
  file << "# This is a comment\n";
  file << "v 0.0 0.0 0.0\n";
  file << "# Another comment\n";
  file << "v 1.0 0.0 0.0\n";
  file << "# Face comment\n";
  file << "f 1 2\n";
  file.close();

  model_->SetFileName("test_comments.obj");
  model_->Parser();

  EXPECT_EQ(model_->GetError(), 0);
  EXPECT_EQ(model_->GetVertexCoord().size(), 6);
  EXPECT_EQ(model_->GetVertexIndex().size(), 4);

  std::remove("test_comments.obj");
}

// Тест для проверки обработки файлов с отрицательными индексами
TEST_F(ModelTest, Parser_NegativeIndices_Ignored) {
  std::ofstream file("test_negative.obj");
  file << "v 0.0 0.0 0.0\n";
  file << "v 1.0 0.0 0.0\n";
  file << "f -1 2\n";
  file.close();

  model_->SetFileName("test_negative.obj");
  model_->Parser();

  EXPECT_EQ(model_->GetError(), 0);

  std::remove("test_negative.obj");
}
