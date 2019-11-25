#ifndef HMM_HEADER_
#define HMM_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
	
#ifndef MAX_STATE
#	define MAX_STATE	10
#endif

#ifndef MAX_OBSERV
#	define MAX_OBSERV	26
#endif

#ifndef MAX_SEQ
#	define	MAX_SEQ		200
#endif

#ifndef MAX_LINE
#	define MAX_LINE 	256
#endif

typedef struct{
   char *model_name;
   int state_num;					//number of state
   int observ_num;					//number of observation
   double initial[MAX_STATE];			//initial prob.
   double transition[MAX_STATE][MAX_STATE];	//transition prob.
   double observation[MAX_OBSERV][MAX_STATE];	//observation prob.
} HMM;

static FILE *open_or_die( const char *filename, const char *ht )
{
   FILE *fp = fopen( filename, ht );
   if( fp == NULL ){
      perror( filename);
      exit(1);
   }

   return fp;
}

static void loadHMM( HMM *hmm, const char *filename )
{
   int i, j;
   FILE *fp = open_or_die( filename, "r");

   hmm->model_name = (char *)malloc( sizeof(char) * (strlen( filename)+1));
   strcpy( hmm->model_name, filename );

   char token[MAX_LINE] = "";
   while( fscanf( fp, "%s", token ) > 0 )
   {
      if( token[0] == '\0' || token[0] == '\n' ) continue;

      if( strcmp( token, "initial:" ) == 0 ){
         fscanf(fp, "%d", &hmm->state_num );

         for( i = 0 ; i < hmm->state_num ; i++ )
            fscanf(fp, "%lf", &( hmm->initial[i] ) );
      }
      else if( strcmp( token, "transition:" ) == 0 ){
         fscanf(fp, "%d", &hmm->state_num );

         for( i = 0 ; i < hmm->state_num ; i++ )
            for( j = 0 ; j < hmm->state_num ; j++ )
               fscanf(fp, "%lf", &( hmm->transition[i][j] ));
      }
      else if( strcmp( token, "observation:" ) == 0 ){
         fscanf(fp, "%d", &hmm->observ_num );

         for( i = 0 ; i < hmm->observ_num ; i++ )
            for( j = 0 ; j < hmm->state_num ; j++ )
               fscanf(fp, "%lf", &( hmm->observation[i][j]) );
      }
   }
}

static void dumpHMM( FILE *fp, HMM *hmm )
{
   int i, j;

   //fprintf( fp, "model name: %s\n", hmm->model_name );
   fprintf( fp, "initial: %d\n", hmm->state_num );
   for( i = 0 ; i < hmm->state_num - 1; i++ )
      fprintf( fp, "%.5lf ", hmm->initial[i]);
   fprintf(fp, "%.5lf\n", hmm->initial[ hmm->state_num - 1 ] );

   fprintf( fp, "\ntransition: %d\n", hmm->state_num );
   for( i = 0 ; i < hmm->state_num ; i++ ){
      for( j = 0 ; j < hmm->state_num - 1 ; j++ )
         fprintf( fp, "%.5lf ", hmm->transition[i][j] );
      fprintf(fp,"%.5lf\n", hmm->transition[i][hmm->state_num - 1]);
   }

   fprintf( fp, "\nobservation: %d\n", hmm->observ_num );
   for( i = 0 ; i < hmm->observ_num ; i++ ){
      for( j = 0 ; j < hmm->state_num - 1 ; j++ )
         fprintf( fp, "%.5lf ", hmm->observation[i][j] );
      fprintf(fp,"%.5lf\n", hmm->observation[i][hmm->state_num - 1]);
   }
}

static int load_models( const char *listname, HMM *hmm, const int max_num )
{
   FILE *fp = open_or_die( listname, "r" );

   int count = 0;
   char filename[MAX_LINE] = "";
   while( fscanf(fp, "%s", filename) == 1 ){
      loadHMM( &hmm[count], filename );
      count ++;

      if( count >= max_num ){
         return count;
      }
   }
   fclose(fp);

   return count;
}

static void dump_models( HMM *hmm, const int num )
{
   int i = 0;
   for( ; i < num ; i++ ){ 
      //		FILE *fp = open_or_die( hmm[i].model_name, "w" );
      dumpHMM( stderr, &hmm[i] );
   }
}


