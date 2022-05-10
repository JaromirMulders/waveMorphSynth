CC = g++
CFLAGS =  -g -Wall
LDFLAGS = -L /usr/local/lib -ljack -lpthread
OBJ= main.o oscillator.o adsr.o lowPassGate.o voice.o jackMidi.o 

main: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)


.cpp.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o
	rm -f *.h.gch
	rm main
