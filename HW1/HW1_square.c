/*
Multiple Perceptron Neural Network Implementation in C

Square edition
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "pbPlots.h"
#include "supportLib.h"

#define MIN_COORD -500
#define MAX_COORD 500
#define SQUARE_LEN 250
#define MIN_WEIGHT -10
#define MAX_WEIGHT 10
#define NUM_INPUTS 3
#define NUM_OUTPUTS 1
#define TEST_FILENAME "square_plot.png"

void InitializeWeightMatrix(double** w, int numInputs, int numOutputs, double minWeight, double maxWeight);
void CalculateOutput(double** w, double* x, double* output, int numInputs, int numOutputs);
void UpdateWeights(double** w, double* x, double* t, double* o, double lambda, int numInputs, int numOutputs);
void Train(double** w, int iterations, double lambda, int numInputs, int numOutputs, double minCoord, double maxCoord);
void CorrectOutput(double* output, double x, double y);
void Test(double** w, int iterations, int numInputs, int numOutputs, double minCoord, double maxCoord);
void RandDoubleRange(double* output, double min, double max);
void AddCoord(double** coords, int newLen, double coord);

// correct points during testing
double* correct_test_x = NULL;
double* correct_test_y = NULL;
int num_correct = 0;
// incorrect points during testing
double* incorrect_test_x = NULL;
double* incorrect_test_y = NULL;
int num_incorrect = 0;

int main()
{
    double* weights[NUM_INPUTS]; // weight matrix
    int training_iterations = 0; // number of iterations during training
    int testing_iterations = 0; // number of iterations during testing
    double learning_rate = 0.0; // learning rate
    char do_single_step = 0;
    /*
    User input
    */
    printf("Enter the number of TRAINING iterations (0 for default, negative to quit): ");
    scanf("%d", &training_iterations);
    if(training_iterations < 0) exit(EXIT_SUCCESS);
    printf("\n");
    printf("Enter the number of TESTING iterations (0 for default, negative to quit): ");
    scanf("%d", &testing_iterations);
    if(testing_iterations < 0) exit(EXIT_SUCCESS);
    printf("\n");
    printf("Enter the LEARNING RATE (0 for default, negative to quit): ");
    scanf("%lf", &learning_rate);
    if(learning_rate < 0) exit(EXIT_SUCCESS);
    printf("\n");

    // CLEAR STDIN BUFFER :)
    int _tmp;
    while ((_tmp = getchar()) != '\n') {};

    printf("Would you like to use single-step mode after training and testing? (y)es/(N)o : ");

    scanf("%c", &do_single_step);
    if(do_single_step == 'y' || do_single_step == 'Y') {
        do_single_step = 1;
    } else {
        do_single_step = 0;
    }
    printf("\n");

    /*
    Do training and testing with provided values
    */
    srand(time(NULL)); // seed PRG with current timestamp (changes every second)
    // "weight matrix will be m x n where m is the number of inputs and n is the number of outputs."
    InitializeWeightMatrix(weights, NUM_INPUTS, NUM_OUTPUTS, MIN_WEIGHT, MAX_WEIGHT);
    printf("TRAINING\n----------\n");
    Train(weights, training_iterations, learning_rate, NUM_INPUTS, NUM_OUTPUTS, MIN_COORD, MAX_COORD);
    printf("TESTING\n----------\n");
    Test(weights, testing_iterations, NUM_INPUTS, NUM_OUTPUTS, MIN_COORD, MAX_COORD);
    printf("----------\n");
    /*
    Plotting the graphs.

    Library from https://github.com/InductiveComputerScience/pbPlots
    */
    _Bool success;
    RGBABitmapImageReference *imageRef = CreateRGBABitmapImageReference();
    StringReference *errorMessage = CreateStringReference(L"", 0);
    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();

    // INCORRECT TEST POINTS (RED)
    ScatterPlotSeries *incorrect_test = GetDefaultScatterPlotSeriesSettings();
    incorrect_test->color = CreateRGBColor(1, 0, 0); // GREEN
    incorrect_test->xs = incorrect_test_x;
    incorrect_test->ys = incorrect_test_y;
    incorrect_test->ysLength = num_incorrect;
    incorrect_test->xsLength = num_incorrect;
    incorrect_test->lineType = L"solid";
    incorrect_test->lineTypeLength = wcslen(incorrect_test->lineType);
    incorrect_test->pointType = L"dots";
    incorrect_test->pointTypeLength = wcslen(incorrect_test->pointType);
    incorrect_test->linearInterpolation = 0;

    // CORRECT TEST POINTS (GREEN)
    ScatterPlotSeries *correct_test = GetDefaultScatterPlotSeriesSettings();
    correct_test->color = CreateRGBColor(0, 1, 0); // GREEN
    correct_test->xs = correct_test_x;
    correct_test->ys = correct_test_y;
    correct_test->ysLength = num_correct;
    correct_test->xsLength = num_correct;
    correct_test->lineType = L"solid";
    correct_test->lineTypeLength = wcslen(correct_test->lineType);
    correct_test->pointType = L"dots";
    correct_test->pointTypeLength = wcslen(correct_test->pointType);
    correct_test->linearInterpolation = 0;

    // SETTINGS
    settings->xMin = -MIN_COORD;
    settings->yMin = -MIN_COORD;
    settings->xMax = MAX_COORD;
    settings->yMax = MAX_COORD;
    settings->xAxisAuto = 1;
    settings->showGrid = 1;
    settings->height = abs(MIN_COORD) + abs(MAX_COORD);
    settings->width = abs(MIN_COORD) + abs(MAX_COORD);

    // hack workaround for some dumb graphing code, please dont mind this.
    int num_scatterplots = 0;
    ScatterPlotSeries *both[] = {correct_test, incorrect_test};
    ScatterPlotSeries *crct[] = {correct_test};
    ScatterPlotSeries *incr[] = {incorrect_test};
    if(num_correct > 0 && num_incorrect > 0) {
        num_scatterplots = 2;
        settings->scatterPlotSeries = both;
    } else if(num_correct > 0) {
        num_scatterplots = 1;
        settings->scatterPlotSeries = crct;
    } else if(num_incorrect > 0) {
        num_scatterplots = 1;
        settings->scatterPlotSeries = incr;
    }
    settings->scatterPlotSeriesLength = num_scatterplots;

    // DRAW!!!
    success = DrawScatterPlotFromSettings(imageRef, settings, errorMessage);
    if(success) {
        size_t length;
		ByteArray *pngdata = ConvertToPNG(imageRef->image);
		WriteToFile(pngdata, TEST_FILENAME);
		DeleteImage(imageRef->image);
        printf("Output test result plot to %s\n", TEST_FILENAME);
    }
    else {
		fprintf(stderr, "Error: ");
		for(int i = 0; i < errorMessage->stringLength; i++){
			fprintf(stderr, "%c", errorMessage->string[i]);
		}
		fprintf(stderr, "\n");
	}
    /*
    Single-step
    */
    if(do_single_step) {
        double input[3];
        input[0] = 1;
        double output[NUM_OUTPUTS];
        while(1) {
            printf("Enter X coordinate value for single step: ");
            scanf("%lf", &input[1]);
            printf("\n");
            printf("Enter Y coordinate value for single step: ");
            scanf("%lf", &input[2]);
            printf("\n");
            CalculateOutput(weights, input, output, NUM_INPUTS, NUM_OUTPUTS);
            printf("output value: %lf\n", output[0]);
            printf("(%.02lf, %.02lf) is ", input[1], input[2]);
            // print region number based on binary output
            if(output[0] < 0.00001) printf("not in the square\n");
            else printf("in the square\n");
            printf("--------------------------------------------------\n");
        }
    }
    /*
    Free memory
    */
    FreeAllocations(); // plot allocations
    free(correct_test_x);
    free(correct_test_y);
    free(incorrect_test_x);
    free(incorrect_test_y);
    for(int m = 0; m < NUM_INPUTS; m++) { // weights
        free(weights[m]);
    }

    return 0;
}

