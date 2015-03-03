#include <cstdlib>

#include "./Simulation.h"

int main(int argc, char** argv) 
{
	std::string root;
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