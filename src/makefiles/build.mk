_build:
	@echo "🔧 Сборка 3DViewer..."
	mkdir -p ../build && cd ../build && qmake6 ../src/view/3DViewer.pro && make
