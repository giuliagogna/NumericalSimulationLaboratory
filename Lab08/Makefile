CC = g++
CFLAGS = -Wall -O3 --std=c++17
BUILD = .build

CC = g++
CFLAGS = -Wall -O3 --std=c++17
BUILD = .build

# Main target: compile all executables
all: $(BUILD)/exercise08_2.exe

# Create the hidden directory if it doesn't exist
$(BUILD):
	mkdir -p $(BUILD)

# Exercise 08_1 is solved in a header file. For sake of clarity I will keep the second file named
# exercise08_2 since it solves the second part of the exercise

# Exercise 2
$(BUILD)/exercise08_2.exe : $(BUILD)/exercise08_2.o $(BUILD)/random.o $(BUILD)/functions.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/exercise08_2.o -o $(BUILD)/exercise08_2.exe

# Added the src/ path to the header and the order-only dependency | $(BUILD)
$(BUILD)/exercise08_2.o : src/exercise08_2.cpp src/compute_energy.h | $(BUILD)
	$(CC) -c src/exercise08_2.cpp -o $(BUILD)/exercise08_2.o $(CFLAGS)

# Shared Libraries (Added | $(BUILD) here too)
$(BUILD)/random.o : ../utils/random/random.cpp ../utils/random/random.h | $(BUILD)
	$(CC) -c ../utils/random/random.cpp -o $(BUILD)/random.o $(CFLAGS)

$(BUILD)/functions.o : ../utils/auxiliary_functions/functions.cpp ../utils/auxiliary_functions/functions.h ../utils/random/random.h | $(BUILD)
	$(CC) -c ../utils/auxiliary_functions/functions.cpp -o $(BUILD)/functions.o $(CFLAGS)

# Execution commands (Removed the duplicate run1 override)
run: $(BUILD)/exercise08_2.exe
	./$(BUILD)/exercise08_2.exe

# Clean up
clean :
	rm -rf $(BUILD) outputs/seed.out