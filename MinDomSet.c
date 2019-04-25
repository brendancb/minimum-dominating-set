//Brendan Beach
//
//The purpose of this program is to find a minimum dominating set for a given graph using a recursive vertex colouring algorithm.
//
//Input : Verbose flag <0 or 1>, a .txt file of graphs
//
//Output: If Verbose = 0, the program prints the graph number, number of vertices, and size of the minimum dominating set to stdout.
//        If Verbose = 1, the program prints out a certificate containing the current graph and its dominating set.
//
//Original Algorithm pseudocode taken from slides provided by Dr. Wendy Myrvold here: http://webhome.cs.uvic.ca/~wendym/courses/422/17/notes/422_03_dom_alg.pdf

#include <stdio.h>
#include <stdlib.h>
#define NMAX 2187
#define DEG_MAX 16
#define DEBUG 0
void print_vector(int len, int vector[len]){
	//Given an array of length len, prints the current array
	int i;
	for(i = 0; i < len; i++){
		printf("%d ", vector[i]);
	}
	printf("\n");
} 

void colour_blue(int level, int n, int degree[NMAX], int G[NMAX][DEG_MAX], int num_choice[n]){
	//Changes colour of current vertex from white to blue
	int i;
	for(i = 0; i < degree[level] + 1; i++){
		num_choice[G[level][i]]--;
	}
}

void uncolour_blue(int level, int n, int degree[NMAX], int G[NMAX][DEG_MAX], int num_choice[n]){
	//Changes colour of current vertex from blue to white
	int i;
	for(i = 0; i < degree[level] + 1; i++){
		num_choice[G[level][i]]++;
	}
}

void colour_red(int level, int n, int degree[NMAX], int G[NMAX][DEG_MAX], int dom[n], int * n_dominated, int num_dominated[n], int * size){
	//Changes colour of current vertex from white to red
	dom[*size] = level;
	*size = *size + 1;
	int i;
	int count_nonzeroes_before = 0;
	int count_nonzeroes_after = 0;
	for(i = 0; i < degree[level] + 1; i++){
		//Count the number of neighbouring vertices that are dominated, change the vertex's colour, and count again
		if(num_dominated[G[level][i]] > 0){
			count_nonzeroes_before++;
		}
		num_dominated[G[level][i]]++;
		if(num_dominated[G[level][i]] > 0){
			count_nonzeroes_after++;
		}
	}
	*n_dominated = *n_dominated + (count_nonzeroes_after - count_nonzeroes_before);
}

void uncolour_red(int level, int n, int degree[NMAX], int G[NMAX][DEG_MAX], int dom[n], int * n_dominated, int num_dominated[n], int * size){
	//Changes colour of current vertex from red to white
	dom[*size - 1] = -1;
	*size = *size - 1;
	int i;
	int count_nonzeroes_before = 0;
	int count_nonzeroes_after = 0;
	for(i = 0; i < degree[level] + 1; i++){
		//Count the number of neighbouring vertices that are dominated, change the vertex's colour, and count again
		if(num_dominated[G[level][i]] > 0){
			count_nonzeroes_before++;
		}
		num_dominated[G[level][i]]--;
		if(num_dominated[G[level][i]] > 0){
			count_nonzeroes_after++;
		}
	}
	*n_dominated = *n_dominated + (count_nonzeroes_after - count_nonzeroes_before);
}

int read_graph(int * n, int degree[NMAX], int G[NMAX][DEG_MAX]){
	//This function reads the current graph from a .txt file given in stdin into an adjacency matrix G 
	int i;
	int j;
	int scanf_check;

	scanf_check = scanf("%d", n);
	if(scanf_check != 1){
		//End of input file
		return scanf_check;
	}
	for(i = 0; i < *n; i++){
		scanf_check = scanf("%d", &degree[i]);
		if(scanf_check != 1){
			printf("ERROR: degree of vertex %d could not be read correctly\n", i);
			exit(0);
		}
		if(degree[i] > DEG_MAX){
			printf("ERROR: Current vertex has a degree of %d, DEG_MAX = %d\n", degree[i], DEG_MAX);
			exit(0);
		}
		for(j = 0; j < degree[i]; j++){
			//Read all neighbours of vertex i
			scanf_check = scanf("%d", &G[i][j]);
			if(scanf_check != 1){
				printf("ERROR: neighbour %d of vertex %d could not be read correctly\n", j, i);
				exit(0);
			}
		}
		G[i][degree[i]] = i;
	}
	return scanf_check;
}

