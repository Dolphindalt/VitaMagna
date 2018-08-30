CC=g++
LIBS=-lGL -lGLEW -lSDL2 -lm
NAME=vitamagna
SOURCES=main.cpp
SOURCES+= ./imgui/imgui.cpp ./imgui_draw.cpp
SOURCES+= ./imgui/examples/imgui_impl_sdl.cpp ./imgui/examples/imgui_impl_opengl3.cpp
FLAGS=-I./imgui -I./imgui/examples
OBJS= $(addsuffix .o, $(basename $(notdir $(SOURCES))))

%.o:%.cpp
	$(CC) $(FLAGS) -c -o $@ $< $(LIBS)


%.o:imgui/%.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

%.o:imgui/examples/%.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

clean:
	rm -f $(NAME) $(OBJS)