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
	miItems = items;  // ���ٸ������ʼ�����ĽṹҲ��ȷ����
	mpiItemLength = (size_t**)malloc(sizeof(size_t*)); // ��ʼ���룬�����
	mpiItemLength[miRows] = (size_t *)malloc(miItems * sizeof(size_t)); // �ȶ�����һ��, ���ã��μ�update
	memset(mpiItemLength[miRows], 0, miItems * sizeof(size_t));
	mArrTableData = (table_item_data_t**)malloc(sizeof(table_item_data_t*)); // ��ʼ���룬�������

	addRowImpl(head, true); // ����ӱ���ϱ߽�
	addRow(head);  // ��ӱ�ͷ

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

	addRowImpl(rowData, false);  // �������
	addRowImpl(rowData, true);   // ����±߽�
}

void PrettyTable::addRowImpl(const char *rowData[], bool isBoundary)
{
	miRows++;
	mArrTableData = (table_item_data_t**)realloc(mArrTableData, (miRows) * sizeof(table_item_data_t*));

	// mpiItemLength�����±��1��ʼ��ʽʹ��, mArrTableData��0��ʼ��ʽʹ��
	mpiItemLength[miRows] = (size_t *)malloc(miItems * sizeof(size_t));
	memset(mpiItemLength[miRows], 0, miItems * sizeof(size_t));
	mArrTableData[miRows - 1] = (table_item_data_t*)malloc(miItems * sizeof(table_item_data_t));
	memset(mArrTableData[miRows - 1], 0, miItems * sizeof(table_item_data_t));

	// ƴ������
	for (int col = 0; col < miItems; col++) {
		mpiItemLength[miRows][col] = strlen(rowData[col]);

		//mArrTableData[row][col].isBoundary = isBoundary;
		mArrTableData[miRows - 1][col].item_size = sizeof('|') + sizeof(' ') + mpiItemLength[miRows][col] + sizeof(' ') + sizeof('\0'); // ÿ��iteam��β����һ���ո��ײ�һ��'|'���ַ���β��'\0'

		miEntryWidth += mArrTableData[miRows - 1][col].item_size;
	}
	miEntryWidth += (sizeof('|') + sizeof('\0')); // ���һ����һ��'\0'

	for (int col = 0; col < miItems; col++) {
		size_t boundary_extra = 0;
		if (col == miItems - 1) {
			boundary_extra = 1;
		}

		// ÿ��item��������ռ䣬������֤�ַ�����'\0'����
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
	// TODO(tangxuan): ���ṹ����Ӧ��ַ�������
	// ʹ�� mpiItemLength[0] ��¼���ֵ
	for (int row = 3; row < miRows; row += 2) { // �����ֵ�Ĺ��̿��Ż���
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
