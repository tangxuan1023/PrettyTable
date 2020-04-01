#include "PrettyTable.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <iostream>

#include <stdio.h>
using namespace std;

int main(int argc, char* const argv[])
{
	string str = "{task_id|content|begin_date|end_date|status|remark|}";
	string str1 = "{task_id1|content1|begin_date1|end_date1||remark1|score1|}";
	string str2 = "{task_id22|content22|begin_date22|end_date22|status22|remark22|score22||}";
	string str3 = "{task_id333|content333|begin_date333|end_date333|status333|remark333|score333}";
	PrettyTable table(str);

	table.addRow(str1);
	table.addRow(str2);
	table.addRow(str3);

	const char *outfile = "./out.txt";

	//PrettyTable table1(table);
	PrettyTable table1 = table;

	table1.printTable();

	getchar();
	return 0;
}