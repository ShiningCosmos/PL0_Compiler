#ifndef _CODEGENANDEXEC_
#define _CODEGENANDEXEC_

#include "common.h"

//加入符号表
void enter(vector<symbItem> stable,string name,int kind,int lev,int value)
{
	if(position(name,lev)!=-1)//重复定义的标识符
		error(duplicateDefinitionIdentifier);
	if(kind == symConst)
	{
		Table.push_back(symbItem(name,kind,lev,value));
		tx++;//符号表指针指向下一项
	}
	else if(kind == symVar)
	{
		Table.push_back(symbItem(name,kind,lev,dx));
		dx++;//地址增加
		tx++;
	}
	else if(kind == symProc)
	{
		Table.push_back(symbItem(name,kind,lev));
		tx++;
	}
	else
		error(symKindError);
}

//在本层中查找标识符
int findSameLevel(string name,int lev)
{
	int i;
	for(i=tx-1;i>=0;i--)
	{
		if(Table[i].kind == symProc 
			&& Table[i].level == lev 
			&& Table[i].name == name)
			return i;//是proc且lev相同
		if(Table[i].kind != symProc && Table[i].level == lev)
			break;//不是proc但是lev相同，进一步查找
		if(Table[i].level < lev)//level小于lev，本层中没有var、cons和合适的proc
			return -1;
	}
	for(;i>=0;i--)
	{//在本层中查找var和cons
		if(Table[i].level < lev)
			return -1;//本层中没有该标识符
		if(Table[i].level == lev && Table[i].name == name)
			return i;//找到了该标识符，返回
	}
	return -1;//本层中没有该标识符
}

//在其他层中查找标识符
int findOtherLevel(string name,int lev)
{
	int currentLev = lev-1;
	for(int i=tx-1;i>=0;i--)
	{
		if(Table[i].level < currentLev)
			currentLev = Table[i].level;
		if(Table[i].level <= currentLev && Table[i].name == name)
			return i;
	}
	return -1;
}

//查找标识符在符号表中的位置
int position(string name,int lev)
{
	int i = findSameLevel(name,lev);//在本层中查找标识符
	if(i == -1)//本层中没有找到该标识符
		i = findOtherLevel(name,lev);//在其他层中查找标识符
	return i;
}

//生成目标代码，f功能码，l层差，a位移量
void gen(int f,int l,int a)
{
    code.push_back(instruction(f,l,a));
    cx++;
}

//展示目标代码
void showCode()
{
    cout<<"目标代码表："<<endl;
    for(int i=0;i<code.size();i++)
    {
        cout<<i<<" "<<ins[code[i].f]<<" "
            <<code[i].l<<" "<<code[i].a<<endl;
    }
}

//展示符号表
void showTable()
{
	cout<<"符号表："<<endl;
	for(int i=0;i<Table.size();i++)
	{
		if(Table[i].kind == symConst)
		{
			cout<<"NAME:"<<Table[i].name;
			cout<<"\tKIND:"<<KIND[Table[i].kind];
			cout<<"\tVAL:"<<Table[i].value;
			cout<<endl;
		}
		else if(Table[i].kind == symVar)
		{
			cout<<"NAME:"<<Table[i].name;
			cout<<"\tKIND:"<<KIND[Table[i].kind];
			cout<<"\tLEVEL:"<<Table[i].level;
			cout<<"\tADR:"<<Table[i].address;
			cout<<endl;
		}
		else if(Table[i].kind == symProc)
		{
			cout<<"NAME:"<<Table[i].name;
			cout<<"\tKIND:"<<KIND[Table[i].kind];
			cout<<"\tLEVEL:"<<Table[i].level;
			cout<<"\tADR:"<<Table[i].address;
			cout<<"\tSIZE:"<<Table[i].size;
			cout<<endl;
		}
		else
			error(symKindError);
	}
}

