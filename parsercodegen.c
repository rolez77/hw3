#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_TOKENS     20000
#define MAX_IDENT_LEN  11

typedef enum {
    skipsym = 1 , // Skip / ignore token
    identsym =2, // Identifier
    numbersym = 3 , // Number
    plussym = 4, // +
    minussym = 5, // -
    multsym = 6, // *
    slashsym = 7, // /
    eqsym =8, // =
    neqsym =9, // <>
    lessym =10, // <
    leqsym =11, // <=
    gtrsym =12, // >
    geqsym =13, // >=
    lparentsym =14, // (
    rparentsym =15, // )
    commasym =16, // ,
    semicolonsym = 17, // ;
    periodsym =18, // .
    becomessym =19, // :=
    beginsym = 20, // begin
    endsym = 21, // end
    ifsym = 22, // if
    fisym = 23, // fi
    thensym = 24, // then
    whilesym = 25, // while
    dosym =26, // do
    callsym =27, // call
    constsym =28, // const
    varsym =29, // var
    procsym =30, // procedure
    writesym =31, // write
    readsym =32, // read
    elsesym =33, // else
    evensym =34 // even
} TokenType ;

typedef enum{
    LIT =1,
    OPR =2,
    LOD =3,
    STO =4,
    CAL =5,
    INC =6,
    JMP =7,
    JPC =8,
    SYS =9
}OpCode;

typedef enum{
   RTN = 0,
   ADD = 1,
   SUB = 2,
   MUL = 3,
   DIV = 4,
   EQUL = 5,
   NEQ = 6,
   LSS = 7,
   LEQ = 8,
   GTR = 9,
   GEQ = 10,
   EVEN = 11
}oprcodes;



typedef struct ir{
    int op;
    int l;
    int m;
} InstructionRegister;

typedef struct{
    int kind;
    char name[12];
    int val;
    int level;
    int addr;
    int mark;
}symbol;

typedef struct {
    int type;
    char ident[MAX_IDENT_LEN + 1]; 
    int number;                   
} Token;

void program(void);
void block(void);
void statement(void);
void condition(void);
void expression(void);
void constDeclaration(void);
int  varDeclaration(void);

// you already have these in your file; keep your versions:
int symbolTableCheck(const char *name);
void addToSymbolTable(int kind, const char *name, int val, int level, int addr);

static Token tokens[MAX_TOKENS];
static int ntokens = 0;
static int cur = 0;

//globals
int tokenList[MAX_SYMBOL_TABLE_SIZE];
int tokenInd =0;
int tokenCount = 0;
int codeIdx = 0;
int symbolInd = 1;
int numVars = 0;
int numConsts = 0;
InstructionRegister code[MAX_SYMBOL_TABLE_SIZE];

// list of error messages each corresponding to an index
const char* errorMessages[] = {
    "Scanning error detected by lexer (skipsym present)", //0
    "program must end with period", //1
    "const, var, and read keywords must be followed by identifier",//2
    "symbol name has already been declared",//3
    "constants must be assigned with =",//4
    "constants must be assigned an integer value",//5
    "constant and variable declarations must be followed by a semicolon",//6
    "undeclared identifier",//7
    "only variable values may be altered",//8
    "assignment statements must use :=",//9
    "begin must be followed by end",//10
    "if must be followed by then",//11
    "while must be followed by do",//12
    "condition must contain comparison operator",//13
    "right parenthesis must follow left parenthesis",//14
    "arithmetic equations must contain operands, parentheses, numbers, or symbols"//15
};


symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];


// linear search of the table, takes in a name as a parameter, if found it will return the index, otherwise returns -1 if not found
int symbolTableCheck(const char*name){
    for(int i =0; i<symbolInd;i++){
        if(strcmp(symbol_table[i].name,name) ==0 && symbol_table[i].mark ==0){
            return i;
        }
    }
    return -1;
}

/*
Function to add to the symbol table
Takes in kind,name,val,level,addr
// level will always be 0 for this project
*/
void addToSymbolTable(int kind, const char* name, int val, int level, int addr){
    int len = strlen(name);
    if(symbolInd >= MAX_SYMBOL_TABLE_SIZE){
        printf("Overflow");
        return;
    }
    symbol_table[symbolInd].kind = kind;
    strncpy(symbol_table[symbolInd].name, name, len);
    symbol_table[symbolInd].name[len] = '\0';
    symbol_table[symbolInd].val = val;
    symbol_table[symbolInd].level = level;
    symbol_table[symbolInd].addr = addr;
    symbol_table[symbolInd].mark = 0; // unmarked
    symbolInd++;

}


void getNextToken(){
    if(tokenInd < tokenCount)
    {
     tokenInd++;
    }
}

void expression(){
    if(tokenList[symbolInd] == minussym){
        getNextToken();
        //TERM
        //emit NEG
        while(tokenList[symbolInd] == plussym || tokenList[symbolInd] == minussym){
            if(tokenList[symbolInd] == plussym){
                getNextToken();
                //TERM
                //EMIT ADD
            }else{
                getNextToken();
                //TERM
                //EMIT SUB
            }
        }
    }else{
        if(tokenList[symbolInd] == plussym){
            getNextToken();
        }
        //TERM
        while(tokenList[symbolInd] == plussym || tokenList[symbolInd] == minussym){
            if(tokenList[symbolInd] == plussym){
                getNextToken();
                //TERM
                //EMIT ADD
            }else{
                getNextToken();
                //TERM
                //EMIT SUB
            }
        }
    }
}

