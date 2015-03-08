#include <cstdlib>

#include "./Simulation.h"

int main(int argc, char** argv) 
{
	std::string root;
	if (argc <= 1)
	{
		root = "./assets/";
	} else {
		root = std::string(argv[1]);
	}

	bool restart = true;
	while (restart)
	{
		Config config = Config::fromFile(root + "config.txt");
		config.setString("assetRoot", root);
		Config::setGlobalInstance(&config);

		restart = Simulation().execute();
	}
}