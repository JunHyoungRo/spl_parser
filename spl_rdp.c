#define _CRT_SECURE_NO_WARNINGS
#define NO_KEYWORD 11
#define ID_LENGTH 12
#define FNAME_SIZE 12
#include <ctype.h>
#include <string.h>
#include <stdio.h>

//scanner function
int superLetter(char ch); int superLetterOrDigit(char ch); void lexicalError(int n); struct tokenType scanner();
int fget_c(FILE * stream); int unget_c(int ch, FILE * stream);

//parser function
void pSpl_program(); void pBlock(); void pDcl(); void pSt_list(); void pSt(); void pH(); void pStatement(); void pStatement_else(); void pAssign();
void pAssign_else(); void pGoto_st(); void pIf_st(); void pWrite_st(); void pCondition(); void pExp(); void pExp_else(); void pTerm(); void pFactor();
void getNextSymbol();  void apply(int n);  void syntaxError(int n);


enum tsymbol {
	tnull = -1, //total 28 (normal symbols(14) + word symbols(11) + ident + number + eof)
	tdot, tsemicolon, tcomma, tcolon, tassign,
	/*0          1          2          3          4*/

	tlbrace, trbrace, tless, tgreat, tequal,
	/*5          6          7          8          9*/

	tplus, tminus, tmul, tdiv,
	/*10          11          12          13*/


	/*------------------word symbols(11)------------------*/
	tbegin, tend, tlabel, tinteger, tgoto,
	/*14          15          16          17          18*/

	tif, tthen, telse, tfi, toutput,
	/*19          20          21          22          23*/

	tinput, tident, tnumber, teof
	/*24          25          26          27*/

};

char *tokenName[] = {
	".",        ";",       ",",       ":",       "=>",
	/*0          1          2          3          4*/

	"(",        ")",       "<",       ">",       "=",
	/*5          6          7          8          9*/

	"+",         "-",        "*",         "/",
	/*10          11          12          13*/

	/*------------------word symbols(11)------------------*/
	"begin",    "end",      "label",  "integer",    "goto",
	/*14          15          16          17          18*/

	"if",       "then",     "else",      "fi",     "output",
	/*19          20          21          22          23*/
	"input",    "%ident",   "%number",   "eof"
	/*24          25          26          27*/

};

char* keyword[NO_KEYWORD] = {
	"begin", "end", "label", "integer", "goto", "if", "then", "else", "fi", "output", "input"
};

enum tsymbol tnum[NO_KEYWORD] = {
	tbegin, tend, tlabel, tinteger, tgoto, tif, tthen, telse, tfi, toutput, tinput
};

struct tokenType {
	int number;
	int row_num;
	int col_num;
	union {
		char id[ID_LENGTH];
		int num;
	} value;
};

FILE *sourceFile;
char filename[FNAME_SIZE];
struct tokenType nextSymbol;
int cursorRow = 1, cursorCol = 0, cursorRowBack = 1, cursorColBack=0;
int rowNum = 1, colNum = 0;

/**************** Scanner Start ****************/
struct tokenType scanner() {
	struct tokenType token;
	int i, index;
	char ch, id[ID_LENGTH];

	token.number = tnull;

	do {
		while (isspace(ch = fget_c(sourceFile))); //공백제거
		rowNum = cursorRow; colNum = cursorCol;
		if (superLetter(ch)) { //identifier 나 keyword 인식 (_ 이나 알파벳이면)
			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fget_c(sourceFile);
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1);
			id[i] = '\0';
			unget_c(ch, sourceFile);
			for (index = 0; index < NO_KEYWORD; index++)
				if (!strcmp(id, keyword[index])) break; //keyword 찾기
			if (index < NO_KEYWORD) token.number = tnum[index];
			else { //identifier 일 경우
				token.number = tident;
				strcpy(token.value.id, id);
			}
		}
		else if (isdigit(ch)) { //숫자인식
			token.number = tnumber;
			int num = 0;
			do {
				num = 10 + num + (int)(ch - '0');
				ch = fget_c(sourceFile);
			} while (isdigit(ch));
			unget_c(ch, sourceFile);
			token.value.num = num;
		}
		else switch (ch) {
		case '.': token.number = tdot; break;
		case ';': token.number = tsemicolon; break;
		case ',': token.number = tcomma; break;
		case ':': token.number = tcolon; break;
		case '=': //assign(4) or equal(9)
			ch = fget_c(sourceFile);
			if (ch == '>') token.number = tassign;
			else {
				token.number = tequal;
				unget_c(ch, sourceFile);
			}
			break;
		case '(': token.number = tlbrace; break;
		case ')': token.number = trbrace; break;
		case '<': token.number = tless; break;
		case '>': token.number = tgreat; break;
		case '+': token.number = tplus;	break;
		case '-': token.number = tminus; break;
		case '*': token.number = tmul; break;
		case '/': token.number = tdiv; break;
		case EOF: token.number = teof; break;
		default: {
			printf("Current character is : %c", ch);
			lexicalError(2);
			break;
		}
		} //switch end
	} while (token.number == tnull);
	token.row_num = rowNum;
	token.col_num = colNum;;
	return token;
}

