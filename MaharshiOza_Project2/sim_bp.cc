#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_bp.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;
#include <math.h>



void bimodal_predictor_setup(int M2)
{
    M2_f = 0;
    M2_f = M2;
    int prediction_table_size = pow(2, M2_f);
    bimodal_table = new int[prediction_table_size];
    //cout << "size" << prediction_table_size << endl;
    //initializing the predictoin table with default values.
    for (int i = 0; i < prediction_table_size; i++)
    {
        bimodal_table[i] = 2;
        //cout << "Array" << bimodal_table[i] << endl;
    }
 
}
int calculate_index(unsigned long int addr)
{
    int indexMask = pow(2, M2_f) - 1;
    return (addr & indexMask);
}
void bimodal_predictor_update(unsigned long int addr_, char true_predictions)
{
    //calculate index
    unsigned long int temp_addr = addr_ >> 2;
    //cout << "temp_addr" << temp_addr << endl;
    BM_Index = calculate_index(temp_addr);
    //cout << "BM_Index" << BM_Index << endl;
    if (bimodal_table[BM_Index] >= 2)
        prediction_outcome_BM_value = 't';      //change it to bool
    else if ((bimodal_table[BM_Index] <= 1) && (bimodal_table[BM_Index] >= 0))
        prediction_outcome_BM_value = 'n';

    //find the location (@PC)
    int prediction_outcome_BM_counter = bimodal_table[BM_Index];
    //cout << "Before Count" << prediction_outcome_BM_counter << endl;
    if (!(prediction_outcome_BM_value == true_predictions))
        miss_predictions_general++;

    if (true_predictions == 't') {
        if (bimodal_table[BM_Index] < 3) {
            bimodal_table[BM_Index] = bimodal_table[BM_Index] + 1;
           // cout << "After Count" << bimodal_table[BM_Index] << endl;
            
        }
        else
        {
            //Do nothing
           // cout << "After Count" << bimodal_table[BM_Index] << endl;
        }
    }
    else if (true_predictions == 'n') {
        if (bimodal_table[BM_Index] > 0) {
            bimodal_table[BM_Index] = bimodal_table[BM_Index] - 1;
           // cout << "After Count" << bimodal_table[BM_Index] << endl;
        }
        else
        {
            //Do nothing
           // cout << "After Count" << bimodal_table[BM_Index] << endl;
        }
    }
}

void gshare_predictor_setup(int M1, int N)
{
    M1_f = 0;
    bhr = 0;
    M1_f = M1;
    N_f = N;
    int prediction_table_size = pow(2, M1_f);
    gshare_table = new int[prediction_table_size];
    //cout << "size" << prediction_table_size << endl;
    //initializing the predictoin table with default values.
    for (int i = 0; i < prediction_table_size; i++)
    {
        gshare_table[i] = 2;
        //cout << i <<"Array" << gshare_table[i] << endl;
    }

}
int calculate_index_GS(unsigned long int addr)
{
    int indMask = pow(2, M1_f) - 1;
    int bhrMask = pow(2, N_f) - 1;

    int diff = M1_f - N_f;
    int Mask = (pow(2, diff) - 1);
    
    int Idx = addr & indMask;
    //cout << "Idx 1 -->" << Idx << endl;

    int temp = Idx & Mask;
    //cout << "temp" << temp << endl;

    Idx = (Idx >> diff) ^ (bhr & bhrMask);
    //cout << "Idx 2 -->" << Idx << endl;
    //cout << "bhr" << bhr << endl;
    Idx = (Idx << diff) | temp;
    //gsh_index = Idx;
    //cout << "Idx 3 -->" << Idx << endl;
    return Idx;
}
void gshare_predictor_update(unsigned long int addr_, char true_predictions)
{
    int isTaken_ = 0;
    ////++l;
    ////cout << "=" << l << "       " << hex << addr_ << endl;
    //calculate index
    unsigned long int temp_addr = addr_ >> 2;
    //cout << "temp_addr" << temp_addr << endl;
    GS_Index = calculate_index_GS(temp_addr);
    //cout << "GS_Index" << GS_Index << endl;
    if (gshare_table[GS_Index] >= 2)
        prediction_outcome_GS_value = 't';      //change it to bool
    else if ((gshare_table[GS_Index] <= 1) && (gshare_table[GS_Index] >= 0))
        prediction_outcome_GS_value = 'n';

    //find the location (@PC)
    int prediction_outcome_GS_counter = gshare_table[GS_Index];
    //cout << "Before Count--" << prediction_outcome_GS_counter << endl;
    ////cout << "GP: " << dec << GS_Index << "  " << prediction_outcome_GS_counter << endl;
    if (!(prediction_outcome_GS_value == true_predictions))
        miss_predictions_general++;

    if (true_predictions == 't') {
        isTaken_ = 1;
        if (gshare_table[GS_Index] < 3) {
            gshare_table[GS_Index] = gshare_table[GS_Index] + 1;
             //cout << "After Count-t" << gshare_table[GS_Index] << endl;
            ////cout << "GU: " << GS_Index << "  " << gshare_table[GS_Index] << endl;
        }
        else
        {
            //Do nothing
           // cout << "After Count" << bimodal_table[BM_Index] << endl;
        }
    }
    else if (true_predictions == 'n') {
        isTaken_ = 0;
        if (gshare_table[GS_Index] > 0) {
            gshare_table[GS_Index] = gshare_table[GS_Index] - 1;
             //cout << "After Count-n" << gshare_table[GS_Index] << endl;
            ////cout << "GU: " << GS_Index << "  " << gshare_table[GS_Index] << endl;
        }
        else
        {
            //Do nothing
           // cout << "After Count" << bimodal_table[BM_Index] << endl;
        }
    }

    bhr = bhr >> 1;
    //cout << "bhr1 -->" << bhr << endl;
    int tempp = isTaken_ << (N_f - 1);
    bhr = bhr | tempp;
    //cout << "bhr2 -->" << bhr << endl;
    //cout << "_____________________" << endl;

}

