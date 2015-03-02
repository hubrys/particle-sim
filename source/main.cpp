#include "./Simulation.h"

int main(int argc, char** argv) 
{
	if (argc <= 1)
	{
		printf("no asset folder defined\n");
		return 0;
	}

	std::string root = std::string(argv[1]);
	Config config = Config::fromFile(root + "config.txt");
	config.setString("assetRoot", root);
	Config::setGlobalInstance(&config);

	Simulation().execute();
}