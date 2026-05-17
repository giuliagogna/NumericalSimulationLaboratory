CC = mpicxx
CFLAGS = -Wall -O3 --std=c++17 -I/opt/homebrew/include
AFLAGS = -L/opt/homebrew/lib -larmadillo
BUILD = .build

# Main target: create the hidden directory first, then compile all executables
all: $(BUILD) $(BUILD)/exercise10.exe

# Create the hidden directory if it doesn't exist
$(BUILD):
	mkdir -p $(BUILD)


# Exercise 1
$(BUILD)/exercise10.exe : $(BUILD)/exercise10.o $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/individual.o $(BUILD)/population.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/individual.o $(BUILD)/population.o $(BUILD)/exercise10.o -o $(BUILD)/exercise10.exe $(AFLAGS)

$(BUILD)/exercise10.o : src/exercise10.cpp src/Initializer.h src/GenerateCoords.h
	$(CC) -c src/exercise10.cpp -o $(BUILD)/exercise10.o $(CFLAGS)


# Shared Libraries
$(BUILD)/random.o : ../utils/random/random.cpp ../utils/random/random.h
	$(CC) -c ../utils/random/random.cpp -o $(BUILD)/random.o $(CFLAGS)

$(BUILD)/functions.o : ../utils/auxiliary_functions/functions.cpp ../utils/auxiliary_functions/functions.h ../utils/random/random.h
	$(CC) -c ../utils/auxiliary_functions/functions.cpp -o $(BUILD)/functions.o $(CFLAGS)

$(BUILD)/individual.o : src/Individual.cpp src/Individual.h ../utils/random/random.h 
	$(CC) -c src/Individual.cpp -o $(BUILD)/individual.o $(CFLAGS)

$(BUILD)/population.o : src/Population.cpp src/Population.h ../utils/random/random.h 
	$(CC) -c src/Population.cpp -o $(BUILD)/population.o $(CFLAGS)


# Execution commands
# Running simulation
# Run with mpiexec to spawn 7 continents 
# - my machine has 8 cores, 4 performance and 4 efficiency
run: $(BUILD)/exercise10.exe
	time mpiexec -np 7 ./$(BUILD)/exercise10.exe

# Clean up
clean :
	rm -rf $(BUILD) outputs/seed.out