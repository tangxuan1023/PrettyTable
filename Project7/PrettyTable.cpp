#include "PrettyTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

PrettyTable::PrettyTable(const string &head) :
	m_rows(0), m_items(0),
	m_maxItemSize(nullptr)
{
	m_tableMat.clear();
	clearRow();

	init(head);
}

PrettyTable::PrettyTable(const PrettyTable &tb)
{
	// TODO: implement, copy members
	this->m_tableMat = tb.m_tableMat;
	this->m_rows = tb.m_rows;
	this->m_items = tb.m_items;
	this->m_maxItemSize = new size_t[this->m_items]();
	for (int col = 0; col < this->m_items; col++) {
		this->m_maxItemSize[col] = tb.m_maxItemSize[col];
	}
}

PrettyTable& PrettyTable::operator= (const PrettyTable &tb)
{
	if (this != &tb) {
		// TODO: implement, copy members
		this->m_tableMat = tb.m_tableMat;
		this->m_rows = tb.m_rows;
		this->m_items = tb.m_items;
		this->m_maxItemSize = new size_t[this->m_items]();
		for (int col = 0; col < this->m_items; col++) {
			this->m_maxItemSize[col] = tb.m_maxItemSize[col];
		}
	}
	return *this;
}

PrettyTable::~PrettyTable()
{
	uninit();
}

void PrettyTable::clearRow()
{
	m_tableRow.isBoundary = false;
	m_tableRow.items.clear();
}

TableRow &PrettyTable::buildDataRow(const string& _str, char symbol, bool create_empty_item)
{
	string str = getRowDataString(_str);
	clearRow();
	TableRow &row = m_tableRow;
	TableItem item = { "" };
	string &word = item.word;
	vector<TableItem> &vec = row.items;
	row.isBoundary = false;

	while (!str.empty()) {
		if (str[0] == symbol) {
			if (!word.empty() || create_empty_item) {
				if (!create_empty_item || vec.size() < m_items) {
					vec.push_back(item);
				}
				word = "";
			}
		}
		else {
			word = word + str[0];
		}
		str = str.substr(1, str.length() - 1);
	}

	if (word != "" || create_empty_item) {
		if (!create_empty_item || vec.size() < m_items) {
			vec.push_back(item);
		}
	}

	return row;
}

TableRow &PrettyTable::buildBoundaryRow()
{
	clearRow();
	TableRow &row = m_tableRow;
	TableItem item = { "" };
	string &word = item.word;
	vector<TableItem> &vec = row.items;

	int col = 0;
	word = '+';
	while (col < m_items) {
		string tmp(m_maxItemSize[col], '-');
		word = word + tmp + '+';
		vec.push_back(item);
		col++;
		word = "";
	}

	row.isBoundary = true;

	return row;
}

void PrettyTable::init(const string &head)
{
	TableRow row = buildDataRow(head, '|', false); // head data row
	m_items = row.items.size();
	m_maxItemSize = new size_t[m_items]();

	vector<TableItem>::iterator it = row.items.begin();
	for (int col = 0; col < m_items; col++) {
		TableItem item = *it++;
		m_maxItemSize[col] = sizeof(' ') + item.word.length() + sizeof(' ');
	}

	row.isBoundary = false;
	addRow(row);
}

string PrettyTable::getRowDataString(const string &_str)
{
	string str = _str.substr(1, _str.length() - 2);
	//printf("Check String: %s\n", str.c_str());
	return str;
}

void PrettyTable::uninit()
{
	m_tableMat.clear();
	clearRow();

	if (m_maxItemSize) {
		delete[] m_maxItemSize;
		m_maxItemSize = nullptr;
	}
}

void PrettyTable::addRow(TableRow &row)
{
	m_rows++;
	m_tableMat.push_back(row);
	update();
}

void PrettyTable::addRow(const string &_str)
{
	string str = _str;
	m_rows++;
	TableRow row;
	row = buildDataRow(str, '|', true); // data row

	m_tableMat.push_back(row);
	update();
}

void PrettyTable::showTable(FILE *fp)
{
	TableRow boundaryRow = buildBoundaryRow();
	vector<TableRow>::iterator mat_it = m_tableMat.begin();
	for (; mat_it != m_tableMat.end(); mat_it++) {
		TableRow tableRow = (*mat_it);

		// print top boundary for each row
		vector<TableItem>::iterator item_it = tableRow.items.begin();
		int col = 0;
		for (; item_it != tableRow.items.end(); item_it++) {
			fprintf(fp, "%s", boundaryRow.items[col].word.c_str());
			col++;
		}
		fprintf(fp, "\n");

		//print data row
		item_it = tableRow.items.begin();
		col = 0;
		for (; item_it != tableRow.items.end(); item_it++) {
				if (col == 0) {
					fprintf(fp, "|");  // left boundary for each row
				}
				size_t spaceSize = m_maxItemSize[col] - (*item_it).word.length();
				string headSpace(spaceSize / 2, ' ');
				string tailSpace(spaceSize - spaceSize / 2, ' ');
				fprintf(fp, "%s|", (headSpace +(*item_it).word + tailSpace).c_str());
			col++;
		}
		fprintf(fp, "\n"); 
	}

	// print bottom boundary for last row
	for (int col = 0; col < m_items; col++) {
		fprintf(fp, "%s", boundaryRow.items[col].word.c_str());
	}
}

void PrettyTable::update()
{
	if (m_rows <= 1) return;
	vector<TableRow>::iterator mat_it = m_tableMat.begin();
	mat_it += 1; // excluding head data row
	for (; mat_it != m_tableMat.end(); mat_it++) {
		TableRow tableRow = (*mat_it);
		vector<TableItem>::iterator item_it = tableRow.items.begin();
		if (!tableRow.isBoundary) {	// assert
			int col = 0;
		    for (; item_it != tableRow.items.end(); item_it++) {
				m_maxItemSize[col] = std::max<size_t>(m_maxItemSize[col], sizeof(' ') + (*item_it).word.length() + sizeof(' '));
				//printf("%s %zu\n", (*item_it).word.c_str(), (*item_it).word.length());
				col++;
			}
		}
	}
}


void PrettyTable::printTable(const char *filename)
{
	FILE *fp = nullptr;

	if (filename) {
		fp = fopen(filename, "w");
	}

	if (fp == nullptr) {
		fp = stdout;
	}

	showTable(fp);

	if (filename && fp) {
		fclose(fp);
		fp = nullptr;
	}
}
