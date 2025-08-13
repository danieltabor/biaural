CC = gcc
CFLAGS = -g

biaural: tones.o gui.o file.o oggfile.o main.o
	$(CC) -o biaural main.o file.o oggfile.o gui.o tones.o -lm -lportaudio -lncurses -lvorbis -lvorbisenc -logg

clean:
	rm -f *.o
	rm -f *.c~
	rm -f *.h~


distclean: clean
	rm -f biaural
	rm -f *.sav
	rm -f *.ogg

tones.o: tones.c tones.h
	$(CC) $(CFLAGS) -c tones.c

gui.o: gui.c gui.h tones.h
	$(CC) $(CFLAGS) -c gui.c

file.o: file.c file.h
	$(CC) $(CFLAGS) -c file.c

oggfile.o: oggfile.c oggfile.h
	$(CC) $(CFLAGS) -c oggfile.c

main.o: main.c gui.h file.h tones.h
	$(CC) $(CFLAGS) -c main.c

