#include "hmm.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
/*
	HMM hmms[5];
	load_models( "modellist.txt", hmms, 5);
	dump_models( hmms, 5);
*/
	
	//./train iteration model_init.txt seq_model_01.txt model_01.txt
	
	if(argc != 5)
	{
		printf("invalid argument\n");
		exit(1);
	}
	int train_iteration = atoi(argv[1]);
	char* model_init_name = argv[2];
	char* train_data_name = argv[3];
	char* model_name = argv[4];
	
	HMM hmm_initial;
	loadHMM( &hmm_initial, model_init_name );
	dumpHMM( stderr, &hmm_initial );
	
	forward_algorithm(&hmm_initial , train_data_name,model_name,train_iteration);
	
	//printf("%f\n", log(1.5) ); // make sure the math library is included
	return 0;
}

