//
// Created by jamie on 4/21/20.
//

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ffnn.h"
using namespace std;

#define cp(to,from,len)\
{\
   char *_to,*_from;\
   int _i,_l;\
   _to = (char *)(to);\
   _from = (char *)(from);\
   _l = (len);\
   for (_i = 0; _i < _l; _i++) *_to++ = *_from++;\
}

#define ABS(x)          (((x) > 0.0) ? (x) : (-(x)))

double ffnn::dpn1() {
    return ((((double) rand()/ (double) RAND_MAX) * 2.0) - 1.0);;
}

double ffnn::squash(double x) {
    double rs = (1.0 / (1.0 + exp(-x)));
    return rs;
}

void ffnn::put_zero_weights(double **w, int m, int n)
{
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            w[i][j] = 0.0;
        }
    }
}

void ffnn::put_random_weights(double **w, int m, int n) {
    for(int i = 0; i <= m; i++) {
        for(int j = 0; j <= n; j++) {
            w[i][j] = dpn1();
        }
    }
}

double *alloc_1d_dbl(int n)
{
    double *na;

    na = (double *) malloc ((unsigned) (n * sizeof (double)));
    if (na == NULL) {
        printf("ALLOC_1D_DBL: Couldn't allocate array of doubles\n");
        return (NULL);
    }
    return (na);
}

double **new2dDArray(int m, int n) {
    int i;
    double **na;

    na = (double **) malloc ((unsigned) (m * sizeof (double *)));
    if (na == nullptr) {
        printf("ALLOC_2D_DBL: Couldn't allocate array of dbl pointers\n");
        return (nullptr);
    }

    for (i = 0; i < m; i++) {
        na[i] = alloc_1d_dbl(n);
    }

    return (na);
}

int ffnn::getInputN() const {
    return input_n;
}

void ffnn::setInputN(int inputN) {
    input_n = inputN;
}

int ffnn::getHiddenN() const {
    return hidden_n;
}

void ffnn::setHiddenN(int hiddenN) {
    hidden_n = hiddenN;
}

int ffnn::getOutputN() const {
    return output_n;
}

void ffnn::setOutputN(int outputN) {
    output_n = outputN;
}

void ffnn::internal_build(int n_in, int n_hidden, int n_out) {
    setInputN(n_in);
    setHiddenN(n_hidden);
    setOutputN(n_out);

    input_units = alloc_1d_dbl(getInputN() + 1);
    hidden_units = alloc_1d_dbl(getHiddenN() + 1);
    output_units = alloc_1d_dbl(getOutputN() + 1);

    hidden_delta = alloc_1d_dbl(getHiddenN() + 1);
    output_delta = alloc_1d_dbl(getOutputN() + 1);
    target = alloc_1d_dbl(getOutputN() + 1);

    input_weights = new2dDArray(getInputN() + 1, getHiddenN() + 1);
    hidden_weights = new2dDArray(getHiddenN() + 1, getOutputN() + 1);

    input_prev_weights = new2dDArray(getInputN() + 1, getHiddenN() + 1);
    hidden_prev_weights = new2dDArray(getHiddenN() + 1, getOutputN() + 1);
}

ffnn::ffnn(int n_in, int n_hidden, int n_out) {
    internal_build(n_in, n_hidden, n_out);
    srand(time(nullptr));
#ifdef INITZERO
    put_zero_weights(input_weights, getInputN(), getHiddenN());
#else
    put_random_weights(input_weights, getInputN(), getHiddenN());
#endif
    put_random_weights(hidden_weights, getHiddenN(), getOutputN());
    put_zero_weights(input_prev_weights, getInputN(), getHiddenN());
    put_zero_weights(hidden_prev_weights, getHiddenN(), getOutputN());
}

void ffnn::saveModel(char *filename) {

    int n1, n2, n3, mem_cnt;
    double dValue, **w;
    char *mem;
    FILE *modelFile;

    if((modelFile = fopen(filename, "wb")) == nullptr) {
        printf("FFNN_SAVE: Cannot internal_build '%s'\n", filename);
        return;
    }

    n1 = getInputN();
    n2 = getHiddenN();
    n3 = getOutputN();

    printf("Saving %dx%dx%d network to '%s'\n", n1, n2, n3, filename);
    fflush(stdout);

    fwrite(&n1, sizeof(int), 1, modelFile);
    fwrite(&n2, sizeof(int), 1, modelFile);
    fwrite(&n3, sizeof(int), 1, modelFile);

    mem_cnt = 0;
    w = input_weights;
    mem = (char *) malloc ((unsigned) ((n1+1) * (n2+1) * sizeof(double)));
    for(int i = 0; i <= n1; i++) {
        for(int j = 0; j <= n2; j++) {
            dValue = w[i][j];
            cp(&mem[mem_cnt], &dValue, sizeof(double));
            mem_cnt += sizeof(double);
        }
    }
    fwrite(mem, sizeof(double), (n1+1) * (n2+1), modelFile);
    free(mem);

    mem_cnt = 0;
    w = hidden_weights;
    mem = (char *) malloc ((unsigned) ((n2+1) * (n3+1) * sizeof(double)));
    for (int i = 0; i <= n2; i++) {
        for (int j = 0; j <= n3; j++) {
            dValue = w[i][j];
            cp(&mem[mem_cnt], &dValue, sizeof(double));
            mem_cnt += sizeof(double);
        }
    }
    fwrite(mem, sizeof(double), (n2+1) * (n3+1), modelFile);
    free(mem);
    fclose(modelFile);
}

