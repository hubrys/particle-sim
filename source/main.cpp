#include <cstdlib>

#include "./Simulation.h"

int main(int argc, char** argv) 
{
	std::string root;
	char* envRoot = getenv("HUBRYS_NBODY_PATH");
	printf("path: %s\n", envRoot);

	if (argc <= 1)
	{
		printf("no asset folder defined, using current dir\n");
		root = "./";
	} else {
		root = std::string(argv[1]);
	}

	Config config = Config::fromFile(root + "config.txt");
	config.setString("assetRoot", root);
	Config::setGlobalInstance(&config);


	Simulation().execute();
}