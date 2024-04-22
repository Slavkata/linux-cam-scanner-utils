all:
	gcc cam.c -o cam.out
	gcc scanner.c -o scanner.out

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf *.so

run-scanner:
	./scanner.out

test-scanner:
	gcc scanner.c -o scanner.out
	./scanner.out

build-so-scanner:
	gcc -c -o scanner.o scanner.c -fPIC -g
	gcc -shared -o scanner.so scanner.o -g

test-cam:
	gcc -pg cam.c -o cam.out
	./cam.out

build-so-cam:
	gcc -c -o cam.o cam.c -fPIC -g
	gcc -shared -o libcam.so cam.o -g

build-so-all:
	gcc -c -o cam.o cam.c -fPIC -g
	gcc -shared -o libcam.so cam.o -g
	gcc -c -o scanner.o scanner.c -fPIC -g
	gcc -shared -o scanner.so scanner.o -g
