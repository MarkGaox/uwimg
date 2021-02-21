#include <math.h>
#include <stdlib.h>
#include "image.h"
#include "matrix.h"

// Run an activation function on each element in a matrix,
// modifies the matrix in place
// matrix m: Input to activation function
// ACTIVATION a: function to run
void activate_matrix(matrix m, ACTIVATION a)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        double sum = 0;
        for(j = 0; j < m.cols; ++j){
            double x = m.data[i][j];
            if(a == LOGISTIC){
                m.data[i][j] = 1 / (1 + exp(-x));
            } else if (a == RELU){
                m.data[i][j] = x < 0 ? 0 : x;
            } else if (a == LRELU){
                m.data[i][j] = x < 0 ? 0.1 * x : x;
            } else if (a == SOFTMAX){
                m.data[i][j] = exp(x);
            }
            sum += m.data[i][j];
        }
        if (a == SOFTMAX) {
            for (j = 0; j < m.cols; ++j) {
                m.data[i][j] = m.data[i][j] / sum;
            }
        }
    }
}

// Calculates the gradient of an activation function and multiplies it into
// the delta for a layer
// matrix m: an activated layer output
// ACTIVATION a: activation function for a layer
// matrix d: delta before activation gradient
void gradient_matrix(matrix m, ACTIVATION a, matrix d)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            double x = m.data[i][j];
            if (a == LOGISTIC) {
                d.data[i][j] = x * (1 - x) * d.data[i][j];
            } else if (a == RELU) {
                double grad = x > 0 ? 1 : 0;
                d.data[i][j] = grad * d.data[i][j];
            } else if (a == LRELU) {
                double grad = x > 0 ? 1 : 0.1;
                d.data[i][j] = grad * d.data[i][j]; 
            } else {
                d.data[i][j] = 1 * d.data[i][j];
            }
        }
    }
}

// Forward propagate information through a layer
// layer *l: pointer to the layer
// matrix in: input to layer
// returns: matrix that is output of the layer
matrix forward_layer(layer *l, matrix in)
{

    l->in = in;  // Save the input for backpropagation

    // TODO: fix this! multiply input by weights and apply activation function.
    matrix out = matrix_mult_matrix(in, l->w);

    activate_matrix(out, l->activation);

    free_matrix(l->out);// free the old output
    l->out = out;       // Save the current output for gradient calculation
    return out;
}

// Backward propagate derivatives through a layer
// layer *l: pointer to the layer
// matrix delta: partial derivative of loss w.r.t. output of layer
// returns: matrix, partial derivative of loss w.r.t. input to layer
matrix backward_layer(layer *l, matrix delta)
{
    // 1.4.1
    // delta is dL/dy
    // TODO: modify it in place to be dL/d(xw)
    gradient_matrix(l->out, l->activation, delta);

    // 1.4.2
    // TODO: then calculate dL/dw and save it in l->dw
    free_matrix(l->dw);
    matrix dw = matrix_mult_matrix(transpose_matrix(l->in), delta); // replace this
    l->dw = dw;
    
    // 1.4.3
    // TODO: finally, calculate dL/dx and return it.
    matrix dx = matrix_mult_matrix(delta, transpose_matrix(l->w)); // replace this

    return dx;
}

// Update the weights at layer l
// layer *l: pointer to the layer
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_layer(layer *l, double rate, double momentum, double decay)
{
    // TODO:
    // Calculate Δw_t = dL/dw_t - λw_t + mΔw_{t-1}
    // save it to l->v
    
    matrix wt = copy_matrix(l->dw);
    for (int i = 0; i < wt.rows; i++) {
        for (int j = 0; j < wt.cols; j++) {
            wt.data[i][j] += - decay * l->w.data[i][j] + momentum * l->v.data[i][j];
        }
    }

    free_matrix(l->v);
    l->v = wt;

    // Update l->w
    for (int i = 0; i < wt.rows; i++) {
        for (int j = 0; j < wt.cols; j++) {
            l->w.data[i][j] += rate * wt.data[i][j];
        }
    }

    // Remember to free any intermediate results to avoid memory leaks

}

// Make a new layer for our model
// int input: number of inputs to the layer
// int output: number of outputs from the layer
// ACTIVATION activation: the activation function to use
layer make_layer(int input, int output, ACTIVATION activation)
{
    layer l;
    l.in  = make_matrix(1,1);
    l.out = make_matrix(1,1);
    l.w   = random_matrix(input, output, sqrt(2./input));
    l.v   = make_matrix(input, output);
    l.dw  = make_matrix(input, output);
    l.activation = activation;
    return l;
}

// Run a model on input X
// model m: model to run
// matrix X: input to model
// returns: result matrix
matrix forward_model(model m, matrix X)
{
    int i;
    for(i = 0; i < m.n; ++i){
        X = forward_layer(m.layers + i, X);
    }
    return X;
}

// Run a model backward given gradient dL
// model m: model to run
// matrix dL: partial derivative of loss w.r.t. model output dL/dy
void backward_model(model m, matrix dL)
{
    matrix d = copy_matrix(dL);
    int i;
    for(i = m.n-1; i >= 0; --i){
        matrix prev = backward_layer(m.layers + i, d);
        free_matrix(d);
        d = prev;
    }
    free_matrix(d);
}

// Update the model weights
// model m: model to update
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_model(model m, double rate, double momentum, double decay)
{
    int i;
    for(i = 0; i < m.n; ++i){
        update_layer(m.layers + i, rate, momentum, decay);
    }
}

