GDB = -g

all: config srv
config:
	@if [ ! -d "/tmp/my_ftp" ];\
	then \
		mkdir "/tmp/my_ftp";\
		mkdir "/tmp/my_ftp/server_path";\
		echo "mkdir /tmp/my_ftp";\
		echo "mkdir /tmp/my_ftp/server_path";\
	elif [ ! -d "/tmp/my_ftp/server_path" ];\
	then \
		mkdir "/tmp/my_ftp/server_path";\
		echo "mkdir tmp/my_ftp/server_path";\
	fi

srv: srv_main.o server.o processpool.o socket.o ftp.o analyser.o parse.o guider.o
	g++ -o ../srv srv_main.o server.o processpool.o socket.o ftp.o analyser.o parse.o guider.o -levent
srv_main.o: srv_main.cpp
	g++ -c srv_main.cpp $(GDB)
processpool.o: ../processpool/processpool.cpp
	g++ -c ../processpool/processpool.cpp $(GDB)
socket.o: ../socket/socket.cpp
	g++ -c ../socket/socket.cpp $(GDB)
server.o: server.cpp
	g++ -c server.cpp $(GDB)
ftp.o: ../ftp/ftp.cpp
	g++ -c ../ftp/ftp.cpp $(GDB)
analyser.o: ../analyser/analyser.cpp
	g++ -c ../analyser/analyser.cpp $(GDB)
parse.o: ../parse/parse.cpp
	g++ -c ../parse/parse.cpp $(GDB)
guider.o: ../guider/guider.cpp
	g++ -c ../guider/guider.cpp $(GDB)

.PHONY:clean
clean:
	-rm -f *.o

uninst:
	-rm -r /tmp/my_ftp/server_path
