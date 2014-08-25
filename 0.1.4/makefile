all: cli srv
cli: */*.*
	(cd client; make; cd -)
srv: */*.*
	(cd server; make; cd -)

.PHONY: clean
clean:
	-rm cli srv

uninst:
	(cd client; make clean; make uninst; cd -)
	(cd server; make clean; make uninst; cd -)
	-rmdir /tmp/my_ftp
