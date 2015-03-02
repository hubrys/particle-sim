#include "./Simulation.h"

int main(int argc, char** argv) 
{
	if (argc <= 1)
	{
		printf("no asset folder defined\n");
		return 0;
	}

	std::string root = std::string(argv[1]);
	Simulation simulation = Simulation();
	simulation.execute(root);
}