#include <iostream>
#include <cstdlib>
#include <cstring>
#include "ffnn.h"
#include "pgmImage.h"

enum frTyp {sunglasses, one20, pose};

bool singleImagePerformance(ffnn *network, double *errPtr);

double imageListPerformance(ffnn *network, IMAGELIST *il, frTyp recType, bool listErrs);

void outputListPredictions(ffnn *network, IMAGELIST *il, frTyp recType);

void ffnnBackProp(IMAGELIST *trainList, IMAGELIST *validList, IMAGELIST *testList,
                  int epochs, char *netName, int hn, int on, bool ifListErr, frTyp recType);

void printUsage(char *agv);

int main(int argc, char *argv[]) {
    char netName[256], trainName[256], validName[256], testName[256];
    IMAGELIST *trainList, *validList, *testList;
    int epochs, hiddenN, outputN;
    bool listErrors;

    epochs = 100;
    listErrors = false;
    hiddenN = 20;
    outputN = 20;
    netName[0] = trainName[0] = validName[0] = testName[0] = '\0';

    frTyp tp = sunglasses;

    if (argc < 2) {
        printUsage(argv[0]);
        return -1;
    }

    trainList = imgl_alloc();
    validList = imgl_alloc();
    testList = imgl_alloc();

    for (int idx = 1 ; idx < argc; idx++) {
        if(argv[idx][0] == '-') {
            switch (argv[idx][1]) {
                case 'n': strcpy(netName, argv[++idx]);
                    break;
                case 'e': epochs = atoi(argv[++idx]);
                    break;
                case 'r': strcpy(trainName, argv[++idx]);
                    break;
                case 'v': strcpy(validName, argv[++idx]);
                    break;
                case 't': strcpy(testName, argv[++idx]);
                    break;
                case 'T': listErrors = true; epochs = 0;
                    break;
                case 'h': hiddenN = atoi(argv[++idx]);
                    break;
                case 's': tp = sunglasses; outputN = 2;
                    break;
                case 'f': tp = one20; outputN = 20;
                    break;
                case 'p': tp = pose; hiddenN = 6; outputN = 4;
                default:
                    printf("Unknown switch '%c'\n", argv[idx][1]);
                    break;
            }
        }
    }

    if (trainName[0] != '\0') imgl_load_images_from_textfile(trainList, trainName);
    if (validName[0] != '\0') imgl_load_images_from_textfile(validList, validName);
    if (testName[0] != '\0') imgl_load_images_from_textfile(testList, testName);

    /*** Show number of images in train, test1, test2 ***/
    printf("%d images in training set\n", trainList->n);
    printf("%d images in test1 set\n", validList->n);
    printf("%d images in test2 set\n", testList->n);

    ffnnBackProp(trainList, validList, testList, epochs, netName, hiddenN, outputN, listErrors, tp);

    return 0;
}

bool singleImagePerformance(ffnn *network, double *errPtr) {
    *errPtr = 0.0;
    double delta;

    for (int j = 1; j <= network->getOutputN() ; j++) {
        delta = network->target[j] - network->output_units[j];
        *errPtr += (0.5 * delta * delta);
    }

    for (int j = 1; j <= network->getOutputN() ; j++) {
        //If the target unit is on...
        if (network->target[j] > 0.5) {
            //If the output unit is on, then we correctly recognized this person!
            if (network->output_units[j] > 0.5) ; else {
                //otherwise, we didn't think it was this person and got it wrong...
                return false;
            }
        } else {
            //Else, the target unit is off...
            //If the output unit is on, then we mistakenly thought it was someone else
            if (network->output_units[j] > 0.5) {
                return false;
            } else ;
        }
    }
    // All passed, it is right!
    return true;
}

