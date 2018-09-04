#include "PrettyTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

PrettyTable::PrettyTable(const char *head[], size_t items) :
	miRows(0), miItems(0),
	mpiItemLength(NULL),
	mArrTableData(NULL), 
	mpData(NULL)
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
	mpiItemLength = (size_t**)malloc(sizeof(size_t*)); // 初始申请，表项指针长度，4
	mpiItemLength[miRows] = (size_t *)malloc(miItems * sizeof(size_t)); // 先多申请一行, 备用，参见update
	memset(mpiItemLength[miRows], 0, miItems * sizeof(size_t));
	mArrTableData = (table_item_data_t**)malloc(sizeof(table_item_data_t*)); // 初始申请，表格数据
	mArrTableData[miRows] = (table_item_data_t*)malloc(miItems * sizeof(table_item_data_t));  // 先多申请一行，不作使用

	// TODO(tangxuan):表格行数未知，这里先假定15
	mpData = (void*)malloc( 15 * sizeof(long));  // 用于存储外界输入的字符指针数组地址（转换为整数）

	addRow(head);  // 添加表头
}

void PrettyTable::unInitPrettyTable()
{
	if (mpData) {
		free(mpData);
		mpData = NULL;
	}
	
	if (mArrTableData) {
		for (int i = 0; i < miRows; i++) {
			free(mArrTableData[i]);
			mArrTableData[i] = NULL;
		}
		free(mArrTableData);
		mArrTableData = NULL;
	}

	if (mpiItemLength) {
		for (int i = 0; i < miRows; i++) {
			free(mpiItemLength[i]);
			mpiItemLength[i] = NULL;
		}
		free(mpiItemLength);
		mpiItemLength = NULL;
	}
}

void PrettyTable::addRow(const char *rowData[])
{
	//mpData = (void*)realloc(mpData, (miRows + 1) * sizeof(long));  // 异常

	addRowImpl(rowData, true);   // 添加上边界
	addRowImpl(rowData, false);  // 添加数据行
	updateTable(rowData);  // 更新表结构
}

void PrettyTable::addRowImpl(const char *rowData[], bool isBoundary)
{
	miRows++;
	mArrTableData =
		(table_item_data_t**)realloc(mArrTableData, (miRows + 1) * sizeof(table_item_data_t*));

	// mpiItemLength数组下标从1开始正式使用
	mpiItemLength[miRows] = (size_t *)malloc(miItems * sizeof(size_t));
	memset(mpiItemLength[miRows], 0, miItems * sizeof(size_t));
	mArrTableData[miRows] = (table_item_data_t*)malloc(miItems * sizeof(table_item_data_t));  // 每行miItems个节点,由初始化决定
	memset(mArrTableData[miRows], 0, miItems * sizeof(table_item_data_t));

	(reinterpret_cast<long*>(mpData))[miRows] = reinterpret_cast<long>(reinterpret_cast<void*>(rowData));

	for (int col = 0; col < miItems; col++) {
		mpiItemLength[miRows][col] = strlen(rowData[col]);

		mArrTableData[miRows][col].isBoundary = isBoundary;
		mArrTableData[miRows][col].item_size = sizeof('|') + sizeof(' ') \
			+ mpiItemLength[miRows][col] + sizeof(' ') + sizeof('\0'); // 每个iteam首尾各添一个空格，首部一个'|'，字符串尾部'\0'
	}
}

