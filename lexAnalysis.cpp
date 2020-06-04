#ifndef _LEXANALYSIS_
#define _LEXANALYSIS_

#include "common.h"

//��ʼ��
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

//�ӻ������л�ȡ�ַ�
void getChar()
{
	if(bufPos<inputBuf.size())
		nowChar = inputBuf[bufPos++];
}

//������Ч�ַ�
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

//���ַ����ӵ�strToken
void concat()
{
	strToken += nowChar;
}

//����һ��
void retract()
{
	nowChar=inputBuf[--bufPos];
}

//��ĸ
bool letterCase(char c)
{
	return (c>='a'&&c<='z')||(c>='A' && c<='Z');
}

//����
bool numberCase(char c)
{
	return c>='0'&&c<='9';
}

//�����ʶ��������λ��
int insertID()
{
	for(int i=0;i<symTable.size();i++)
		if(symTable[i]==strToken)
			return i;
	symTable.push_back(strToken);
	return symTable.size()-1;
}

//���볣��������λ��
int insertConst()
{
	for(int i=0;i<constTable.size();i++)
		if(constTable[i]==strToken)
			return i;
	constTable.push_back(strToken);
	return constTable.size()-1;
}

//��ȡ�ؼ��֣�����λ��
int reserve()
{
	for(int i=0;i<keyword.size();i++)
		if(keyword[i]==strToken)
			return i+1;
	return 0;
}

//����ʷ������Ľ��
void showLexResult(int opt)
{
	cout<<"�ʷ�������Ԫʽ�����"<<endl;
	if(opt == 1 || opt == 3)
	for(int i=0;i<lexTable.size();i++)
	{
		cout<<"line:"<<lexTable[i].line<<"\tcolumn:"<<lexTable[i].col<<"\t"<<i+1<<".\t";
		cout<<"("<<lexTable[i].category<<"��";
		if(lexTable[i].category==SYM)	//�����ʶ��
			cout<<symTable[lexTable[i].attribute];
		else if(lexTable[i].category==CONS)	//�������
			cout<<constTable[lexTable[i].attribute];
		else if(lexTable[i].category==myEOF)	//����ļ�������
			cout<<"EOF";
		else
		{//����ؼ��֡�����������
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
		cout<<"ԭʼ��Ԫʽ�����"<<endl;
		for(int i=0;i<lexTable.size();i++)
		{
			cout<<"line:"<<lexTable[i].line<<"\tcolumn:"<<lexTable[i].col<<"\t"<<i+1<<".\t";
			cout<<"("<<lexTable[i].category<<"��";
			if(lexTable[i].category==SYM)	//�����ʶ��
				cout<<lexTable[i].attribute;
			else if(lexTable[i].category==CONS)	//�������
				cout<<lexTable[i].attribute;
			else if(lexTable[i].category==EOF)	//����ļ�������
				cout<<"EOF";
			else
			{//����ؼ��֡�����������
				cout<<" ";
			}
			cout<<")"<<endl;
		}
	}
	cout<<"��ʶ����"<<endl;
	for(int i=0;i<symTable.size();i++)
	{
		cout<<i+1<<"\t"<<symTable[i]<<endl;
	}
	cout<<"������"<<endl;
	for(int i=0;i<constTable.size();i++)
	{
		cout<<i+1<<"\t"<<constTable[i]<<endl;
	}
}

//ÿ�η���һ�����ʣ����ض�Ԫʽ
binaryWord getSym()
{
	strToken = "";
	int code=0,value=0;
	getChar();
	getBC();
	if(nowChar == '$')
	{//������
		return binaryWord(myEOF,line,col);
	}
	if(letterCase(nowChar))
	{//����Ϊ��ʶ�����߹ؼ���
		while(letterCase(nowChar)||numberCase(nowChar))
		{
			concat();
			getChar();
		}
		retract();
		code = reserve();
		if(code==0)//����Ϊ��ʶ��
		{
			if(strToken.size() > MAXSYMLEN)//��ʶ��������󳤶ȣ����нض�
				strToken = strToken.substr(0,10);
			value = insertID();
			return binaryWord(SYM,value,line,col);
		}
		else//����Ϊ�ؼ���
			return binaryWord(code,line,col);
	}
	else if(numberCase(nowChar))
	{//����Ϊ����
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
	{//����Ϊ���������
		switch(nowChar)
		{
			//����Ϊ�����
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
			//����Ϊ���
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
			//����
			default:
				error(lexError);
		}
	}
	return binaryWord(-1);//ֻ��һ����ʽ��ʵ���ϲ��ᵽ���return
}

//�ʷ�����������ѭ������Դ����
void lexAnalysis()
{
	lexInit();
	binaryWord tmp;
	while(bufPos<inputBuf.size())
	{
		tmp = getSym();
		lexTable.push_back(tmp);//�ѽ���������ʷ����������
		if(nowChar == '$')
			break;
	}
}


#endif //_LEXANALYSIS_