void hybrid_predictor_setup(int K)
{
    K_f = K;
    int prediction_table_size = pow(2, K_f);
    hybrid_table = new int[prediction_table_size];
    //cout << "Csize" << prediction_table_size << endl;
    //initializing the predictoin table with default values.
    for (int i = 0; i < prediction_table_size; i++)
    {
        hybrid_table[i] = 1;
        //cout << i <<"Array" << gshare_table[i] << endl;
    }

}

int calculate_index_HY(unsigned long int addr)
{
    int indMask = pow(2, M1_f) - 1;
    int bhrMask = pow(2, N_f) - 1;

    int diff = M1_f - N_f;
    int Mask = (pow(2, diff) - 1);

    int Idx = addr & indMask;
    //cout << "Idx 1 -->" << Idx << endl;

    int temp = Idx & Mask;
    //cout << "temp" << temp << endl;

    Idx = (Idx >> diff) ^ (bhr & bhrMask);
    //cout << "Idx 2 -->" << Idx << endl;
    //cout << "bhr" << bhr << endl;
    Idx = (Idx << diff) | temp;
    //gsh_index = Idx;
    //cout << "Idx 3 -->" << Idx << endl;
    //return Idx;
    int hyMask = pow(2, K_f) - 1;
    int hybrid_Idx = addr & hyMask;
    return hybrid_Idx;
}
void hybrid_predictor_update(unsigned long int addr_, char true_predictions)
{
    int isTaken_ = 0;
    //////++l;
    //////cout << "=" << l << "       " << hex << addr_ << endl;
    //calculate index
    unsigned long int temp_addr = addr_ >> 2;
    //cout << "temp_addr" << temp_addr << endl;
    BM_Index = calculate_index(temp_addr);
    GS_Index = calculate_index_GS(temp_addr);
    HY_Index = calculate_index_HY(temp_addr);
    //cout << "HY_Index" << HY_Index << endl;
    

    if (gshare_table[GS_Index] >= 2)
        prediction_outcome_GS_value = 't';      //change it to bool
    else if ((gshare_table[GS_Index] <= 1) && (gshare_table[GS_Index] >= 0))
        prediction_outcome_GS_value = 'n';

    if (bimodal_table[BM_Index] >= 2)
        prediction_outcome_BM_value = 't';      //change it to bool
    else if ((bimodal_table[BM_Index] <= 1) && (bimodal_table[BM_Index] >= 0))
        prediction_outcome_BM_value = 'n';

    //find the location (@PC)
    int prediction_outcome_HY_counter = hybrid_table[HY_Index];
    //////cout << "   GP: " << dec << GS_Index << "  " << gshare_table[GS_Index] << endl;
    //////cout << "   BP: " << dec << BM_Index << "  " << bimodal_table[BM_Index] << endl;

    //cout << "Before Count--" << prediction_outcome_GS_counter << endl;
    //////cout << "   CP: " << dec << HY_Index << "  " << prediction_outcome_HY_counter << endl;

    //if (!(prediction_outcome_HY_value == true_predictions))
    //    miss_predictions_general++;

    if ((hybrid_table[HY_Index] <= 1) && (hybrid_table[HY_Index] >= 0))
    {
        if (true_predictions == 't') {
            isTaken_ = 1;
            if (bimodal_table[BM_Index] < 3) {
                bimodal_table[BM_Index] = bimodal_table[BM_Index] + 1;
                // cout << "After Count" << bimodal_table[BM_Index] << endl;
                //////cout << "   BU: " << BM_Index << "  " << bimodal_table[BM_Index] << endl;
            }
            else
            {
                //Do nothing
               // cout << "After Count" << bimodal_table[BM_Index] << endl;
            }
        }
        else if (true_predictions == 'n') {
            isTaken_ = 0;
            if (bimodal_table[BM_Index] > 0) {
                bimodal_table[BM_Index] = bimodal_table[BM_Index] - 1;
                // cout << "After Count" << bimodal_table[BM_Index] << endl;
                //////cout << "   BU: " << BM_Index << "  " << bimodal_table[BM_Index] << endl;
            }
            else
            {
                //Do nothing
               // cout << "After Count" << bimodal_table[BM_Index] << endl;
            }
        }
        if (!(prediction_outcome_BM_value == true_predictions))
            miss_predictions_general++;
    }
    else if (hybrid_table[HY_Index] >= 2)
    {
        if (true_predictions == 't') {
            isTaken_ = 1;
            if (gshare_table[GS_Index] < 3) {
                gshare_table[GS_Index] = gshare_table[GS_Index] + 1;
                //cout << "After Count-t" << gshare_table[GS_Index] << endl;
               //////cout << "    GU: " << GS_Index << "  " << gshare_table[GS_Index] << endl;
            }
            else
            {
                //Do nothing
               // cout << "After Count" << bimodal_table[BM_Index] << endl;
            }
        }
        else if (true_predictions == 'n') {
            isTaken_ = 0;
            if (gshare_table[GS_Index] > 0) {
                gshare_table[GS_Index] = gshare_table[GS_Index] - 1;
                //cout << "After Count-n" << gshare_table[GS_Index] << endl;
               //////cout << "    GU: " << GS_Index << "  " << gshare_table[GS_Index] << endl;
            }
            else
            {
                //Do nothing
               // cout << "After Count" << bimodal_table[BM_Index] << endl;
            }
        }
        if (!(prediction_outcome_GS_value == true_predictions))
            miss_predictions_general++;
    }


    bhr = bhr >> 1;
    //cout << "bhr1 -->" << bhr << endl;
    int tempp = isTaken_ << (N_f - 1);
    bhr = bhr | tempp;
    //cout << "bhr2 -->" << bhr << endl;
    //cout << "_____________________" << endl;

    //Update Hybrid counter Step 6 of Hybrid.
    if (!(prediction_outcome_GS_value == true_predictions)) {
        
        if (!(prediction_outcome_BM_value == true_predictions)) {
            //Do Nothing
        }
        else {
            if (hybrid_table[HY_Index] > 0) {
                hybrid_table[HY_Index] = hybrid_table[HY_Index] - 1;

            }
            //////cout << "   CU: " << HY_Index << "  " << hybrid_table[HY_Index] << endl;
        }
    
    }
    else {
        if (!(prediction_outcome_BM_value == true_predictions)) {
            if (hybrid_table[HY_Index] < 3) {
                hybrid_table[HY_Index] = hybrid_table[HY_Index] + 1;

            }
            //////cout << "   CU: " << HY_Index << "  " << hybrid_table[HY_Index] << endl;
        }
        else {
            //DN
        }

    }
    //if (!(prediction_outcome_GS_value == true_predictions)) {

    //    //if ((prediction_outcome_BM_value == true_predictions)) {
    //    //    if (hybrid_table[HY_Index] > 0) {
    //    //        hybrid_table[HY_Index] = hybrid_table[HY_Index] - 1;
    //    //        
    //    //    }
    //    //    //////cout << "   CU: " << HY_Index << "  " << hybrid_table[HY_Index] << endl;
    //    //}

    //}
    //if ((prediction_outcome_GS_value == true_predictions)) {

    //    if (!(prediction_outcome_BM_value == true_predictions)) {
    //        if (hybrid_table[HY_Index] < 3) {
    //            hybrid_table[HY_Index] = hybrid_table[HY_Index] + 1;
    //            
    //        }
    //        //////cout << "   CU: " << HY_Index << "  " << hybrid_table[HY_Index] << endl;
    //    }
    //}
    //if ((prediction_outcome_GS_value == true_predictions)) {

    //    if ((prediction_outcome_BM_value == true_predictions)) {
    //        //Do Nothing
    //    }

    //}
    
}


