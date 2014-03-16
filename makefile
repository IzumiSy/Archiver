
archiver: 
	g++ -Wall -o archiver main.cpp

.PHONY: clean dummy
clean:
	rm archiver dummy1 dummy2 dummy3
dummy:
	dd if=/dev/zero of=dummy1 bs=1b count=182
	dd if=/dev/zero of=dummy2 bs=1b count=124
	dd if=/dev/zero of=dummy3 bs=1b count=102

