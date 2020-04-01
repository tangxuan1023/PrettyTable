#pragma once

#include <string>
#include <vector>
using namespace std;
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
	bool isBoundary;
	//struct _table_item_data_s *next_item;
} table_item_data_t;

typedef struct {
	string word;
} TableItem;

typedef struct {
	vector<TableItem> items;
	bool isBoundary;
} TableRow, TableHead;

typedef vector<TableRow> TableMat;

class PrettyTable
{
public:
	PrettyTable(const string &head);
	PrettyTable(const PrettyTable &tb);
	PrettyTable& operator= (const PrettyTable &tb);
	~PrettyTable();
public:
	void addRow(const string &str);
	void printTable(const char *filename=nullptr);

private:
	void init(const string &head);
	void uninit();

	string getRowDataString(const string &str);
	void addRow(TableRow &row);
	TableRow &buildDataRow(const string& str, char symbol, bool create_empty_item = false);
	TableRow &buildBoundaryRow();
	void update();
	void clearRow();
	void showTable(FILE *fp);
private:
	int m_rows;
	int m_items;
	size_t *m_maxItemSize;
	TableMat m_tableMat;
	TableRow m_tableRow;
};
