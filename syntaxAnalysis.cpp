#ifndef _SYNTAXANALYSIS_
#define _SYNTAXANALYSIS_

#include "common.h"

//����﷨��,�ݹ����
void getSyntaxTree(int level,syntaxTreeNode *nowNode)
{
	if(level>syntaxTree.size())
	{
		vector<string> v;
		syntaxTree.push_back(v);
	}
	syntaxTree[level-1].push_back(nowNode->wordStr);
	for(int k=0;k<nowNode->subTreeNode.size();k++)
		getSyntaxTree(level+1,nowNode->subTreeNode[k]);
}

void showSyntaxTree()
{
	for(int i=0;i<syntaxTree.size();i++)
	{
		cout<<i+1<<"�㣺"<<endl;
		for(int j=0;j<syntaxTree[i].size();j++)
			cout<<syntaxTree[i][j]<<" ";
		cout<<endl;
	}
}

//��һ������
void advance()
{
	binaryWord tmp = getSym();
	lexTable.push_back(tmp);//�Ѷ�Ԫʽ����ʷ����������
	lexTablePos++;//������ʹ��
	//nowID = lexTable[lexTablePos++].category;
	nowID = tmp.category;
}

//�﷨������ʼ��
void syntaxInit()
{
	lexTablePos = 0;
	advance();
}

//ƥ��
bool match(int id)
{
	if(nowID == id)
	{
		//advance();
		return true;
	}
	return false;
}

//����
void prog(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(prog_FIRST,nowID))
	{
		nowNode->subTreeNode.push_back(new syntaxTreeNode("�ֳ���"));
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

//�ֳ���,�������block����
void subProg(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(subProg_FIRST,nowID) || inFOLLOW(subProg_FOLLOW,nowID))
	{
		int curtx = tx;//��ʼ���ű�ָ��ָ��ǰ��ķ����ڷ��ű��еĿ�ʼλ��
		dx = 3;/*��ַָʾ������ÿ��ֲ�����ǰ�ѷ��䵽�����λ�á�
              �ó�ʼֵΪ3��ԭ���ǣ�ÿһ���ʼ��λ���������ռ�
              ���ڴ�ž�̬��SL����̬��DL�ͷ��ص�ַRA*/
		int curcx = cx;//��¼��ǰָ��λ��
		gen(JMP,0,0);//����һ����תָ���תλ����ʱδ֪��0
		if(inFIRST(constDscpSection_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("����˵������"));
			constDscpSection(nowNode->subTreeNode.back(),lev);
		}
		if(inFIRST(varDscpSection_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("����˵������"));
			varDscpSection(nowNode->subTreeNode.back(),lev);
		}
		int curdx = dx;//��¼��ǰ��dx
		if(inFIRST(procDscpSection_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("����˵������"));
			procDscpSection(nowNode->subTreeNode.back(),lev);
		}
		code[curcx].a = cx;//��ǰ�����ɵ���ת������תλ�øĳɵ�ǰλ��
		if(curtx>0)
		{//��¼���̵������Ϣ
			Table[curtx-1].address = cx;//�ڷ��ű��м�¼��ַΪ��ǰ��������ַ
			Table[curtx-1].size = dx;//�ڷ��ű��м�¼����Ϊ��ǰ���ݷ���λ��
		}
		gen(INT,0,curdx);//���ɷ���ռ�ָ�����dx���ռ�
		nowNode->subTreeNode.push_back(new syntaxTreeNode("���"));
		statement(nowNode->subTreeNode.back(),lev);
		gen(OPR,0,0);//���ɴ��ӳ��򷵻ز���ָ��
	}
	else
		error(syntaxError);
}

//����˵������
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
		nowNode->subTreeNode.push_back(new syntaxTreeNode("��������"));
		constDef(nowNode->subTreeNode.back(),lev);
		while(match(COM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(","));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("��������"));
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

//��������
void constDef(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(constDef_FIRST,nowID))
	{
		string numName = symTable[lexTable.back().attribute];//��¼��ʶ����
		nowNode->subTreeNode.push_back(new syntaxTreeNode("��ʶ��"));
		identifier(nowNode->subTreeNode.back());
		if(match(EQU))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("="));
			advance();
		}
		else
			error(syntaxError);
		int constNum = stoi(constTable[lexTable.back().attribute]);//��¼����ֵ
		nowNode->subTreeNode.push_back(new syntaxTreeNode("�޷�������"));
		unsignedInt(nowNode->subTreeNode.back());
		enter(Table,numName,symConst,lev,constNum);//�������
	}
	else
		error(syntaxError);
}

