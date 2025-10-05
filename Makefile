.PHONY: source build run restart test play clean
.ONESHELL:

restart: clean source build run
test: source build run

source:
# Generating the compile_commands file for clangd lsp
	cd .\make
	cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
	move .\compile_commands.json ..
# Actual cmake step
	cd ..\build
	cmake ..

build:
	cd .\build
	cmake --build . --config Debug

run:
	".\build\Debug\main.exe"

clean:
	del "compile_commands.json"
	rmdir /S /Q build
	mkdir build
	rmdir /S /Q make
	mkdir make
	rmdir /S /Q GAME
