#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

#define stackSize 500//ջ��С
#define MAXSYMLEN 10  //��ʶ����󳤶�

enum errortype{
	Error,lexError,symKindError,fileOpenError,syntaxError,stackFull,
	wrongOpCode,wrongFuncCode,undefinedVariable,wrongFactor,undefinedProcedure,
	notProcedure,notVariable,wrongRelaOperator,duplicateDefinitionIdentifier
};
void error(errortype type = Error);

const int symConst = 1;	//����
const int symVar = 2;	//����
const int symProc = 3;	//����
const string KIND[4]={"","CONSTANT","VARIABLE","PROCEDURE"};
//Ŀ��ָ��
const int LIT = 0;
const int LOD = 1;
const int STO = 2;
const int CAL = 3;
const int INT = 4;
const int JMP = 5;
const int JPC = 6;
const int OPR = 7;
//Ŀ��ָ���
const vector<string> ins={"LIT","LOD","STO","CAL","INT","JMP","JPC","OPR"};

// OPRָ����a��ȡֵ������
// 0�����̷��أ�������������򣬾��ǳ������
// 1��ȡ��
// 2���ӷ�
// 3������
// 4���˷�
// 5������
// 6����ż�ж�
// 7��
// 8���е�
// 9���в���
// 10����С
// 11���в�С��
// 12���д�
// 13���в�����
// 14������Ļ���ջ������
// 15�����һ������
// 16�����̶�������

//��Ԫʽ
struct binaryWord
{
	int category;	//�������
	int attribute;	//���ʷ��ŵ�����ֵ����Ӧ���е��ʷ��ŵ���ڵ�ַ
	int line;		//��λ�õģ������Ƕ�Ԫʽ������
	int col;		//��λ�õģ������Ƕ�Ԫʽ������
	binaryWord() {}
	binaryWord(int cg):category(cg) {}
	binaryWord(int cg,int ln,int cl):category(cg),line(ln),col(cl) {}
	binaryWord(int cg,int ab,int ln,int cl):category(cg),attribute(ab),line(ln),col(cl) {}
};

//�﷨���ڵ�
struct syntaxTreeNode
{
	string wordStr;	//�ڵ�����
	vector<syntaxTreeNode*> subTreeNode; //�ӽڵ�
	syntaxTreeNode(string wstr):wordStr(wstr) {}
};

//���ű���
struct symbItem
{
	string name;//���֣���ʶ����
	int kind;//���ͣ���������������������֮һ
	int value;//������ֵ
	int level;//��
	int address;//���е���Ե�ַ
	int size;
	symbItem() {}
	symbItem(string na,int kd,int lev,int va = 0):name(na),kind(kd),level(lev)
	{
		if(kd == symConst)
			value = va;
		else if(kd == symVar)
			address = va;
		else if(kd != symProc)
			error(symKindError);
	}
};

//ָ��
struct instruction
{
	int f;//������
	int l;//���
	int a;//λ����
	instruction() {}
	instruction(int ff,int ll,int aa):f(ff),l(ll),a(aa) {}
};

//��Ҫ�����Ĵ����ļ���
//const string fileName = "PL0_code/PL0_code2.in";
const string fileName = "PL0_code/demo.txt";
//const string fileName = "PL0_code/demoModified.txt";
//const string fileName = "PL0_code/test.txt";
//�ؼ��֡�������������
const vector<string> keyword = {"const","var","procedure","begin","end",
					"odd","if","then","call","while","do","read","write"};
const vector<string> operators = {"+","-","*","/","=","#","<","<=",">",">=",":=","<>"};
const vector<string> boundSymb = {".",",",";","(",")"};
string inputBuf;	//���뻺����
//�ؼ���һ��һ��
//�����һ��һ��
//���һ��һ��
//��ʶ��һ��
//����һ��

//�ؼ���
const int KWBASE=0;	//�ؼ��ֵĻ���
const int CNT=1;	//const
const int VAR=2;	//var
const int PROC=3;	//procdure
const int BEG=4;	//begin
const int END=5;	//end
const int ODD=6;	//odd
const int IF=7;		//if
const int THEN=8;	//then
const int CL=9;	//call
const int WHI=10;	//while
const int DO=11;	//do
const int REA=12;	//read
const int WRI=13;	//write
//�����
const int OPBASE=20;//������Ļ���
const int ADD=21;	//"+"
const int SUB=22;	//"-"
const int MUL=23;	//"*"
const int DIV=24;	//"/"
const int EQU=25;	//"="
const int NEQU1=26;	//"#"
const int LES=27;	//"<"
const int LESE=28;	//"<="
const int LAR=29;	//">"
const int LARE=30;	//">="
const int ASSI=31;	//":="
const int NEQU2=32;	//"<>"
//���
const int BSBASE=40;//����Ļ���
const int POI=41;	//"."
const int COM=42;	//","
const int SEM=43;	//";"
const int LBR=44;	//"("
const int RBR=45;	//")"
//��ʶ��
const int SYM=50;
//����
const int CONS=60;
//�ļ�������
const int myEOF=-1;

//�����ս����FIRST��
vector<int> prog_FIRST = {CNT,VAR,PROC,SYM,IF,WHI,CL,REA,WRI,BEG};
vector<int> subProg_FIRST = {CNT,VAR,PROC,SYM,IF,WHI,CL,REA,WRI,BEG};
vector<int> constDscpSection_FIRST = {CNT};
vector<int> constDef_FIRST = {SYM};
vector<int> unsignedInt_FIRST = {CONS};
vector<int> varDscpSection_FIRST = {VAR};
vector<int> identifier_FIRST = {SYM};
vector<int> procDscpSection_FIRST = {PROC};
vector<int> procHeader_FIRST = {PROC};
vector<int> statement_FIRST = {SYM,IF,WHI,CL,REA,WRI,BEG};
vector<int> assiStatement_FIRST = {SYM};
vector<int> compStatement_FIRST = {BEG};
vector<int> conditions_FIRST = {ADD,SUB,SYM,CONS,LBR,ODD};
vector<int> expressions_FIRST = {ADD,SUB,SYM,CONS,LBR};
vector<int> items_FIRST = {SYM,CONS,LBR};
vector<int> factors_FIRST = {SYM,CONS,LBR};
vector<int> addSubOp_FIRST = {ADD,SUB};
vector<int> mulDivOp_FIRST = {MUL,DIV};
vector<int> relaOp_FIRST = {EQU,NEQU1,NEQU2,LES,LESE,LAR,LARE};
vector<int> condStatement_FIRST = {IF};
vector<int> procCallStatement_FIRST = {CL};
vector<int> loopStatement_FIRST = {WHI};
vector<int> readStatement_FIRST = {REA};
vector<int> writeStatement_FIRST = {WRI};
//�ֳ�������FIRST��FOLLOW���ཻ
vector<int> subProg_FOLLOW = {POI,SEM};
vector<int> statement_FOLLOW = {POI,SEM,END};

//�Ƿ��ڶ�Ӧ��FIRST����
bool inFIRST(vector<int> FIRST,int id)
{
	return find(FIRST.begin(),FIRST.end(),id) != FIRST.end();
}

//�Ƿ��ڶ�Ӧ��FOLLOW����
bool inFOLLOW(vector<int> FOLLOW,int id)
{
	return find(FOLLOW.begin(),FOLLOW.end(),id) != FOLLOW.end();
}

//lexAnalysis
int bufPos;			//���ڴ����λ��
char nowChar;		//���ڴ�����ַ�
string strToken;	//���ڴ�����ַ���
int line;			//���ڴ��������
int col;			//���ڴ��������
vector<string> symTable;	//��ʶ����
vector<string> constTable;	//������
vector<binaryWord> lexTable;//�ʷ����������

void lexInit();
void getChar();
void getBC();
void concat();
void retract();
bool letterCase(char c);
bool numberCase(char c);
int insertID();
int insertConst();
int reserve();
void showLexResult(int opt = 3);
binaryWord getSym();
void lexAnalysis();