bool ffnn::readModel(char *filename) {
    int n1, n2, n3, mem_cnt;
    char *mem;
    FILE *modelFile;

    if((modelFile = fopen(filename, "rb")) == nullptr) {
        printf("No such model file: '%s'\n", filename);
        return false;
    }

    printf("Reading '%s'\n", filename);  fflush(stdout);

    fread((char *) &n1, sizeof(int), 1, modelFile);
    fread((char *) &n2, sizeof(int), 1, modelFile);
    fread((char *) &n3, sizeof(int), 1, modelFile);

    internal_build(n1, n2, n3);

    printf("'%s' contains a %dx%dx%d network\n", filename, n1, n2, n3);
    printf("Reading input weights...");
    fflush(stdout);

    mem_cnt = 0;
    mem = (char *) malloc ((unsigned) ((n1+1) * (n2+1) * sizeof(double)));
    fread(mem, sizeof(double), (n1+1) * (n2+1), modelFile);
    for (int i = 0; i <= n1; i++) {
        for (int j = 0; j <= n2; j++) {
            cp(&(input_weights[i][j]), &mem[mem_cnt], sizeof(double));
            mem_cnt += sizeof(double);
        }
    }
    free(mem);

    printf("Done\nReading hidden weights...");
    fflush(stdout);

    mem_cnt = 0;
    mem = (char *) malloc ((unsigned) ((n2+1) * (n3+1) * sizeof(double)));
    fread(mem, sizeof(double), (n2+1) * (n3+1), modelFile);
    for (int i = 0; i <= n2; i++) {
        for (int j = 0; j <= n3; j++) {
            cp(&(hidden_weights[i][j]), &mem[mem_cnt], sizeof(double));
            mem_cnt += sizeof(double);
        }
    }
    free(mem);
    fclose(modelFile);

    printf("Done\n");  fflush(stdout);

    put_zero_weights(input_prev_weights, n1, n2);
    put_zero_weights(hidden_prev_weights, n2, n3);

    return true;
}

void ffnn::feedforward() {
    layerForward(input_units, hidden_units, input_weights, getInputN(), getHiddenN());
    layerForward(hidden_units, output_units, hidden_weights, getHiddenN(), getOutputN());
}

void ffnn::layerForward(double *l1, double *l2, double **conn, int n1, int n2) {
    double sum;

    l1[0] = 1.0;

    for(int j = 1; j <= n2; j++) {
        sum = 0.0;
        for(int k = 1; k <= n1; k++) {
            sum += conn[k][j] * l1[k];
        }
        l2[j] = squash(sum);
    }
}

void ffnn::outputError(double *delta, const double *tgt, const double *opt, int nj, double *err) {
    double o, t, err_sum;

    err_sum = .0;

    for(int j = 1; j <= nj; j++) {
        o = opt[j];
        t = tgt[j];
        delta[j] = o * (1.0 - o) * (t - o);
        err_sum += ABS(delta[j]);
    }

    *err = err_sum;
}

void ffnn::hiddenError(double *delta_h, int nh, const double *delta_o, int no,
                       double **who, const double *hidden, double *err) {
    double h, sum, err_sum;

    err_sum = .0;

    for (int j = 1; j <= nh; j++) {
        h = hidden[j];
        sum = .0;

        for(int k =1; k <= no; k++) {
            sum += delta_o[k] * who[j][k];
        }
        delta_h[j] = h * (1.0 - h) * sum;
        err_sum += ABS(delta_h[j]);
    }

    *err = err_sum;
}

void ffnn::adjustWeights(const double *delta, int nDelta, double *ly, int nly, double **w, double **oldW, double eta,
                         double momentum) {
    double n_dw;

    ly[0] = 1.0;

    for(int j = 1; j <= nDelta; j++) {
        for(int k = 0; k <= nly; k++) {
            n_dw = ((eta) *delta[j] * ly[k] + (momentum * oldW[k][j]));
            w[k][j] += n_dw;
            oldW[k][j] = n_dw;
        }
    }
}

void ffnn::trainOnce(double eta, double momentum, double *eo, double *eh) {
    double out_err, hid_err;

    layerForward(input_units, hidden_units, input_weights, getInputN(), getHiddenN());
    layerForward(hidden_units, output_units, hidden_weights, getHiddenN(), getOutputN());

    outputError(output_delta, target, output_units, getOutputN(), &out_err);
    hiddenError(hidden_delta, getHiddenN(), output_delta, getOutputN(), hidden_weights, hidden_units, &hid_err);
    *eo = out_err;
    *eh = hid_err;

    adjustWeights(output_delta, getOutputN(), hidden_units, getHiddenN(),
            hidden_weights, hidden_prev_weights, eta, momentum);
    adjustWeights(hidden_delta, getHiddenN(), input_units, getInputN(),
            input_weights, input_prev_weights, eta, momentum);
}

ffnn::ffnn() {
    printf("Created a new feedforward artificial neural network pointer, but not yet initialized.");
}
