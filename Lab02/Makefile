CC = g++
CFLAGS = -Wall -O3 --std=c++17
BUILD = .build

# Main target: create the hidden directory first, then compile all executables
all: $(BUILD) $(BUILD)/exercise02-1.exe $(BUILD)/exercise02-2.exe

# Create the hidden directory if it doesn't exist
$(BUILD):
	mkdir -p $(BUILD)


# Exercise 1
$(BUILD)/exercise02-1.exe : $(BUILD)/exercise02-1.o $(BUILD)/random.o $(BUILD)/functions.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/exercise02-1.o -o $(BUILD)/exercise02-1.exe

$(BUILD)/exercise02-1.o : src/exercise02-1.cpp ../utils/data_blocking/data_blocking.h
	$(CC) -c src/exercise02-1.cpp -o $(BUILD)/exercise02-1.o $(CFLAGS)


# Exercise 2
$(BUILD)/exercise02-2.exe : $(BUILD)/exercise02-2.o $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/RWlib.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/RWlib.o $(BUILD)/exercise02-2.o -o $(BUILD)/exercise02-2.exe

$(BUILD)/exercise02-2.o : src/exercise02-2.cpp
	$(CC) -c src/exercise02-2.cpp -o $(BUILD)/exercise02-2.o $(CFLAGS)



# Shared Libraries
$(BUILD)/random.o : ../utils/random/random.cpp ../utils/random/random.h
	$(CC) -c ../utils/random/random.cpp -o $(BUILD)/random.o $(CFLAGS)

$(BUILD)/functions.o : ../utils/auxiliary_functions/functions.cpp ../utils/auxiliary_functions/functions.h ../utils/random/random.h
	$(CC) -c ../utils/auxiliary_functions/functions.cpp -o $(BUILD)/functions.o $(CFLAGS)

$(BUILD)/RWlib.o : ../utils/RWlib/RWlib.cpp ../utils/RWlib/RWlib.h ../utils/random/random.h
	$(CC) -c ../utils/RWlib/RWlib.cpp -o $(BUILD)/RWlib.o $(CFLAGS)


# Execution commands
# Executes firs exercise
run1: $(BUILD)/exercise02-1.exe
	time ./$(BUILD)/exercise02-1.exe

# Executes second exercise
run2: $(BUILD)/exercise02-2.exe
	time ./$(BUILD)/exercise02-2.exe

# Master command to execute all exercises
run: run1 run2


# Clean up
clean :
	rm -rf $(BUILD) outputs/seed.out