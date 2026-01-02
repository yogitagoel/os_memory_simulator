run:
	g++ src/**/*.cpp src/*.cpp -o out && ./out

clean:
	rm -f out
