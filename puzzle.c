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
    int last_action;
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
    printf ("\n");
}

void printf_comma (long unsigned int n) {
    if (n < 0) {
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

/* return the sum of manhattan distances from state to goal */
int manhattan( int* state )
{
	int sum = 0;

	int i = 0;
	for(i = 0; i < 16; i++){
		if(state[i] == 0) continue;
		int x = i % 4;
		int y = i / 4;
		int tx = state[i] % 4;
		int ty = state[i] / 4;
		int dx = tx > x ? tx - x : x - tx;
		int dy = ty > y ? ty - y : y - ty;
		sum += (dx + dy);
		// printf("%d %d %d\n", i, state[i], dx+dy);
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
node* ida( node* n, int threshold, int* newThreshold )
{

	/**
	 * FILL WITH YOUR CODE
	 *
	 * Algorithm in Figure 2 of handout
	 */
	int action = 0;
	for(action = 0; action < 4; action++){
        if ((action == 0 && n->last_action == 1) ||
            (action == 1 && n->last_action == 0) ||
            (action == 2 && n->last_action == 3) ||
            (action == 3 && n->last_action == 2)) {
            continue;
        }
		struct node n_node;
		int i = 0;
		for(i = 0; i < 16; i++) {
			if(n->state[i] == 0) blank_pos = i;
		}
		if(applicable(action)){
			generated++;
			for(i = 0; i < 16; i++) {
				n_node.state[i] = n->state[i];
			}
			apply(&n_node, action);
			// print_state(n_node.state);
			n_node.g = n->g + 1;
			n_node.f = n_node.g + manhattan(n_node.state);
            n_node.last_action = action;
			if(n_node.f > threshold){
				*newThreshold = n_node.f < *newThreshold ? n_node.f : *newThreshold;
				// printf("%d ", *newThreshold);
			}
			else{
				if(manhattan(n_node.state) == 0){
					// print_state(n_node.state);
					n->f = n_node.f;
					n->g = n_node.g;
					memcpy(n->state, n_node.state, 16);
					return n;
				}
				expanded++;
				struct node *r;
				r = ida(&n_node, threshold, newThreshold);
				if(r != NULL){
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

	/* initialize statistics */
	generated = 0;
	expanded = 0;

	/* compute initial threshold B */
	initial_node.f = manhattan( initial_node.state );
	threshold = initial_node.f;

	printf( "Initial Estimate = %d\nThreshold = ", threshold );


	/**
	 * FILL WITH YOUR CODE
	 *
	 * Algorithm in Figure 1 of handout
	 */
		while (r == NULL){
			int newB = 99999999;
			struct node n;
			generated++;
			int i = 0;
			for(i = 0; i < 16; i++) n.state[i] = initial_node.state[i];
			n.g = 0;
            n.last_action = -1;
			r = ida(&n, threshold, &newB);
			printf("%d ", threshold);
			if(r == NULL){
				threshold = newB;
			}
		}

	if(r)
		return r->g;
	else
		return -1;
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
