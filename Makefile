all: lisrun
	
lisrun:
	g++ data.cpp localSearch.cpp solution.cpp main.cpp -o suco -O3
	g++ data.cpp localSearch.cpp solution.cpp benchmark_main.cpp -o bsuco -O3

run:
	start ./suco

clean:
	rm ./suco