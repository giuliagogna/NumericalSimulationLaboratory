# Laboratory 12 - Deep and Convolutional Neural Networks for Image Recognition

This directory contains the experiments and data analysis for the twelfth laboratory session, which focuses on image classification using the MNIST dataset. It explores the performance of different optimizers on a standard Deep Neural Network (DNN), introduces Convolutional Neural Networks (CNNs) for efficient spatial feature extraction, and tests the models' generalization capabilities on custom, out-of-distribution data.

## Project Structure

- `exercise12.ipynb`: Jupyter Notebook containing all the neural network models, training logic, data visualization, and comparative analysis.

- `numbers/`: Directory containing the custom handwritten digits (e.g., `number8a.png`) created with GIMP, used for the final testing phase of the trained networks.

## Usage and Execution

The entire exercise is self-contained within the `exercise12.ipynb` notebook. 

To ensure the models are compiled correctly and to avoid memory garbage or tensor shape mismatches (especially when reshaping data for different network architectures), it is highly recommended to run all the cells sequentially from the top of the notebook.

## Overview of Experiments

### Exercise 12.1 - Optimizers Comparison: 
Explores the performance of a standard DNN trained with SGD, Adam, and RMSprop. The experiment highlights the critical role of hyperparameter tuning, especially adjusting the learning rate and the momentum, to achieve stable convergence and navigate the narrow valleys of the cost function without introducing excessive noise.

### Exercise 12.2 - Convolutional Neural Network: 
Implements a CNN architecture utilizing convolutional filters and max-pooling layers. It demonstrates how CNNs are vastly superior for image analysis, achieving excellent accuracy while requiring an order of magnitude fewer trainable parameters (roughly 31,000 compared to the DNN's 350,000) by effectively performing local feature extraction.

### Exercise 12.3 - Out-of-Distribution Testing: 
Tests the trained networks on entirely new, self-generated handwritten digits. The experiment visually demonstrates the vulnerability of all the networks to spatial translations. Furthermore, it highlights the general limitation of neural networks when dealing with data that falls outside their original training domain.