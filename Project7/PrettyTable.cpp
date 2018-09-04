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
	miItems = items;  // ���ٸ������ʼ�����ĽṹҲ��ȷ����
	mpiItemLength = (size_t**)malloc(sizeof(size_t*)); // ��ʼ���룬����ָ�볤�ȣ�4
	mpiItemLength[miRows] = (size_t *)malloc(miItems * sizeof(size_t)); // �ȶ�����һ��, ���ã��μ�update
	memset(mpiItemLength[miRows], 0, miItems * sizeof(size_t));
	mArrTableData = (table_item_data_t**)malloc(sizeof(table_item_data_t*)); // ��ʼ���룬�������
	mArrTableData[miRows] = (table_item_data_t*)malloc(miItems * sizeof(table_item_data_t));  // �ȶ�����һ�У�����ʹ��

	// TODO(tangxuan):�������δ֪�������ȼٶ�15
	mpData = (void*)malloc( 15 * sizeof(long));  // ���ڴ洢���������ַ�ָ�������ַ��ת��Ϊ������

	addRow(head);  // ��ӱ�ͷ
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
	//mpData = (void*)realloc(mpData, (miRows + 1) * sizeof(long));  // �쳣

	addRowImpl(rowData, true);   // ����ϱ߽�
	addRowImpl(rowData, false);  // ���������
	updateTable(rowData);  // ���±�ṹ
}

void PrettyTable::addRowImpl(const char *rowData[], bool isBoundary)
{
	miRows++;
	mArrTableData =
		(table_item_data_t**)realloc(mArrTableData, (miRows + 1) * sizeof(table_item_data_t*));

	// mpiItemLength�����±��1��ʼ��ʽʹ��
	mpiItemLength[miRows] = (size_t *)malloc(miItems * sizeof(size_t));
	memset(mpiItemLength[miRows], 0, miItems * sizeof(size_t));
	mArrTableData[miRows] = (table_item_data_t*)malloc(miItems * sizeof(table_item_data_t));  // ÿ��miItems���ڵ�,�ɳ�ʼ������
	memset(mArrTableData[miRows], 0, miItems * sizeof(table_item_data_t));

	(reinterpret_cast<long*>(mpData))[miRows] = reinterpret_cast<long>(reinterpret_cast<void*>(rowData));

	for (int col = 0; col < miItems; col++) {
		mpiItemLength[miRows][col] = strlen(rowData[col]);

		mArrTableData[miRows][col].isBoundary = isBoundary;
		mArrTableData[miRows][col].item_size = sizeof('|') + sizeof(' ') \
			+ mpiItemLength[miRows][col] + sizeof(' ') + sizeof('\0'); // ÿ��iteam��β����һ���ո��ײ�һ��'|'���ַ���β��'\0'
	}
}

void PrettyTable::showTable()
{
	// ƴ������
	for (int row = 1; row < miRows; row++) {
		const char **rowData = reinterpret_cast<const char **> \
			(reinterpret_cast<void*>((reinterpret_cast<long*>(mpData))[row]));

		for (int col = 0; col < miItems; col++) {
			size_t boundary_extra = 0;
			if (col == miItems - 1) {
				boundary_extra = 1;
			}

			// ÿ��item��������ռ䣬������֤�ַ�����'\0'����
			// update֮��mpiItemLength[0]�洢ÿ�����ֵ
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
			else { // ���һ��һ���Ǳ߽磬�� row ����Խ��
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
	// TODO(tangxuan): ���ṹ����Ӧ��ַ�������
	// ʹ�� mpiItemLength[0] ��¼���ֵ
	for (int row = 3; row < miRows; row += 2) { // �����ֵ�Ĺ��̿��Ż���
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
				+ mpiItemLength[0][col] + sizeof(' ') + sizeof('\0'); // ����Ϊÿ����Ǹ�iteam��β����һ���ո��ײ�һ��'|'���ַ���β��'\0'
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