double imageListPerformance(ffnn *network, IMAGELIST *il, frTyp recType, bool listErrs) {
    double err, val;
    int correctCnt, iln;

    err = .0;
    correctCnt = 0;
    iln = il->n;

    if (iln > 0) {
        for (int i = 0; i < iln; i++) {

            //Load the image into input layer
            load_input_with_image(il->list[i], network);

            //feedforward pass
            network->feedforward();

            switch (recType) {

                case sunglasses: load_sunglasses_target(il->list[i], network);
                    break;
                case one20: load_20_face_target(il->list[i], network);
                    break;
                case pose: load_pose_target(il->list[i], network);
            }

            if(singleImagePerformance(network, &val)) {
                correctCnt++;
            }
            else if (listErrs) {
                printf("%s - outputs ", NAME(il->list[i]));

                for (int j = 1; j <= network->getOutputN(); j++) {
                    printf("%.3f ", network->output_units[j]);
                }

                putchar('\n');
            }

            err+=val;
        }

        err /= (double) iln;

        if (!listErrs) printf("%g %g ", ((double) correctCnt / (double) iln) * 100.0, err);
    }
    else {
        if (!listErrs) printf("0.0, 0.0 ");
    }
    return (double) correctCnt / (double) iln;
}

void outputListPredictions(ffnn *network, IMAGELIST *il, frTyp recType) {
    int iln = il->n;

    if (iln > 0) {
        for (int i = 0; i < iln; i++) {
            load_input_with_image(il->list[i], network);
            network->feedforward();
            printf("Prediction for image ' %s ':", NAME(il->list[i]));
            switch (recType) {

                case sunglasses:
                {
                    if(network->output_units[1] > .5) {
                        printf("On.\n");
                    }
                    else {
                        printf("Off.\n");
                    }

                    fflush(stdout);
                }
                    break;
                case one20:
                {
                    load_20_face_target(il->list[i], network);

                    int activatedCnt = 0;
                    int activatedUnitIdx = 0;

                    for (int j = 1; j <= 20; j++) {
                        if (network->output_units[j] > .5) {
                            activatedCnt++;
                            activatedUnitIdx = j;
                        }
                    }

                    if (activatedCnt == 1) {
                        char name[64];
                        name[0] = '\0';
                        switch(activatedUnitIdx) {
                            case 1: strcpy(name, "an2i");
                                break;
                            case 2: strcpy(name, "at33");
                                break;
                            case 3: strcpy(name, "boland");
                                break;
                            case 4: strcpy(name, "bpm");
                                break;
                            case 5: strcpy(name, "ch4f");
                                break;
                            case 6: strcpy(name, "cheyer");
                                break;
                            case 7: strcpy(name, "choon");
                                break;
                            case 8: strcpy(name, "danieln");
                                break;
                            case 9: strcpy(name, "glickman");
                                break;
                            case 10: strcpy(name, "karyadi");
                                break;
                            case 11: strcpy(name, "kawamura");
                                break;
                            case 12: strcpy(name, "kk49");
                                break;
                            case 13: strcpy(name, "megak");
                                break;
                            case 14: strcpy(name, "mitchell");
                                break;
                            case 15: strcpy(name, "night");
                                break;
                            case 16: strcpy(name, "phoebe");
                                break;
                            case 17: strcpy(name, "saavik");
                                break;
                            case 18: strcpy(name, "steffi");
                                break;
                            case 19: strcpy(name, "sz24");
                                break;
                            case 20: strcpy(name, "tammo");
                                break;
                            default:
                                break;
                        }

                        printf("%s\n", name);
                    }
                    else {
                        printf("Couldn't make a prediction.\n");
                    }

                    fflush(stdout);
                }
                    break;
                case pose:
                {
                    load_20_face_target(il->list[i], network);

                    int activatedCnt = 0;
                    int activatedUnitIdx = 0;

                    for (int j = 1; j <= 4; j++) {
                        if (network->output_units[j] > .5) {
                            activatedCnt++;
                            activatedUnitIdx = j;
                        }
                    }
                    if (activatedCnt == 1) {
                        char pos[64];
                        pos[0] = '\0';
                        switch(activatedUnitIdx) {
                            case 1: strcpy(pos, "straight");
                                break;
                            case 2: strcpy(pos, "up");
                                break;
                            case 3: strcpy(pos, "left");
                                break;
                            case 4: strcpy(pos, "right");
                                break;
                            default:
                                break;
                        }

                        printf("%s\n", pos);
                    }
                    else {
                        printf("Couldn't make a prediction.\n");
                    }

                    fflush(stdout);
                }
            }
        }
    }
}

