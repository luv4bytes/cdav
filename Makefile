makedirs=mkdir obj/ bin/
removedirs=rm -rf obj/ bin/
options=-Wall
debug=-g -D DEBUG
test=-D TEST

LIBS= -lcurl

all: prep main dav parser err
	$(CC) obj/*.o -o bin/cdav $(options) $(debug) $(test) $(LIBS)

main: prep src/main.c
	$(CC) -c src/main.c -o obj/main.o $(options) $(debug) $(test) $(LIBS)

dav: prep src/dav.c
	$(CC) -c src/dav.c -o obj/dav.o $(options) $(debug) $(test) $(LIBS)

parser: prep src/cdav_parser.c
	$(CC) -c src/cdav_parser.c -o obj/cdav_parser.o $(options) $(debug) $(test) $(LIBS)

err: prep src/err.c
	$(CC) -c src/err.c -o obj/err.o $(options) $(debug) $(test) $(LIBS)

prep: clean

clean:
	$(removedirs)
	$(makedirs)