/*
Allocate and initialize weights with random values using logic from Slide 75
*/
void InitializeWeightMatrix(double** w, int numInputs, int numOutputs, double minWeight, double maxWeight)
{
    for(int m = 0; m < numInputs; m++) {
        w[m] = malloc(sizeof(double) * numOutputs);
        for(int n = 0; n < numOutputs; n++) {
            RandDoubleRange(&w[m][n], minWeight, maxWeight);
        }
    }
}

/*
Calculate outputs logic taken from Slide 76
*/
void CalculateOutput(double** w, double* x, double* output, int numInputs, int numOutputs)
{
    if(w == NULL || x == NULL || output == NULL) {
        // defensive
        fprintf(stderr, "One or more of the vector args are NULL.\n");
        return;
    }
    for(int i = 0; i < numOutputs; i++) {
        output[i] = 0.0;
        for(int j = 0; j < numInputs; j++) {
            output[i] += x[j] * w[j][i];
        }
        if(output[i] > 0) output[i] = 1;
        else output[i] = 0;
    }
}

/*
Update weights logic taken from Slide 77
*/
void UpdateWeights(double** w, double* x, double* t, double* o, double lambda, int numInputs, int numOutputs)
{
    for(int n = 0; n < numOutputs; n++) {
        for(int m = 0; m < numInputs; m++) {
            // e = t - o
            // w_new = w_old + lambda * e * x
            w[m][n] += lambda * (t[n] - o[n]) * x[m];
        }
    }
}

