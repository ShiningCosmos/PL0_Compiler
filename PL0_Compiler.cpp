#include <iostream>
#include "common.h"
#include "lexAnalysis.cpp"
#include "syntaxAnalysis.cpp"
#include "codeGenAndExec.cpp"
#include "error.cpp"

using namespace std;

int main()
{
	readSourceCode();//读取源代码
	showSourceCode();//展示源代码
	syntaxAnalysis();//包括词法分析、语法分析、语法树、目标代码
	interpret();//解释执行
	return 0;
}