static void viterbi_algorithm(HMM *hmm , const char *test_data_name , const char *result_data_name, int model_num)
{
	
	FILE *fp = open_or_die(test_data_name, "r");

	FILE *result_fp = open_or_die(result_data_name, "w");	

		
	
	const int state_N = hmm -> state_num;
	char token[MAX_LINE] = "";
	
	int label_arr[2505];
	double label_prob_arr[2505];
	int token_num = 0;
	
	int i=0,j=0,k=0,t=0,m=0;
	while( fscanf( fp, "%s", token ) > 0 )
	{
		const int observation_t = strlen(token);
		int observation_array[observation_t];
		
		for(i = 0 ; i < observation_t ; ++i)
		{
			if(token[i] == 'A'){observation_array[i] = 0;}
                        else if (token[i] == 'B'){observation_array[i] = 1;}
                        else if (token[i] == 'C'){observation_array[i] = 2;}
                        else if (token[i] == 'D'){observation_array[i] = 3;}
                        else if (token[i] == 'E'){observation_array[i] = 4;}
                        else if (token[i] == 'F'){observation_array[i] = 5;}
		}
		
		int label_m = 0;
		double label_prob = DBL_MIN;
		for(m = 0 ; m < model_num ; ++m)
		{
		
			double delta_matrix[observation_t][state_N];
			for(i = 0 ; i < state_N ; ++i)
			{
				delta_matrix[0][i] = hmm[m] . initial[i] * hmm[m] . observation[observation_array[0]][i];
			
			}
		
			for(t = 1 ; t < observation_t ; ++t)
			{
				for(j = 0 ; j < state_N ; ++j)
				{
					double max_val = DBL_MIN;
					for(i = 0 ; i <state_N ; ++i)
					{
						if(max_val < (delta_matrix[t-1][i] * hmm[m] .transition[i][j]))
						{
							max_val = (delta_matrix[t-1][i] * hmm[m] .transition[i][j]);
						}
					}
					delta_matrix[t][j] = max_val * hmm[m] . observation[observation_array[t]][j];
				}
			}
		
			double max_P_val = DBL_MIN;
			for(i = 0 ; i < state_N ; ++i)
			{	
				if(max_P_val < delta_matrix[observation_t-1][i]){max_P_val = delta_matrix[observation_t-1][i];}
			}
			
			if(max_P_val > label_prob)
			{
				label_m = m; label_prob = max_P_val;
			}
			
		}
		
		label_arr[token_num] = label_m;
		label_prob_arr[token_num] = label_prob;
		token_num ++;
		
	}


	for(i = 0 ; i < token_num ; ++i)
	{
		int print_index = label_arr[i]+1;
		
		fprintf( result_fp, "model_0%d.txt %e\n",print_index,label_prob_arr[i]);
	}
	
	

	
}


