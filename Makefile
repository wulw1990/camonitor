CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

TARGETS = main image2video
all: $(TARGETS)
	@echo compiling finished.

main: main.o CameraMonitor.o
	g++ -o $@ $^ $(LIBS) 
main.o: main.cpp CameraMonitor.h
	g++ -c $(CFLAGS) $< $(LIBS) 
CameraMonitor.o: CameraMonitor.cpp
	g++ -c $(CFLAGS) $< $(LIBS) 

image2video: image2video.o
	g++ -o $@ $^ $(LIBS) 
image2video.o: image2video.cpp
	g++ -c $(CFLAGS) $< $(LIBS) 

.PHONY:clean
clean:
	rm -f *.d
	rm -f $(TARGETS)