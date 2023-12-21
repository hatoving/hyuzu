CXX = g++
CXXFLAGS = -g -O2 -fdiagnostics-color=always -Wall -pedantic -static -Iinc/
LDFLAGS = -Llib/ -lraylib -lopengl32 -lgdi32 -lwinmm

BUILDDIR = build
OBEJCTSDIR = objects
SRCDIR = src

SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBEJCTSDIR)/%.o, $(SOURCES))
EXEC = build/hyuzu.exe

all: $(EXEC)

$(EXEC): $(OBJECTS)
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	cp -r data/ build/

$(OBEJCTSDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBEJCTSDIR) $(BUILDDIR)
