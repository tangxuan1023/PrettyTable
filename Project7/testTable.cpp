#include "PrettyTable.h"
#include <stdio.h>

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

int main(void) 
{
	PrettyTable tb(CameraMessages, sizeof(CameraMessages) / sizeof(const char*));
	tb.addRow(data);
	tb.addRow(CameraMessages);
	tb.addRow(CameraMessages);
	tb.addRow(data);
	tb.addRow(CameraMessages);

	tb.printTable("");
	tb.updateTable(data);
	getchar();
	return 0;
}