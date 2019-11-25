#include "hmm.h"
#include <math.h>

int main(int argc, char *argv[])
{
/*
	HMM hmms[5];
	load_models( "modellist.txt", hmms, 5);
	dump_models( hmms, 5);
*/


	char* model_list_name = argv[1];
	char* test_data_name = argv[2];
	char* result_name = argv[3];
	
	
	HMM hmms[5];
	load_models(model_list_name , hmms, 5);

	viterbi_algorithm(hmms , test_data_name , result_name, 5);
	
	//printf("%f\n", log(1.5) ); // make sure the math library is included
	return 0;
}
