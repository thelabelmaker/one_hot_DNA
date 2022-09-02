#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>


struct args
{
	char** seq;
	double*** one_hot_rep;
	int start;
	int end;
	int L;
	double* A;
	double* C;
	double* G;
	double* T;
	double* n;

};

void *_oneHot(void *input){
	//char** seq; double*** one_hot_rep; int start; int end; int L; double* A; double* C; double* G; double* T; double* n;
	struct args *args_dict = (struct args*) input;
	//printf("Struct is good");
	for(int i = args_dict->start; i < args_dict->end; i++){
		for(int j = 0; j < args_dict->L; j++){
			
			if(args_dict->seq[i][j] == "A"[0]){
				args_dict->one_hot_rep[i][j] = args_dict->A;
				//printf("Assigned A");
			}

			if(args_dict->seq[i][j] == "C"[0]){
                args_dict->one_hot_rep[i][j] = args_dict->C;
            }

			if(args_dict->seq[i][j] == "G"[0]){
                args_dict->one_hot_rep[i][j]= args_dict->G;
            }

			if(args_dict->seq[i][j] == "T"[0]){
                args_dict->one_hot_rep[i][j] = args_dict->T;
            }

			if(args_dict->seq[i][j] == "N"[0]){
                args_dict->one_hot_rep[i][j] = args_dict->n;
            }
			
			//one_hot_rep[i][j] = malloc(4*sizeof(double));
			//one_hot_rep[i][j][0] = 0.;
		}
	}
	printf("\nChunk Encoded!\n");
	return NULL;
}

double*** oneHot(char** seq, int N, int L, int threads){

	double* A = malloc(4*sizeof(double));
	A[0] = 1.;
	A[1] = .0;
	A[2] = .0;
	A[3] = .0;

	double* C = malloc(4*sizeof(double));
	C[1] = 1.;
	C[0] = .0;
	C[2] = .0;
	C[3] = .0;

	double* G = malloc(4*sizeof(double));
	G[2] = 1.;
	G[1] = .0;
	G[0] = .0;
	G[3] = .0;

	double* T = malloc(4*sizeof(double));
	T[3] = 1.;
	T[1] = .0;
	T[2] = .0;
	T[0] = .0;

	double* n = malloc(4*sizeof(double));
	n[0] = .25;
	n[1] = .25;
	n[2] = .25;
	n[3] = .25;

	printf("%f \n", n[0]);

	double ***one_hot_rep = malloc(N*sizeof(double**));

	struct args *args_dict = (struct args *)malloc(sizeof(struct args));

	for(int i=0; i<N; i++){
		double** sequence = malloc(L*sizeof(double*));
		one_hot_rep[i] = sequence;
	}

	args_dict->one_hot_rep = one_hot_rep;
	args_dict->seq = seq;
	args_dict->L = L;

	args_dict->A = A;
	args_dict->C = C;
	args_dict->G = G;
	args_dict->T = T;
	args_dict->n = n;


	int slice_size = (int) (N / threads);

	//printf("slice_size: %d \n", slice_size);

	pthread_t thread_ids[threads];

	int thread = 0;

	for(int i = 0; i <= N-slice_size; i+=slice_size){
		//printf("this is i: %d through %d \n", i, i+slice_size);
		args_dict->start = i;
		args_dict->end = i+slice_size;
		pthread_create(&thread_ids[thread], NULL, _oneHot, (void *)args_dict);
		thread++;
	}

	for(int i = 0; i < threads; i++){
		pthread_join(thread_ids[i], NULL);
	}

	int current_ind = slice_size*threads;
	int num_samples = N - current_ind;
	args_dict->start = current_ind;
	args_dict->end = N;

	thread = 0;

	for(int i = current_ind; i < N; i++){
		printf("this is i: %d through %d\n", i, i+1);
		args_dict->start = i;
		args_dict->end = i+1;
		pthread_create(&thread_ids[thread], NULL, _oneHot, (void *)args_dict);
		thread++;
	}

	for(int i = 0; i < thread; i++){
		pthread_join(thread_ids[i], NULL);
	}

	return one_hot_rep;
}

int main(){
	//setup perams
	int n_seq = 200000;
	int seq_length = 4096;

	//make char array that will be input seqs
	char **arr = malloc(n_seq*sizeof(char*));
	
	//DNA alphabet
	char alphabet[5] = "AGCTN";

	//init random
	time_t t;
	srand((unsigned) time(&t));

	//generate random sequences
	for(int i = 0; i < n_seq; i++){
		arr[i] = malloc(seq_length*sizeof(char));
		for(int j = 0; j < seq_length; j++){
			arr[i][j] = alphabet[rand()%5];
		}
	}

	//do the encoding
	double ***one_hot = oneHot(arr, n_seq, seq_length, 16);

	//print encoding
	printf("[");
	for(int i = 0; i < n_seq; i++){
		printf("[");
		//printf("sequence %d\n", i);
		for(int j = 0; j < 4096; j++){
			printf("[");
			for(int k = 0; k < 4; k++){
				printf("%f ", one_hot[i][j][k]);
			}
			printf("],\n");
		}
		printf("], ");
		printf("\n\n");

	}
	printf("]");
	return 0;
}


