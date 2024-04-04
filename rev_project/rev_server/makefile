PREFIX ?=/home/iot24/yanglu/project/rev_project/rev_server
LIBNAME=myser
APP_NAME=server
CFLAGS +=-I ./include/ 
LDFLAGS +=-L ./lib/ 
ARGUMENTS=-p 78965

all:install
	gcc main.c -o ${APP_NAME} ${CFLAGS} -l${LIBNAME} ${LDFLAGS} -I ../../../sqlite/include/ -lsqlite3 -L ../../../sqlite/lib/ 

shared_lib:
	gcc -shared -fPIC -o lib${LIBNAME}.so ./src/*.c -I ./include/ -I ../../../sqlite/include/

clean:
	rm -f *.so

install:shared_lib
	mkdir -p ${PREFIX}/lib 
	mv lib*.so ${PREFIX}/lib 

uninstall:
	rm -f ${PREFIX}/lib/lib${LIBNAME}.so

run:
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PREFIX}/lib&&./${APP_NAME} ${ARGUMENTS}
