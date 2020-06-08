#ifndef _SYNTAXANALYSIS_
#define _SYNTAXANALYSIS_

#include "common.h"

// 一个数组，数组长度不低于树的高度，为了简便起见，这里假设是100
int vec_left[100] = {0};

// 显示树的函数，只要调用showSyntaxTree(rootNode, 0)即可
void showSyntaxTree(syntaxTreeNode* root, int level)
{
	if(level > 0)
	{
		for(int i = 0; i < level - 1; ++i)
		{
			printf(vec_left[i] ? "│   " : "    ");
		}
		printf(vec_left[level-1] ? "├── " : "└── ");
	}

	cout<<root->wordStr<<endl;
	for(int i=0;i<root->subTreeNode.size();i++)
	{
		if(i==0)
			vec_left[level] = 1;
		if(i==root->subTreeNode.size()-1)
			vec_left[level] = 0;
		showSyntaxTree(root->subTreeNode[i], level + 1);
	}
}

//下一个单词
void advance()
{
	binaryWord tmp = getSym();
	lexTable.push_back(tmp);//把二元式放入词法分析结果表
	lexTablePos++;//错误处理使用
	//nowID = lexTable[lexTablePos++].category;
	nowID = tmp.category;
}

//语法分析初始化
void syntaxInit()
{
	lexTablePos = 0;
	advance();
}

//匹配
bool match(int id)
{
	if(nowID == id)
	{
		//advance();
		return true;
	}
	return false;
}

//程序
void prog(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(prog_FIRST,nowID))
	{
		nowNode->subTreeNode.push_back(new syntaxTreeNode("分程序"));
		subProg(nowNode->subTreeNode.back(),lev);
		if(match(POI))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("."));
			advance();
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//分程序,这个就是block函数
void subProg(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(subProg_FIRST,nowID) || inFOLLOW(subProg_FOLLOW,nowID))
	{
		int curtx = tx;//初始符号表指针指向当前层的符号在符号表中的开始位置
		dx = 3;/*地址指示器给出每层局部量当前已分配到的相对位置。
              置初始值为3的原因是：每一层最开始的位置有三个空间
              用于存放静态链SL、动态链DL和返回地址RA*/
		int curcx = cx;//记录当前指令位置
		gen(JMP,0,0);//产生一行跳转指令，跳转位置暂时未知填0
		if(inFIRST(constDscpSection_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("常量说明部分"));
			constDscpSection(nowNode->subTreeNode.back(),lev);
		}
		if(inFIRST(varDscpSection_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("变量说明部分"));
			varDscpSection(nowNode->subTreeNode.back(),lev);
		}
		int curdx = dx;//记录当前的dx
		if(inFIRST(procDscpSection_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("过程说明部分"));
			procDscpSection(nowNode->subTreeNode.back(),lev);
		}
		code[curcx].a = cx;//把前面生成的跳转语句的跳转位置改成当前位置
		if(curtx>0)
		{//记录过程的相关信息
			Table[curtx-1].address = cx;//在符号表中记录地址为当前代码分配地址
			Table[curtx-1].size = dx;//在符号表中记录长度为当前数据分配位置
		}
		gen(INT,0,curdx);//生成分配空间指令，分配dx个空间
		nowNode->subTreeNode.push_back(new syntaxTreeNode("语句"));
		statement(nowNode->subTreeNode.back(),lev);
		gen(OPR,0,0);//生成从子程序返回操作指令
	}
	else
		error(syntaxError);
}

//常量说明部分
void constDscpSection(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(constDscpSection_FIRST,nowID))
	{
		if(match(CNT))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("const"));
			advance();
		}
		else
			error(syntaxError);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("常量定义"));
		constDef(nowNode->subTreeNode.back(),lev);
		while(match(COM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(","));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("常量定义"));
			constDef(nowNode->subTreeNode.back(),lev);
		}
		if(match(SEM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(";"));
			advance();
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//常量定义
void constDef(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(constDef_FIRST,nowID))
	{
		string numName = symTable[lexTable.back().attribute];//记录标识符名
		nowNode->subTreeNode.push_back(new syntaxTreeNode("标识符"));
		identifier(nowNode->subTreeNode.back());
		if(match(EQU))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("="));
			advance();
		}
		else
			error(syntaxError);
		int constNum = stoi(constTable[lexTable.back().attribute]);//记录常量值
		nowNode->subTreeNode.push_back(new syntaxTreeNode("无符号整数"));
		unsignedInt(nowNode->subTreeNode.back());
		enter(Table,numName,symConst,lev,constNum);//常量入表
	}
	else
		error(syntaxError);
}

