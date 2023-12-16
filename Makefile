default:
	g++ -std=c++17 -g -o build/game.exe src/*.cpp src/pak/*.cpp -O2 -fdiagnostics-color=always -Wall -pedantic -Wno-missing-braces -I inc/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm