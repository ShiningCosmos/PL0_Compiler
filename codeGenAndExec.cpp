#ifndef _CODEGENANDEXEC_
#define _CODEGENANDEXEC_

#include "common.h"

//������ű�
void enter(vector<symbItem> stable,string name,int kind,int lev,int value)
{
	if(position(name,lev)!=-1)//�ظ�����ı�ʶ��
		error(duplicateDefinitionIdentifier);
	if(kind == symConst)
	{
		Table.push_back(symbItem(name,kind,lev,value));
		tx++;//���ű�ָ��ָ����һ��
	}
	else if(kind == symVar)
	{
		Table.push_back(symbItem(name,kind,lev,dx));
		dx++;//��ַ����
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

//�ڱ����в��ұ�ʶ��
int findSameLevel(string name,int lev)
{
	int i;
	for(i=tx-1;i>=0;i--)
	{
		if(Table[i].kind == symProc 
			&& Table[i].level == lev 
			&& Table[i].name == name)
			return i;//��proc��lev��ͬ
		if(Table[i].kind != symProc && Table[i].level == lev)
			break;//����proc����lev��ͬ����һ������
		if(Table[i].level < lev)//levelС��lev��������û��var��cons�ͺ��ʵ�proc
			return -1;
	}
	for(;i>=0;i--)
	{//�ڱ����в���var��cons
		if(Table[i].level < lev)
			return -1;//������û�иñ�ʶ��
		if(Table[i].level == lev && Table[i].name == name)
			return i;//�ҵ��˸ñ�ʶ��������
	}
	return -1;//������û�иñ�ʶ��
}

//���������в��ұ�ʶ��
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

//���ұ�ʶ���ڷ��ű��е�λ��
int position(string name,int lev)
{
	int i = findSameLevel(name,lev);//�ڱ����в��ұ�ʶ��
	if(i == -1)//������û���ҵ��ñ�ʶ��
		i = findOtherLevel(name,lev);//���������в��ұ�ʶ��
	return i;
}

//����Ŀ����룬f�����룬l��aλ����
void gen(int f,int l,int a)
{
    code.push_back(instruction(f,l,a));
    cx++;
}

//չʾĿ�����
void showCode()
{
    cout<<"Ŀ������"<<endl;
    for(int i=0;i<code.size();i++)
    {
        cout<<i<<" "<<ins[code[i].f]<<" "
            <<code[i].l<<" "<<code[i].a<<endl;
    }
}

//չʾ���ű�
void showTable()
{
	cout<<"���ű�"<<endl;
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

//�ҵ����Ϊl��������
int base(int l)
{
	int b0 = B;//�ӵ�ǰ�㿪ʼ
	while(l > 0)
	{//��l��
		b0 = S[b0];//���ݻ�ַ���ݣ�SL���ҵ���һ��Ļ�ַ
		l--;
	}
	return b0;//���ز��Ϊl��������
}

//չʾ��������
void showS()
{
	cout<<"P:"<<P<<"\tT:"<<T<<"\tB:"<<B<<endl;
	cout<<"pc:"<<P-1<<" "<<ins[I.f]<<" "<<I.l<<" "<<I.a<<endl;
	for(int i = 0;i<T;i++)
		cout<<i<<" "<<S[i]<<endl;
	cout<<"end"<<endl;
}

//���ͳ���
void interpret()
{
	cout<<"Ŀ�������Ϳ�ʼ��"<<endl;
	P = 0;//�����ַ�Ĵ���
	T = 0;//ջ���Ĵ���
	B = 0;//����ַ�Ĵ���
	I = code[P];//ָ��Ĵ���
	fill(S,S+stackSize,0);//��ʼ��������
	S[0] = 0;//������SLΪ0
	S[1] = 0;//������DLΪ0
	S[2] = 0;//������RAΪ0
	do{
		I = code[P++];
		switch(I.f)
		{//���ݹ�����ִ�в�ͬ�Ĺ���
			case LIT://������ջ��
				S[T++] = I.a;
				break;
			case LOD://������ջ��
				S[T++] = S[I.a + base(I.l)];
				break;
			case STO://ջ�����ݵ�����
				//showS();
				T--;
				S[I.a + base(I.l)] = S[T];
				break;
			case CAL://���ù���
				//�����ֳ�
				S[T] = base(I.l);//ջ��ѹ�뾲̬��SL
				S[T+1] = B;//ջ��ѹ�뵱ǰ��������ַ����Ϊ��̬��DL
				S[T+2] = P;//ջ��ѹ�뵱ǰPC����Ϊ���ص�ַRA
				B = T;//��ǰ��������ַΪ��ǰ��ջ��
				P = I.a;//������ת��a������ִ��
				break;
			case INT://����������
				T += I.a;
				break;
			case JMP://������ת��
				P = I.a;
				break;
			case JPC://����ת��
				if(S[T-1] == 0)
					P = I.a;
				break;
			case OPR://��ϵ����������
				switch(I.a)
				{
					case 0://�ӹ��̷���
						T = B;//�ͷ�����ӹ���ռ�õ�����ջ�ռ�
						B = S[T+1];//���ݶλ�ַΪDL
						P = S[T+2];//���ص�ַΪRA
						break;
					case 1://ȡ��
						S[T-1] = -S[T-1];
						break;
					case 2://��
						T--;
						S[T-1] = S[T-1] + S[T];
						break;
					case 3://��
						T--;
						S[T-1] = S[T-1] - S[T];
						break;
					case 4://��
						T--;
						S[T-1] = S[T-1] * S[T];
						break;
					case 5://��
						T--;
						S[T-1] = S[T-1] / S[T];
						break;
					case 6://��ż�жϣ���Ϊ1
						if(S[T-1] & 1)
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 8://�е�
						T--;
						if(S[T-1] == S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 9://�в���
						T--;
						if(S[T-1] != S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 10://��С��
						T--;
						if(S[T-1] < S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 11://�д��ڵ���
						T--;
						if(S[T-1] >= S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 12://�д���
						T--;
						if(S[T-1] > S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 13://��С�ڵ���
						T--;
						if(S[T-1] <= S[T])
							S[T-1] = 1;
						else
							S[T-1] = 0;
						break;
					case 14://���ջ������
						cout<<S[T-1];
						cout<<" ";//����۲�����ո�
						break;
					case 15://�������
						cout<<endl;
						break;
					case 16://���뵽ջ��
						int num;
						cin>>num;
						S[T++] = num;
						break;
					default:
						error(wrongOpCode);//�����������
				}
				break;
			default:
				error(wrongFuncCode);//����Ĺ�����
			if(T > stackSize)
				error(stackFull);//ջ����
		}
		//showS();
	}while(P != 0);//�ٴη���0��ʱ�����������
	cout<<"Ŀ����������ɡ�"<<endl;
}

#endif //_CODEGENANDEXEC_

