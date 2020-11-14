makedirs=mkdir obj/ bin/
removedirs=rm -rf obj/ bin/
options=-Wall
debug= -g -D DEBUG
test= -D TEST
version= -D VERSION=\"1.0\"

LIBS=-lcurl -lxml2
INCLUDE=-I/usr/include/libxml2/

dependencies= libcurl4-openssl-dev libxml2-dev

all: prep main dav prop parser requests err helper
	$(CC) obj/*.o -o bin/cdav $(options) $(debug) $(test) $(version) $(LIBS) $(INCLUDE)

main: prep dav src/main.c
	$(CC) -c src/main.c -o obj/main.o $(options) $(debug) $(test) $(version) $(LIBS) $(INCLUDE)

dav: prep prop requests src/dav.c
	$(CC) -c src/dav.c -o obj/dav.o $(options) $(debug) $(test) $(version) $(LIBS) $(INCLUDE)

prop: prep src/prop.c
	$(CC) -c src/prop.c -o obj/prop.o $(options) $(debug) $(test) $(version) $(LIBS) $(INCLUDE)

requests: prep prop src/requests.c
	$(CC) -c src/requests.c -o obj/requests.o $(options) $(debug) $(test) $(version) $(LIBS) $(INCLUDE)

parser: prep src/parser.c
	$(CC) -c src/parser.c -o obj/parser.o $(options) $(debug) $(test) $(version) $(LIBS) $(INCLUDE)

err: prep src/err.c
	$(CC) -c src/err.c -o obj/err.o $(options) $(debug) $(test) $(version) $(LIBS) $(INCLUDE)

helper: prep src/helper.c
	$(CC) -c src/helper.c -o obj/helper.o $(options) $(debug) $(test) $(version) $(LIBS) $(INCLUDE)

deps:
	apt-get install -y $(dependencies)

prep: clean

clean:
	$(removedirs)
	$(makedirs)
