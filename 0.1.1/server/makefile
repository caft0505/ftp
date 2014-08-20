all: srv
srv: srv_main.o processpool.o socket.o server.o ftp.o analyser.o parse.o
	g++ -o ../srv srv_main.o processpool.o socket.o server.o ftp.o analyser.o parse.o -levent
srv_main.o: srv_main.cpp
	g++ -c srv_main.cpp -g
processpool.o: ../processpool/processpool.cpp
	g++ -c ../processpool/processpool.cpp -g
socket.o: ../socket/socket.cpp
	g++ -c ../socket/socket.cpp -g
server.o: server.cpp
	g++ -c server.cpp
ftp.o: ../ftp/ftp.cpp
	g++ -c ../ftp/ftp.cpp
analyser.o: ../analyser/analyser.cpp
	g++ -c ../analyser/analyser.cpp
parse.o: ../parse/parse.cpp
	g++ -c ../parse/parse.cpp

.PHONY:clean
clean:
	-rm -f *.o
