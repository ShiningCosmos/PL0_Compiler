#include <iostream>
#include "common.h"
#include "lexAnalysis.cpp"
#include "syntaxAnalysis.cpp"
#include "codeGenAndExec.cpp"
#include "error.cpp"

using namespace std;

int main()
{
	readSourceCode();//��ȡԴ����
	showSourceCode();//չʾԴ����
	syntaxAnalysis();//�����ʷ��������﷨�������﷨����Ŀ�����
	interpret();//����ִ��
	return 0;
}

