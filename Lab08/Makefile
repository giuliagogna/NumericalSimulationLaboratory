CC = g++
CFLAGS = -Wall -O3 --std=c++17
BUILD = .build

# Main target: compile both executables
all: $(BUILD)/optimize.exe $(BUILD)/production.exe

$(BUILD):
	mkdir -p $(BUILD)

# Optimize Executable
$(BUILD)/optimize.exe : $(BUILD)/optimize.o $(BUILD)/random.o $(BUILD)/functions.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/optimize.o -o $(BUILD)/optimize.exe

$(BUILD)/optimize.o : src/optimize.cpp src/simulated_annealing.h | $(BUILD)
	$(CC) -c src/optimize.cpp -o $(BUILD)/optimize.o $(CFLAGS)

# Production Executable
$(BUILD)/production.exe : $(BUILD)/production.o $(BUILD)/random.o $(BUILD)/functions.o
	$(CC) $(BUILD)/random.o $(BUILD)/functions.o $(BUILD)/production.o -o $(BUILD)/production.exe

$(BUILD)/production.o : src/production.cpp src/simulated_annealing.h | $(BUILD)
	$(CC) -c src/production.cpp -o $(BUILD)/production.o $(CFLAGS)

# Shared Libraries (Added | $(BUILD) here too)
$(BUILD)/random.o : ../utils/random/random.cpp ../utils/random/random.h | $(BUILD)
	$(CC) -c ../utils/random/random.cpp -o $(BUILD)/random.o $(CFLAGS)

$(BUILD)/functions.o : ../utils/auxiliary_functions/functions.cpp ../utils/auxiliary_functions/functions.h ../utils/random/random.h | $(BUILD)
	$(CC) -c ../utils/auxiliary_functions/functions.cpp -o $(BUILD)/functions.o $(CFLAGS)

# Execution commands
run_opt: $(BUILD)/optimize.exe
	time ./$(BUILD)/optimize.exe

run_prod: $(BUILD)/production.exe
	time ./$(BUILD)/production.exe

# Master command to run the whole pipeline
run_all: run_opt run_prod

# Cleanup
clean:
	rm -rf $(BUILD) outputs/seed.out