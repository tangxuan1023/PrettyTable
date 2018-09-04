#include "PrettyTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

PrettyTable::PrettyTable(const char *head[], size_t items) : mTableHandle(NULL), miRows(0),
miDataRow(miDataRow), miItems(0), miEntryWidth(0), mpiItemLength(NULL), mArrTableData(NULL), mpData(NULL)
{
	initPrettyTable(head, items);
}


PrettyTable::~PrettyTable()
{
	unInitPrettyTable();
}

void PrettyTable::initPrettyTable(const char *head[], size_t items)
{
	miItems = items;  // 多少个表项，初始化后表的结构也就确定了
	mpiItemLength = (size_t**)malloc(sizeof(size_t*)); // 初始申请，表项长度
	mpiItemLength[miRows] = (size_t *)malloc(miItems * sizeof(size_t)); // 先多申请一行, 备用，参见update
	memset(mpiItemLength[miRows], 0, miItems * sizeof(size_t));
	mArrTableData = (table_item_data_t**)malloc(sizeof(table_item_data_t*)); // 初始申请，表格数据

	addRowImpl(head, true); // 先添加表格上边界
	addRow(head);  // 添加表头

}

void PrettyTable::unInitPrettyTable()
{

}

table_item_data_t* PrettyTable::createItem()
{
	table_item_data_t *itemNode = (table_item_data_t*)malloc(sizeof(table_item_data_t));
	if (!itemNode) {
		return NULL;
	}

	return itemNode;
}

void PrettyTable::addRow(const char *rowData[])
{
	miDataRow++;
	mpData = (void*)malloc(miDataRow * sizeof(const char**));
	//updateTable(rowData);

	addRowImpl(rowData, false);  // 添加数据
	addRowImpl(rowData, true);   // 添加下边界
}

void PrettyTable::addRowImpl(const char *rowData[], bool isBoundary)
{
	miRows++;
	mArrTableData = (table_item_data_t**)realloc(mArrTableData, (miRows) * sizeof(table_item_data_t*));

	// mpiItemLength数组下标从1开始正式使用, mArrTableData从0开始正式使用
	mpiItemLength[miRows] = (size_t *)malloc(miItems * sizeof(size_t));
	memset(mpiItemLength[miRows], 0, miItems * sizeof(size_t));
	mArrTableData[miRows - 1] = (table_item_data_t*)malloc(miItems * sizeof(table_item_data_t));
	memset(mArrTableData[miRows - 1], 0, miItems * sizeof(table_item_data_t));

	// 拼接数据
	for (int col = 0; col < miItems; col++) {
		mpiItemLength[miRows][col] = strlen(rowData[col]);

		//mArrTableData[row][col].isBoundary = isBoundary;
		mArrTableData[miRows - 1][col].item_size = sizeof('|') + sizeof(' ') + mpiItemLength[miRows][col] + sizeof(' ') + sizeof('\0'); // 每个iteam首尾各添一个空格，首部一个'|'，字符串尾部'\0'

		miEntryWidth += mArrTableData[miRows - 1][col].item_size;
	}
	miEntryWidth += (sizeof('|') + sizeof('\0')); // 最后一个添一个'\0'

	for (int col = 0; col < miItems; col++) {
		size_t boundary_extra = 0;
		if (col == miItems - 1) {
			boundary_extra = 1;
		}

		// 每个item单独分配空间，操作保证字符串以'\0'结束
		mArrTableData[miRows - 1][col].item_data = (void*)malloc((mArrTableData[miRows - 1][col].item_size + boundary_extra) * sizeof(char));

		if (isBoundary) {
			memset(mArrTableData[miRows - 1][col].item_data, '-', (mArrTableData[miRows - 1][col].item_size + boundary_extra) * sizeof(char));
			memcpy((char*)mArrTableData[miRows - 1][col].item_data, "+", sizeof('+'));
			memcpy((char*)mArrTableData[miRows - 1][col].item_data + sizeof('+') + sizeof(' ') + mpiItemLength[miRows][col] * sizeof(char) + sizeof(' '), "\0", sizeof('\0'));
			if (col == miItems - 1) {
				memcpy((char*)mArrTableData[miRows - 1][col].item_data + sizeof('+') + sizeof(' ') + mpiItemLength[miRows][col] * sizeof(char) + sizeof(' '), "+\0", sizeof('+') + sizeof('\0'));
			}
		}
		else {
			memset(mArrTableData[miRows - 1][col].item_data, 0, (mArrTableData[miRows - 1][col].item_size + boundary_extra) * sizeof(char));
			memcpy((char*)mArrTableData[miRows - 1][col].item_data, "| ", sizeof('|') + sizeof(' '));
			memcpy((char*)mArrTableData[miRows - 1][col].item_data + sizeof('|') + sizeof(' '), rowData[col], mpiItemLength[miRows][col] * sizeof(char));
			memcpy((char*)mArrTableData[miRows - 1][col].item_data + sizeof('|') + sizeof(' ') + mpiItemLength[miRows][col] * sizeof(char), " ", sizeof(' '));

			if (col == miItems - 1) {
				memcpy((char*)mArrTableData[miRows - 1][col].item_data + sizeof('|') + sizeof(' ') + mpiItemLength[miRows][col] * sizeof(char) + sizeof(' '), "|\0", sizeof('|') + sizeof('\0'));
			}
		}
	}
}

void PrettyTable::updateTable(const char *rowData[])
{
	// TODO(tangxuan): 表格结构自适应最长字符串表项
	// 使用 mpiItemLength[0] 记录最大值
	for (int row = 3; row < miRows; row += 2) { // 找最大值的过程可优化？
		for (int col = 0; col < miItems; col++) {
			//printf("%d\t", mpiItemLength[row][col]);
				mpiItemLength[0][col] = mpiItemLength[row][col] > mpiItemLength[row - 2][col] ? mpiItemLength[row][col] : mpiItemLength[row - 2][col];
		}
	}

	for (int col = 0; col < miItems; col++) {
		printf("%d\t", mpiItemLength[0][col]);
	}
}
void PrettyTable::printTable(const char *filename)
{
	printf("row = %d\tinter_row = %d\tinter_item = %d\n", miDataRow, miRows, miItems);
	for (int row = 1; row < miRows; row++) {
		for (int col = 0; col < miItems; col++) {
			printf("%s", (char*)mArrTableData[row][col].item_data);
			//printf("%d\n", mpiItemLength[col]);
		}
		printf("\n");
	}
}
