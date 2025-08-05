# Показать справку
help:
	@echo "$(GREEN)Доступные команды:$(NC)"
	@$(show_help)

# Сборка
build: _build

# Сборка документации
doc: _clean _docs_report
dvi: doc

# Запуск тестов
test: _clean test-style _start_test

# Запуск тестов с покрытием кода
test-coverage: _clean test-style _start_test _start_test_coverage

# Запуск тестов с проверкой утечек памяти
test-valgrind: _clean test-style _start_test _start_test_coverage _start_valgrind_tail

# Проверка и форматирование кода
test-style: _style_cpp _style_sh _style_ui _style_qss

# Запуск контейнера ubuntu_dev
docker-ubuntu_dev:
	@cd ../Docker/ubuntu_dev && make

# Вход в контейнер ubuntu_dev
docker-ubuntu_dev-enter:
	@cd ../Docker/ubuntu_dev && make enter

# Выключение контейнера ubuntu_dev
docker-ubuntu_dev-off:
	@cd ../Docker/ubuntu_dev && make off

# Запуск контейнера ubuntu_ci
docker-ubuntu_ci:
	@cd ../Docker/ubuntu_ci && make

# Генерация моделей
models-gen:
	@chmod +x tests/scripts/generate_sphere.py
	@cd tests/scripts && python3 ./generate_sphere.py

# Очистка
clean: _clean

include makefiles/variables.mk
include makefiles/utils.mk
include makefiles/style.mk
include makefiles/doxygen.mk
include makefiles/clean.mk
include makefiles/build.mk
include makefiles/test.mk
include makefiles/valgrind.mk

PHONY: help docker-ubuntu_dev docker-ubuntu_dev-off docker-ubuntu_ci test-style doc dvi clean models-gen