int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	else return 0;
}

//int fgetc ( FILE * stream )
int fget_c(FILE * stream) {
	char ch;
	cursorRowBack = cursorRow; cursorColBack = cursorCol;
	ch = fgetc(stream);
	if (ch == '\n') {
		cursorRow +=1; cursorCol = 0;
	}
	else cursorCol += 1;
	return ch;
}

//int ungetc (int chracter, FILE *stream)
int unget_c(int ch, FILE * stream) {
	if (ch == '\n') {
		cursorRow = cursorRowBack; cursorCol = cursorColBack;
	}
	else cursorCol -= 1;
	return ungetc(ch, stream);
}


void lexicalError(int n)
{
	printf(" *** Lexical Error : ");
	switch (n) {
	case 1: printf("Identifier length must be less than %d.\n", ID_LENGTH);	break;
	case 2: printf("Invalid character\n"); break;
	}
	exit(1);
}
/**************** Scanner End ****************/

/**************** RDP Start ****************/
/**
	*** Total Grammar
	<spl_program> ::= <block> '.'
	<block> ::= 'begin' {<dcl>';'} <st_list> 'end'
	<dcl> ::= ('label'|'integer')'id'{',''id'}
	<st_list> ::= <st> {';'<st>}
	<st> ::= 'id'<H> | <statement_else>
	<H> ::= ':'<statement> | {('*'|'/')<factor>} {('+'|'-')<term>} '=>' 'id' {'=>''id'}
	<statement> ::= <assign> | <goto_st> | <if_st> | <write_st> | <block>
	<statement_else> ::= <assign_else> | <goto_st> | <if_st> | <write_st> | <block>
	<assign> ::= <exp> '=>' 'id' {'=>''id'}
	<assign_else> ::= <exp_else> '=>' 'id' {'=>''id'}
	<goto_st>::= 'goto' 'id'
	<if_st> ::= 'if' <condition> 'then' <st_list> ['else'<st_list>] 'fi'
	<write_st> ::= 'output' '('<exp> {','<exp>} ')'
	<condition> ::= <exp> ('<'|'>'|'=')<exp>
	<exp> ::= <term> {('+'|'-')<term>}
	<exp_else> ::= ('input'|'number'|'('<exp>')') {('*'|'/')<factor>} {('+'|'-')<term>}
	<term> ::= <factor> {('*'|'/')<factor>}
	<factor> ::= 'input' | 'id' | 'number' | '('<exp>')'
*/

void pSpl_program() { //<spl_program> ::= <block> '.'
	apply(1);
	pBlock();
	if (nextSymbol.number == tdot) getNextSymbol();
	else syntaxError(20);
}

void pBlock() { //<block> ::= 'begin' {<dcl>';'} <st_list> 'end'
	if (nextSymbol.number == tbegin) {
		apply(2);
		getNextSymbol();
		while ((nextSymbol.number == tlabel) || (nextSymbol.number == tinteger)) {
			apply(3);
			pDcl();
			if (nextSymbol.number == tsemicolon) getNextSymbol();
			else syntaxError(18);
		}
		apply(4);
		pSt_list();
		if (nextSymbol.number == tend) getNextSymbol();
		else syntaxError(19);
	}
	else syntaxError(16);
}

void pDcl() { //<dcl> ::= ('label'|'integer')'id'{',''id'}
	if ((nextSymbol.number = tlabel) || (nextSymbol.number == tinteger)) {
		if (nextSymbol.number = tlabel) apply(5); else apply(6);
		getNextSymbol();
		if (nextSymbol.number == tident) {
			getNextSymbol();
			while (nextSymbol.number == tcomma) {
				apply(7);
				getNextSymbol();
				if (nextSymbol.number == tident) getNextSymbol();
				else syntaxError(11);
			}
			apply(8);
		}
		else syntaxError(11);
	}
	else syntaxError(17);
}

