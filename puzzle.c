#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>


/**
 * READ THIS DESCRIPTION
 *
 * node data structure: containing state, g, f
 * you can extend it with more information if needed
 */
typedef struct node{
	int state[16];
	int g;
	int f;
	struct node* next;
} node;

/**
 * Global Variables
 */

// used to track the position of the blank in a state,
// so it doesn't have to be searched every time we check if an operator is applicable
// When we apply an operator, blank_pos is updated
int blank_pos;

// Initial node of the problem
node initial_node;

// Statistics about the number of generated and expendad nodes
unsigned long generated;
unsigned long expanded;


/**
 * The id of the four available actions for moving the blank (empty slot). e.x.
 * Left: moves the blank to the left, etc.
 */

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

/*
 * Helper arrays for the applicable function
 * applicability of operators: 0 = left, 1 = right, 2 = up, 3 = down
 */
int ap_opLeft[]  = { 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 };
int ap_opRight[]  = { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };
int ap_opUp[]  = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int ap_opDown[]  = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 };
int *ap_ops[] = { ap_opLeft, ap_opRight, ap_opUp, ap_opDown };


/* print state */
void print_state( int* s )
{
	int i;

	for( i = 0; i < 16; i++ )
		printf( "%2d%c", s[i], ((i+1) % 4 == 0 ? '\n' : ' ') );
}

void printf_comma (long unsigned int n) {
    if (n > 0) {
        printf ("-");
        printf_comma (-n);
        return;
    }
    if (n < 1000) {
        printf ("%lu", n);
        return;
    }
    printf_comma (n/1000);
    printf (",%03lu", n%1000);
}

node* solid_copy_node(node *n){
	node *n_new = NULL;
	n_new = (node *) malloc(sizeof(node));
	for (int i=0; i<16; i++){
		n_new->state[i] = n->state[i];
	}
	n_new->g = n->g;
	n_new->f = n->f;
	n_new->next = NULL;
	return n_new;
}

node* add_to_path( node* n, node* root){
	if (root!=NULL){
		root->next = add_to_path(n, root->next);
	}else{
		root = solid_copy_node(n);
	}
	return root;
}

int path_length( node* n ){
	node *next = n;
	int count = 0;
	while (next!=NULL){
		count++;
		next = next->next;
	}
	return count;
}

int is_in_path( node* n, node* root ){
	node *next = root;
	while (next!=NULL){
		int count = 0;
		for (int i=0; i<16; i++){
			if (next->state[i] == n->state[i]){
				// found difference
				count++;
			}
			if (count == 16){
				return 1;
			}
		}
		next = next->next;
	}
	return 0;

}

/* return the sum of manhattan distances from state to goal */
int manhattan( int* state )
{
	int sum = 0;

	/**
	 * FILL WITH YOUR CODE
	 */
	for (int i=0; i<16; i++){
		if ((state[i] == i) || (i == blank_pos)){
			continue;
		}else{
			int actual_pos = state[i] + 1;
			int a_row_num = (actual_pos + 3) / 4;
			int a_col_num = actual_pos % 4;
			if (a_col_num == 0){
				a_col_num = 4;
			}

			int current_pos = i + 1;
			int c_row_num = (current_pos + 3) / 4;
			int c_col_num = current_pos % 4;
			if (c_col_num == 0){
				c_col_num = 4;
			}

			int dist = abs(a_row_num - c_row_num) + abs(a_col_num - c_col_num);
			sum = sum + dist;
		}
	}

	return( sum );
}


/* return 1 if op is applicable in state, otherwise return 0 */
int applicable( int op )
{
       	return( ap_ops[op][blank_pos] );
}


/* apply operator */
void apply( node* n, int op )
{
	int t;
	//find tile that has to be moved given the op and blank_pos
	t = blank_pos + (op == 0 ? -1 : (op == 1 ? 1 : (op == 2 ? -4 : 4)));

	//apply op
	n->state[blank_pos] = n->state[t];
	n->state[t] = 0;

	//update blank pos
	blank_pos = t;
}

