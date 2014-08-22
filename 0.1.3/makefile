all: cli srv
cli: */*.*
	(cd client; make; cd -)
srv: */*.*
	(cd server; make; cd-)

.PHONY: clean
clean:
	-rm cli srv
	(cd client; make clean; cd -)
	(cd server; make clean; cd -)
