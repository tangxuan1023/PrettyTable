#include "ArgsParser.h"
#include "basic_defs.h"
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>

#include <stdio.h>
#include <stdarg.h>
#include <varargs.h>

namespace {  // anonymous namespace
	const char* noneValue = "<none>";

	string cat_string(const string& str)
	{
		int left = 0, right = (int)str.length();
		while (left < right && str[left] == ' ')
			left++;
		while (right > left && str[right - 1] == ' ')
			right--;
		return left >= right ? string("") : str.substr(left, right - left);
	}

	bool parse_bool(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		std::istringstream is(str);
		bool b;
		is >> (str.size() > 1 ? std::boolalpha : std::noboolalpha) >> b;
		printf("parsed as: %d\n", b);
		return b;
	}

	const char* get_type_name(int type)
	{
		if (type == Param::INT)
			return "int";
		if (type == Param::UINT)
			return "unsigned";
		if (type == Param::UINT64)
			return "unsigned long long";
		if (type == Param::BOOLEAN)
			return "bool";
		if (type == Param::FLOAT)
			return "float";
		if (type == Param::DOUBLE)
			return "double";
		if (type == Param::STRING)
			return "string";
		return "unknown";
	}

	string format(const char *fmt, ...)
	{
		char buf[256] = { 0 };
		va_list ap;

		va_start(ap, fmt);
		vsnprintf(buf, sizeof(buf), fmt, ap);
		va_end(ap);
		return string(buf);
	}

	//static bool cmp_params(const ArgsParserParams & p1, const ArgsParserParams & p2)
	//{
	//	if (p1.number < p2.number)
	//		return true;

	//	if (p1.number > p2.number)
	//		return false;

	//	return p1.keys[0].compare(p2.keys[0]) < 0;
	//}

	void from_str(const string& str, int type, void* dst)
	{
		std::stringstream ss(str.c_str());
		if (type == Param::INT)
			ss >> *(int*)dst;
		else if (type == Param::BOOLEAN) {
			std::string temp;
			ss >> temp;
			*(bool*)dst = parse_bool(temp);
		}
		else if (type == Param::UINT)
			ss >> *(unsigned*)dst;
		else if (type == Param::UINT64)
			ss >> *(uint64*)dst;
		else if (type == Param::FLOAT)
			ss >> *(float*)dst;
		else if (type == Param::DOUBLE)
			ss >> *(double*)dst;
		else if (type == Param::STRING)
			*(string*)dst = str;
		else
			printf("unknown/unsupported parameter type\n");

		if (ss.fail()) {
			printf("can not convert: %s to %s\n", str.c_str(), get_type_name(type));
		}
	}
};  // end of anonymous namespace

struct ArgsParserParams
{
public:
	string help_message;
	string def_value;
	std::vector<string> keys;
	int number;
};

// class ArgsParser::Impl
struct ArgsParser::Impl
{
	bool error;
	string error_message;
	string about_message;

	string path_to_app;
	string app_name;

	std::vector<ArgsParserParams> data;

	std::vector<string> split_range_string(const string& str, char fs, char ss) const;
	std::vector<string> split_string(const string& str, char symbol = ' ', bool create_empty_item = false) const;

	void apply_params(const string& key, const string& value);
	void apply_params(int i, string value);

	void sort_params();
	int refcount;
};

void ArgsParser::Impl::apply_params(const string& key, const string& value)
{
	for (size_t i = 0; i < data.size(); i++) {
		for (size_t k = 0; k < data[i].keys.size(); k++) {
			if (key.compare(data[i].keys[k]) == 0) {
				data[i].def_value = value;
				break;
			}
		}
	}
}

void ArgsParser::Impl::apply_params(int i, string value)
{
	for (size_t j = 0; j < data.size(); j++) {
		if (data[j].number == i) {
			data[j].def_value = value;
			break;
		}
	}
}

void ArgsParser::Impl::sort_params()
{
	for (size_t i = 0; i < data.size(); i++) {
		std::sort(data[i].keys.begin(), data[i].keys.end());
	}

	std::sort(data.begin(), data.end(),
		[](auto &p1, auto &p2) {
		if (p1.number < p2.number) return true;
		if (p1.number > p2.number) return false;
		return p1.keys[0].compare(p2.keys[0]) < 0; }
	);
}

