CC = g++
CFLAGS = -Wall -O2 --std=c++17
BUILD = .build

# Main target: create the hidden directory first, then compile all executables
all: $(BUILD) $(BUILD)/exercise03_1.exe $(BUILD)/exercise03_2.exe

# Create the hidden directory if it doesn't exist
$(BUILD):
	mkdir -p $(BUILD)


# Exercise 1
$(BUILD)/exercise03_1.exe : $(BUILD)/exercise03_1.o $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/pricing_options.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/pricing_options.o $(BUILD)/exercise03_1.o -o $(BUILD)/exercise03_1.exe $(CFLAGS)

$(BUILD)/exercise03_1.o : src/exercise03_1.cpp ../utils/data_blocking/data_blocking.h
	$(CC) -c src/exercise03_1.cpp -o $(BUILD)/exercise03_1.o $(CFLAGS)


# Exercise 2
$(BUILD)/exercise03_2.exe : $(BUILD)/exercise03_2.o $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/pricing_options.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/pricing_options.o $(BUILD)/exercise03_2.o -o $(BUILD)/exercise03_2.exe $(CFLAGS)

$(BUILD)/exercise03_2.o : src/exercise03_2.cpp ../utils/data_blocking/data_blocking.h
	$(CC) -c src/exercise03_2.cpp -o $(BUILD)/exercise03_2.o $(CFLAGS)


# Shared Libraries
$(BUILD)/random.o : ../utils/random/random.cpp ../utils/random/random.h
	$(CC) -c ../utils/random/random.cpp -o $(BUILD)/random.o $(CFLAGS)

$(BUILD)/functions.o : ../utils/auxiliary_functions/functions.cpp ../utils/auxiliary_functions/functions.h ../utils/random/random.h
	$(CC) -c ../utils/auxiliary_functions/functions.cpp -o $(BUILD)/functions.o $(CFLAGS)

$(BUILD)/pricing_options.o : ../utils/pricing_options/pricing_options.cpp ../utils/pricing_options/pricing_options.h ../utils/random/random.h
	$(CC) -c ../utils/pricing_options/pricing_options.cpp -o $(BUILD)/pricing_options.o $(CFLAGS)


# Execution commands
# Executes firs exercise
run1: $(BUILD)/exercise03_1.exe
	./$(BUILD)/exercise03_1.exe

# Executes second exercise
run2: $(BUILD)/exercise03_2.exe
	./$(BUILD)/exercise03_2.exe

# Master command to execute all exercises
run: run1 run2


# Clean up
clean :
	rm -rf $(BUILD) outputs/seed.out