//无符号整数
void unsignedInt(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(unsignedInt_FIRST,nowID))
	{
		if(match(CONS))
		{
			int entry = lexTable[lexTablePos-1].attribute;//常量表入口
			nowNode->subTreeNode.push_back(new syntaxTreeNode(constTable[entry]));
			advance();
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//变量说明部分
void varDscpSection(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(varDscpSection_FIRST,nowID))
	{
		if(match(VAR))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("var"));
			advance();
		}
		else
			error(syntaxError);
		string symName = symTable[lexTable.back().attribute];//记录标识符名
		nowNode->subTreeNode.push_back(new syntaxTreeNode("标识符"));
		identifier(nowNode->subTreeNode.back());
		enter(Table,symName,symVar,lev);//变量入表
		while(match(COM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(","));
			advance();
			string symName = symTable[lexTable.back().attribute];//记录标识符名
			nowNode->subTreeNode.push_back(new syntaxTreeNode("标识符"));
			identifier(nowNode->subTreeNode.back());
			enter(Table,symName,symVar,lev);//变量入表
		}
		if(match(SEM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(";"));
			advance();
		}
		else
			error(syntaxError);
	}
	
}

//标识符
void identifier(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(identifier_FIRST,nowID))
	{
		if(match(SYM))
		{
			int entry = lexTable[lexTablePos-1].attribute;//标识符表入口
			nowNode->subTreeNode.push_back(new syntaxTreeNode(symTable[entry]));
			advance();
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//过程说明部分
void procDscpSection(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(procDscpSection_FIRST,nowID))
	{
		nowNode->subTreeNode.push_back(new syntaxTreeNode("过程首部"));
		procHeader(nowNode->subTreeNode.back(),lev);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("分程序"));
		subProg(nowNode->subTreeNode.back(),lev+1);
		if(match(SEM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(";"));
			advance();
		}
		else
			error(syntaxError);
		if(inFIRST(procDscpSection_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("过程说明部分"));
			procDscpSection(nowNode->subTreeNode.back(),lev);
		}
	}
	else
		error(syntaxError);
}

//过程首部
void procHeader(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(procHeader_FIRST,nowID))
	{
		if(match(PROC))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("procedure"));
			advance();
		}
		else
			error(syntaxError);
		string procName = symTable[lexTable.back().attribute];//记录标识符名
		nowNode->subTreeNode.push_back(new syntaxTreeNode("标识符"));
		identifier(nowNode->subTreeNode.back());
		enter(Table,procName,symProc,lev);//过程入表
		if(match(SEM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(";"));
			advance();
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//语句
void statement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(statement_FIRST,nowID) || inFOLLOW(statement_FOLLOW,nowID))
	{
		if(inFIRST(assiStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("赋值语句"));
			assiStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(condStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("条件语句"));
			condStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(loopStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("当型循环语句"));
			loopStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(procCallStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("过程调用语句"));
			procCallStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(readStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("读语句"));
			readStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(writeStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("写语句"));
			writeStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(compStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("复合语句"));
			compStatement(nowNode->subTreeNode.back(),lev);
		}
		else//null
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("空"));
			return;
		}
	}
	else
		error(syntaxError);
}