void pSt_list() { //<st_list> ::= <st> {';'<st>}
	apply(9);
	pSt();
	while (nextSymbol.number == tsemicolon) {
		apply(10);
		getNextSymbol();
		pSt();
	}
	apply(11);
}

void pSt() { //<st> ::= 'id'<H> | <statement_else>
	if (nextSymbol.number == tident) {
		apply(12);
		getNextSymbol();
		pH();
	}
	else if ((nextSymbol.number == tinput) || (nextSymbol.number == tnumber) || (nextSymbol.number == tlbrace) || (nextSymbol.number == tgoto) || (nextSymbol.number == tif) || (nextSymbol.number == toutput) || (nextSymbol.number == tbegin)) {
		apply(13);
		pStatement_else();
	}
	else syntaxError(15);
}

void pH() { //<H> ::= ':'<statement> | {('*'|'/')<factor>} {('+'|'-')<term>} '=>' 'id' {'=>''id'}
	if (nextSymbol.number == tcolon) {
		apply(14);
		getNextSymbol();
		pStatement();
	}
	else if ((nextSymbol.number == tmul)|| (nextSymbol.number == tdiv)||(nextSymbol.number == tplus) ||(nextSymbol.number == tminus) ||(nextSymbol.number == tassign)) {
		while ((nextSymbol.number == tmul) || (nextSymbol.number == tdiv)) {
			if(nextSymbol.number == tmul) apply(47); else apply(48);
			getNextSymbol();
			pFactor();
		}
		apply(49);
		while ((nextSymbol.number == tplus) || (nextSymbol.number == tminus)) {
			if(nextSymbol.number == tplus) apply(40); else apply(41);
			getNextSymbol();
			pTerm();
		}
		apply(42);
		if (nextSymbol.number == tassign) {
			getNextSymbol();
			if (nextSymbol.number == tident) {
				getNextSymbol();
				while (nextSymbol.number == tassign) {
					apply(27);
					getNextSymbol();
					if (nextSymbol.number == tident) getNextSymbol();
					else syntaxError(11);
				}
				apply(28);
			}
			else syntaxError(11);
		}
		else syntaxError(12);
	}
	else syntaxError(14);
}

void pStatement() { //<statement> ::= <assign> | <goto_st> | <if_st> | <write_st> | <block>
	switch (nextSymbol.number) {
	case(tinput): pAssign(); apply(16); break;
	case(tident): pAssign(); apply(16); break;
	case(tnumber): pAssign(); apply(16); break;
	case(tlbrace): pAssign(); apply(16); break;
	case (tgoto): pGoto_st(); apply(17); break;
	case(tif): pIf_st(); apply(18); break;
	case(toutput): pWrite_st(); apply(19); break;
	case(tbegin): pBlock(); apply(20); break;
	}
}

void pStatement_else() { //<statement_else> ::= <assign_else> | <goto_st> | <if_st> | <write_st> | <block>
	switch (nextSymbol.number) {
	case(tinput): pAssign_else(); apply(21); break;
	case(tnumber): pAssign_else(); apply(21); break;
	case(tlbrace): pAssign_else(); apply(21); break;
	case (tgoto): pGoto_st(); apply(22); break;
	case(tif): pIf_st(); apply(23); break;
	case(toutput): pWrite_st(); apply(24); break;
	case(tbegin): pBlock(); apply(25); break;
	}
}

void pAssign() { //<assign> ::= <exp> '=>' 'id' {'=>''id'}
	apply(26);
	pExp();
	if (nextSymbol.number == tassign) {
		getNextSymbol();
		if (nextSymbol.number == tident) {
			getNextSymbol();
			while (nextSymbol.number == tassign) {
				apply(27);
				getNextSymbol();
				if (nextSymbol.number == tident) getNextSymbol();
				else syntaxError(11);
			}
			apply(28);
		}
		else syntaxError(11);
	}
	else syntaxError(12);
}

void pAssign_else() { //<assign_else> ::= <exp_else> '=>' 'id' {'=>''id'}
	apply(29);
	pExp_else();
	if (nextSymbol.number == tassign) {
		getNextSymbol();
		if (nextSymbol.number == tident) {
			getNextSymbol();
			while (nextSymbol.number == tassign) {
				apply(27);
				getNextSymbol();
				if (nextSymbol.number == tident) getNextSymbol();
				else syntaxError(11);
			}
			apply(28);
		}
		else syntaxError(11);
	}
	else syntaxError(12);
}