void condition(){
    if(tokenList[symbolInd] == evensym){
        getNextToken();
        //emit ODD
    }else{
        expression();
        if(tokenList[symbolInd] == eqsym){
            getNextToken();
            expression();
            //emit EQUL
        }else if(tokenList[symbolInd] == neqsym){
            getNextToken();
            expression();
            //emit NEQ
        }else if(tokenList[symbolInd] == lessym){
            getNextToken();
            expression();
            //emit LSS
        }else if(tokenList[symbolInd] == leqsym){
            getNextToken();
            expression();
            //emit LEQ
        }else if(tokenList[symbolInd] == gtrsym){
            getNextToken();
            expression();
            //emit GTR
        }else if(tokenList[symbolInd] == geqsym){
            getNextToken();
            expression();
            //emit GEQ
        }else{
            printf("%s",errorMessages[13]);
            return;
        }
    }
}

void statement(){
  if (tokenList[symbolInd] == identsym){
    int symIdx = symbolTableCheck(tokenList[symbolInd]);
    if(symIdx == -1){
        printf("%s",errorMessages[7]);
        return;
    }
    getNextToken();
    // not a var
    if(symbol_table[symIdx].kind != 2){
        printf("%s",errorMessages[8]);
        return;
    }
    if(tokenList[symbolInd]!=becomessym){
        printf("%s",errorMessages[9]);
        return;
    }
    getNextToken();
    // emit STO M = symbol_table[symIdx].addr
    return;
  }
  if(tokenList[symbolInd] ==beginsym){
    do{
        getNextToken();
        statement();
    }while(tokenList[symbolInd]==semicolonsym);
    if(tokenList[symbolInd]!=endsym){
        printf("%s",errorMessages[10]);
        return;
    }
    getNextToken();
    return;
  }

  // if token is ifsym
  if(tokenList[symbolInd]==ifsym){

    getNextToken();
    //condition
    int jpcIdx = codeIdx;
    // emit(JPC)
    if(tokenList[symbolInd]!=thensym){
        printf("%s",errorMessages[11]);
        return;
    }
    getNextToken();
    statement();
    code[jpcIdx].m = codeIdx;
    return;
  }

}

void constDeclaration(const char* token){
    if(token == constsym){
        do{
            numConsts++;
            getNextToken();
            if(token!=identsym){
                printf("%s",errorMessages[2]);
                return;
            }
            if(symbolTableCheck(token) != -1){
                printf("%s",errorMessages[7]);
                return;
            }
            // save name
            getNextToken();
            if(token != eqsym){
                printf("%s",errorMessages[4]);
                return;
            }
            getNextToken();
            if(token != numbersym){
                printf("%s",errorMessages[5]);
                return;
            }
            addToSymbolTable(1, token, 0, 0, numVars+2);
            getNextToken();
        }while(token == commasym);
        if(token != semicolonsym){
            printf("%s",errorMessages[6]);
            return;
        }
        getNextToken();
    }
    if(tokenList[tokenInd] == whilesym){
        getNextToken();
        int loopIdx = codeIdx;
        //CONDITION
        if(tokenList[tokenInd]!=dosym){
            printf("%s",errorMessages[12]);
            return;
        }
        getNextToken();
        int jpcIdx = codeIdx;
        // emit(JPC)
        statement();
        // emitJMP m = loopidx
        code[jpcIdx].m = codeIdx;
        return;
    }

    //readsym
    if(tokenList[tokenInd] == readsym){
        getNextToken();
        if(tokenList[tokenInd] != identsym){
            printf("%s",errorMessages[2]);
            return;
        }
        int symIdx = symbolTableCheck(tokenList[tokenInd]);
        if(symIdx == -1){
            printf("%s",errorMessages[7]);
            return;
        }
        if(symbol_table[symIdx].kind != 2){
            printf("%s",errorMessages[8]);
            return;
        }
        getNextToken();
        // emit read
        // emit sto m = symbol_table[symIdx].addr
        return;
    }
    //writesym
    if(tokenList[tokenInd] == writesym){
        getNextToken();
        //EXPRESSION
        // emit write
        return;
    }
}
int varDeclaration(const char* token){
    if(token == varsym){
        do{
            numVars++;
            getNextToken();
            if(token!=identsym){
                printf("%s",errorMessages[2]);
                return;
            }
            if(symbolTableCheck(token) != -1){
                printf("%s",errorMessages[7]);
                return;
            }
            addToSymbolTable(2, token, 0, 0, numVars+2);
            getNextToken();
        }while(token == commasym);
        if(token != semicolonsym){
            printf("%s",errorMessages[6]);
            return;
        }
        getNextToken();
    }
    return numVars;

}


void program(){
    block();
    if(tokenList[symbolInd] != periodsym){
        printf("%s",errorMessages[1]);
        return;
    }
    // halt instr.
}

void block(){
   // constDeclaration();
   // int numVars = varDeclaration();
    // emit INC M=3, numVars
    statement();

}

void emit(int op, int l, int m){
    code[codeIdx].op = op;
    code[codeIdx].l = l;
    code[codeIdx].m = m;
    codeIdx++;
}

static void load_tokens_from_file(void) {
    FILE *f = fopen("tokens.txt", "r");
    if (!f) { perror("tokens.txt"); exit(1); }

    int t;
    while (fscanf(f, "%d", &t) == 1) {
        Token tok;
        memset(&tok, 0, sizeof tok);
        tok.type = t;

        if (t == identsym) {
            if (fscanf(f, "%11s", tok.ident) != 1) break;
        } else if (t == numbersym) {
            if (fscanf(f, "%d", &tok.number) != 1) break;
        }

        if (ntokens < MAX_TOKENS) tokens[ntokens++] = tok;
        else { fprintf(stderr, "Too many tokens\n"); break; }
    }
    fclose(f);
}


int main(){
    load_tokens_from_file();


    FILE *file = fopen("output.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }



    


    return 0;
}