//赋值语句
void assiStatement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(assiStatement_FIRST,nowID))
	{
		string name = symTable[lexTable.back().attribute];
		int pos = position(name,lev);
		if(pos == -1)
			error(undefinedVariable);//未定义的变量
		if(Table[pos].kind != symVar)
			error(notVariable);//该标识符不是变量
		nowNode->subTreeNode.push_back(new syntaxTreeNode("标识符"));
		identifier(nowNode->subTreeNode.back());
		if(match(ASSI))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(":="));
			advance();
		}
		else
			error(syntaxError);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("表达式"));
		expressions(nowNode->subTreeNode.back(),lev);
		gen(STO,lev - Table[pos].level,Table[pos].address);//生成STO指令
	}
	else
		error(syntaxError);
}

//复合语句
void compStatement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(compStatement_FIRST,nowID))
	{
		if(match(BEG))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("begin"));
			advance();
		}
		else
			error(syntaxError);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("语句"));
		statement(nowNode->subTreeNode.back(),lev);
		while(match(SEM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(";"));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("语句"));
			statement(nowNode->subTreeNode.back(),lev);
		}
		if(match(END))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("end"));
			advance();
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//条件
void conditions(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(conditions_FIRST,nowID))
	{
		if(inFIRST(expressions_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("表达式"));
			expressions(nowNode->subTreeNode.back(),lev);
			int op = nowID;
			nowNode->subTreeNode.push_back(new syntaxTreeNode("关系运算符"));
			relaOp(nowNode->subTreeNode.back());
			nowNode->subTreeNode.push_back(new syntaxTreeNode("表达式"));
			expressions(nowNode->subTreeNode.back(),lev);
			switch(op)
			{
				case EQU:
					gen(OPR,0,8);//=：产生8号判等指令
					break;
				case NEQU1:	
				case NEQU2:
					gen(OPR,0,9);//<>或#：产生9号判不等指令
					break;
				case LES:
					gen(OPR,0,10);//<：产生10号判小指令
					break;
				case LARE:
					gen(OPR, 0, 11);//>=：产生11号判不小于指令
					break;
				case LAR:
					gen(OPR, 0, 12);//>：产生12号判大于指令
					break;
				case LESE:
					gen(OPR,0,13);//<=：产生13号判不大于指令
					break;
				default:
					error(wrongRelaOperator);//错误的关系运算符
			}
		}
		else if(match(ODD))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("odd"));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("表达式"));
			expressions(nowNode->subTreeNode.back(),lev);
			gen(OPR,0,6);//生成6号操作指令，奇偶判断运算
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//表达式
void expressions(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(expressions_FIRST,nowID))
	{
		bool flag = false;//表达式是否由负号开头
		if(match(ADD))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("+"));
			advance();
		}
		else if(match(SUB))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("-"));
			advance();
			flag = true;
		}
		nowNode->subTreeNode.push_back(new syntaxTreeNode("项"));
		items(nowNode->subTreeNode.back(),lev);
		if(flag)//表达式是负号开头的
			gen(OPR,0,1);//生成一条1号操作指令，取反运算
		while(match(ADD) || match(SUB))
		{
			int op = nowID;//保存运算符
			if(match(ADD))
				nowNode->subTreeNode.push_back(new syntaxTreeNode("+"));
			else if(match(SUB))
				nowNode->subTreeNode.push_back(new syntaxTreeNode("-"));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("项"));
			items(nowNode->subTreeNode.back(),lev);
			if(op == ADD)//如果项与项之间的运算符是加号
				gen(OPR,0,2);//生成2号操作指令，加法
			else//否则是减法
				gen(OPR,0,3);//生成3号操作指令，减法
		}
	}
	else
		error(syntaxError);
}

//项
void items(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(items_FIRST,nowID))
	{
		nowNode->subTreeNode.push_back(new syntaxTreeNode("因子"));
		factors(nowNode->subTreeNode.back(),lev);
		while(match(MUL) || match(DIV))
		{
			int op = nowID;//保存运算符
			if(match(MUL))
				nowNode->subTreeNode.push_back(new syntaxTreeNode("*"));
			else if(match(DIV))
				nowNode->subTreeNode.push_back(new syntaxTreeNode("/"));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("因子"));
			factors(nowNode->subTreeNode.back(),lev);
			if(op == MUL)//乘号
				gen(OPR,0,4);//生成4号指令，乘法
			else//除号
				gen(OPR,0,5);//生成5号指令，除法
		}
	}
	else
		error(syntaxError);
}

