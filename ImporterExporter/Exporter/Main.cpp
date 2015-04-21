// Jonas Petersson
// jnp@bth.se
// 14-03-31

#include "Exporter.h"

int main()
{	
	printf("%d %d %d", sizeof(MFloatMatrix), sizeof(MFloatArray), sizeof(MMatrix));
	Exporter exporter;

	// destinationskatalog för den exporterade filen som parameter.
	exporter.StartExporter("../Bin/Models/");
}