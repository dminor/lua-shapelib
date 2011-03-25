
INCS = -I /usr/include/lua5.1
LIBS = -llua5.1 -lshp
CFLAGS = -std=gnu99 -fpic
OBJS = shapelib.o
BINDIR = ..
TARGET = shapelib.so

all: $(OBJS)
	gcc $(LIBS) $(FLAGS) -shared -Wl,-soname,$(TARGET) $(OBJS) -o $(BINDIR)/$(TARGET)

.c.o:
	gcc $(INCS) $(CFLAGS) -c $< -o $@

clean:
	rm *.o $(BINDIR)/$(TARGET) 