//因子
void factors(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(factors_FIRST,nowID))
	{
		if(inFIRST(identifier_FIRST,nowID))
		{
			string name = symTable[lexTable.back().attribute];
			int pos = position(name,lev);
			if(pos == -1)
				error(undefinedVariable);//未定义的变量
			if(Table[pos].kind == symConst)//是常量
				gen(LIT,0,Table[pos].value);//生成LIT指令
			else if(Table[pos].kind == symVar)//是变量
				gen(LOD,lev - Table[pos].level,Table[pos].address);//生成LOD指令
			else//是过程
				error(wrongFactor);//过程名作为因子
			nowNode->subTreeNode.push_back(new syntaxTreeNode("标识符"));
			identifier(nowNode->subTreeNode.back());
		}
		else if(inFIRST(unsignedInt_FIRST,nowID))
		{
			int entry = lexTable.back().attribute;//常量表入口
			int num = stoi(constTable[entry]);//常数
			gen(LIT,0,num);//生成LIT指令
			nowNode->subTreeNode.push_back(new syntaxTreeNode("无符号整数"));
			unsignedInt(nowNode->subTreeNode.back());
		}
		else if(match(LBR))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("("));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("表达式"));
			expressions(nowNode->subTreeNode.back(),lev);
			if(match(RBR))
			{
				nowNode->subTreeNode.push_back(new syntaxTreeNode(")"));
				advance();
			}
			else
				error(syntaxError);
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//关系运算符
void relaOp(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(relaOp_FIRST,nowID))
	{
		// if(match(EQU) || match(NEQU1) || match(NEQU2) || match(LES)
		// 	|| match(LESE) || match(LAR) || match(LARE))
		// 	advance();
		if(match(EQU))
			nowNode->subTreeNode.push_back(new syntaxTreeNode("="));
		else if(match(NEQU1))
			nowNode->subTreeNode.push_back(new syntaxTreeNode("#"));
		else if(match(NEQU2))
			nowNode->subTreeNode.push_back(new syntaxTreeNode("<>"));
		else if(match(LES))
			nowNode->subTreeNode.push_back(new syntaxTreeNode("<"));
		else if(match(LESE))
			nowNode->subTreeNode.push_back(new syntaxTreeNode("<="));
		else if(match(LAR))
			nowNode->subTreeNode.push_back(new syntaxTreeNode(">"));
		else if(match(LARE))
			nowNode->subTreeNode.push_back(new syntaxTreeNode(">="));
		else
			error(syntaxError);
		advance();
	}
	else
		error(syntaxError);
}

//条件语句
void condStatement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(condStatement_FIRST,nowID))
	{
		if(match(IF))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("if"));
			advance();
		}
		else
			error(syntaxError);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("条件"));
		conditions(nowNode->subTreeNode.back(),lev);
		if(match(THEN))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("then"));
			advance();
		}
		else
			error(syntaxError);
		int curcx = cx;//记下当前代码分配指针位置
		gen(JPC,0,0);//生成条件跳转指令，跳转位置暂时填0，分析完语句后再填写
		nowNode->subTreeNode.push_back(new syntaxTreeNode("语句"));
		statement(nowNode->subTreeNode.back(),lev);
		code[curcx].a = cx;//修改跳跃位置
	}
	else
		error(syntaxError);
}

//过程调用语句
void procCallStatement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(procCallStatement_FIRST,nowID))
	{
		if(match(CL))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("call"));
			advance();
		}
		else
			error(syntaxError);
		string name = symTable[lexTable.back().attribute];
		int pos = position(name,lev);
		if(pos == -1)
			error(undefinedProcedure);//未定义的标识符
		if(Table[pos].kind != symProc)
			error(notProcedure);//该标识符不是过程名
		nowNode->subTreeNode.push_back(new syntaxTreeNode("标识符"));
		identifier(nowNode->subTreeNode.back());
		gen(CAL,lev - Table[pos].level,Table[pos].address);//生成cal目标代码
	}
	else
		error(syntaxError);
}

