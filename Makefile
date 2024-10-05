all: lisrun
	
lisrun:
	g++ *.cpp -o suco -O3

run:
	start ./suco

clean:
	rm ./suco