static void forward_algorithm(HMM *hmm , const char *data_name , const char *model_name , int iteration_num)
{

	const int state_N = hmm -> state_num;	
                
	int i = 0, j = 0, k = 0 , t = 0;
	FILE *fp = open_or_die(data_name, "r");
		
	for(int iteration = 0 ; iteration < iteration_num ; ++iteration)
	{
	
	char token[MAX_LINE] = "";
	int token_num = 0;
	
	
	double gamma_0_i_sum[state_N] ;
	double epsilon_allt_sum[state_N][state_N] ;
	double gamma_allt_sum[state_N];
	
	double gamma_event_sum[state_N][state_N];
	double gamma_allt_sum_b[state_N];
	
	rewind(fp);
	
	while( fscanf( fp, "%s", token ) > 0 )
   	{
			
		const int observation_t = strlen(token);
		int observation_array[observation_t];
		double alpha_matrix[observation_t][state_N];	
		double beta_matrix[observation_t][state_N];
	
		double gamma_matrix[observation_t][state_N];
		double epsilon_matrix[observation_t][state_N][state_N];
		
		for(i = 0 ; i < observation_t ; ++i)
		{
			if(token[i] == 'A'){observation_array[i] = 0;}
			else if (token[i] == 'B'){observation_array[i] = 1;}
			else if (token[i] == 'C'){observation_array[i] = 2;}
			else if (token[i] == 'D'){observation_array[i] = 3;}
			else if (token[i] == 'E'){observation_array[i] = 4;}
			else if (token[i] == 'F'){observation_array[i] = 5;}
		}
		
		// forword algorithm
		// compute forward variable alpha(t,i)
		
		for(i = 0 ; i < state_N ; ++i)
		{
			alpha_matrix[0][i] = (hmm -> initial[i]) * (hmm -> observation[observation_array[0]][i]);
		}
		for(i = 1 ; i < observation_t ; ++i)
		{
			for(j = 0 ; j < state_N ; ++j)
			{
				double transition_prob = 0;
				for(k = 0 ; k < state_N ; ++k)
				{
					transition_prob += (alpha_matrix[i-1][k] * hmm -> transition[k][j]);
				}	
				transition_prob *= hmm -> observation[observation_array[i]][j];
				alpha_matrix[i][j] = transition_prob;
			}
		
		}
		

		 // backward algorithm
		// compute backward variable beta(t,i)
		for(i = 0 ; i < state_N ; ++i)
		{
			beta_matrix[observation_t-1][i] = 1;
		}
		
		for(i = observation_t-2 ; i >= 0 ; --i)
                {
                        for(j = 0 ; j < state_N ; ++j)
                        {
                                double transition_prob = 0;
                                for(k = 0 ; k < state_N ; ++k)
                                {
                                        transition_prob += (beta_matrix[i+1][k] * hmm -> transition[j][k]) 
								* hmm -> observation[observation_array[i+1]][k];
                                }
                                beta_matrix[i][j] = transition_prob;
                        }

                }

		
		
		
		// gamma : double gamma_matrix[10000][MAX_SEQ][MAX_STATE];
		
		for(t = 0 ; t < observation_t ; ++t)
		{
			
			double sum_per_t = 0.0;
			for( i = 0 ; i < state_N ; ++i)
			{
				sum_per_t += (alpha_matrix[t][i] * beta_matrix[t][i]);
			}
			for( i = 0 ; i < state_N ; ++i)
			{
				gamma_matrix[t][i] 
				= (double) (alpha_matrix[t][i]*beta_matrix[t][i])/ (double)(sum_per_t) ;
			}
		}
		for(t = 0 ; t < observation_t-1 ; ++t)
		{
			double sum_per_t = 0.0;
			for( i = 0 ; i < state_N ; ++i)
			{
				for( j = 0 ; j < state_N ; ++j)
				{
					sum_per_t += (alpha_matrix[t][i]*hmm -> transition[i][j]
							*hmm -> observation[observation_array[t+1]][j]*beta_matrix[t+1][j]);
				}
				
			}
		
			for( i = 0 ; i < state_N ; ++i)
			{
				for( j = 0 ; j < state_N ; ++j)
				{
					epsilon_matrix[t][i][j] = (double)(alpha_matrix[t][i]*hmm -> transition[i][j]
						*hmm -> observation[observation_array[t+1]][j]*beta_matrix[t+1][j])/
						(double)(sum_per_t);
				}
			}
			
			
		}
			
		
			
		for(i = 0 ; i < state_N ;++i)
		{	
			if(token_num == 0){gamma_0_i_sum[i] = ( gamma_matrix[0][i]  );}
			else
			{
				gamma_0_i_sum[i] += ( gamma_matrix[0][i]  );
			}
		}
		
		for(i = 0 ; i < state_N ;++i)
		{
			for(j = 0 ; j < state_N ;++j)
			{
				double sum = 0.0;
				for(t = 0 ; t < observation_t-1 ; ++t)
				{
					sum += epsilon_matrix[t][i][j];
				}
				if(token_num == 0){epsilon_allt_sum[i][j] = sum;}
				else{epsilon_allt_sum[i][j] += sum;}
			}
		}
		for(i = 0 ; i < state_N ;++i)
		{
			double sum = 0.0;
			for(t = 0 ; t < observation_t-1 ; ++t)
			{	
				sum += gamma_matrix[t][i];
			}
			if(token_num == 0){gamma_allt_sum[i] = sum;}
			else{gamma_allt_sum[i] += sum;}
		}
				
	
		for(j = 0 ; j < state_N ;++j)
		{
			for(k = 0 ; k < state_N ;++k)
			{
				double sum = 0.0;
				for(t = 0 ; t < observation_t ; ++t)
				{
					if(observation_array[t] == k)
					{
						sum += gamma_matrix[t][j];
					}
				}
				if(token_num == 0){gamma_event_sum[j][k] = sum;}	
				else{gamma_event_sum[j][k] += sum;}
			}
		}
		
			
		for(i = 0 ; i < state_N ;++i)
		{
			double sum = 0.0;
			for(t = 0 ; t < observation_t ; ++t)
			{
				sum += gamma_matrix[t][i];
			}
			
			if(token_num == 0){gamma_allt_sum_b[i] = sum;}
			else{gamma_allt_sum_b[i] += sum;}
		}
		
		
		token_num ++;
		
	}

	///////////////////////////////////
	// update of pi , alpha , beta/////
	///////////////////////////////////
	
	double check_pi = 0.0;
	for(i = 0 ; i < state_N ;++i)
	{
		hmm -> initial[i] = (double) gamma_0_i_sum[i] / (double) token_num;
		check_pi += hmm -> initial[i];
	}
		
	for(i = 0 ; i < state_N ;++i)
	{
		check_pi = 0.0;
		for(j = 0 ; j < state_N ;++j)
		{
			
			hmm -> transition[i][j] = (double) epsilon_allt_sum[i][j] / (double)gamma_allt_sum[i];
			check_pi += hmm -> transition[i][j];
		
		}
	}
	
	for(j = 0 ; j < state_N ;++j)
	{
		check_pi = 0.0;
		for(k = 0 ; k < state_N ;++k)
		{
			
			hmm -> observation[k][j] = (double) gamma_event_sum[j][k]/ (double)gamma_allt_sum_b[j];	
		}
	}
		
	for(int j=0;j<state_N;++j)
	{
		check_pi = 0.0;
		for(int i=0;i<state_N;++i)
		{
			check_pi += hmm -> observation[i][j];
		}
		
	}

	} // iteration loop
	
	FILE *fp_model = open_or_die(model_name, "w");
	dumpHMM(fp_model , hmm);	

}



#endif
