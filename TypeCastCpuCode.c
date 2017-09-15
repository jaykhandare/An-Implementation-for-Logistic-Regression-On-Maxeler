/**
 * Document: MaxCompiler Tutorial (maxcompiler-tutorial.pdf)
 * Chapter: 6      Example: 1      Name: Typecast
 * MaxFile name: TypeCast
 * Summary:
 *        Casts from an unsigned int to a float and back.
 */
#include "Maxfiles.h"
#include <MaxSLiCInterface.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#define SIZE 512

int i;

float x[SIZE],y[SIZE],
      dev_x[SIZE],dev_y[SIZE],
      dev_xy[SIZE],dev_x2[SIZE],
      dev_xy_dfe[SIZE],dev_x2_dfe[SIZE];

float sum_x,sum_y,
      mean_x,mean_y;
      
float cpu_b0,cpu_b1,
      cpu_p,cpu_odds,
      dfe_p,dfe_odds,
      dfe_b0,dfe_b1,
      rhs;

void dataEntry(){
    printf("\nNotice :\n\tIn this model, y has binary,dichotomous,and mutual exclusive values.(i.e. y = 0 or y = 1)\n");

    for(i = 0; i < SIZE; i++){
        printf("x : ");
        scanf("%f",&x[i]);
        printf("y : ");
        scanf("%f",&y[i]);
    }
}

void dataGen(){
    time_t t;
    srand((unsigned) time(&t));

    for(i = 0; i < SIZE; i++){
        x[i] = i + 1;

        if(i < SIZE/3)
            y[i] = 0;
        else if(i < SIZE/1.5)
            y[i] = rand() % 2;
        else
            y[i] = 1;
    }
}

float sum(float * ser){
    float total = 0;

    for(i = 0; i < SIZE; i++)
        total = total + ser[i];

    return total;
}

/*  CPU methods */

void logRegress_CPU(float val){
    sum_x = 0; sum_y = 0; mean_x = 0; mean_y = 0;
    cpu_b0 = 0; cpu_b1 = 0; cpu_p = 0; cpu_odds = 0; rhs = 0;

    for(i = 0; i < SIZE; i++){
        sum_x = sum_x + x[i];
        sum_y = sum_y + y[i];
    }

    mean_x = sum_x / SIZE;
    mean_y = sum_y / SIZE;

    for(i = 0; i < SIZE; i++){
        dev_x[i] = x[i] - mean_x;
        dev_y[i] = y[i] - mean_y;
        dev_xy[i] = dev_x[i] * dev_y[i];
        dev_x2[i] = dev_x[i] * dev_x[i];
    }    
    cpu_b1 = sum(dev_xy)/ sum(dev_x2);
    cpu_b0 = mean_y - cpu_b1 * mean_x;

    rhs = cpu_b0 + (cpu_b1 * (val));
    cpu_p = exp(rhs) / (1 + exp(rhs));
    cpu_odds = cpu_p / (1 - cpu_p);

}

void printResults_CPU(){
    printf("\n***CPU code calculations***\n");
    printf("\nThe equation of line : y = %2.2f + (%2.2f)x\n",cpu_b0,cpu_b1);

    /* Accuracy Check   */
    if(mean_y == cpu_b0 + cpu_b1 * mean_x)
        printf("\nThis line passes through Centroid(%2.2f,%2.2f)\n",mean_x,mean_y);
}

/*  DFE methods */

void logRegress_DFE(float check_val){
    sum_x = 0; sum_y = 0; mean_x = 0; mean_y = 0;
    dfe_p = 0; dfe_odds = 0; rhs = 0;

    for(i = 0; i < SIZE; i++){
        sum_x = sum_x + x[i];
        sum_y = sum_y + y[i];
    }

    mean_x = sum_x / SIZE;
    mean_y = sum_y / SIZE;

	TypeCast(SIZE, mean_x, mean_y, x, y, dev_xy_dfe, dev_x2_dfe);
/*  inputs : SIZE(int), mean_x(float), mean_y(float), x(float[]), y(float[])
    outputs : dev_xy_dfe(float[]),dev_x2_dfe(float[])
*/
    dfe_b1 = sum(dev_xy_dfe) / sum(dev_x2_dfe);
    dfe_b0 = mean_y - dfe_b1 * mean_x;

    rhs = dfe_b0 + (dfe_b1 * check_val);
    dfe_p = exp(rhs) / (1 + exp(rhs));
    dfe_odds = dfe_p / (1 - dfe_p);
}

void printResults_DFE(){
    printf("\n***DFE code calculations***\n");
    printf("\nThe equation of line : y = %2.2f + (%2.2f)x\n",dfe_b0,dfe_b1);

    /* Accuracy Check   */
    if(mean_y == dfe_b0 + dfe_b1 * mean_x)
        printf("\nThis line passes through Centroid(%2.2f,%2.2f)\n",mean_x,mean_y);
}
void correctResults(){
        printf("\n\n***********************RESULTS***************************\n");
        printf("\nCPU output = DFE output\n");
        printf("\nThe equation for line of regression : y = %2.2f + (%2.2f)x\n",dfe_b0,dfe_b1);
        printf("\nThe equation for logit() : y = (e ^ k) / (1 + (e ^ k)) \n\twhere k = (%2.2f + x * %2.2f)",dfe_b0,dfe_b1);
        printf("\nIntercept = (%2.2f) \t Slope = (%2.2f)\n",dfe_b0,dfe_b1);
        printf("\nThis line having equation given above passes through Centroid(%2.2f,%2.2f) which shows the correctness.\n",mean_x,mean_y);
        printf("\nFor x = %2.2f,\t Probability = %2.2f\t Odds = %2.2f\n",val,dfe_p,dfe_odds);
        if(dfe_p < 0.5) dfe_p = 0;
        else dfe_p = 1;    
        printf("\nHence, as per Logistic model, probability = %2.2f\n",dfe_p);
        printf("\n\n*********************END OF RESULTS***********************\n");
}

int main()
{
    float val = 240;

    dataGen();

//    dataEntry();

    /* DFE methods running  */

    logRegress_DFE(val);

    /* CPU methods running  */

    logRegress_CPU(val);

    /* Verification */
    if(cpu_b1 != dfe_b1 || cpu_b0 != dfe_b0){
        printResults_DFE();
        printResults_CPU();
    }
    else correctResults();
 
    printf("\n");
    return 0;
}
