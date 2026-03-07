CC = g++
CFLAGS = -Wall -O3 --std=c++17
BUILD = .build

# Main target: create the hidden directory first, then compile all executables
all: $(BUILD) $(BUILD)/exercise01-1.exe $(BUILD)/exercise01-2.exe $(BUILD)/exercise01-3.exe

# Create the hidden directory if it doesn't exist
$(BUILD):
	mkdir -p $(BUILD)


# Exercise 1
$(BUILD)/exercise01-1.exe : $(BUILD)/exercise01-1.o $(BUILD)/random.o $(BUILD)/functions.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/exercise01-1.o -o $(BUILD)/exercise01-1.exe

$(BUILD)/exercise01-1.o : exercise01-1.cpp
	$(CC) -c exercise01-1.cpp -o $(BUILD)/exercise01-1.o $(CFLAGS)


# Exercise 2
$(BUILD)/exercise01-2.exe : $(BUILD)/exercise01-2.o $(BUILD)/random.o $(BUILD)/functions.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/exercise01-2.o -o $(BUILD)/exercise01-2.exe

$(BUILD)/exercise01-2.o : exercise01-2.cpp
	$(CC) -c exercise01-2.cpp -o $(BUILD)/exercise01-2.o $(CFLAGS)


# Exercise 3
$(BUILD)/exercise01-3.exe : $(BUILD)/exercise01-3.o $(BUILD)/random.o $(BUILD)/functions.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/exercise01-3.o -o $(BUILD)/exercise01-3.exe

$(BUILD)/exercise01-3.o : exercise01-3.cpp
	$(CC) -c exercise01-3.cpp -o $(BUILD)/exercise01-3.o $(CFLAGS)


# Shared Libraries
$(BUILD)/random.o : random/random.cpp random/random.h
	$(CC) -c random/random.cpp -o $(BUILD)/random.o $(CFLAGS)

$(BUILD)/functions.o : auxiliary_functions/functions.cpp auxiliary_functions/functions.h random/random.h
	$(CC) -c auxiliary_functions/functions.cpp -o $(BUILD)/functions.o $(CFLAGS)


# Execution commands
# Executes firs exercise
run1: $(BUILD)/exercise01-1.exe
	./$(BUILD)/exercise01-1.exe

# Executes second exercise
run2: $(BUILD)/exercise01-2.exe
	./$(BUILD)/exercise01-2.exe

# Executes third exercise
run3: $(BUILD)/exercise01-3.exe
	./$(BUILD)/exercise01-3.exe

# Master command to execute all exercises
run: run1 run2 run3


# Clean up
clean :
	rm -rf $(BUILD) outputs/seed.out