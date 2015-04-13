#include <fstream>
#include <iostream>

#define DELTA 0.0001
#define EQUAL(A,B) (abs((A)-(B)) < DELTA) ? true:false


struct MyHeader {
	int vertexCount;
};
struct Vertex {
	float position[3];
	float normal[3];
	float rgba[4];
};


int main() {
	// variables to "dump"
	MyHeader h{ 100 };

	//Create some random variables 
	Vertex *vArray = new Vertex[h.vertexCount];


	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vArray[i].position[j] = rand();
			vArray[i].normal[j] = rand();
		}
		for (int j = 0; j < 4; j++)
		{
			vArray[i].rgba[j] = rand();
		}
	}

	// open file for binary
	std::ofstream outfile("testBin.bin", std::ofstream::binary);
	// write header
	outfile.write((const char*)&h, sizeof(MyHeader));
	// write vertices
	outfile.write((const char*)vArray, sizeof(Vertex)*h.vertexCount);
	outfile.close();

	// How to read
	std::ifstream infile("testBin.bin", std::ifstream::binary);
	// first read the header
	MyHeader h2;
	infile.read((char*)&h2, sizeof(MyHeader));
	// this will call default constructor of Vertex
	Vertex *vertices = new Vertex[h2.vertexCount];
	// read ALL vertices in one go
	infile.read((char*)vertices, h2.vertexCount*sizeof(Vertex));
	infile.close();

	// options for comparison:
	// FAST MEMCMP
	// Will only work if all values (datatypes) went through the 
	// same process and no casting occurred anywhere.
	int result = memcmp(vertices, vArray, h.vertexCount * sizeof(Vertex));
	// ONE BY ONE
	bool equal = true;
	for (int i = 0; i < h.vertexCount; i++)
	{
		if (!EQUAL(vArray[i].position[0], vertices[i].position[0]) ||
			!EQUAL(vArray[i].position[1], vertices[i].position[1]) ||
			!EQUAL(vArray[i].position[2], vertices[i].position[2]))
		{
			equal = false;
			break;
		}
	}
	std::cout << "Streams are equal, method 2: " << equal << std::endl;
	std::cout << "Streams are equal, method 1: " << (result == 0) ? true : false;
	getchar();
	getchar();
	return 0;
}
