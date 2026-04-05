#ifndef DATA_BLOCKING_H
#define DATA_BLOCKING_H

#include <cmath>

class DataBlocker {
private:
    int L;                  // Block size
    int current_step;       // Step counter within the current block
    int completed_blocks;   // Number of completed blocks

    double current_block_sum; // Sum of measurements in the current block
    double prog_mean;         // Progressive mean up to the current block
    double prog_mean_sq;      // Progressive mean squared up to the current block

public:
    // Constructor initializes the block size and resets all counters and sums
    DataBlocker(int block_size) : 
        L(block_size), 
        current_step(0), 
        completed_blocks(0),
        current_block_sum(0.0), 
        prog_mean(0.0), 
        prog_mean_sq(0.0) {}

    // Adds a single measurement to the current block and updates the block sum and step counter
    void add_measurement(double value) {
        current_block_sum += value;
        current_step++;

        if (current_step == L) {
            close_block();
        }
    }

    // Closes the block and updates statistics
    void close_block() {
        double block_mean = current_block_sum / L;
        double n = completed_blocks + 1.0; 

        prog_mean = ((n - 1.0) / n) * prog_mean + block_mean / n;
        prog_mean_sq = ((n - 1.0) / n) * prog_mean_sq + (block_mean * block_mean) / n;

        completed_blocks++;
        current_step = 0;
        current_block_sum = 0.0;
    }

    // Returns the current number of throws processed up to the current block
    int get_current_throws() const {
        return completed_blocks * L;
    }

    // Returns the number of completed blocks
    int get_completed_blocks() const {
        return completed_blocks;
    }

    // Returns the statistical uncertainty of the progressive mean
    double get_error() const {
        if (completed_blocks <= 1) return 0.0; 
        
        double variance = prog_mean_sq - (prog_mean * prog_mean);
        if (variance < 0.0) return 0.0; // Avoid floating-point approximation issues
        
        return std::sqrt(variance / (completed_blocks - 1.0));
    }

    // Estimate of the mean up to the current block
    double get_mean() const { 
        return prog_mean; 
    }
};

#endif // DATA_BLOCKING_H