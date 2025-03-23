debug:
	gcc -g -o menger_sponge.exe .\main.c -Og -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall

release:
	gcc -o menger_sponge.exe .\main.c -O3 -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall
