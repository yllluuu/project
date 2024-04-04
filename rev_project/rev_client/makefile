PREFIX ?=/home/iot24/yanglu/project/rev_project/rev_client
LIBNAME=mycli
APP_NAME=client
CFLAGS +=-I ./include/ 
LDFLAGS +=-L ./lib/ 
ARGUMENTS=-i 127.0.0.1 -p 78965 -t 2

all:install
	@gcc main.c -o ${APP_NAME} ${CFLAGS} -l${LIBNAME} ${LDFLAGS} -I ../../../sqlite/include/ -lsqlite3 -L ../../../sqlite/lib/ 

shared_lib:
	@gcc -shared -fPIC -o lib${LIBNAME}.so ./src/*.c -I ./include/ -I ../../../sqlite/include/ 

clean:
	@rm -f ${PREFIX}/lib/*.so 
	@rm ${APP_NAME}

install:shared_lib
	@mkdir -p ${PREFIX}/lib
	@mv lib*.so ${PREFIX}/lib

uninstall:
	@rm -f ${PREFIX}/lib/lib${LIBNAME}.so 

run:
	@export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PREFIX}/lib&&./${APP_NAME} -i 127.0.0.1 -p 78965 -t 2