// Find the index of the maximum element in an array
// double *a: array
// int n: size of a, |a|
// returns: index of maximum element
int max_index(double *a, int n)
{
    if(n <= 0) return -1;
    int i;
    int max_i = 0;
    double max = a[0];
    for (i = 1; i < n; ++i) {
        if (a[i] > max){
            max = a[i];
            max_i = i;
        }
    }
    return max_i;
}

// Calculate the accuracy of a model on some data d
// model m: model to run
// data d: data to run on
// returns: accuracy, number correct / total
double accuracy_model(model m, data d)
{
    matrix p = forward_model(m, d.X);
    int i;
    int correct = 0;
    for(i = 0; i < d.y.rows; ++i){
        if(max_index(d.y.data[i], d.y.cols) == max_index(p.data[i], p.cols)) ++correct;
    }
    return (double)correct / d.y.rows;
}

// Calculate the cross-entropy loss for a set of predictions
// matrix y: the correct values
// matrix p: the predictions
// returns: average cross-entropy loss over data points, 1/n Σ(-ylog(p))
double cross_entropy_loss(matrix y, matrix p)
{
    int i, j;
    double sum = 0;
    for(i = 0; i < y.rows; ++i){
        for(j = 0; j < y.cols; ++j){
            sum += -y.data[i][j]*log(p.data[i][j]);
        }
    }
    return sum/y.rows;
}


// Train a model on a dataset using SGD
// model m: model to train
// data d: dataset to train on
// int batch: batch size for SGD
// int iters: number of iterations of SGD to run (i.e. how many batches)
// double rate: learning rate
// double momentum: momentum
// double decay: weight decay
void train_model(model m, data d, int batch, int iters, double rate, double momentum, double decay)
{
    int e;
    for(e = 0; e < iters; ++e){
        data b = random_batch(d, batch);
        matrix p = forward_model(m, b.X);
        fprintf(stderr, "%06d: Loss: %f\n", e, cross_entropy_loss(b.y, p));
        matrix dL = axpy_matrix(-1, p, b.y); // partial derivative of loss dL/dy
        backward_model(m, dL);
        update_model(m, rate/batch, momentum, decay);
        free_matrix(dL);
        free_data(b);
    }
}


// Questions 
//
// 5.2.2.1 Why might we be interested in both training accuracy and testing accuracy? What do these two numbers tell us about our current model?
// Because the training accuracy is often a optimistic estimation, it is hard to tell the performance when using the model
// in real life example, a.k.a. the samples it has never seen. Therefore testing accuracy gives a good estimate about how
// good the model deal with unseen example. The training accuracy tells us if the model has converged yet, the testing accuracy tells us how
// well the model is performing.
//
// 5.2.2.2 Try varying the model parameter for learning rate to different powers of 10 (i.e. 10^1, 10^0, 10^-1, 10^-2, 10^-3) and training the model. What patterns do you see and how does the choice of learning rate affect both the loss during training and the final model accuracy?
// When the learning rate gets too big or too small the model will not converage in a long time （and maybe even
// encounter overflow due to parameter getting too big while calculating exponentials). Increasing the training rate can
// make the model converage faster, but given long enough time it is unable to get as good of a result as smaller training rate can acheive
// (around 94% accuracy for 0.01 training rate). Though when the training rate gets too small, although the model converages in a steady rate,
//  the waiting time for the model to finish converging just gets tedious.
//
// 5.2.2.3 Try varying the parameter for weight decay to different powers of 10: (10^0, 10^-1, 10^-2, 10^-3, 10^-4, 10^-5). How does weight decay affect the final model training and test accuracy?
// Having a decay parameter is essentially the regularization effect in traditional logistic regression. Making the decay weight large is efficient in stop the model
// from overfitting. Though the model will result in worse performance overall in favor of smaller weight. Having too little of decay weight guarantee the model to work
// as good as possible on the giving training set, but risks the potential of overfitting and performing worse on the test set.
//
// 5.2.3.1 Currently the model uses a logistic activation for the first layer. Try using a the different activation functions we programmed. How well do they perform? What's best?
// They perform roughly equally well (all around ~90% accuracy). Under the same parameter, LINEAR converges fastest, and RELU seem to perform the best (95%).
//
// 5.2.3.2 Using the same activation, find the best (power of 10) learning rate for your model. What is the training accuracy and testing accuracy?
// Learning rate of 0.1 works best for me. It results in training accuracy of 97% and testing accuracy of 95%.
//
// 5.2.3.3 Right now the regularization parameter `decay` is set to 0. Try adding some decay to your model. What happens, does it help? Why or why not may this be?
// It reduces the gap between training accuracy and testing accuracy. As mentioned above, it serve as a regularization technique.
//
// 5.2.3.4 Modify your model so it has 3 layers instead of two. The layers should be `inputs -> 64`, `64 -> 32`, and `32 -> outputs`. Also modify your model to train for 3000 iterations instead of 1000. Look at the training and testing error for different values of decay (powers of 10, 10^-4 -> 10^0). Which is best? Why?
// Decay of 0.01 seem to work the best. After increasing the size of the layer the modoel has a training accuracy at a blazing 100% and and testing accuracy of 97%.
// It is because a small decay can pervent the model from overfitting on the data, and still have a smooth enough interpolation between unseen data.
//
// 5.3.2.1 How well does your network perform on the CIFAR dataset?
// It performed at 37.1 accuracy for training set and 33.8 at the testing set. The testing program is surprisingly
// slow at loading training data so I have to reduce the size of training data by a factor of 10, as well as reducing the size of
// the training model. I would expect the model to perform much better (around 50-70%) with larger model and better hyper-parameters.
//