void pGoto_st() { //<goto_st>::= 'goto' 'id'
	if (nextSymbol.number == tgoto) {
		apply(30);
		getNextSymbol();
		if (nextSymbol.number == tident) getNextSymbol();
		else syntaxError(11);
	}
	else syntaxError(10);
}

void pIf_st() { //<if_st> ::= 'if' <condition> 'then' <st_list> ['else'<st_list>] 'fi'
	if (nextSymbol.number == tif) {
		getNextSymbol();
		pCondition();
		if (nextSymbol.number == tthen) {
			getNextSymbol();
			pSt_list();
			if (nextSymbol.number == telse) {
				apply(32);
				getNextSymbol();
				pSt_list();
				if (nextSymbol.number == tfi) getNextSymbol();
				else syntaxError(6);
			}
			else if (nextSymbol.number == tfi) {
				apply(31);
				getNextSymbol();
			}
			else syntaxError(7);
		}
		else syntaxError(9);
	}
	else syntaxError(8);
}

void pWrite_st() { //<write_st> ::= 'output' '('<exp> {','<exp>} ')'
	if (nextSymbol.number == toutput) {
		apply(33);
		getNextSymbol();
		if (nextSymbol.number == tlbrace) {
			getNextSymbol();
			pExp();
			while (nextSymbol.number == tcomma) {
				getNextSymbol();
				pExp();
			}
			if (nextSymbol.number == trbrace) getNextSymbol();
			else syntaxError(2);
		}
		else syntaxError(5);
	}
	else syntaxError(4);
}

void pCondition() { //<condition> ::= <exp> ('<'|'>'|'=')<exp>
	pExp();
	if ((nextSymbol.number == tless) || (nextSymbol.number == tgreat) || (nextSymbol.number == tequal)) {
		if (nextSymbol.number == tless) apply(36); else if(nextSymbol.number== tgreat)apply(37);else apply(38);
		getNextSymbol();
		pExp();
	}
	else syntaxError(13);
}

void pExp() { //<exp> ::= <term> {('+'|'-')<term>}
	apply(39);
	pTerm();
	while ((nextSymbol.number == tplus) || (nextSymbol.number == tminus)) {
		if(nextSymbol.number==tplus)apply(40); else apply(41);
		getNextSymbol();
		pTerm();
	}
	apply(42);
}

void pExp_else() { //<exp_else> ::= ('input'|'number'|'('<exp>')') {('*'|'/')<factor>} {('+'|'-')<term>}
	if ((nextSymbol.number == tinput) || (nextSymbol.number == tnumber)) {
		if(nextSymbol.number==tinput) apply(43); else if (nextSymbol.number == tnumber) apply(44);
		getNextSymbol();
	}
	else if (nextSymbol.number == tlbrace) {
		apply(45);
		getNextSymbol();
		pExp();
		if (nextSymbol.number == trbrace) getNextSymbol();
		else syntaxError(2);
	}
	else syntaxError(3);
	while ((nextSymbol.number == tmul) || (nextSymbol.number == tdiv)) {
		getNextSymbol();
		pFactor();
	}
	while ((nextSymbol.number == tplus) || (nextSymbol.number == tminus)) {
		getNextSymbol();
		pTerm();
	}
}

void pTerm() { //<term> ::= <factor> {('*'|'/')<factor>}
	apply(46);
	pFactor();
	while ((nextSymbol.number == tmul) || (nextSymbol.number == tdiv)) {
		if (nextSymbol.number==tmul)apply(47);else apply(48);
		getNextSymbol();
		pFactor();
	}
	apply(49);
}

void pFactor() { //<factor> ::= 'input' | 'id' | 'number' | '('<exp>')'
	if ((nextSymbol.number == tinput) || (nextSymbol.number == tident) || (nextSymbol.number == tnumber)) {
		if (nextSymbol.number == tinput) apply(50); else if (nextSymbol.number == tident) apply(51); else apply(52);
		getNextSymbol();
	}
	else if (nextSymbol.number == tlbrace) {
		apply(53);
		getNextSymbol();
		pExp();
		if (nextSymbol.number == trbrace) getNextSymbol();
		else syntaxError(2);
	}
	else syntaxError(1);
}

