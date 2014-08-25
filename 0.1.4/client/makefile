GDB = -g

all: config cli
config:
	@if [ ! -d "/tmp/my_ftp" ];\
	then \
		mkdir "/tmp/my_ftp";\
		mkdir "/tmp/my_ftp/client_path";\
		echo "mkdir /tmp/my_ftp";\
		echo "mkdir /tmp/my_ftp/client_path";\
	elif [ ! -d "/tmp/my_ftp/client_path" ];\
	then \
		mkdir "/tmp/my_ftp/client_path";\
		echo "mkdir tmp/my_ftp/client_path";\
	fi

cli: cli_main.o socket.o client.o ftp.o analyser.o command.o parse.o guider.o ls.a
	g++ -o ../cli cli_main.o socket.o client.o ftp.o analyser.o command.o parse.o guider.o ls.a
cli_main.o: cli_main.cpp
	g++ -c cli_main.cpp $(GDB)
socket.o: ../socket/socket.h ../socket/socket.cpp
	g++ -c ../socket/socket.cpp $(GDB)
client.o: client.h client.cpp
	g++ -c client.cpp $(GDB)
ftp.o: ../ftp/ftp.h ../ftp/ftp.cpp
	g++ -c ../ftp/ftp.cpp $(GDB)
analyser.o: ../analyser/analyser.h ../analyser/analyser.cpp
	g++ -c ../analyser/analyser.cpp $(GDB)
command.o: ../command/command.h ../command/command.cpp
	g++ -c ../command/command.cpp $(GDB)
parse.o: ../parse/parse.h ../parse/parse.cpp
	g++ -c ../parse/parse.cpp $(GDB)
guider.o: ../guider/guider.h ../guider/guider.cpp
	g++ -c ../guider/guider.cpp $(GDB)
ls.a: ls.o list.o formart.o
	ar -crv ls.a ls.o list.o formart.o
ls.o: ../tools/ls/*.*
	g++ -c ../tools/ls/ls.cpp $(GDB)
list.o: ../tools/list/*.*
	g++ -c ../tools/list/list.cpp $(GDB)
formart.o: ../tools/formart/*.*
	g++ -c ../tools/formart/formart.cpp $(GDB)

.PHONY: clean
clean:
	-rm -f *.o *.a

uninst:
	-rm -r /tmp/my_ftp/client_path
