CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

all:main
	@echo compiling finished.

main: main.o CameraMonitor.o
	g++ -o $@ $^ $(LIBS) 

main.o: main.cpp CameraMonitor.h
	g++ -c $(CFLAGS) $< $(LIBS) 

CameraMonitor.o: CameraMonitor.cpp
	g++ -c $(CFLAGS) $< $(LIBS) 

.PHONY:clean
clean:
	rm -f *.d
	rm -f main