void ffnnBackProp(IMAGELIST *trainList, IMAGELIST *validList, IMAGELIST *testList,
                  int epochs, char *netName, int hn, int on, bool ifListErr, frTyp recType) {
    IMAGE *initImg;
    ffnn *network = new ffnn();
    int trainN, imgSize;
    double outErr, hidErr, sumErr;

    trainN = trainList->n;

    if (!(network->readModel(netName))) {
        if (trainN > 0) {
            printf("Creating a new network '%s'\n", netName);
            initImg = trainList->list[0];
            imgSize = ROWS(initImg) * COLS(initImg);
            network = new ffnn(imgSize, hn, on);
        }
        else {
            printf("Need some images to train on, use -t\n");
            return;
        }
    }

    if (epochs > 0 && !ifListErr) {
        printf("Training underway (going to %d epochs)\n", epochs);
        printf("Will saveModel network with every better validation set performance\n");
        fflush(stdout);
    }

    printf("0 0.0 ");
    imageListPerformance(network, trainList, recType, false);
    double bestValPerf = imageListPerformance(network, validList, recType, false);
    imageListPerformance(network, testList, recType, false);

    // Training mode, else test mode.
    if (!ifListErr) {
        printf("\n");
        fflush(stdout);

        double valPerf;

        for(int e = 1; e <= epochs; e++) {
            printf("%d ", e);
            fflush(stdout);

            sumErr = .0;

            for (int i = 0; i < trainN; i++) {
                load_input_with_image(trainList->list[i], network);

                switch (recType) {

                    case sunglasses: load_sunglasses_target(trainList->list[i], network);
                        break;
                    case one20: load_20_face_target(trainList->list[i], network);
                        break;
                    case pose: load_pose_target(trainList->list[i], network);
                }

                network->trainOnce(.3, .3, &outErr, &hidErr);

                sumErr += (outErr + hidErr);
            }

            printf("%g ", sumErr);

            imageListPerformance(network, trainList, recType, false);
            valPerf = imageListPerformance(network, validList, recType, false);
            imageListPerformance(network, testList, recType, false);

            printf("\n");
            fflush(stdout);

            if (valPerf > bestValPerf) {
                network->saveModel(netName);
                bestValPerf = valPerf;
            }
        }

    }
    else {
        printf("\n\n The predictions for test image list is as follows:\n");
        outputListPredictions(network, testList, recType);

        printf("\n\nFailed to classify the following images from the training set:\n");
        imageListPerformance(network, trainList, recType, true);

        printf("\n\nFailed to classify the following images from the validation set:\n");
        imageListPerformance(network, validList, recType, true);

        printf("\n\nFailed to classify the following images from the test set:\n");
        imageListPerformance(network, testList, recType, true);
    }
}

void printUsage(char *agv) {
    printf("USAGE: %s\n", agv);
    printf("       [-n] <network file>\n");
    printf("       [-e] <number of epochs (default 100)>\n");
    printf("       [-r] <training set list>\n");
    printf("       [-v] <validation set list>\n");
    printf("       [-t] <testing set list>\n");
    printf("       [-T] <testing mode, if don't specify then training mode>\n");
    printf("       [-h] <hidden units number (default 20)>\n");
    printf("       [-s] <sunglasses detector (default)>\n");
    printf("       [-f] <1 out of 20 faces detector>\n");
}