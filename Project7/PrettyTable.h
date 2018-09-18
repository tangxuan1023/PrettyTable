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
	bool isBoundary;
	//struct _table_item_data_s *next_item;
} table_item_data_t;

// TODO(tangxuan): 使用动态表（表扩张和收缩）优化，结果需支持写入文件（大流量I/O管理）
class PrettyTable
{
public:
	PrettyTable(const char *head[], size_t items);
	~PrettyTable();
public:
	void initPrettyTable(const char *head[], size_t items);  // 表头
	void unInitPrettyTable();
	void addRow(const char *rowData[]);  
	void printTable(const char *filename);  // 打印或输出到文件

private:
	PrettyTable();
	PrettyTable(const PrettyTable &tb);   //TODO(tangxuan): 复制表格，待实现
	PrettyTable& operator= (const PrettyTable &tb);

	void updateTable(const char *rowData[]); // 更新表格布局（根据新添加的数据长度）
	void showTable();
	void addRowImpl(const char *rowData[], bool isBoundary);  // 添加一行数据
private:
	int miRows;  // 多少行数据，包含边界、表头
	int miItems;  // 每行多少个items
	size_t **mpiItemLength; // 每列字符串长度，即多少个字符，以最多那行作参考
	table_item_data_t **mArrTableData; // 存储表格的所有数据、包含表头、边界
	void *mpData; // 数据行（包含表头，不包含边界）
};

#endif // __PRETTYTABLE_H__