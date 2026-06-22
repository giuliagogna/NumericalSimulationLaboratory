# Laboratory 11 - Feed Forward Neural Networks

This directory contains the experiments and data analysis for the eleventh laboratory session, which focuses on exploring the phenomenology of Feed Forward Neural Networks (FFNNs) using TensorFlow and Keras. It investigates hyperparameter tuning, the limits of neural networks in extrapolating polynomial functions, and the challenges of fitting continuous 2D trigonometric surfaces.

## Project Structure

- `exercise11.ipynb`: Jupyter Notebook containing all the neural network models, training logic, and data visualization.

- `*.keras` / `*.pkl`: Pre-trained model architectures and their corresponding training histories (dynamically generated during execution).

## Usage and Execution

The entire exercise is self-contained within the `exercise11.ipynb` notebook.

**Model Saving Logic:** To save computation time, the notebook is designed to automatically load pre-trained models if they exist in the directory. If you wish to retrain a model from scratch or test new hyperparameters, you must set the `force_retrain = True` flag within the respective configuration cells. When `force_retrain` is enabled, the new model is saved with a timestamp to prevent overwriting previous successful experiments.

To make sure you are running what you want, run all the cells from the top of the notebook before coming to the cell of interest, in order to avoid memory garbage.

## Overview of Experiments

### Exercise 11.1 - Linear Fit: 
Investigates the impact of hyperparameters on network performance. It specifically explores the trade-off between Data Quality (low variance) and Data Quantity (sample size), as well as the risks of excessive training epochs.

### Exercise 11.2 - Polynomial Fit: 
Trains a multi-layer model to fit a 1D cubic polynomial. The experiment highlights the fundamental inability of neural networks, especially those using saturating activation functions like tanh, to extrapolate beyond their training domain and in general the difficulty in predicting the behaviour at the boundaries of the domain.

### Exercise 11.3 - 2D Surface Fit: 
Extends the architecture to model a 3D target geometry ($f(x,y) = \sin(x^2+y^2)$). It utilizes the `swish` activation function to map smooth curves without saturating, and implements advanced regularization techniques, such as Early Stopping (with optimal weight restoration), to prevent overfitting.