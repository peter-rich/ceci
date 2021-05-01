/*
 *
 * Zhanfu Yang
 * 2021
 * 
 * */
#include <iostream>
#include "subgraph.h"
#include "wtime.h"
#include <cstdio>
#include <fstream>
#include <cstring>

using namespace std;

char *dataGraphFileName, *queryGraphFileName;

void help();

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("Wrong parameters \n");
		help();
		exit(1);
	}	
	
	dataGraphFileName = argv[1];
	queryGraphFileName = argv[2];

	graph* datagraph = new graph(dataGraphFileName);
	graph* querygraph = new graph(queryGraphFileName);

	return 0;
}

void help(){
	count << "1. application.bin\n" << "2. dataGraph\n" << "3. queryGraph\n" << endl;
}
