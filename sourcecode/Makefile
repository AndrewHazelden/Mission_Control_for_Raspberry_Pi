# Makefile for Linux
# Mission Control Version 1.4 - 2014-02-06
# ----------------------------------------
# Created by Andrew Hazelden
# Email andrew@andrewhazelden.com

# Compile Tip: Try using this command to compile the program:
# make -f Makefile
# or
# clear; make clean; make;

# add sdl to the system
# sudo apt-get install libsdl-image1.2-dev libsdl-gfx1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev libsdl1.2-dev
# or
# sudo apt-get install libsdl-image1.2 libsdl-gfx1.2-4 libsdl-mixer1.2 libsdl-ttf2.0-0 libsdl1.2-all


INCLUDE = -I/usr/include -I/usr/include/SDL
#INCLUDE = -I/usr/include
LIBDIR  =  
COMPILERFLAGS =  `sdl-config --cflags --libs`

CC = gcc
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)

#LIBRARIES = -lSDL -lSDL_gfx -lSDL_ttf -lSDL_mixer -lm
LIBRARIES = -lSDL_gfx -lSDL_ttf -lSDL_mixer -lSDL_image -lm
#LIBRARIES = -lm

all:  missioncontrol

missioncontrol : missioncontrol.o
	$(CC) $(CFLAGS) missioncontrol.c $(LIBDIR) $(LIBRARIES) -o missioncontrol 
	sudo cp ./missioncontrol /opt/missioncontrol
	
clean:
	rm -f serialio *.o


