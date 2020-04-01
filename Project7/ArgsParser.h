#pragma once

#include <string>
using namespace std;

typedef unsigned long long uint64;

struct Param {
	enum { INT = 0, UINT, UINT64, BOOLEAN, FLOAT, DOUBLE, STRING, };
};

template<typename _Tp> struct ParamType {};

template<> struct ParamType<int>
{
	typedef int const_param_type;
	typedef int member_type;

	enum { type = Param::INT };
};

template<> struct ParamType<unsigned int>
{
	typedef unsigned int const_param_type;
	typedef unsigned int member_type;

	enum { type = Param::UINT };
};

template<> struct ParamType<uint64>
{
	typedef uint64 const_param_type;
	typedef uint64 member_type;

	enum { type = Param::UINT64 };
};

template<> struct ParamType<bool>
{
	typedef bool const_param_type;
	typedef bool member_type;

	enum { type = Param::BOOLEAN };
};

template<> struct ParamType<float>
{
	typedef float const_param_type;
	typedef float member_type;

	enum { type = Param::FLOAT };
};

template<> struct ParamType<double>
{
	typedef double const_param_type;
	typedef double member_type;

	enum { type = Param::DOUBLE };
};

template<> struct ParamType<string>
{
	typedef const string& const_param_type;
	typedef string member_type;

	enum { type = Param::STRING };
};


class ArgsParser
{
public:
	ArgsParser(const ArgsParser &parser);
	ArgsParser(int argc, const char* const argv[], const string& keys);
	ArgsParser& operator=(const ArgsParser& parser);
	virtual ~ArgsParser();

	string getPathToApplication() const;

	bool has(const string& name) const;
	bool check() const;
	void about(const string& message);
	void printMessage() const;
	void printErrors() const;

	template <typename T>
	T get(const string& name, bool space_delete = true) const
	{
		T val = T();
		getByName(name, space_delete, ParamType<T>::type, (void*)&val);
		return val;
	}

	template <typename T>
	T get(int index, bool space_delete = true) const
	{
		T val = T();
		getByIndex(index, space_delete, ParamType<T>::type, (void*)&val);
		return val;
	}

protected:
	void getByName(const string& name, bool space_delete, int type, void* dst) const;
	void getByIndex(int index, bool space_delete, int type, void* dst) const;

	struct Impl;
	Impl* impl;
};


