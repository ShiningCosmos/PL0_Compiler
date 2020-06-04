#ifndef _LEXANALYSIS_
#define _LEXANALYSIS_

#include "common.h"

//初始化
void lexInit()
{
	bufPos = 0;
	strToken = "";
	line = 1;
	col = 0;
	symTable.clear();
	constTable.clear();
	lexTable.clear();
}

//从缓冲区中获取字符
void getChar()
{
	if(bufPos<inputBuf.size())
		nowChar = inputBuf[bufPos++];
}

//跳过无效字符
void getBC()
{
	col++;
	while((bufPos < inputBuf.size()) && 
		(nowChar==' '||nowChar=='\t'||nowChar=='\n'))
	{
		if(nowChar == '\n')
		{
			line++;
			col = 1;
		}
		getChar();
	}
	if(bufPos >= inputBuf.size())
		nowChar = '$';
}

//将字符附加到strToken
void concat()
{
	strToken += nowChar;
}

//回退一步
void retract()
{
	nowChar=inputBuf[--bufPos];
}

//字母
bool letterCase(char c)
{
	return (c>='a'&&c<='z')||(c>='A' && c<='Z');
}

//数字
bool numberCase(char c)
{
	return c>='0'&&c<='9';
}

//放入标识符表，返回位置
int insertID()
{
	for(int i=0;i<symTable.size();i++)
		if(symTable[i]==strToken)
			return i;
	symTable.push_back(strToken);
	return symTable.size()-1;
}

//放入常量表，返回位置
int insertConst()
{
	for(int i=0;i<constTable.size();i++)
		if(constTable[i]==strToken)
			return i;
	constTable.push_back(strToken);
	return constTable.size()-1;
}

//获取关键字，返回位置
int reserve()
{
	for(int i=0;i<keyword.size();i++)
		if(keyword[i]==strToken)
			return i+1;
	return 0;
}

//输出词法分析的结果
void showLexResult(int opt)
{
	cout<<"词法分析二元式结果："<<endl;
	if(opt == 1 || opt == 3)
	for(int i=0;i<lexTable.size();i++)
	{
		cout<<"line:"<<lexTable[i].line<<"\tcolumn:"<<lexTable[i].col<<"\t"<<i+1<<".\t";
		cout<<"("<<lexTable[i].category<<"，";
		if(lexTable[i].category==SYM)	//输出标识符
			cout<<symTable[lexTable[i].attribute];
		else if(lexTable[i].category==CONS)	//输出常量
			cout<<constTable[lexTable[i].attribute];
		else if(lexTable[i].category==myEOF)	//输出文件结束符
			cout<<"EOF";
		else
		{//输出关键字、运算符、界符
			if(lexTable[i].category>KWBASE&&lexTable[i].category<OPBASE)
				cout<<keyword[lexTable[i].category - KWBASE -1];
			else if(lexTable[i].category>OPBASE&&lexTable[i].category<BSBASE)
				cout<<operators[lexTable[i].category - OPBASE -1];
			else
				cout<<boundSymb[lexTable[i].category - BSBASE -1];
		}
		cout<<")"<<endl;
	}
	if(opt == 2 || opt == 3)
	{
		cout<<"原始二元式结果："<<endl;
		for(int i=0;i<lexTable.size();i++)
		{
			cout<<"line:"<<lexTable[i].line<<"\tcolumn:"<<lexTable[i].col<<"\t"<<i+1<<".\t";
			cout<<"("<<lexTable[i].category<<"，";
			if(lexTable[i].category==SYM)	//输出标识符
				cout<<lexTable[i].attribute;
			else if(lexTable[i].category==CONS)	//输出常量
				cout<<lexTable[i].attribute;
			else if(lexTable[i].category==EOF)	//输出文件结束符
				cout<<"EOF";
			else
			{//输出关键字、运算符、界符
				cout<<" ";
			}
			cout<<")"<<endl;
		}
	}
	cout<<"标识符表："<<endl;
	for(int i=0;i<symTable.size();i++)
	{
		cout<<i+1<<"\t"<<symTable[i]<<endl;
	}
	cout<<"常数表："<<endl;
	for(int i=0;i<constTable.size();i++)
	{
		cout<<i+1<<"\t"<<constTable[i]<<endl;
	}
}

//每次分析一个单词，返回二元式
binaryWord getSym()
{
	strToken = "";
	int code=0,value=0;
	getChar();
	getBC();
	if(nowChar == '$')
	{//结束了
		return binaryWord(myEOF,line,col);
	}
	if(letterCase(nowChar))
	{//单词为标识符或者关键字
		while(letterCase(nowChar)||numberCase(nowChar))
		{
			concat();
			getChar();
		}
		retract();
		code = reserve();
		if(code==0)//单词为标识符
		{
			if(strToken.size() > MAXSYMLEN)//标识符超过最大长度，进行截断
				strToken = strToken.substr(0,10);
			value = insertID();
			return binaryWord(SYM,value,line,col);
		}
		else//单词为关键字
			return binaryWord(code,line,col);
	}
	else if(numberCase(nowChar))
	{//单词为常数
		while(numberCase(nowChar))
		{
			concat();
			getChar();
		}
		retract();
		value = insertConst();
		return binaryWord(CONS,value,line,col);
	}
	else
	{//单词为运算符或界符
		switch(nowChar)
		{
			//单词为运算符
			case '+':
				return binaryWord(ADD,line,col);
			case '-':
				return binaryWord(SUB,line,col);
			case '*':
				return binaryWord(MUL,line,col);
			case '/':
				return binaryWord(DIV,line,col);
			case '=':
				return binaryWord(EQU,line,col);
			case '#':
				return binaryWord(NEQU1,line,col);
			case '<':
				getChar();
				if(nowChar == '=')
					return binaryWord(LESE,line,col);
				else if(nowChar == '>')
					return binaryWord(NEQU2,line,col);
				else
					retract();
				return binaryWord(LES,line,col);
			case '>':
				getChar();
				if(nowChar == '=')
					return binaryWord(LARE,line,col);
				else
					retract();
				return binaryWord(LAR,line,col);
			case ':':
				getChar();
				if(nowChar == '=')
					return binaryWord(ASSI,line,col);
				else
					error(lexError);
			//单词为界符
			case '.':
				return binaryWord(POI,line,col);
			case ',':
				return binaryWord(COM,line,col);
			case ';':
				return binaryWord(SEM,line,col);
			case '(':
				return binaryWord(LBR,line,col);
			case ')':
				return binaryWord(RBR,line,col);
			//出错
			default:
				error(lexError);
		}
	}
	return binaryWord(-1);//只是一个形式，实际上不会到这个return
}

//词法分析函数，循环处理源代码
void lexAnalysis()
{
	lexInit();
	binaryWord tmp;
	while(bufPos<inputBuf.size())
	{
		tmp = getSym();
		lexTable.push_back(tmp);//把结束符放入词法分析结果表
		if(nowChar == '$')
			break;
	}
}


#endif //_LEXANALYSIS_