void getNextSymbol() {
	nextSymbol = scanner();
}
void apply(int n) {
	printf(" * rule number = %3d\n",n);
	/** List of rule number(53)
		1) <spl_program> ::= <block> '.'
		2) <block> ::= 'begin' <A> <st_list> 'end'
		3) <A> ::= <dcl>';'<A>
		4) <A> ::= ε
		5) <dcl> ::= 'label' 'id' <B>
		6) <dcl> ::= 'integer' 'id' <B>
		7) <B> ::= ',''id'<B>
		8) <B> ::= ε
		9) <st_list> ::= <st><C>
		10) <C> ::= ';' <st><C>
		11) <C> ::= ε
		12) <st> ::= 'id'<H>
		13) <st> ::= <statement_else>
		14) <H> ::= ':'<statement>
		15) <H> ::= <G><F>'=>''id'<D>
		16) <statement> ::= <assign>
		17) <statement> ::= <goto_st>
		18) <statement> ::= <if_st>
		19) <statement> ::= <write_st>
		20) <statement> ::= <block>
		21) <statement_else> ::= <assign_else>
		22) <statement_else> ::= <goto_st>
		23) <statement_else> ::= <if_st>
		24) <statement_else> ::= <write_st>
		25) <statement_else> ::= <block>
		26) <assign> ::= <exp> '=>' 'id' <D>
		27) <D> ::= '=>''id'<D>
		28) <D> ::= ε
		29) <assign_else> ::= <exp_else> '=>' 'id' <D>
		30) <goto_st>::= 'goto' 'id'
		31) <if_st> ::= 'if' <condition> 'then' <st_list> 'fi'
		32) <if_st> ::= 'if' <condition> 'then' <st_list> 'else' <st_list> 'fi'
		33) <write_st> ::= 'output' '(' <exp><E> ')'
		34) <E> ::= ','<exp><E>
		35) <E> ::= ε
		36) <condition> ::= <exp> '<' <exp>
		37) <condition> ::= <exp> '>' <exp>
		38) <condition> ::= <exp> '=' <exp>
		39) <exp> ::= <term><F>
		40) <F> ::= '+'<term><F>
		41) <F> ::= '-'<term><F>
		42) <F> ::= ε
		43) <exp_else> ::= 'input' <G> <F>
		44) <exp_else> ::= 'number' <G> <F>
		45) <exp_else> ::= '('<exp>')' <G> <F>
		46) <term> ::= <factor><G>
		47) <G> ::= '*'<factor><G>
		48) <G> ::= '/'<factor><G>
		49) <G> ::= ε
		50) <factor> ::= 'input'
		51) <factor> ::= 'id'
		52) <factor> ::= 'number'
		53) <factor> ::= '('<exp>')'
	*/
}

void syntaxError(int n) {
	printf(" \n*** syntax Error : ");
	switch (n) {
	case 1: printf("err(1) expected : input, identifier, number, ("); break;
	case 2: printf("err(2) expected : )"); break;
	case 3: printf("err(3) expected : input, number, ("); break;
	case 4: printf("err(4) expected : output"); break;
	case 5: printf("err(5) expected : ("); break;
	case 6: printf("err(6) expected : fi"); break;
	case 7: printf("err(7) expected : else, fi"); break;
	case 8: printf("err(8) expected : if"); break;
	case 9: printf("err(9) expected : then"); break;
	case 10: printf("err(10) expected : goto"); break;
	case 11: printf("err(11) expected : identifier"); break;
	case 12: printf("err(12) expected : =>"); break;
	case 13: printf("err(13) expected : <, >, ="); break;
	case 14: printf("err(14) expected : :, +, -, =>"); break;
	case 15: printf("err(15) expected : identifier, input, number, (, goto, if, ouptut, begin"); break;
	case 16: printf("err(16) expected : begin"); break;
	case 17: printf("err(17) expected : label, integer"); break;
	case 18: printf("err(18) expected : ;"); break;
	case 19: printf("err(19) expected : end"); break;
	case 20: printf("err(20) expected : ."); break;
	}
	printf("\n*** At row %d, col%d", nextSymbol.row_num, nextSymbol.col_num);
	printf("\n");
	exit(1);
}
/**************** RDP End ****************/


void main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("\nEnter filename : ");
		scanf("%s", filename);
	}
	else if (argc == 2) {
		strcpy(filename, argv[1]);
	}
	else {
		printf("Argument error!\n");
		exit(1);
	}

	if ((sourceFile = fopen(filename, "r")) == NULL) {
		printf("File open error!\n");
		exit(2);
	}

	getNextSymbol();
	pSpl_program();
	if (nextSymbol.number == teof)
		printf("Accepted\n");
	else
		printf("Invalid source\n");
}