//当型循环语句
void loopStatement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(loopStatement_FIRST,nowID))
	{
		if(match(WHI))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("while"));
			advance();
		}
		else
			error(syntaxError);
		int curcx1 = cx;//记下当前代码分配位置，这是while循环的开始位置
		nowNode->subTreeNode.push_back(new syntaxTreeNode("条件"));
		conditions(nowNode->subTreeNode.back(),lev);
		int curcx2 = cx;//记下当前代码分配位置，这是while的do中的语句的开始位置
		gen(JPC,0,0);//生成条件跳转指令，跳转位置暂时填0
		if(match(DO))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("do"));
			advance();
		}
		else
			error(syntaxError);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("语句"));
		statement(nowNode->subTreeNode.back(),lev);
		gen(JMP,0,curcx1);//循环跳转到curcx1位置，即再次进行逻辑判断
		code[curcx2].a = cx;
	}
	else
		error(syntaxError);
}

//读语句
void readStatement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(readStatement_FIRST,nowID))
	{
		if(match(REA))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("read"));
			advance();
		}
		else
			error(syntaxError);
		if(match(LBR))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("("));
			advance();
		}
		else
			error(syntaxError);
		string name = symTable[lexTable.back().attribute];
		int pos = position(name,lev);
		if(pos == -1)
			error(undefinedVariable);//未定义的变量
		if(Table[pos].kind != symVar)
			error(notVariable);//该标识符不是变量
		nowNode->subTreeNode.push_back(new syntaxTreeNode("标识符"));
		identifier(nowNode->subTreeNode.back());
		//生成16号操作指令:从键盘读入数字
		gen(OPR,0,16);
		//生成sto指令，把读入的值存入指定变量所在的空间
		gen(STO,lev - Table[pos].level,Table[pos].address);
		while(match(COM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(","));
			advance();
			name = symTable[lexTable.back().attribute];
			pos = position(name,lev);
			if(pos == -1)
				error(undefinedVariable);//未定义的变量
			if(Table[pos].kind != symVar)
				error(notVariable);//该标识符不是变量
			nowNode->subTreeNode.push_back(new syntaxTreeNode("标识符"));
			identifier(nowNode->subTreeNode.back());
			//生成16号操作指令:从键盘读入数字
			gen(OPR,0,16);
			//生成sto指令，把读入的值存入指定变量所在的空间
			gen(STO,lev - Table[pos].level,Table[pos].address);
		}
		if(match(RBR))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(")"));
			advance();
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//写语句
void writeStatement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(writeStatement_FIRST,nowID))
	{
		if(match(WRI))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("write"));
			advance();
		}
		else
			error(syntaxError);
		if(match(LBR))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("("));
			advance();
		}
		else
			error(syntaxError);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("表达式"));
		expressions(nowNode->subTreeNode.back(),lev);
		gen(OPR,0,14);//生成14号指令，向屏幕输出
		while(match(COM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(","));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("表达式"));
			expressions(nowNode->subTreeNode.back(),lev);
			gen(OPR,0,14);//生成14号指令，向屏幕输出
		}
		if(match(RBR))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(")"));
			advance();
		}
		else
			error(syntaxError);
		gen(OPR,0,15);//生成一个15号操作的目标代码，功能是输出一个换行
	}
	else
		error(syntaxError);
}

//递归下降子程序法进行语法分析,同时进行词法分析和语法分析
void syntaxAnalysis()
{
	cout<<"分析开始："<<endl;
	lexInit();
	syntaxInit();
	rootNode = new syntaxTreeNode("程序");
	prog(rootNode,0);
	if(nowChar == '$')
		cout<<"词法分析成功!"<<endl;
	else
		error(lexError);
	if(nowID == myEOF)
		cout<<"语法分析成功!"<<endl;
	else
		error(syntaxError);
	showLexResult(1);
	cout<<"语法树："<<endl;
	showSyntaxTree(rootNode,0);
	showTable();
	showCode();
	cout<<"分析结束。"<<endl;
}

#endif //_SYNTAXANALYSIS_
