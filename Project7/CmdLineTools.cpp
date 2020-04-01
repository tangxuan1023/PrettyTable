#include "ArgsParser.h"
#include <stdio.h>

void test_process(string &in, string &out)
{
	printf("input: %s, output: %s", in.c_str(), out.c_str());
}

#if 0
int main(int argc, char* const argv[])
{
	string keys = "{h usage ? help  |     | Show this help message and exit. }" \
		          "{@input          |     | Full path to input file. }" \
		          "{@output         |     | Full path to output file. }";

	ArgsParser parser(argc, argv, keys);

	parser.about("\n\n>This tools Created on " __TIME__ " " __DATE__ \
		         " @Author: tangxuan \n\n");

	if (argc <= 1) { // no args option entered
		parser.printMessage();
		return -1;
	}

	if (argc == 2) {
		std::string option = string(argv[1]);
		if (option.compare("-h") == 0 || 
			option.compare("--help") == 0 ||
			option.compare("--usage") == 0 ||
			option.compare("-?") == 0) {
			printf("help> ");
			parser.printMessage();
			return 0;
		}
		else { // error
			printf("invalid option.\n");
			return -1;
		}
	}

	string input = parser.get<string>(0);
	//printf("check input: %s\n", input.c_str());
	string output = parser.get<string>(1);
	//printf("check output: %s\n", output.c_str());

	// TODO(tangxuan):
	test_process(input, output);

	return 0;
}
#endif