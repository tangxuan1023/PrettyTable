#ifndef __PRETTYTABLE_H__
#define __PRETTYTABLE_H__

//typedef void* HANDLE;
/**
	+---------+------------------+------------+------------+--------+--------+-------+
	| task_id |     content      | begin_date |  end_date  | status | remark | score |
	+---------+------------------+------------+------------+--------+--------+-------+
	|  0x001  | test of the code | 2018-08-16 | 2018-08-17 |   0    |  None  |   0   |
	+---------+------------------+------------+------------+--------+--------+-------+
*/

typedef struct _table_item_data_s {
	size_t item_size;
	void *item_data;
	//bool isBoundary;
	//struct _table_item_data_s *next_item;
} table_item_data_t;

class PrettyTable
{
public:
	PrettyTable(const char *head[], size_t items);
	~PrettyTable();
public:
	void initPrettyTable(const char *head[], size_t items);  // ��ͷ
	void unInitPrettyTable();
	table_item_data_t* createItem();
	void addRow(const char *rowData[]);  

	void updateTable(const char *rowData[]); // ���±�񲼾֣���������ӵ����ݳ��ȣ�
	void printTable(const char *filename);  // ��ӡ��������ļ�

private:
	PrettyTable();
	PrettyTable(const PrettyTable &tb);
	PrettyTable& operator= (const PrettyTable &tb);

	void addRowImpl(const char *rowData[], bool isBoundary);  // ���һ������
private:
	void *mTableHandle;
	int miRows;  // ���������ݣ������߽硢��ͷ
	int miDataRow; // �����У�������ͷ��
	int miItems;  // ÿ�ж��ٸ�items
	size_t miEntryWidth; // ÿ�п�ȣ������ٸ��ַ���������������ο�
	size_t **mpiItemLength; // ÿ���ַ������ȣ������ٸ��ַ���������������ο�
	table_item_data_t **mArrTableData; // �洢�����������ݡ�������ͷ���߽�
	void *mpData; // �����У�������ͷ��
};

#endif // __PRETTYTABLE_H__