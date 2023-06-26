app: msg dist game main

msg: 
	@echo Hello, we're getting things ready for ya...

dist: 
	mkdir dist

game:
	g++ -c src/utils/string.cpp -o dist/string.o
	g++ -c src/utils/time.cpp -o dist/time.o
	g++ -c src/Game.cpp -o dist/Game.o
	g++ -c src/main.cpp -o dist/main.o

main: 
	g++ -o dist/main dist/main.o dist/Game.o
	./dist/main

reload: app	