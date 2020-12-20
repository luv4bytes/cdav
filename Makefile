# Makefile for cdav

makedirs=mkdir obj/ bin/
removedirs=rm -rf obj/ bin/
options=-Wall
debug= #-g -D DEBUG
ignore_ssl_erros = -D IGNORE_SSL_ERRORS
version= -D VERSION=\"1.0\"

LIBS=-lcurl -lxml2 -lpthread
INCLUDE=-I/usr/include/libxml2/

dependencies= libcurl4-openssl-dev libxml2-dev libpthread-stubs0-dev

all: prep main dav prop args requests helper cmdfile
	$(CC) obj/*.o -o bin/cdav $(options) $(debug) $(ignore_ssl_erros) $(version) $(LIBS) $(INCLUDE)

main: prep dav src/main.c
	$(CC) -c src/main.c -o obj/main.o $(options) $(debug) $(ignore_ssl_erros) $(version) $(LIBS) $(INCLUDE)

dav: prep prop requests src/dav.c
	$(CC) -c src/dav.c -o obj/dav.o $(options) $(debug) $(ignore_ssl_erros) $(version) $(LIBS) $(INCLUDE)

prop: prep src/prop.c
	$(CC) -c src/prop.c -o obj/prop.o $(options) $(debug) $(ignore_ssl_erros) $(version)

requests: prep prop src/requests.c
	$(CC) -c src/requests.c -o obj/requests.o $(options) $(debug) $(ignore_ssl_erros) $(version) $(LIBS) $(INCLUDE)

args: prep src/args.c
	$(CC) -c src/args.c -o obj/args.o $(options) $(debug) $(ignore_ssl_erros) $(version)

cmdfile: prep src/cmdfile.c
	$(CC) -c src/cmdfile.c -o obj/cmdfile.o $(options) $(debug) $(ignore_ssl_erros) $(version) $(LIBS) $(INCLUDE)

helper: prep src/helper.c
	$(CC) -c src/helper.c -o obj/helper.o $(options) $(debug) $(ignore_ssl_erros) $(version)

deps:
	apt-get install -y $(dependencies)

prep: clean

clean:
	$(removedirs)
	$(makedirs)
