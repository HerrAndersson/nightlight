#include <fstream>
#include <iostream>
//#include <fbxsdk.h>
//#include <fbxsdk\fileio\fbxiosettings.h>

#define DELTA 0.0000000000001
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

	//variables to "dump"
	MyHeader h;

	////Create some random variables 
	//Vertex *vArray = new Vertex[h.vertexCount];


	//for (int i = 0; i < 100; i++)
	//{
	//	for (int j = 0; j < 3; j++)
	//	{
	//		vArray[i].position[j] = rand();
	//		vArray[i].normal[j] = rand();
	//	}
	//	for (int j = 0; j < 4; j++)
	//	{
	//		vArray[i].rgba[j] = rand();
	//	}
	//}

	//// open file for binary
	//std::ofstream outfile("testBin.bin", std::ofstream::binary);
	//// write header
	//outfile.write((const char*)&h, sizeof(MyHeader));
	//// write vertices
	//outfile.write((const char*)vArray, sizeof(Vertex)*h.vertexCount);
	//outfile.close();

	// How to read
	std::ifstream infile("box.fbx", std::ifstream::binary);
	// first read the header
	infile.read((char*)&h, sizeof(MyHeader));
	h.vertexCount = 300;
	// this will call default constructor of Vertex
	Vertex *vertices = new Vertex[h.vertexCount];
	// read ALL vertices in one go
	infile.read((char*)vertices, h.vertexCount*sizeof(Vertex));
	infile.close();

	// How to read
	std::ifstream infile2("box.fbx", std::ifstream::binary);
	// first read the header
	MyHeader h2;
	infile2.read((char*)&h2, sizeof(MyHeader));
	h2.vertexCount = 300;
	// this will call default constructor of Vertex
	Vertex *vertices2 = new Vertex[h2.vertexCount];
	// read ALL vertices in one go
	infile2.read((char*)vertices2, h2.vertexCount*sizeof(Vertex));
	infile2.close();

	// options for comparison:
	// FAST MEMCMP
	// Will only work if all values (datatypes) went through the 
	// same process and no casting occurred anywhere.
	int result = memcmp(vertices, vertices2, h.vertexCount * sizeof(Vertex));
	// ONE BY ONE
	bool equal = false;
	for (int i = 0; i < h.vertexCount; i++)
	{
		if (!EQUAL(vertices[i].position[0], vertices2[i].position[0]) ||
			!EQUAL(vertices[i].position[1], vertices2[i].position[1]) ||
			!EQUAL(vertices[i].position[2], vertices2[i].position[2]) //||
			//!EQUAL(vertices[i].normal[0], vertices2[i].normal[0]) ||
			//!EQUAL(vertices[i].normal[1], vertices2[i].normal[1]) ||
			//!EQUAL(vertices[i].normal[2], vertices2[i].normal[2])||
			/*!EQUAL(vertices[i].rgba[0], vertices2[i].rgba[0]) ||
			!EQUAL(vertices[i].rgba[1], vertices2[i].rgba[1]) ||
			!EQUAL(vertices[i].rgba[2], vertices2[i].rgba[2]) ||
			!EQUAL(vertices[i].rgba[3], vertices2[i].rgba[3])*/)
		{
			std::cout << std::endl;
			std::cout << "Fail: " << vertices[i].position[0] << " not equal " << vertices2[i].position[0] << std::endl;
			std::cout << "Fail: " << vertices[i].position[1] << " not equal " << vertices2[i].position[1] << std::endl;
			std::cout << "Fail: " << vertices[i].position[2] << " not equal " << vertices2[i].position[2] << std::endl;
			equal = false;
			break;
		}
		else
		{
			std::cout << "Success: " << vertices[i].position[0] << " equal " << vertices2[i].position[0] << std::endl;
			std::cout << "Success: " << vertices[i].position[1] << " equal " << vertices2[i].position[1] << std::endl;
			std::cout << "Success: " << vertices[i].position[2] << " equal " << vertices2[i].position[2] << std::endl;
			equal = true;
		}
	}
	if (equal == true)
	{
		std::cout << "Streams are equal, method 2: " << equal << std::endl;
		std::cout << "Streams are equal, method 1: " << (result == 0) ? true : false;
	}
	getchar();
	getchar();
	return 0;
}
