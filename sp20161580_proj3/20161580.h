#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //셸 명령어 dir에서 쓰이는 library
#include <sys/stat.h>//셸 명령어 dir에서 쓰이는 library

#define line_size 16
#define line_max 65536
#define tablesize 20
#define max_command_size 50

//ERROR 종류
#define ERROR_OutofRange -1
#define ERROR_StartIsLargerThanEnd -2
#define ERROR_WrongCommand -3
#define ERROR_WrongOpcode -4

#define NotInSymtab -5
//셸 명령어 history에 쓰이는 자료구조
typedef struct _node{
	char name[30];
	struct _node *next;
}node;
node* ActivatedCommands_head;
node* ActivatedCommands_tail;

typedef struct _hash {
	char sym[10];
	char order[6];
	char format[6];
	struct _hash *next;
}hash; //hashtable을 위한 구조체 
hash* hashtable[tablesize];
void AddHash(int key, char* sym, char* order, char* format);

unsigned char** mem;

void help();
int dir();
void history();
int dump(int start, int end); 
int dump_start = 0; //dump의 시작 주소

void edit(int address, int value);
void fill(int start, int end, int value);
void reset(void);
int opcode(char* Mnemonic);
int opcode1(char* Mnemonic);
void opcodelist(void);

int key(int opcode);
int Input_Data(void);
int Input_Order(char* order);
int Activate(char* whattodo, int par1, int par2, int par3, int parametercount, char *opcode, char file[][20]);
void AddCommand(char* command);

int minus_flag = 0; //입력받은 parameter에 음수가 존재할 경우 1이 됨

/*****************************************/
void type(char* filename);
int assemble(char* filename);

typedef struct __node {
	char name[30];
	int loc;
	struct __node *next;
}SYMTAB;
SYMTAB* symtab;
int symtabEXIST = 0;

int opcode_format(char* Mnemonic);
int FindSymtab(char* name);
void AddSymtab(int loc, char* name);
void symbol(void);

char* itoa(int num, char* str, int base);
int regis_to_data(char c);


/******************************************/
char EXTAB_CSN[3][10];
char EXTAB_CSN_ADR[3][10];
char EXTAB_CSN_LEN[3][10];

char EXTAB_SYM[3][10][10]; //[X][Y][Z] - X: Control Section Code Y: Control Section 내에서 몇 번째인지 Z: SYM_NAME
char EXTAB_ADR[3][10][10];

char Breakingpoint[10][10];
int Bpcount = 0;
void addbp(char string[]);
void clearbp(void);
void printbp(void);

void progaddr(char address[]);
int PROGADDR = 0;
int totallength = 0;

void loader(int parametercount, char file[][20]);
void run(void);

//Register의 종류
int A = 0, L = 0, B = 0, T = 0, X = 0, PC = 0, S = 0;
//run 시작 주소
int runEXECADDR;
//run이 이미 실행된 적이 있을 때 flag = 1
int actingflag = 0;

int CC = 0; //CC: 0 -> '=' -1 -> '<'
