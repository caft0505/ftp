all: srv cli
srv:
	(cd server; make)
cli:
	(cd client; make)

.PHONY: clean
clean:
	-rm -f srv cli
