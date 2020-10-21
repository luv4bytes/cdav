makedirs=mkdir obj/ bin/
removedirs=rm -rf obj/ bin/
options=-Wall
debug=-g -D DEBUG
test=-D TEST

LIBS= -lcurl -lxml2
INCLUDE= -I/usr/include/libxml2/

all: prep main dav prop parser requests err
	$(CC) obj/*.o -o bin/cdav $(options) $(debug) $(test) $(LIBS) $(INCLUDE)

main: prep src/main.c
	$(CC) -c src/main.c -o obj/main.o $(options) $(debug) $(test) $(LIBS) $(INCLUDE)

dav: prep src/dav.c
	$(CC) -c src/dav.c -o obj/dav.o $(options) $(debug) $(test) $(LIBS) $(INCLUDE)

prop: prep src/prop.c
	$(CC) -c src/prop.c -o obj/prop.o $(options) $(debug) $(test) $(LIBS) $(INCLUDE)

requests: prep src/requests.c
	$(CC) -c src/requests.c -o obj/requests.o $(options) $(debug) $(test) $(LIBS) $(INCLUDE)

parser: prep src/parser.c
	$(CC) -c src/parser.c -o obj/parser.o $(options) $(debug) $(test) $(LIBS) $(INCLUDE)

err: prep src/err.c
	$(CC) -c src/err.c -o obj/err.o $(options) $(debug) $(test) $(LIBS) $(INCLUDE)

prep: clean

clean:
	$(removedirs)
	$(makedirs)