//�޷�������
void unsignedInt(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(unsignedInt_FIRST,nowID))
	{
		if(match(CONS))
		{
			int entry = lexTable[lexTablePos-1].attribute;//���������
			nowNode->subTreeNode.push_back(new syntaxTreeNode(constTable[entry]));
			advance();
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//����˵������
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
		string symName = symTable[lexTable.back().attribute];//��¼��ʶ����
		nowNode->subTreeNode.push_back(new syntaxTreeNode("��ʶ��"));
		identifier(nowNode->subTreeNode.back());
		enter(Table,symName,symVar,lev);//�������
		while(match(COM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(","));
			advance();
			string symName = symTable[lexTable.back().attribute];//��¼��ʶ����
			nowNode->subTreeNode.push_back(new syntaxTreeNode("��ʶ��"));
			identifier(nowNode->subTreeNode.back());
			enter(Table,symName,symVar,lev);//�������
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

//��ʶ��
void identifier(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(identifier_FIRST,nowID))
	{
		if(match(SYM))
		{
			int entry = lexTable[lexTablePos-1].attribute;//��ʶ�������
			nowNode->subTreeNode.push_back(new syntaxTreeNode(symTable[entry]));
			advance();
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//����˵������
void procDscpSection(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(procDscpSection_FIRST,nowID))
	{
		nowNode->subTreeNode.push_back(new syntaxTreeNode("�����ײ�"));
		procHeader(nowNode->subTreeNode.back(),lev);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("�ֳ���"));
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
			nowNode->subTreeNode.push_back(new syntaxTreeNode("����˵������"));
			procDscpSection(nowNode->subTreeNode.back(),lev);
		}
	}
	else
		error(syntaxError);
}

//�����ײ�
void procHeader(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(procHeader_FIRST,nowID))
	{
		if(match(PROC))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("procdure"));
			advance();
		}
		else
			error(syntaxError);
		string procName = symTable[lexTable.back().attribute];//��¼��ʶ����
		nowNode->subTreeNode.push_back(new syntaxTreeNode("��ʶ��"));
		identifier(nowNode->subTreeNode.back());
		enter(Table,procName,symProc,lev);//�������
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

//���
void statement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(statement_FIRST,nowID) || inFOLLOW(statement_FOLLOW,nowID))
	{
		if(inFIRST(assiStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("��ֵ���"));
			assiStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(condStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("�������"));
			condStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(loopStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("����ѭ�����"));
			loopStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(procCallStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("���̵������"));
			procCallStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(readStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("�����"));
			readStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(writeStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("д���"));
			writeStatement(nowNode->subTreeNode.back(),lev);
		}
		else if(inFIRST(compStatement_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("�������"));
			compStatement(nowNode->subTreeNode.back(),lev);
		}
		else//null
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("��"));
			return;
		}
	}
	else
		error(syntaxError);
}

//��ֵ���
void assiStatement(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(assiStatement_FIRST,nowID))
	{
		string name = symTable[lexTable.back().attribute];
		int pos = position(name,lev);
		if(pos == -1)
			error(undefinedVariable);//δ����ı���
		if(Table[pos].kind != symVar)
			error(notVariable);//�ñ�ʶ�����Ǳ���
		nowNode->subTreeNode.push_back(new syntaxTreeNode("��ʶ��"));
		identifier(nowNode->subTreeNode.back());
		if(match(ASSI))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(":="));
			advance();
		}
		else
			error(syntaxError);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("���ʽ"));
		expressions(nowNode->subTreeNode.back(),lev);
		gen(STO,lev - Table[pos].level,Table[pos].address);//����STOָ��
	}
	else
		error(syntaxError);
}

//�������
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
		nowNode->subTreeNode.push_back(new syntaxTreeNode("���"));
		statement(nowNode->subTreeNode.back(),lev);
		while(match(SEM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(";"));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("���"));
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

