
INCS = -I /usr/include/lua5.1
LIBS = -llua5.1 -lshp
CFLAGS = -std=gnu99 -fpic
OBJS = shapelib.o
BINDIR = ..
TARGET = shapelib.so

all: shapelib dbflib 

shapelib: shapelib.o 
	gcc $(LIBS) $(FLAGS) -shared -Wl,-soname,shapelib.so shapelib.o -o $(BINDIR)/shapelib.so

dbflib: dbflib.o 
	gcc $(LIBS) $(FLAGS) -shared -Wl,-soname,dbflib.so dbflib.o -o $(BINDIR)/dbflib.so

.c.o:
	gcc $(INCS) $(CFLAGS) -c $< -o $@

clean:
	rm *.o $(BINDIR)/$(TARGET) 