void min_dom_set(int level, int n, int degree[NMAX], int G[NMAX][DEG_MAX], int * min_size, int min_dom[NMAX], int * size, int dom[n], int * n_dominated, int num_dominated[n], int num_choice[n], int delta){
	//This function recursively finds the minimum dominating set for the given graph G using the algorithm discussed in class.
	#if DEBUG
		printf("Level %3d: ", level);
		print_vector(*size, dom);
		printf("Number of vertices dominated: %3d\n", *n_dominated);
		printf("Number of choices per vertex:\n");
		print_vector(n, num_choice);
		printf("Number of times dominated:\n");
		print_vector(n, num_dominated);
		printf("Current min dominating set:\n");
		print_vector(*min_size, min_dom);
	#endif
	
	int i;
	for(i = 0; i < n; i++){
		//Check if there is any vertex that cannot be dominated by any of its neighbours
		if(num_choice[i] == 0){
			//printf("Vertex %d cannot be dominated, returning\n", i);
			return;
		}
	}
	
	int n_undominated = n - *n_dominated;
	int n_extra = (n_undominated + delta) / (delta + 1);
	if(*size + n_extra >= *min_size){
		return;
	}

	//Check if current set of vertices in dom is a new minimum dominating set
	if(level == n || n_undominated == 0){
		if(*size < *min_size){
			for(i = 0; i < *size; i++){
				min_dom[i] = dom[i];
			}
			*min_size = *size;
		}
		return;
	}
	//Try current vertex as blue
	colour_blue(level, n, degree, G, num_choice);
	min_dom_set(level + 1, n, degree, G, min_size, min_dom, size, dom, n_dominated, num_dominated, num_choice, delta);
	uncolour_blue(level, n, degree, G, num_choice);
	
	//Try current vertex as red
	colour_red(level, n, degree, G, dom, n_dominated, num_dominated, size);
	min_dom_set(level + 1, n, degree, G, min_size, min_dom, size, dom, n_dominated, num_dominated, num_choice, delta);
	uncolour_red(level, n, degree, G, dom, n_dominated, num_dominated, size);
}

void find_dom_set(int n, int degree[NMAX], int G[NMAX][DEG_MAX], int * min_size, int min_dom[NMAX]){
	//This function initializes all variables before calling the recursive function min_dom_set()
	int size = 0; //Size of incomplete dom. set
	int dom[n]; //Incomplete dom. set
	int n_dominated = 0; //Number of dominated vertices
	int num_dominated[n]; //Number of times each vertex is dominated by another
	int num_choice[n]; //Number of times this vertex could be dominated if all white vertices were red (any 0 shows current dominating set cannot be completed)
	int i;
	int delta = 0;//Maximum degree of any vertex in graph G
	*min_size = n;
	for(i = 0; i < n; i++){
		num_dominated[i] = 0;
		dom[i] = -1;
		num_choice[i] = degree[i] + 1;
		min_dom[i] = i;
		if(delta < degree[i]){
			delta = degree[i];
		}
	}
	
	min_dom_set(0, n, degree, G, min_size, min_dom, &size, dom, &n_dominated, num_dominated, num_choice, delta);
	
}

int check_args(int argc, char *argv[]){
	//Checks the passed in value of the verbose flag and sets a variable containing this value
	int verbose_value;
	if(argc != 2){
		printf("Usage: %s <Verbose flag = 0 or 1>\n", argv[0]);
		exit(-1);
	}
	if(argv[1][0] == '0'){
		verbose_value = 0;
	} else if(argv[1][0] == '1'){
		verbose_value = 1;
	} else {
		printf("Usage: %s <Verbose flag = 0 or 1>", argv[0]);
	}
	return verbose_value;
}

void print_output(int verbose, int graph_num, int n, int min_size, int min_dom[NMAX], int G[NMAX][DEG_MAX], int degree[NMAX]){
	//Prints information found about the minimum dominating set for the current graph
	//The format of this information is determined by the value of the verbose flag
	if(verbose == 0){
		printf("%5d %3d %3d\n", graph_num, n, min_size);
	} else if (verbose == 1){
		printf("%d\n", n);
		int i;
		for(i = 0; i < n; i++){
			//Print degree of vertex and neighbours of vertex
			printf("%d ", degree[i]);
			print_vector(degree[i], G[i]);
		}
		printf("\n");
		printf("%d\n", min_size);
		print_vector(min_size, min_dom);
		printf("\n");
	}
	fflush(stdout);
}

int main(int argc, char *argv[]){
	int n; //Number of vertices
	int degree[NMAX]; //The degree of each vertex
	int G[NMAX][DEG_MAX]; //An adjacency list representing the graph
	int verbose; 
	int graph_num = 1; //Current graph number
	int min_size; //Size of minimum dominating set
	int min_dom[NMAX]; //Minimum dominating set
	
	verbose = check_args(argc, argv);
	
	//Iterate through all graphs provided in the stdin .txt file
	while(read_graph(&n, degree, G) == 1){		
		//run only if read_graph reads a new graph successfully
		find_dom_set(n, degree, G, &min_size, min_dom);
		print_output(verbose, graph_num, n, min_size, min_dom, G, degree);
		graph_num++;
	}
	return 0;
}