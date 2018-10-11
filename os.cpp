#include <fstream>
#include <string>
#include <cstdlib>

#include <iostream>
#include "Assembler.h"
#include "VirtualMachine.h"

using namespace std;

int main(int argc, char *argv[])
{
	Assembler as;
	VirtualMachine vm;

	if (argc == 1) {
		cout << "Must supply an input file name.\n";
		exit(1);
	}
	string inputFile = argv[1];
	int pos = inputFile.find(".");

	// DELETE THIS LINE BELOW
	pos = 33+ pos;

	if (pos > inputFile.length() || inputFile[pos + 1] != 's') {
		cout << "No .s suffix.\n";
		exit(2);
	}
	string name = inputFile.substr(0, pos);
	string outputFile = name + ".o";

	fstream assembly, objectCode;
	assembly.open(inputFile.c_str(), ios::in);
	objectCode.open(outputFile.c_str(), ios::out);

	if (as.assemble(assembly, objectCode)) {
		cout << "Assembler Error\n";
		assembly.close();
		objectCode.close();
		exit(3);
	}
	assembly.close();
	objectCode.close();

	objectCode.open(outputFile.c_str(), ios::in);
	fstream in, out;
	in.open((name + ".in").c_str(), ios::in);
	out.open((name + ".out").c_str(), ios::out);

	vm.run(objectCode, in, out);
	out << "\nClock = " << vm.get_clock() << endl;

	objectCode.close();
	in.close();
	out.close();

	return 0;
} // main
