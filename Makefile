OBJ=antiVir
LIBS=-lcrypto
FLAGS=-O3 -fopenmp

all:
	g++ *.cpp -o $(OBJ) $(FLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJ)