std::vector<string> ArgsParser::Impl::split_range_string(const string& _str, char fs, char ss) const
{
	string str = _str;
	std::vector<string> vec;
	string word = "";
	bool begin = false;
	while (!str.empty()) {
		if (str[0] == fs) {
			if (begin == true) {
				printf("error in split_range_string(%s, %c, %c), %s, %d", str.c_str(), fs, ss, __FILE__, __LINE__);
			}
			begin = true;
			word = "";
			str = str.substr(1, str.length() - 1);
		}

		if (str[0] == ss) {
			if (begin == false) {
				printf("error in split_range_string(%s, %c, %c), %s, %d", str.c_str(), fs, ss, __FILE__, __LINE__);
			}
			begin = false;
			vec.push_back(word);
		}

		if (begin == true) {
			word = word + str[0];
		}
		str = str.substr(1, str.length() - 1);
	}

	if (begin == true) {
		printf("error in split_range_string(%s, %c, %c), %s, %d", str.c_str(), fs, ss, __FILE__, __LINE__);
	}
	return vec;
}

std::vector<string> ArgsParser::Impl::split_string(const string& _str, char symbol, bool create_empty_item) const
{
	string str = _str;
	std::vector<string> vec;
	string word = "";

	while (!str.empty()) {
		if (str[0] == symbol) {
			if (!word.empty() || create_empty_item) {
				vec.push_back(word);
				word = "";
			}
		}
		else {
			word = word + str[0];
		}
		str = str.substr(1, str.length() - 1);
	}

	if (word != "" || create_empty_item) {
		vec.push_back(word);
	}

	return vec;
}

// class ArgsParser
ArgsParser::ArgsParser(int argc, const char* const argv[], const string& keys)
{
	impl = new Impl;
	impl->refcount = 1;

	// path to application
	size_t pos_s = string(argv[0]).find_last_of("/\\");
	if (pos_s == string::npos) {
		impl->path_to_app = "";
		impl->app_name = string(argv[0]);
	}
	else {
		impl->path_to_app = string(argv[0]).substr(0, pos_s);
		impl->app_name = string(argv[0]).substr(pos_s + 1, string(argv[0]).length() - pos_s);
	}

	impl->error = false;
	impl->error_message = "";

	// parse keys
	// string keys is something like "{ key | value | help_message }"
	std::vector<string> k_vec = impl->split_range_string(keys, '{', '}');

	int idx = 0;
	for (size_t i = 0; i < k_vec.size(); i++) {
		std::vector<string> l = impl->split_string(k_vec[i], '|', true);
		ArgsParserParams p;
		p.keys = impl->split_string(l[0]);
		p.def_value = l[1];
		p.help_message = cat_string(l[2]);
		p.number = -1;
		if (p.keys.size() <= 0) {
			impl->error = true;
			impl->error_message = "Field KEYS could not be empty\n";
		}
		else {
			if (p.keys[0][0] == '@') {
				p.number = idx;
				idx++;
			}

			impl->data.push_back(p);
		}
	}

	// parse argv
	idx = 0;
	for (int i = 1; i < argc; i++) {
		string s(argv[i]);
		bool hasSingleDash = s.length() > 1 && s[0] == '-';

		if (hasSingleDash) {
			bool hasDoubleDash = s.length() > 2 && s[1] == '-';
			string key = s.substr(hasDoubleDash ? 2 : 1);
			string value = "true";
			size_t equalsPos = key.find('=');

			if (equalsPos != string::npos) {
				value = key.substr(equalsPos + 1);
				key = key.substr(0, equalsPos);
			}
			impl->apply_params(key, value);
		}
		else {
			impl->apply_params(idx, s);
			idx++;
		}
	}

	impl->sort_params();
}

ArgsParser::~ArgsParser()
{
	if (FETCH_XADD(&impl->refcount, -1) == 1)
		delete impl;
}

ArgsParser::ArgsParser(const ArgsParser& parser)
{
	impl = parser.impl;
	FETCH_XADD(&impl->refcount, 1);
}

ArgsParser& ArgsParser::operator = (const ArgsParser& parser)
{
	if (this != &parser) {
		FETCH_XADD(&parser.impl->refcount, 1);
		if (FETCH_XADD(&impl->refcount, -1) == 1)
			delete impl;
		impl = parser.impl;
	}
	return *this;
}