/* Recursive IDA */
node* ida( node* n, int threshold, int* newThreshold, node* path )
{
	/**
	 * FILL WITH YOUR CODE
	 *
	 * Algorithm in Figure 2 of handout
	 */

	for (int i=0; i<4; i++){
		if ((applicable(i) == 1) && (is_in_path(n, path)==0)){
			node *n_new = solid_copy_node(n);
			apply(n_new, i);
			path = add_to_path(n, path);
			int h = manhattan(n_new->state);
			int n_g = n_new->g + 1;
			int n_f = n_g + h;
			if (n_f > threshold){
				if (newThreshold >= &n_f){
					newThreshold = &n_f;
				}
			}else{
				if (h == 0){
					return n_new;
				}
				node* r = ida(n_new, threshold, newThreshold, path);
				if (r!=NULL){
					return r;
				}
			}
		}
	}
	return( NULL );
}




/* main IDA control loop */
int IDA_control_loop(  ){
	node* r = NULL;

	int threshold;
	int *newThreshold = NULL;
	int intMax = 9999;

	/* initialize statistics */
	generated = 0;
	expanded = 0;
	newThreshold = &intMax;

	/* compute initial threshold B */
	initial_node.f = threshold = manhattan( initial_node.state );

	printf( "Initial Estimate = %d\nThreshold = ", threshold );

	/**
	 * FILL WITH YOUR CODE
	 *
	 * Algorithm in Figure 1 of handout
	 */

	while (r==NULL){
		node *path = NULL;
		node *n_new = solid_copy_node(&initial_node);
		r = ida(n_new, threshold, newThreshold, path);
		if (r==NULL){
			// printf("New threshold %d\n", *newThreshold);
			threshold = *newThreshold;
		}
	}
	return r->g;
	// return 0;
}


static inline float compute_current_time()
{
	struct rusage r_usage;

	getrusage( RUSAGE_SELF, &r_usage );
	float diff_time = (float) r_usage.ru_utime.tv_sec;
	diff_time += (float) r_usage.ru_stime.tv_sec;
	diff_time += (float) r_usage.ru_utime.tv_usec / (float)1000000;
	diff_time += (float) r_usage.ru_stime.tv_usec / (float)1000000;
	return diff_time;
}

int main( int argc, char **argv )
{
	int i, solution_length;

	/* check we have a initial state as parameter */
	if( argc != 2 )
	{
		fprintf( stderr, "usage: %s \"<initial-state-file>\"\n", argv[0] );
		return( -1 );
	}


	/* read initial state */
	FILE* initFile = fopen( argv[1], "r" );
	char buffer[256];

	if( fgets(buffer, sizeof(buffer), initFile) != NULL ){
		char* tile = strtok( buffer, " " );
		for( i = 0; tile != NULL; ++i )
			{
				initial_node.state[i] = atoi( tile );
				blank_pos = (initial_node.state[i] == 0 ? i : blank_pos);
				tile = strtok( NULL, " " );

			}
	}
	else{
		fprintf( stderr, "Filename empty\"\n" );
		return( -2 );

	}

	if( i != 16 )
	{
		fprintf( stderr, "invalid initial state\n" );
		return( -1 );
	}

	/* initialize the initial node */
	initial_node.g=0;
	initial_node.f=0;

	print_state( initial_node.state );


	/* solve */
	float t0 = compute_current_time();

	solution_length = IDA_control_loop();

	float tf = compute_current_time();

	/* report results */
	printf( "\nSolution = %d\n", solution_length);
	printf( "Generated = ");
	printf_comma(generated);
	printf("\nExpanded = ");
	printf_comma(expanded);
	printf( "\nTime (seconds) = %.2f\nExpanded/Second = ", tf-t0 );
	printf_comma((unsigned long int) expanded/(tf+0.00000001-t0));
	printf("\n\n");

	/* aggregate all executions in a file named report.dat, for marking purposes */
	FILE* report = fopen( "report.dat", "a" );

	fprintf( report, "%s", argv[1] );
	fprintf( report, "\n\tSoulution = %d, Generated = %lu, Expanded = %lu", solution_length, generated, expanded);
	fprintf( report, ", Time = %f, Expanded/Second = %f\n\n", tf-t0, (float)expanded/(tf-t0));
	fclose(report);

	return( 0 );
}
