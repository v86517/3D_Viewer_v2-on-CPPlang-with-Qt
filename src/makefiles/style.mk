CODE_STYLE = Google
CFORMAT_FILES = -name "*.cpp" -o -name "*.h"
CODE_STYLE_SH = -w -i 4 -bn -ci
UI_FILES = -name "*.ui"
QSS_FILES = -name "*.qss"

_style_sh_check:
	@echo ""
	@echo "$(GREEN)=== ПРОВЕРКА SHELL СКРИПТОВ ====$(NC)"
	@find . -name "*.sh" -exec echo "Проверка: {}" \; -exec shellcheck {} \; || true

_style_sh_format:
	@echo ""
	@echo "$(GREEN)=== ФОРМАТИРОВАНИЕ SHELL СКРИПТОВ ====$(NC)"
	@find . -name "*.sh" -exec echo "Форматирование: {}" \; -exec shfmt $(CODE_STYLE_SH) {} \;

_style_sh: _style_sh_check _style_sh_format

_style_sh_ci: _style_sh_check

_cformat_check:
	@echo ""
	@echo "$(GREEN)=== ПРОВЕРКА СТИЛЯ C++ ====$(NC)"
	$(call run_cformat,-n)

_cformat_format:
	@echo ""
	@echo "$(GREEN)=== ФОРМАТИРОВАНИЕ C++ ====$(NC)"
	$(call run_cformat,-i)

_style_cpp: _cformat_check _cformat_format

_style_cpp_ci:
	find . \( \( $(CFORMAT_FILES) \) \) -print0 | xargs -0 -n 1 clang-format -style=$(CODE_STYLE) --Werror -n --verbose

define run_cformat
	@find . -type f \( $(CFORMAT_FILES) \) -exec clang-format -style=$(CODE_STYLE) $(1) {} \;
endef

_style_ui:
	@echo ""
	@echo "$(GREEN)=== ФОРМАТИРОВАНИЕ UI ====$(NC)"
	@find . \( $(UI_FILES) \) -exec echo "Форматирование: {}" \; -exec xmllint --format --output {} {} \;

_style_qss:
	@echo ""
	@echo "$(GREEN)=== ФОРМАТИРОВАНИЕ QSS ====$(NC)"
	@find . \( $(QSS_FILES) \) -exec echo "Форматирование: {}" \; -exec sh -c '/home/developer/venv/bin/python3 -c "import cssbeautifier; print(cssbeautifier.beautify(open(\"$$1\").read()))" > tmp && mv tmp "$$1"' _ {} \;