/*
Train the MLP NN
*/
void Train(double** w, int iterations, double lambda, int numInputs, int numOutputs, double minCoord, double maxCoord)
{
    double* output = malloc(sizeof(double) * numOutputs); // output vector
    double* correct = malloc(sizeof(double) * numOutputs); // test vector
    int inarow = 0;
    int numcorrect = 0;
    double* x = malloc(sizeof(double) * numInputs); // input vector
    x[0] = 1; // bias
    for(int i = 0; i < iterations; i++) {
        for(int j = 0; j < numInputs; j++) { // fill x,y with random numbers
            RandDoubleRange(&x[j], minCoord, maxCoord);
        }
        // calculate output vector
        CalculateOutput(w, x, output, numInputs, numOutputs);
        // calculate the correct output
        CorrectOutput(correct, x[1], x[2]);
        // update weights
        if(correct[0] - output[0] < 0.000001) {
            inarow++;
            numcorrect++;
        } else {
            inarow = 0;
            UpdateWeights(w, x, correct, output, lambda, numInputs, numOutputs);
        }
    }
    printf("In-a-row: %d\n", inarow);
    printf("Number correct: %d\n", numcorrect);
    printf("Percent correct: %.02f\n", ((double)numcorrect/iterations) * 100);
    free(output);
    free(correct);
    free(x);
}

/*
Test the MLP NN
*/
void Test(double** w, int iterations, int numInputs, int numOutputs, double minCoord, double maxCoord)
{
    double* output = malloc(sizeof(double) * numOutputs); // output vector
    double* correct = malloc(sizeof(double) * numOutputs); // test vector
    double* x = malloc(sizeof(double) * numInputs); // input vector
    x[0] = 1; // bias
    // fill x,y with random numbers
    for(int i = 0; i < iterations; i++) {
        for(int j = 1; j < numInputs; j++) RandDoubleRange(&x[j], minCoord, maxCoord);
        CalculateOutput(w, x, output, numInputs, numOutputs);
        CorrectOutput(correct, x[1], x[2]);
        /*
        Essentially, it counts the number of correct and incorrect values and stores the coordinates
        in lists. The library I used to graph training needed an exact length array as well as the
        length. If there was one value uninitialized, it wouldn't make the graph image!
        */
        // if result is correct
        if(correct[0] - output[0] < 0.000001) {
            num_correct++;
            // add coordinate to coordinate array
            AddCoord(&correct_test_x, num_correct, x[1]);
            AddCoord(&correct_test_y, num_correct, x[2]);
        }
        // if result is incorrect
        else {
            num_incorrect++;
            // add coordinate to coordinate array
            AddCoord(&incorrect_test_x, num_incorrect, x[1]);
            AddCoord(&incorrect_test_y, num_incorrect, x[2]);
        }
    }
    printf("Iterations: %d\n", iterations);
    printf("Correct: %d\n", num_correct);
    printf("Percent: %.04f%%\n", ((double)num_correct/iterations) * 100);
    free(output);
    free(correct);
    free(x);
};

/*
Calculate correct output, based on Slide 79, for error calculation
*/
void CorrectOutput(double* t, double x, double y)
{
    if(x <= SQUARE_LEN && x >= 0 && y <= SQUARE_LEN && y >= 0) {
        t[0] = 1;
    }
    else t[0] = 0; 
}

/*
Generates a random double in a specified range (min, max)

MIT licensed, see code below for more information:
https://github.com/portfoliocourses/c-example-code/blob/main/random_double.c
*/
void RandDoubleRange(double* output, double min, double max)
{
    *output = min + ((max - min) * (((double) rand()) / RAND_MAX));
}

/*
Dynamic memory allocation for graph coordinate arrays
*/
void AddCoord(double** coords, int newLen, double coord)
{   // allocate or reallocate depending if NULL
    if(*coords == NULL) *coords = malloc(sizeof(double) * newLen);
    else {
        double * temp = realloc(*coords, sizeof(double) * newLen);
        if(!temp) {
            printf("Error when reallocating graph coordinate.\n");
            exit(EXIT_FAILURE);
        }
        *coords = temp;
    }
    // set value
    (*coords)[newLen - 1] = coord;
}


