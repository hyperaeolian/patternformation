LDFLAGS_COMMON = -framework Accelerate -framework GLUT -framework OpenGL -lstdc++ -L/opt/local/lib/ -ljpeg -lpng
CFLAGS_COMMON = -c -Wall -I./ -I/opt/local/include/ -O3

# calls:
CC         = g++
CFLAGS     = ${CFLAGS_COMMON}
LDFLAGS    = ${LDFLAGS_COMMON}
EXECUTABLE = fieldViewer

SOURCES    = fieldViewer.cpp \
	FIELD_2D.cpp \
	VEC3F.cpp \
	MATRIX.cpp \
	VECTOR.cpp 

OBJECTS    = $(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