void ArgsParser::about(const string& message)
{
	impl->about_message = message;
}

string ArgsParser::getPathToApplication() const
{
	return impl->path_to_app;
}

bool ArgsParser::has(const string& name) const
{
	for (size_t i = 0; i < impl->data.size(); i++) {
		for (size_t j = 0; j < impl->data[i].keys.size(); j++) {
			if (name == impl->data[i].keys[j]) {
				const string v = cat_string(impl->data[i].def_value);
				return !v.empty() && v != noneValue;
			}
		}
	}

	printf("undeclared key '%s' requested", name.c_str());
	return true;
}

bool ArgsParser::check() const
{
	return impl->error == false;
}

void ArgsParser::printErrors() const
{
	if (impl->error) {
		printf("\nERRORS:\n%s\n", impl->error_message.c_str());
		fflush(stdout);
	}
}

void ArgsParser::printMessage() const
{
	if (impl->about_message != "")
		printf("%s\n", impl->about_message.c_str());

	printf("Usage: %s [params] ", impl->app_name.c_str());

	for (size_t i = 0; i < impl->data.size(); i++) {
		if (impl->data[i].number > -1) {
			string name = impl->data[i].keys[0].substr(1, impl->data[i].keys[0].length() - 1);
			printf("%s ", name.c_str());
		}
	}

	printf("\n\n");

	for (size_t i = 0; i < impl->data.size(); i++) {
		if (impl->data[i].number == -1) {
			printf("\t");
			for (size_t j = 0; j < impl->data[i].keys.size(); j++) {
				string k = impl->data[i].keys[j];
				if (k.length() > 1) {
					printf("--");
				}
				else {
					printf("-");
				}
				printf("%s", k.c_str());

				if (j != impl->data[i].keys.size() - 1) {
					printf(", ");
				}
			}
			string dv = cat_string(impl->data[i].def_value);
			if (dv.compare("") != 0) {
				printf(" (value:%s)", dv.c_str());
			}
			printf("\n\t\t%s\n", impl->data[i].help_message.c_str());
		}
	}
	printf("\n");

	for (size_t i = 0; i < impl->data.size(); i++) {
		if (impl->data[i].number != -1) {
			printf("\t");
			string k = impl->data[i].keys[0];
			k = k.substr(1, k.length() - 1);

			printf("%s", k.c_str());

			string dv = cat_string(impl->data[i].def_value);
			if (dv.compare("") != 0) {
				printf(" (value:%s)", dv.c_str());
			}
			printf("\n\t\t%s\n", impl->data[i].help_message.c_str());
		}
	}
}

void ArgsParser::getByName(const string& name, bool space_delete, int type, void* dst) const
{
	try {
		for (size_t i = 0; i < impl->data.size(); i++) {
			for (size_t j = 0; j < impl->data[i].keys.size(); j++) {
				if (name == impl->data[i].keys[j]) {
					string v = impl->data[i].def_value;
					if (space_delete) v = cat_string(v);

					// the key was neither specified nor has a default value
					if ((v.empty() && type != Param::STRING) || v == noneValue) {
						impl->error = true;
						impl->error_message = impl->error_message + "Missing parameter: '" + name + "'\n";
						return;
					}

					from_str(v, type, dst);
					return;
				}
			}
		}
	}
	catch (exception e) {
		impl->error = true;
		impl->error_message = impl->error_message + "Parameter '" + name + "': " + e.what() + "\n";
		return;
	}
	printf("undeclared key '%s' requested", name.c_str());
}

void ArgsParser::getByIndex(int index, bool space_delete, int type, void* dst) const
{
	try {
		for (size_t i = 0; i < impl->data.size(); i++) {
			if (impl->data[i].number == index) {
				string v = impl->data[i].def_value;
				if (space_delete == true) v = cat_string(v);

				// the key was neither specified nor has a default value
				if ((v.empty() && type != Param::STRING) || v == noneValue) {
					impl->error = true;
					impl->error_message = impl->error_message + format("Missing parameter #%d\n", index);
					return;
				}
				from_str(v, type, dst);
				return;
			}
		}
	}
	catch (exception e) {
		impl->error = true;
		impl->error_message = impl->error_message + format("Parameter #%d: ", index) + e.what() + "\n";
		return;
	}
	printf("undeclared position %d requested", index);
}