//找到层差为l的数据区
int base(int l)
{
	int b0 = B;//从当前层开始
	while(l > 0)
	{//找l次
		b0 = S[b0];//根据基址内容（SL）找到上一层的基址
		l--;
	}
	return b0;//返回层差为l的数据区
}

//展示数据区等
void showS()
{
	cout<<"P:"<<P<<"\tT:"<<T<<"\tB:"<<B<<endl;
	cout<<"pc:"<<P-1<<" "<<ins[I.f]<<" "<<I.l<<" "<<I.a<<endl;
	for(int i = 0;i<T;i++)
		cout<<i<<" "<<S[i]<<endl;
	cout<<"end"<<endl;
}

//解释程序
void interpret()
{
	cout<<"目标代码解释开始："<<endl;
	P = 0;//程序地址寄存器
	T = 0;//栈顶寄存器
	B = 0;//基地址寄存器
	I = code[P];//指令寄存器
	fill(S,S+stackSize,0);//初始化数据区
	S[0] = 0;//主程序SL为0
	S[1] = 0;//主程序DL为0
	S[2] = 0;//主程序RA为0
	do{
		I = code[P++];
		switch(I.f)
		{//根据功能码执行不同的功能
			case LIT://常数到栈顶
				S[T++] = I.a;
				break;
			case LOD://变量到栈顶
				S[T++] = S[I.a + base(I.l)];
				break;
			case STO://栈顶内容到变量
				//showS();
				T--;
				S[I.a + base(I.l)] = S[T];
				break;
			case CAL://调用过程
				//保存现场
				S[T] = base(I.l);//栈顶压入静态链SL
				S[T+1] = B;//栈顶压入当前数据区基址，作为动态链DL
				S[T+2] = P;//栈顶压入当前PC，作为返回地址RA
				B = T;//当前数据区基址为当前的栈顶
				P = I.a;//程序跳转到a处继续执行
				break;
			case INT://开辟数据区
				T += I.a;
				break;
			case JMP://无条件转移
				P = I.a;
				break;
			case JPC://条件转移
				if(S[T-1] == 0)
					P = I.a;
				break;
			case OPR://关系和算术运算
				switch(I.a)
				{
					case 0://子过程返回
						T = B;//释放这段子过程占用的数据栈空间
						B = S[T+1];//数据段基址为DL
						P = S[T+2];//返回地址为RA
						break;
					case 1://取反
						S[T-1] = -S[T-1];
						break;
					case 2://加
						T--;
						S[T-1] = S[T-1] + S[T];
						break;
					case 3://减
						T--;
						S[T-1] = S[T-1] - S[T];
						break;
					case 4://乘
						T--;
						S[T-1] = S[T-1] * S[T];
						break;
					case 5://除
						T--;
						S[T-1] = S[T-1] / S[T];
						break;
					case 6://奇偶判断，奇为1
						if(S[T-1] & 1)
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 8://判等
						T--;
						if(S[T-1] == S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 9://判不等
						T--;
						if(S[T-1] != S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 10://判小于
						T--;
						if(S[T-1] < S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 11://判大于等于
						T--;
						if(S[T-1] >= S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 12://判大于
						T--;
						if(S[T-1] > S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 13://判小于等于
						T--;
						if(S[T-1] <= S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 14://输出栈顶内容
						cout<<S[T-1];
						cout<<" ";//方便观察输出空格
						break;
					case 15://输出换行
						cout<<endl;
						break;
					case 16://输入到栈顶
						int num;
						cin>>num;
						S[T++] = num;
						break;
					default:
						error(wrongOpCode);//错误的运算码
				}
				break;
			default:
				error(wrongFuncCode);//错误的功能码
			if(T > stackSize)
				error(stackFull);//栈满了
		}
		//showS();
	}while(P != 0);//再次返回0的时候主程序结束
	cout<<"目标代码解释完成。"<<endl;
}

#endif //_CODEGENANDEXEC_

