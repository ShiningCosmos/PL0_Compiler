#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

#define stackSize 500//栈大小
#define MAXSYMLEN 10  //标识符最大长度

enum errortype{
	Error,lexError,symKindError,fileOpenError,syntaxError,stackFull,
	wrongOpCode,wrongFuncCode,undefinedVariable,wrongFactor,undefinedProcedure,
	notProcedure,notVariable,wrongRelaOperator,duplicateDefinitionIdentifier
};
void error(errortype type = Error);

const int symConst = 1;	//常量
const int symVar = 2;	//变量
const int symProc = 3;	//过程
const string KIND[4]={"","CONSTANT","VARIABLE","PROCEDURE"};
//目标指令
const int LIT = 0;
const int LOD = 1;
const int STO = 2;
const int CAL = 3;
const int INT = 4;
const int JMP = 5;
const int JPC = 6;
const int OPR = 7;
//目标指令表
const vector<string> ins={"LIT","LOD","STO","CAL","INT","JMP","JPC","OPR"};

// OPR指令中a的取值及含义
// 0：过程返回，如果是在主程序，就是程序结束
// 1：取反
// 2：加法
// 3：减法
// 4：乘法
// 5：除法
// 6：奇偶判断
// 7：
// 8：判等
// 9：判不等
// 10：判小
// 11：判不小于
// 12：判大
// 13：判不大于
// 14：向屏幕输出栈顶内容
// 15：输出一个换行
// 16：键盘读入数字

//二元式
struct binaryWord
{
	int category;	//单词类别
	int attribute;	//单词符号的属性值，对应表中单词符号的入口地址
	int line;		//定位用的，本身不是二元式的属性
	int col;		//定位用的，本身不是二元式的属性
	binaryWord() {}
	binaryWord(int cg):category(cg) {}
	binaryWord(int cg,int ln,int cl):category(cg),line(ln),col(cl) {}
	binaryWord(int cg,int ab,int ln,int cl):category(cg),attribute(ab),line(ln),col(cl) {}
};

//语法树节点
struct syntaxTreeNode
{
	string wordStr;	//节点名称
	vector<syntaxTreeNode*> subTreeNode; //子节点
	syntaxTreeNode(string wstr):wordStr(wstr) {}
};

//符号表项
struct symbItem
{
	string name;//名字，标识符名
	int kind;//类型，常量、变量、过程三者之一
	int value;//常量的值
	int level;//层
	int address;//层中的相对地址
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

//指令
struct instruction
{
	int f;//功能码
	int l;//层差
	int a;//位移量
	instruction() {}
	instruction(int ff,int ll,int aa):f(ff),l(ll),a(aa) {}
};

//需要分析的代码文件名
//const string fileName = "PL0_code/PL0_code2.in";
const string fileName = "PL0_code/demo.txt";
//const string fileName = "PL0_code/demoModified.txt";
//const string fileName = "PL0_code/test.txt";
//关键字、运算符、界符表
const vector<string> keyword = {"const","var","procedure","begin","end",
					"odd","if","then","call","while","do","read","write"};
const vector<string> operators = {"+","-","*","/","=","#","<","<=",">",">=",":=","<>"};
const vector<string> boundSymb = {".",",",";","(",")"};
string inputBuf;	//代码缓冲区
//关键字一字一码
//运算符一符一码
//界符一符一码
//标识符一类
//常量一类

//关键字
const int KWBASE=0;	//关键字的基点
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
//运算符
const int OPBASE=20;//运算符的基点
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
//界符
const int BSBASE=40;//界符的基点
const int POI=41;	//"."
const int COM=42;	//","
const int SEM=43;	//";"
const int LBR=44;	//"("
const int RBR=45;	//")"
//标识符
const int SYM=50;
//常量
const int CONS=60;
//文件结束符
const int myEOF=-1;

//各非终结符的FIRST集
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
//分程序、语句的FIRST和FOLLOW不相交
vector<int> subProg_FOLLOW = {POI,SEM};
vector<int> statement_FOLLOW = {POI,SEM,END};

//是否在对应的FIRST集中
bool inFIRST(vector<int> FIRST,int id)
{
	return find(FIRST.begin(),FIRST.end(),id) != FIRST.end();
}

//是否在对应的FOLLOW集中
bool inFOLLOW(vector<int> FOLLOW,int id)
{
	return find(FOLLOW.begin(),FOLLOW.end(),id) != FOLLOW.end();
}

//lexAnalysis
int bufPos;			//正在处理的位置
char nowChar;		//正在处理的字符
string strToken;	//正在处理的字符串
int line;			//正在处理的行数
int col;			//正在处理的列数
vector<string> symTable;	//标识符表
vector<string> constTable;	//常量表
vector<binaryWord> lexTable;//词法分析结果表

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
int lexTablePos;//现在分析的单词在词法分析结果表中的位置
int nowID;//现在分析的单词的类别
syntaxTreeNode *rootNode;//语法树根节点
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
int dx = 0;//地址指针
int tx = 0;//符号表指针
int cx = 0;//代码表指针
int P = 0;//程序地址寄存器，code的下标
int T = 0;//栈顶寄存器，S的下标
int B = 0;//基地址寄存器，数据区的基地址
instruction I;//指令寄存器，当前正在解释的一条目标指令
int S[stackSize];//运行时数据区
vector<symbItem> Table;//符号表
vector<instruction> code;//目标代码表

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

//读取代码文件，放入缓冲区inputBuf中
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

//输出inputBuf
void showSourceCode()
{
	cout<<"源代码："<<endl;
	cout<<inputBuf<<endl;
	cout<<"源代码部分结束。"<<endl;
}

#endif //_COMMON_H_