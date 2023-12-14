default:
	g++ -std=c++17 -o build/game.exe src/*.cpp src/pak/*.cpp -O2 -Wall -Wno-missing-braces -I inc/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm