OBJS = src\main.c src\player.c src\terrain.c src\p_bullet.c src\lander.c src\human.c

INCLUDE_PATHS = -IC:\Libraries\SDL2_MinGW_32bit\include
LIBRARY_PATHS = -LC:\Libraries\SDL2_MinGW_32bit\lib

OBJ_NAME = Defender

CFLAGS = -g

all : $(OBJS)
	gcc $(CFLAGS) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) -w -lmingw32 -lSDL2main -lSDL2 -o $(OBJ_NAME)