void PrettyTable::showTable()
{
	// 拼接数据
	for (int row = 1; row < miRows; row++) {
		const char **rowData = reinterpret_cast<const char **> \
			(reinterpret_cast<void*>((reinterpret_cast<long*>(mpData))[row]));

		for (int col = 0; col < miItems; col++) {
			size_t boundary_extra = 0;
			if (col == miItems - 1) {
				boundary_extra = 1;
			}

			// 每个item单独分配空间，操作保证字符串以'\0'结束
			// update之后mpiItemLength[0]存储每列最大值
			mArrTableData[row][col].item_data =
				(void*)malloc((mArrTableData[row][col].item_size \
					+ boundary_extra) * sizeof(char));

			if (mArrTableData[row][col].isBoundary) {
				memset(mArrTableData[row][col].item_data,
					'-',
					(mArrTableData[row][col].item_size + boundary_extra) * sizeof(char));

				memcpy((char*)mArrTableData[row][col].item_data,
					"+",
					sizeof('+'));

				memcpy((char*)mArrTableData[row][col].item_data \
					+ sizeof('+') + sizeof(' ') + \
					mpiItemLength[0][col] * sizeof(char) + sizeof(' '),
					"\0",
					sizeof('\0'));

				if (col == miItems - 1) {
					memcpy((char*)mArrTableData[row][col].item_data \
						+ sizeof('+') + sizeof(' ') \
						+ mpiItemLength[0][col] * sizeof(char) + sizeof(' '),
						"+\0",
						sizeof('+') + sizeof('\0'));
				}
			}
			else { // 最后一行一定是边界，故 row 不会越界
				size_t remain = 2 + (mpiItemLength[0][col] - mpiItemLength[row][col] > 0 ? \
					mpiItemLength[0][col] - mpiItemLength[row][col] : 0);
				//printf("len: %u \tremain: %u\n", mpiItemLength[row][col], remain);

				memset(mArrTableData[row][col].item_data,
					0,
					(mArrTableData[row][col].item_size + boundary_extra) * sizeof(char));

				memcpy((char*)mArrTableData[row][col].item_data,
					"|",
					sizeof('|'));

				for (int i = 0; i < (remain >> 1); i++) {
					memcpy((char*)mArrTableData[row][col].item_data + sizeof('|') + i * sizeof(' '),
						" ",
						sizeof(' '));
				}

				memcpy((char*)mArrTableData[row][col].item_data \
					+ sizeof('|') + (remain >> 1) * sizeof(' '),
					rowData[col],
					mpiItemLength[row][col] * sizeof(char));

				for (int i = 0; i < remain - (remain >> 1); i++) {
					memcpy((char*)mArrTableData[row][col].item_data \
						+ sizeof('|') + (remain >> 1) * sizeof(' ') \
						+ mpiItemLength[row][col] * sizeof(char) + i * sizeof(' '),
						" ",
						sizeof(' '));
				}

				if (col == miItems - 1) {
					memcpy((char*)mArrTableData[row][col].item_data \
						+ sizeof('|') + (remain >> 1) * sizeof(' ') \
						+ mpiItemLength[0][col] * sizeof(char) \
						+ (remain - (remain >> 1)) * sizeof(' '),
						"|\0",
						sizeof('|') + sizeof('\0'));
				}
			}
		}  // end for loop col
	} // end for loop row
}

void PrettyTable::updateTable(const char *rowData[])
{
	// TODO(tangxuan): 表格结构自适应最长字符串表项
	// 使用 mpiItemLength[0] 记录最大值
	for (int row = 3; row < miRows; row += 2) { // 找最大值的过程可优化？
		for (int col = 0; col < miItems; col++) {
			mpiItemLength[0][col] = mpiItemLength[row][col] > mpiItemLength[row - 2][col] ? \
				mpiItemLength[row][col] : mpiItemLength[row - 2][col];
		}
	}

	/*for (int col = 0; col < miItems; col++) {
		printf("%d\t", mpiItemLength[0][col]);
	}*/

	for (int row = 0; row < miRows; row++) {
		for (int col = 0; col < miItems; col++) {
			mArrTableData[row][col].item_size = sizeof('|') + sizeof(' ') \
				+ mpiItemLength[0][col] + sizeof(' ') + sizeof('\0'); // 更新为每列最长那个iteam首尾各添一个空格，首部一个'|'，字符串尾部'\0'
		}
	}
}

void PrettyTable::printTable(const char *filename)
{
	showTable();
	for (int row = 1; row < miRows; row++) {
		for (int col = 0; col < miItems; col++) {
			printf("%s", (char*)mArrTableData[row][col].item_data);
		}
		printf("\n");
	}
}
