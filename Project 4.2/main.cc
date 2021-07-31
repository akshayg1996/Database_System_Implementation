#include <iostream>

#include "Interface.h"

using namespace std;

char* catalog_path = "catalog";
char* dbfile_dir = "bin/";
char* tpch_dir = "tpch-dbgen/";


int main (int argc, char* argv[]) {
	Interface interface;
	interface.Run();
	return 0;
}