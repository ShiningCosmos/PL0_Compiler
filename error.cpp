#ifndef _ERROR_
#define _ERROR_

#include "common.h"

void showError()
{
	showLexResult();
	getSyntaxTree(1,rootNode);
	showSyntaxTree();
	showTable();
	showCode();
}

void error(errortype type)
{
	switch(type)
	{
		case Error:
			cout<<"Error "<<Error<<endl;
			break;
		case symKindError:
			cout<<"Bad identifier category"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
			break;
		case lexError:
			cout<<"There is a lexError in Lexical analysis!"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
			break;
		case syntaxError:
			cout<<"There is a syntaxError in Syntax analysis!"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
			break;
		case fileOpenError:
			cout<<"Error,file can't open!"<<endl;
			break;
		case stackFull:
			cout<<"Error!Stack full!"<<endl;
			showS();
			break;
		case wrongOpCode:
			cout<<"Error!Wrong operator code!"<<endl;
			showS();
			showError();
			break;
		case wrongFuncCode:
			cout<<"Error!Wrong function code!"<<endl;
			showS();
			showError();
			break;
		case undefinedVariable:
			cout<<"Undefined variable!"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
			break;
		case wrongFactor:
			cout<<"Wrong factor!Procedure cannot be a factor!"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
			break;
		case undefinedProcedure:
			cout<<"Undefined procedure!"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
			break;
		case notProcedure:
			cout<<"The identifier is not a procedure!"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
			break;
		case notVariable:
			cout<<"The identifier is not a variable!"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
			break;
		case wrongRelaOperator:
			cout<<"Wrong relational operator!"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
		case duplicateDefinitionIdentifier:
			cout<<"Duplicate definition identifier!"<<endl;
			cout<<"The error occurred on the line "<<lexTable[lexTablePos-1].line
				<<", column "<<lexTable[lexTablePos-1].col<<"."<<endl;
			showError();
		default:
			cout<<"Unkown error type!"<<endl;
			break;
	}
	cout<<"Error occurred!"<<endl;
	exit(1);
}

#endif //_ERROR_