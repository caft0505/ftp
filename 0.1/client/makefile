GDB = -g

all: cli
cli: cli_main.o socket.o client.o ftp.o analyser.o command.o parse.o guider.o
	g++ -o ../cli cli_main.o socket.o client.o ftp.o analyser.o command.o parse.o guider.o
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

.PHONY: clean
clean:
	-rm -f *.o
