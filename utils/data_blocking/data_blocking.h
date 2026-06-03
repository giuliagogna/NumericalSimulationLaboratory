#ifndef DATA_BLOCKING_H
#define DATA_BLOCKING_H

#include <cmath>

class DataBlocker {
private:
    int _L;                  // Block size
    int _current_step;       // Step counter within the current block
    int _completed_blocks;   // Number of completed blocks

    double _current_block_sum; // Sum of measurements in the current block
    double _prog_mean;         // Progressive mean up to the current block
    double _prog_mean_sq;      // Progressive mean squared up to the current block

public:
    // Constructor initializes the block size and resets all counters and sums
    DataBlocker(int block_size) : 
        _L(block_size), 
        _current_step(0), 
        _completed_blocks(0),
        _current_block_sum(0.0), 
        _prog_mean(0.0), 
        _prog_mean_sq(0.0) {}

    // Adds a single measurement to the current block and updates the block sum and step counter
    void add_measurement(double value) {
        _current_block_sum += value;
        _current_step++;

        if (_current_step == _L) {
            close_block();
        }
    }

    // Closes the block and updates statistics
    void close_block() {

        _completed_blocks++;

        double block_mean = _current_block_sum / _L;
        double n = _completed_blocks; 

        _prog_mean = ((n - 1.0) / n) * _prog_mean + block_mean / n;
        _prog_mean_sq = ((n - 1.0) / n) * _prog_mean_sq + (block_mean * block_mean) / n;

        _current_step = 0;
        _current_block_sum = 0.0;

    }

    // Returns the current number of throws processed up to the current block
    int get_current_throws() const {
        return _completed_blocks * _L;
    }

    // Returns the number of completed blocks
    int get_completed_blocks() const {
        return _completed_blocks;
    }

    // Returns the statistical uncertainty of the progressive mean
    double get_error() const {
        if (_completed_blocks <= 1) return 0.0; 
        
        double variance = _prog_mean_sq - (_prog_mean * _prog_mean);
        if (variance < 0.0) return 0.0; // Avoid floating-point approximation issues
        
        return std::sqrt(variance / (_completed_blocks - 1.0));
    }

    // Estimate of the mean up to the current block
    double get_mean() const { 
        return _prog_mean; 
    }
};

#endif // DATA_BLOCKING_H