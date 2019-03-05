// Code modified and written by Shivam Agarwal


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
} node;

/**
 * Global Variables
 */

// used to track the position of the blank in a state,
// so it doesn't have to be searched every time we check if an operator is applicable
// When we apply an operator, blank_pos is updated
int blank_pos;

// used to track the previous operation in order to avoid it
int prev_move;

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
      
void printf_comma (long unsigned int n) 
{
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
	/* initialize the sum to 0 */
	int sum = 0;
	int i;
	int x, y;
	int x0, y0;
	for (i = 0; i < 16; i++) 
	{
		/* calculate the distance for everything except blank */
		if(state[i] != 0) 
		{
			x = i % 4;
			y = i / 4;
			x0 = state[i] % 4;
			y0 = state[i] / 4;
			sum += abs(x - x0) + abs(y - y0);
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

/* get the counter action for each eaction */
int get_opposite_action(int action) 
{
	if (action == LEFT)
		return RIGHT;
	else if (action == RIGHT)
		return LEFT;
	else if (action == UP)
		return DOWN;
	else
		return UP;
}

/* Recursive IDA */
node* ida( node* node, int threshold, int* newThreshold )
{
	struct node* new_node;
	int i;
	/* run a loop for every action */
	for (i = 0; i < 4; i++) 
	{
		/* if the action is applicable in aparticular state then only execute it */
		/* get the opposite action and check whether it was the previous action or not */
		/* if its not equal then only apply the action */
		int opp_act = get_opposite_action(i);
		if ((applicable(i) == 1) && (opp_act != prev_move)) 
		{
			/* update the counter */
			generated++;
			new_node = (struct node*)malloc(sizeof(struct node));
			memcpy(new_node->state, node->state, sizeof(int) * 16);
			/* store the blank position before the action is performed */
			int initial_blank = blank_pos;
			/* apply the action to the new_node */
			apply (new_node, i);
			/* update the g of new_node */
			new_node->g = node->g + 1;
			/* update the f of new_node */
			new_node->f = new_node->g + manhattan(new_node->state);
			/* replace the B' by the minimum of new_nodes f and initial B' */ 
			if (new_node->f > threshold)
			{
			 	*newThreshold = ((new_node->f < *newThreshold) ? new_node->f : *newThreshold);
			 	free(new_node);
			
			} else {
				expanded++;
				/* if the state has been found then return the node */
			 	if (manhattan(new_node->state) == 0)
			 		return new_node;
			 	/* execute the recursive ida loop in all other cases */
			 	int backup_prev = prev_move;
			 	prev_move = i;
			 	struct node* resultant = ida(new_node, threshold, newThreshold);
			 	/* if the result is not null then return that */
			 	if (resultant != NULL) 
			 	{
			 		return resultant;
			 	}
			 	free(new_node);
			 	prev_move = backup_prev;
			 }
			 /* update the postion of the blank */
			 blank_pos = initial_blank;
		}
	}

	return NULL;
}


/* main IDA control loop */
int IDA_control_loop(  )
{
	node* r = NULL;
	/* temporary node to store the initial congiguration */
	node* temp;
	
	int threshold;
	
	/* initialize statistics */
	generated = 0;
	expanded = 0;
	int new_threshold;
	
	/* compute initial threshold B */
	initial_node.f = threshold = manhattan( initial_node.state );

	printf( "Initial Estimate = %d\nThreshold = ", threshold );

	/* initialise the previous move to value except 0 to 3 */ 
	prev_move = 4;

	/* repeat the while loop until r is not null */
	while (r == NULL) 
	{
		/* put the B' to infinity */
		new_threshold = INT_MAX;
		temp = (struct node*)malloc(sizeof(struct node));
		/* put the state of the node to the state of the standard state*/
		memcpy(temp->state, initial_node.state, sizeof(int) * 16);
		/* put initial cost as 0 */
		temp->g = 0;
		/* put the result in r from recursive IDA */
		r = ida(temp, threshold, &new_threshold);
		free(temp);
		/* if resultant is null then put threshold as new threshold */
		if (r == NULL) 
		{
			threshold = new_threshold;
			printf(" %d", threshold);
		}
	} 
	/* if solution is found then return the cost of computation else return -1 */
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


