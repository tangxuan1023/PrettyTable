#include "PrettyTable.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <iostream>

#include <stdio.h>
using namespace std;

static const char *CameraMessages[] =
{
	"ERROR",
	"SHUTTER",
	"FOCUS",
	"ZOOM",
	"PREVIEW_FRAME",
	"VIDEO_FRAME",
	"POSTVIEW_FRAME",
	"RAW_IMAGE",
	"COMPRESSED_IMAGE",
	"RAW_IMAGE_NOTIFY",
	"PREVIEW_METADATA"
};

static const char *data[] =
{
	"ER",
	"SHUTTER",
	"FOC",
	"ZOO",
	"PREVIEW_FRAME_teset_ymount",
	"VIDEO_FRAME",
	"POSTVIEW_FRAME_mnutnhywgzsfvsaa",
	"RAW_IMAGE",
	"COMPRESSED_IMAGE",
	"RAW_IMAGE_NOTIFY_mauxryenug",
	"PREVIEW_METADATA"
};
//const char *table_name = fopen("./table.txt", "r");



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

	table.printTable(outfile);

	getchar();
	return 0;
}