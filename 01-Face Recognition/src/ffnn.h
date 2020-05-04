//
// Created by jamie on 4/21/20.
//

#ifndef FFNNFACERECOGNITION_FFNN_H
#define FFNNFACERECOGNITION_FFNN_H

class ffnn {
private:
    int input_n{};                  /* number of input units */
    int hidden_n{};                 /* number of hidden units */
    int output_n{};                 /* number of output units */
    double *hidden_units{};         /* the hidden units */

    double *hidden_delta{};         /* storage for hidden unit error */
    double *output_delta{};         /* storage for output unit error */

    double **input_weights{};       /* weights from input to hidden layer */
    double **hidden_weights{};      /* weights from hidden to output layer */

    /*** The next two are for momentum ***/
    double **input_prev_weights{};  /* previous change on input to hidden wgt */
    double **hidden_prev_weights{}; /* previous change on hidden to output wgt */

    void internal_build(int n_in, int n_hidden, int n_out);

    static double dpn1();

    static void put_zero_weights(double **w, int m, int n);

    static void put_random_weights(double **w, int m, int n);

    static double squash(double x);

    static void layerForward(double *l1, double *l2, double **conn, int n1, int n2);

    static void hiddenError(double *delta_h, int nh, const double *delta_o, int no,
                     double **who, const double *hidden, double *err);

    static void outputError(double *delta, const double *tgt, const double *opt, int nj, double *err);

    static void adjustWeights(const double *delta, int nDelta, double *ly, int nly, double **w, double **oldW, double eta,
                              double momentum);

public:
    double *input_units{};          /* the input units */
    double *output_units{};         /* the output units */
    double *target{};               /* storage for target vector */

    int getInputN() const;

    void setInputN(int inputN);

    int getHiddenN() const;

    void setHiddenN(int hiddenN);

    int getOutputN() const;

    void setOutputN(int outputN);

    ffnn();

    ffnn(int n_in, int n_hidden, int n_out);

    void feedforward();

    void trainOnce(double eta, double momentum, double *eo, double *eh);

    void saveModel(char *filename);

    bool readModel(char *filename);
};
#endif //FFNNFACERECOGNITION_FFNN_H
