makedirs=mkdir obj/ bin/
removedirs=rm -rf obj/ bin/
options=-Wall
debug=-g -D DEBUG
test=-D TEST

LIBS= -lcurl

all: prep main dav prop parser requests err
	$(CC) obj/*.o -o bin/cdav $(options) $(debug) $(test) $(LIBS)

main: prep src/main.c
	$(CC) -c src/main.c -o obj/main.o $(options) $(debug) $(test) $(LIBS)

dav: prep src/dav.c
	$(CC) -c src/dav.c -o obj/dav.o $(options) $(debug) $(test) $(LIBS)

prop: prep src/prop.c
	$(CC) -c src/prop.c -o obj/prop.o $(options) $(debug) $(test) $(LIBS)

requests: prep src/requests.c
	$(CC) -c src/requests.c -o obj/requests.o $(options) $(debug) $(test) $(LIBS)

parser: prep src/parser.c
	$(CC) -c src/parser.c -o obj/parser.o $(options) $(debug) $(test) $(LIBS)

err: prep src/err.c
	$(CC) -c src/err.c -o obj/err.o $(options) $(debug) $(test) $(LIBS)

prep: clean

clean:
	$(removedirs)
	$(makedirs)
