#ifndef SIM_BP_H
#define SIM_BP_H

typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;   //M2 is the number of PC bits used to index the bimodal table. 
    unsigned long int N;
    char*             bp_name;
}bp_params;

// Put additional data structures here as per your requirement
int* bimodal_table = NULL;
int* gshare_table = NULL;
int* hybrid_table = NULL;

char prediction_outcome_BM_value;
char prediction_outcome_GS_value;
char prediction_outcome_HY_value;

int M2_f, M1_f, N_f, K_f;

int BM_Index;
int GS_Index;
int HY_Index;

int type = 0;
char true_predictions;          //this represents the actual output of the predictions.

void bimodal_predictor_setup(int);
void bimodal_predictor_update(unsigned long int, char);

void hybrid_predictor_setup(unsigned long int, char);
void hybrid_predictor_update(unsigned long int, char);


int calculate_index(unsigned long int);
int calculate_index_GS(unsigned long int);
int calculate_index_HY(unsigned long int);


int miss_predictions_general = 0;
int branches_general;
int bhr = 0;
//int temp;
int isTaken = 0;
int l = 0;
#endif
