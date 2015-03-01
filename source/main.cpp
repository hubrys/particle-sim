#include "./Simulation.h"

int main() 
{
	Config config = Config::fromFile("assets/config.txt");
	Simulation simulation = Simulation();
	simulation.execute(config);
}