//����
void conditions(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(conditions_FIRST,nowID))
	{
		if(inFIRST(expressions_FIRST,nowID))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("���ʽ"));
			expressions(nowNode->subTreeNode.back(),lev);
			int op = nowID;
			nowNode->subTreeNode.push_back(new syntaxTreeNode("��ϵ�����"));
			relaOp(nowNode->subTreeNode.back());
			nowNode->subTreeNode.push_back(new syntaxTreeNode("���ʽ"));
			expressions(nowNode->subTreeNode.back(),lev);
			switch(op)
			{
				case EQU:
					gen(OPR,0,8);//=������8���е�ָ��
					break;
				case NEQU1:	
				case NEQU2:
					gen(OPR,0,9);//<>��#������9���в���ָ��
					break;
				case LES:
					gen(OPR,0,10);//<������10����Сָ��
					break;
				case LARE:
					gen(OPR, 0, 11);//>=������11���в�С��ָ��
					break;
				case LAR:
					gen(OPR, 0, 12);//>������12���д���ָ��
					break;
				case LESE:
					gen(OPR,0,13);//<=������13���в�����ָ��
					break;
				default:
					error(wrongRelaOperator);//����Ĺ�ϵ�����
			}
		}
		else if(match(ODD))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("odd"));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("���ʽ"));
			expressions(nowNode->subTreeNode.back(),lev);
			gen(OPR,0,6);//����6�Ų���ָ���ż�ж�����
		}
		else
			error(syntaxError);
	}
	else
		error(syntaxError);
}

//���ʽ
void expressions(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(expressions_FIRST,nowID))
	{
		bool flag = false;//���ʽ�Ƿ��ɸ��ſ�ͷ
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
		nowNode->subTreeNode.push_back(new syntaxTreeNode("��"));
		items(nowNode->subTreeNode.back(),lev);
		if(flag)//���ʽ�Ǹ��ſ�ͷ��
			gen(OPR,0,1);//����һ��1�Ų���ָ�ȡ������
		while(match(ADD) || match(SUB))
		{
			int op = nowID;//���������
			if(match(ADD))
				nowNode->subTreeNode.push_back(new syntaxTreeNode("+"));
			else if(match(SUB))
				nowNode->subTreeNode.push_back(new syntaxTreeNode("-"));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("��"));
			items(nowNode->subTreeNode.back(),lev);
			if(op == ADD)//���������֮���������ǼӺ�
				gen(OPR,0,2);//����2�Ų���ָ��ӷ�
			else//�����Ǽ���
				gen(OPR,0,3);//����3�Ų���ָ�����
		}
	}
	else
		error(syntaxError);
}

//��
void items(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(items_FIRST,nowID))
	{
		nowNode->subTreeNode.push_back(new syntaxTreeNode("����"));
		factors(nowNode->subTreeNode.back(),lev);
		while(match(MUL) || match(DIV))
		{
			int op = nowID;//���������
			if(match(MUL))
				nowNode->subTreeNode.push_back(new syntaxTreeNode("*"));
			else if(match(DIV))
				nowNode->subTreeNode.push_back(new syntaxTreeNode("/"));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("����"));
			factors(nowNode->subTreeNode.back(),lev);
			if(op == MUL)//�˺�
				gen(OPR,0,4);//����4��ָ��˷�
			else//����
				gen(OPR,0,5);//����5��ָ�����
		}
	}
	else
		error(syntaxError);
}

//����
void factors(syntaxTreeNode *nowNode,int lev)
{
	if(inFIRST(factors_FIRST,nowID))
	{
		if(inFIRST(identifier_FIRST,nowID))
		{
			string name = symTable[lexTable.back().attribute];
			int pos = position(name,lev);
			if(pos == -1)
				error(undefinedVariable);//δ����ı���
			if(Table[pos].kind == symConst)//�ǳ���
				gen(LIT,0,Table[pos].value);//����LITָ��
			else if(Table[pos].kind == symVar)//�Ǳ���
				gen(LOD,lev - Table[pos].level,Table[pos].address);//����LODָ��
			else//�ǹ���
				error(wrongFactor);//��������Ϊ����
			nowNode->subTreeNode.push_back(new syntaxTreeNode("��ʶ��"));
			identifier(nowNode->subTreeNode.back());
		}
		else if(inFIRST(unsignedInt_FIRST,nowID))
		{
			int entry = lexTable.back().attribute;//���������
			int num = stoi(constTable[entry]);//����
			gen(LIT,0,num);//����LITָ��
			nowNode->subTreeNode.push_back(new syntaxTreeNode("�޷�������"));
			unsignedInt(nowNode->subTreeNode.back());
		}
		else if(match(LBR))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("("));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("���ʽ"));
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
}

//��ϵ�����
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

