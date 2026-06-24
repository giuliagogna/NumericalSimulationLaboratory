CC = g++
CFLAGS = -Wall -O3 --std=c++17
BUILD = .build

# Main target: create the hidden directory first, then compile all executables
all: $(BUILD) $(BUILD)/exercise05-1.exe $(BUILD)/autocorr_generator.exe

# Create the hidden directory if it doesn't exist
$(BUILD):
	mkdir -p $(BUILD)


# Exercise 1
$(BUILD)/exercise05-1.exe : $(BUILD)/exercise05-1.o $(BUILD)/random.o $(BUILD)/functions.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/exercise05-1.o -o $(BUILD)/exercise05-1.exe $(CFLAGS)

$(BUILD)/exercise05-1.o : src/exercise05-1.cpp ../utils/metropolis/metropolis.h ../utils/data_blocking/data_blocking.h
	$(CC) -c src/exercise05-1.cpp -o $(BUILD)/exercise05-1.o $(CFLAGS)

# ========================================================
# Autocorrelation Generator
# ========================================================
$(BUILD)/autocorr_generator.exe : $(BUILD)/autocorr_generator.o $(BUILD)/random.o $(BUILD)/functions.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/autocorr_generator.o -o $(BUILD)/autocorr_generator.exe $(CFLAGS)

$(BUILD)/autocorr_generator.o : src/autocorrelation/autocorr_generator.cpp ../utils/metropolis/metropolis.h
	$(CC) -c src/autocorrelation/autocorr_generator.cpp -o $(BUILD)/autocorr_generator.o $(CFLAGS)



# Shared Libraries
$(BUILD)/random.o : ../utils/random/random.cpp ../utils/random/random.h
	$(CC) -c ../utils/random/random.cpp -o $(BUILD)/random.o $(CFLAGS)

$(BUILD)/functions.o : ../utils/auxiliary_functions/functions.cpp ../utils/auxiliary_functions/functions.h ../utils/random/random.h
	$(CC) -c ../utils/auxiliary_functions/functions.cpp -o $(BUILD)/functions.o $(CFLAGS)


# Execution commands
# Running simulation
run: $(BUILD)/exercise05-1.exe
	time ./$(BUILD)/exercise05-1.exe

# Running autocorrelation generator
run_auto: $(BUILD)/autocorr_generator.exe
	time ./$(BUILD)/autocorr_generator.exe


# Clean up
clean :
	rm -rf $(BUILD) outputs/seed.out