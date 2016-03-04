
archiver:
	g++ -Wall -o archiver main.cpp

.PHONY: clean
clean:
	rm archiver dummy1 dummy2 dummy3