/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        type = 1;
        //cout << "\n BIMODAL_running";
        bimodal_predictor_setup(params.M2);
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        type = 2;
        //cout << "\n G-SHARE_running";
        gshare_predictor_setup(params.M1, params.N);
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        type = 3;
        //cout << "\n Hybrid_running";

        bimodal_predictor_setup(params.M2);
        gshare_predictor_setup(params.M1, params.N);
        hybrid_predictor_setup(params.K);
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        
        outcome = str[0];
        //if (outcome == 't')
        //    printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
        //else if (outcome == 'n')
        //    printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
        /*************************************
            Add branch predictor code here
        **************************************/
        if (type == 1) {
            //bimodal_predictor_setup(params.M2);
            //cout << "addr and outcome" << addr << outcome << endl;
            //cout << "Binomial_thing happening" << endl;
            bimodal_predictor_update(addr, outcome);
            branches_general++;
        }
        if (type == 2) {
            //bimodal_predictor_setup(params.M2);
            //cout << "addr and outcome" << addr << outcome << endl;
            //cout << "g-share thing happening" << endl;
            gshare_predictor_update(addr, outcome);
            branches_general++;
        }
        if (type == 3) {
            //bimodal_predictor_setup(params.M2);
            //cout << "addr and outcome" << addr << outcome << endl;
            //cout << "g-share thing happening" << endl;
            hybrid_predictor_update(addr, outcome);
            branches_general++;
        }
        //break;
    }
    //cout << "COMMAND \n";
    /*for (int i = 0; i < argc; i++)
        cout << argv[i] << " ";*/





    cout << "OUTPUT";
    cout << "\n number of predictions: " << branches_general;
    cout << "\n number of mispredictions: " << miss_predictions_general;
    cout << "\n misprediction rate: " << fixed << setprecision(2) << ((((float)(miss_predictions_general)) / branches_general) * 100) << "%";
    
    if (type == 3)
    {
        cout << "\n  FINAL CHOOSER CONTENTS ";
        int y = pow(2, params.K);
        for (int i = 0; i < y; i++)
            cout << "\n    " << i << "        " << hybrid_table[i];
    }

    

    if (type == 2 || type == 3) 
    {
        cout << "\n  FINAL GSHARE CONTENTS ";
        int y = pow(2, params.M1);
        for (int i = 0; i < y; i++)
            cout << "\n    " << i << "        " << gshare_table[i];
    }

    if (type == 1 || type == 3) {
        cout << "\n  FINAL BIMODAL CONTENTS ";
        int x = pow(2, params.M2);
        for (int i = 0; i < x; i++)
            cout << "\n    " << i << "        " << bimodal_table[i];
    }
    //// Open the output file stream to write to "ABC.txt"
    //std::ofstream outputFile("Project2.csv", ios::app);

    //// Check if the file was successfully opened
    //if (!outputFile) {
    //    std::cerr << "Error opening file!" << std::endl;
    //    return 1;  // Exit if the file couldn't be opened
    //}

    //// Replace std::cout with outputFile to write to the file
    ////outputFile << "\n OUTPUT";
    ////outputFile << "\n number of predictions: " << branches_general;
    ////outputFile << "\n number of mispredictions: " << miss_predictions_general ;
    //outputFile <<"M:,    " << params.M1 << "     ,N:,  " << params.N << "      ,misprediction rate:,               "
    //    << std::fixed << std::setprecision(2)
    //    << (((float)(miss_predictions_general)) / branches_general) * 100
    //    << "    ,%" << endl;

    ///*if (type == 3) {
    //    outputFile << "\n  FINAL CHOOSER CONTENTS ";
    //    int y = pow(2, params.K);
    //    for (int i = 0; i < y; i++) {
    //        outputFile << "\n    " << i << "        " << hybrid_table[i];
    //    }
    //}

    //if (type == 2 || type == 3) {
    //    outputFile << "\n  FINAL GSHARE CONTENTS ";
    //    int y = pow(2, params.M1);
    //    for (int i = 0; i < y; i++) {
    //        outputFile << "\n    " << i << "        " << gshare_table[i];
    //    }
    //}

    //if (type == 1 || type == 3) {
    //    outputFile << "\n  FINAL BIMODAL CONTENTS ";
    //    int x = pow(2, params.M2);
    //    for (int i = 0; i < x; i++) {
    //        outputFile << "\n    " << i << "        " << bimodal_table[i];
    //    }
    //}*/

    //// Close the file after writing
    //outputFile.close();

    //std::cout << "Output has been written to ABC.txt" << std::endl;

    return 0;
}


