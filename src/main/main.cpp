#include <iostream>

#include "scanner/IScanner.hpp"
#include "scanner/ScannerFile.hpp"

int main(int argc, char* argv[]) {

	IScanner *scanner = new ScannerFile("data/puntos_test.csv");
	
	scanner->initScanner();

	scanner->startScanner();

	scanner->closeScanner();

	delete scanner;

	return 0;
}