//syntaxAnalysis
int lexTablePos;//���ڷ����ĵ����ڴʷ�����������е�λ��
int nowID;//���ڷ����ĵ��ʵ����
syntaxTreeNode *rootNode;//�﷨�����ڵ�
vector<vector<string>> syntaxTree;

void getSyntaxTree(int level,syntaxTreeNode *nowNode);
void showSyntaxTree(int level,syntaxTreeNode *nowNode);
void showSyntaxTree();
void advance();
void syntaxInit();
bool match(int id);
void syntaxAnalysis();
void prog(syntaxTreeNode *nowNode,int lev = 0);
void subProg(syntaxTreeNode *nowNode,int lev = 0);
void constDscpSection(syntaxTreeNode *nowNode,int lev = 0);
void constDef(syntaxTreeNode *nowNode,int lev = 0);
void unsignedInt(syntaxTreeNode *nowNode,int lev = 0);
void varDscpSection(syntaxTreeNode *nowNode,int lev = 0);
void identifier(syntaxTreeNode *nowNode,int lev = 0);
void procDscpSection(syntaxTreeNode *nowNode,int lev = 0);
void procHeader(syntaxTreeNode *nowNode,int lev = 0);
void statement(syntaxTreeNode *nowNode,int lev = 0);
void assiStatement(syntaxTreeNode *nowNode,int lev = 0);
void compStatement(syntaxTreeNode *nowNode,int lev = 0);
void conditions(syntaxTreeNode *nowNode,int lev = 0);
void expressions(syntaxTreeNode *nowNode,int lev = 0);
void items(syntaxTreeNode *nowNode,int lev = 0);
void factors(syntaxTreeNode *nowNode,int lev = 0);
//void addSubOp(syntaxTreeNode *nowNode,int lev = 0);
//void mulDivOp(syntaxTreeNode *nowNode,int lev = 0);
void relaOp(syntaxTreeNode *nowNode,int lev = 0);
void condStatement(syntaxTreeNode *nowNode,int lev = 0);
void procCallStatement(syntaxTreeNode *nowNode,int lev = 0);
void loopStatement(syntaxTreeNode *nowNode,int lev = 0);
void readStatement(syntaxTreeNode *nowNode,int lev = 0);
void writeStatement(syntaxTreeNode *nowNode,int lev = 0);
void letters(syntaxTreeNode *nowNode,int lev = 0);
void numbers(syntaxTreeNode *nowNode,int lev = 0);

//codeGenAndExec
int dx = 0;//��ַָ��
int tx = 0;//���ű�ָ��
int cx = 0;//�����ָ��
int P = 0;//�����ַ�Ĵ�����code���±�
int T = 0;//ջ���Ĵ�����S���±�
int B = 0;//����ַ�Ĵ������������Ļ���ַ
instruction I;//ָ��Ĵ�������ǰ���ڽ��͵�һ��Ŀ��ָ��
int S[stackSize];//����ʱ������
vector<symbItem> Table;//���ű�
vector<instruction> code;//Ŀ������

void enter(vector<symbItem> stable,string name,int kind,int lev,int value = 0);
int findSameLevel(string name,int lev);
int findOtherLevel(string name,int lev);
int position(string name,int lev);
void gen(int f,int l,int a);
void showCode();
void showTable();
int base(int l);
void showS();
void interpret();

//��ȡ�����ļ������뻺����inputBuf��
void readSourceCode()
{
	string tempStr;
	ifstream filein(fileName);
	if(!filein)
		error(fileOpenError);
	inputBuf = "";
	while(getline(filein,tempStr))
		inputBuf += tempStr+"\n";
}

//���inputBuf
void showSourceCode()
{
	cout<<"Դ���룺"<<endl;
	cout<<inputBuf<<endl;
	cout<<"Դ���벿�ֽ�����"<<endl;
}

#endif //_COMMON_H_