//�������
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
		nowNode->subTreeNode.push_back(new syntaxTreeNode("����"));
		conditions(nowNode->subTreeNode.back(),lev);
		if(match(THEN))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("then"));
			advance();
		}
		else
			error(syntaxError);
		int curcx = cx;//���µ�ǰ�������ָ��λ��
		gen(JPC,0,0);//����������תָ���תλ����ʱ��0����������������д
		nowNode->subTreeNode.push_back(new syntaxTreeNode("���"));
		statement(nowNode->subTreeNode.back(),lev);
		code[curcx].a = cx;//�޸���Ծλ��
	}
	else
		error(syntaxError);
}

//���̵������
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
			error(undefinedProcedure);//δ����ı�ʶ��
		if(Table[pos].kind != symProc)
			error(notProcedure);//�ñ�ʶ�����ǹ�����
		nowNode->subTreeNode.push_back(new syntaxTreeNode("��ʶ��"));
		identifier(nowNode->subTreeNode.back());
		gen(CAL,lev - Table[pos].level,Table[pos].address);//����calĿ�����
	}
	else
		error(syntaxError);
}

//����ѭ�����
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
		int curcx1 = cx;//���µ�ǰ�������λ�ã�����whileѭ���Ŀ�ʼλ��
		nowNode->subTreeNode.push_back(new syntaxTreeNode("����"));
		conditions(nowNode->subTreeNode.back(),lev);
		int curcx2 = cx;//���µ�ǰ�������λ�ã�����while��do�е����Ŀ�ʼλ��
		gen(JPC,0,0);//����������תָ���תλ����ʱ��0
		if(match(DO))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode("do"));
			advance();
		}
		else
			error(syntaxError);
		nowNode->subTreeNode.push_back(new syntaxTreeNode("���"));
		statement(nowNode->subTreeNode.back(),lev);
		gen(JMP,0,curcx1);//ѭ����ת��curcx1λ�ã����ٴν����߼��ж�
		code[curcx2].a = cx;
	}
	else
		error(syntaxError);
}

//�����
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
			error(undefinedVariable);//δ����ı���
		if(Table[pos].kind != symVar)
			error(notVariable);//�ñ�ʶ�����Ǳ���
		nowNode->subTreeNode.push_back(new syntaxTreeNode("��ʶ��"));
		identifier(nowNode->subTreeNode.back());
		//����16�Ų���ָ��:�Ӽ��̶�������
		gen(OPR,0,16);
		//����stoָ��Ѷ����ֵ����ָ���������ڵĿռ�
		gen(STO,lev - Table[pos].level,Table[pos].address);
		while(match(COM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(","));
			advance();
			name = symTable[lexTable.back().attribute];
			pos = position(name,lev);
			if(pos == -1)
				error(undefinedVariable);//δ����ı���
			if(Table[pos].kind != symVar)
				error(notVariable);//�ñ�ʶ�����Ǳ���
			nowNode->subTreeNode.push_back(new syntaxTreeNode("��ʶ��"));
			identifier(nowNode->subTreeNode.back());
			//����16�Ų���ָ��:�Ӽ��̶�������
			gen(OPR,0,16);
			//����stoָ��Ѷ����ֵ����ָ���������ڵĿռ�
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

//д���
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
		nowNode->subTreeNode.push_back(new syntaxTreeNode("���ʽ"));
		expressions(nowNode->subTreeNode.back(),lev);
		gen(OPR,0,14);//����14��ָ�����Ļ���
		while(match(COM))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(","));
			advance();
			nowNode->subTreeNode.push_back(new syntaxTreeNode("���ʽ"));
			expressions(nowNode->subTreeNode.back(),lev);
			gen(OPR,0,14);//����14��ָ�����Ļ���
		}
		if(match(RBR))
		{
			nowNode->subTreeNode.push_back(new syntaxTreeNode(")"));
			advance();
		}
		else
			error(syntaxError);
		gen(OPR,0,15);//����һ��15�Ų�����Ŀ����룬���������һ������
	}
	else
		error(syntaxError);
}

//�ݹ��½��ӳ��򷨽����﷨����,ͬʱ���дʷ��������﷨����
void syntaxAnalysis()
{
	cout<<"������ʼ��"<<endl;
	lexInit();
	syntaxInit();
	rootNode = new syntaxTreeNode("����");
	prog(rootNode,0);
	if(nowChar == '$')
		cout<<"�ʷ������ɹ�!"<<endl;
	else
		error(lexError);
	if(nowID == myEOF)
		cout<<"�﷨�����ɹ�!"<<endl;
	else
		error(syntaxError);
	showLexResult(1);
	getSyntaxTree(1,rootNode);
	showSyntaxTree();
	showTable();
	showCode();
	cout<<"����������"<<endl;
}

#endif //_SYNTAXANALYSIS_
