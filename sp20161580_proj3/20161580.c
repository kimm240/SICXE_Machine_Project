#include "20161580.h"

void help(){ //Shell에서 실행 가능한 모든 명령어들의 리스트를 화면에 출력한다.
	printf("\nh[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n\n");
}

int dir(){//현재 Directory에 있는 파일들을 출력한다.
	DIR* dp = NULL;
	struct dirent *dir_entry = NULL;
	struct stat buf;

	//현재 directory를 여는데 실패했을 경우
	//"."은 현재 디렉토리를 지칭한다.
	if((dp = opendir(".")) == NULL){
		printf("Can't open present directory\n");
		return -1;
	}
	//현재 directory에 있는 모든 파일과 directory를 출력한다.
	while((dir_entry = readdir(dp))!= NULL){
		//주어진 parameter(dir_entry)가 파일인지 Directory인지 판단한다.
		lstat(dir_entry->d_name, &buf);		
		//Dir의 결과를 출력할 때 실행 파일은 파일 이름 옆에 "*"표시를 해야한다.
		if (S_ISREG(buf.st_mode)) 
		{
			dir_entry->d_name[strlen(dir_entry->d_name)] = '\0';
			if(!strcmp(".out", dir_entry->d_name + strlen(dir_entry->d_name) - 4)) printf("%s*\t", dir_entry->d_name);
			else printf("%s\t", dir_entry->d_name);
		}
		//Directory는 '/'표시를 해야한다.
		else if(S_ISDIR(buf.st_mode))printf("%s/\t", dir_entry->d_name);
	}
	//이용한 DIR(현재 디렉토리)을  닫아준다.
	closedir(dp);
	printf("\n");
	return 0;
}


void history(){//현재까지 사용한 명령어들을 순서대로 번호(index)와 함께 보여준다.
	int index = 1;
	node* his = ActivatedCommands_head;
	while(his != NULL)
	{
		printf("%d %s\n", index++, his->name);
		his = his->next;
	}
	free(his);	
}


int Input_Order(char* order){//입력받은 명령어의 정보를 상황에 맞게 구분한다.
		
	//char* whattodo는 명령어를 저장하는 array이다.
	//char* num[0]은 명령에 쓰이는 1번째 parameter, 
	//char* num[1]은 2번째 parameter이다. 이들은 우선 16진수로 저장한다.
	//ex)1번째 parameter가 6A(16)일 경우, num[0][0] = '6', num[0][1] = 'A'로 저장한다.
	
	char **num = malloc(3*sizeof(char*));
	for(int n=0; n<3; ++n){
		num[n] = (char*)malloc(10*sizeof(char));
		}
	//char *opcode = malloc(10*sizeof(char));
	//char *whattodo = malloc(10*sizeof(char));
	char opcode[10];
	char whattodo[10];

	char file[3][20]; int f = 0; int k = 0;

	memset(whattodo, 0, sizeof(whattodo));	
	
	int i; int order_len = strlen(order); int NN = 0, nn = 0, wtd = 0, str = 0;
	int IsParameterNum = 0; int IsParameterString = 0; int CASE_loader = 0;
	
	for(i=0; i<order_len; ++i){
		if(IsParameterNum){
			if(order[i] == '-') minus_flag = 1;
			//parameter가 소문자로 입력되었을 때, 대문자로 바꿔준다.
			//따라서 프로그램은 16진수가 소문자이든 대문자이든간에 인식할 수 있다.
			if(order[i] >= 'a' && order[i] <= 'f') order[i] = 'A' + order[i] - 'a';

			if((order[i] >= '0' && order[i] <= '9') || (order[i] >= 'A' && order[i] <= 'F')){ //parameter가  숫자일 경우
			num[NN][nn++] = order[i];
				}
			else if(order[i] == ',') {//NN번째 parameter 정보를 저장했고, NN+1번째 parameter 정보를 저장해야 할 경우
				NN++; nn=0;
				}
			else if(order[i] == ' ') continue;
			}

		else if(IsParameterString){//parameter가 string일 경우
			opcode[str++] = order[i];
			}
		else if(CASE_loader){
			if(order[i] == ' '){
				file[f][k] = '\0';  f++; k=0;	
			}
		
			else{
				file[f][k++] = order[i];
			}

		}
		else{
		
			if(order[i] >= 'a' && order[i] <= 'z'){ //입력받는 값이 소문자 알파벳일 경우
								   //즉, 명령어일 경우
				whattodo[wtd++] = order[i];
				}
			if(order[i] == ' ') {
				whattodo[strlen(whattodo)] = '\0';

				if(!strcmp(whattodo, "opcode") || !strcmp(whattodo, "assemble") || !strcmp(whattodo, "type") || !strcmp(whattodo, "progaddr") || !strcmp(whattodo, "bp")) {IsParameterString = 1;} //명령어 정보를 다 담아낸 후, paramter가 숫자인지 string인지 알아낸다
				else if(!strcmp(whattodo, "loader")){CASE_loader = 1;}
				else if(!strcmp(whattodo, "run")){break;}
				else IsParameterNum = 1;
			}
		}
	}
 	if(CASE_loader) file[f][i] = '\0';

	//char* num을 strtol 함수를 이용하여 10진수로 변환한다.
	//예를 들어, (16진수)37 -> (10진수)55
	int par1 = strtol(num[0], NULL, 16);
	int par2 = strtol(num[1], NULL, 16);
	int par3 = strtol(num[2], NULL, 16);
	//whattodo의 마지막에 NULL값을 저장한다.
	whattodo[wtd] = '\0'; opcode[str] = '\0';
	
	int parametercount = NN + 1;
	//Parameter가 string일 경우 parametercount는 0이다.
	if(!IsParameterNum) parametercount = 0;
	//loader을 실행할 때 parametercount는 f+1이다.
	if(CASE_loader) parametercount = f + 1;
	//bp할 때는 string의 크기가 필요하다.
	if(!strcmp(whattodo, "bp")) parametercount = str;
	//flag에는 실행한 명령어에 ERROR가 있을 경우 해당하는 ERROR을 return받는 변수다.
	//명령어에 ERROR이 없을 경우 아무 값도 return받지 않는다.
	int flag = Activate(whattodo, par1, par2, par3, parametercount, opcode, file);

	if(flag == ERROR_WrongCommand) return flag;
	else if(flag == ERROR_StartIsLargerThanEnd) return flag;
	else if(flag == ERROR_OutofRange) return flag;
	else if(flag == ERROR_WrongOpcode) return flag;
	else return 0; 
}

int Activate(char* whattodo, int par1, int par2, int par3, int parametercount, char *string, char file[][20])
{
//char* whattodo의 정보에 해당하는 함수를 실행한다. 
	int start, end, value;
	int address;
		
	if(!strcmp(whattodo, "h") || !strcmp(whattodo, "help")){help(); return 0;}
	else if(!strcmp(whattodo, "d") || !strcmp(whattodo, "dir")){dir(); return 0;}
	else if(!strcmp(whattodo, "hi") || !strcmp(whattodo, "history")){
		//프로그램의 특성상 미리 명령어를 저장한다.
		AddCommand(whattodo);
		history();
		return 0;
		}
	else if(!strcmp(whattodo, "du") || !strcmp(whattodo, "dump"))
	{
		if(parametercount == 0) //parameter의 개수가 0일 경우. ex) dump
		{
		start = dump_start; end = start + line_size * 10 - 1;
		dump_start = end + 1;
		}
		else if(parametercount == 1) //parameter의 개수가 1일 경우. ex) dump 10
		{
			start = par1; end = par1 + line_size * 10 - 1;
		}
		else //parameter의 개수가 2일 경우. ex)dump 10, 2A
		{
			start = par1; end = par2;
			//parameter가 2개일 때, end가 memory 범위를 벗어날 경우 return ERROR_OutofRange
			if(end >= line_size * line_max) return ERROR_OutofRange;
		}
		//parameter가 0개 혹은 1개일 때, end가 memory 영역 밖으로 벗어날 경우
		if(end >= line_size * line_max){
			//end는 memory 끝 값으로 바꿔준다. 
			end = line_size * line_max - 1;
			//parametercount가 0일 경우 dump를 실행할 때 처음부터 실행되어야 한다.
			//따라서 dump_start를 0으로 초기화한다.
			if(parametercount == 0) dump_start = 0;
			}
		//start 또는 end가 음수일 경우 minus_flag = 1
		if(minus_flag){
		minus_flag = 0;
		return ERROR_OutofRange;
		}
		//start가 memory 영역 끝 값보다 클 경우
		if(start >= line_size * line_max) return ERROR_OutofRange;
		
		else {
			//start가 end보다 클 경우
			if(start > end) return ERROR_StartIsLargerThanEnd;
			else {dump(start, end); return 0;}
		}
	}
	else if((!strcmp(whattodo, "e") || !strcmp(whattodo, "edit")) && parametercount == 2)
	{
		address = par1; value = par2;
		//입력받은 parameter에 음수가 있을 경우
		if(minus_flag){
			minus_flag = 0;
			return ERROR_OutofRange;
			}
		else if(value >= 32 && value <= 126) {edit(address, value); return 0;}
		//value가 영역에 해당하지 않을 경우 ERROR
		else return ERROR_OutofRange;
	}
	else if((!strcmp(whattodo, "f") || !strcmp(whattodo, "fill")) && parametercount == 3)
	{
		start = par1; end = par2; value = par3;
		//입력받은 parameter에 음수가 있을 경우
		if(minus_flag){
			minus_flag = 0;
			return ERROR_OutofRange;
			}
		//start가 end보다 클 경우
		else if(start > end) return ERROR_StartIsLargerThanEnd;
		//start나 end가 메모리 영역을 양의 방향으로 벗어날 경우
		else if(start >= line_size * line_max || end >= line_size * line_max) return ERROR_OutofRange;
		//value가 영역에 해당하지 않을 경우
		else if(value < 32 || value > 126) return ERROR_OutofRange;
		else {fill(start, end, value); return 0;}
	}
	else if(!strcmp(whattodo, "reset")){reset(); return 0;}
	else if(!strcmp(whattodo, "opcode")){
		//opcode.txt에 존재하지 않는 opcode가 넘겨졌을 때
		if(opcode1(string) == ERROR_WrongOpcode){return ERROR_WrongOpcode;}
		else return 0;}
	else if(!strcmp(whattodo, "opcodelist")){opcodelist(); return 0;}
	else if(!strcmp(whattodo, "assemble")){assemble(string); return 0;}
	else if(!strcmp(whattodo, "symbol")){symbol(); return 0;}
	else if(!strcmp(whattodo, "type")){type(string); return 0;}
	else if(!strcmp(whattodo, "loader")){loader(parametercount, file); return 0;}
	else if(!strcmp(whattodo, "progaddr")){progaddr(string); return 0;}
	else if(!strcmp(whattodo, "bp")){
		if(!strcmp(string, "clear")){ clearbp(); return 0;}
		else { 
			if(parametercount == 0) {printbp();}
			else{addbp(string); return 0;}
		}	
	}
	else if(!strcmp(whattodo, "run")){run(); return 0;}
	else return ERROR_WrongCommand;
}

void run(void){
	//ENDADDR: Load한 Program의 끝 주소 
	int ENDADDR = PROGADDR + totallength;
	//breaking point	
	int bp;
	//actingflag: 이전에 이미 run을 실행했을 경우
	//runEXECADDR: actingflag가 1일 경우, 이번 프로그램이 실행되어야할 주소
	if(actingflag) PC = runEXECADDR;
	else{ A = 0; B = 0; T = 0; X = 0; S = 0; PC = PROGADDR; L = ENDADDR; }
	
	int check = 0;
	int break_flag = 0;

	while(PC != ENDADDR){

		int y = PC / line_size; int x = PC % line_size;
		int left[5]; int right[5]; int data[5];
		//data[k]: PC 기준 k번째 memory가 가지고 있는 data
		//left[k]: data[k]에서 왼쪽에 있는 data
		//right[k]: data[k]에서 오른쪽에 있는 data
		//ex)data[k] = 17일 경우, left[k] = 1 right[k] = 7
		for(int k=0; k<3; ++k){
				data[k] = mem[y][x];
				left[k] = data[k] / 16; 
				right[k] = data[k] % 16;
				x++;
				if(x >= line_size) { y++; x = 0;}
		}
		//opcode
		//opcode_inf: opcode의 정보를 저장
		int opcode_inf = data[0]; char opcode_str[5];
		while(opcode_inf % 4 != 0) opcode_inf--;
		sprintf(opcode_str, "%02X", opcode_inf);
		//n, i, x(xx로 저장함), b, p, e
		int n,i,xx,b,p,e;
		n = (right[0] / 2) % 2;
		i = right[0] % 2;
		//주소 지정 방식
		int simpleadr = 0; int immediateadr = 0; int indirectadr = 0;
		if(n == 1 && i == 1) simpleadr = 1;
		else if(n == 1 && i == 0) indirectadr = 1;
		else if(n == 0 && i == 1) immediateadr = 1;
		else { }

		int pcrelative = 0; int baserelative = 0; int basexrelative = 0; int norelative = 0;
		//e,p,b,x
		e = left[1] % 2;
		p = (left[1] / 2) % 2;
		b = (left[1] / 2 / 2) % 2;
		xx = left[1] / 2 / 2 / 2;
		
		//format에 따라 PC count
		//CLEAR, COMPR, TIXR(format = 2)
		if(!strcmp(opcode_str, "B4") || !strcmp(opcode_str, "A0") || !strcmp(opcode_str, "B8")) {PC += 2;}
		else{
			//format = 4
			if(e == 1){
			PC += 4;  
		
			if(x >= line_size) data[3] = mem[y+1][0];
			else data[3] = mem[y][x];

			left[3] = data[3] / 16; right[3] = data[3] % 16; 
			}
			//format = 3
			else {PC += 3;}
		}
		//상대 주소 지정 방법 선택
		if(p == 1) pcrelative = 1;
		else if(b == 1){
			if(xx == 1) basexrelative = 1;
			else baserelative = 1;	
		}
		else norelative = 1;
		//상대 주소 계산
		int relativeadr;
		if(e == 0){relativeadr = right[1]*16*16 + data[2];}
		else{relativeadr = right[1]*16*16*16*16 + data[2]*16*16 + data[3];}
	
		if(right[1] >= 8){
			char adr_b[30];
			itoa(relativeadr, adr_b, 2);
			if(e) adr_b[20] = '\0'; else adr_b[12] = '\0';

			int i=0;
			for(i=0; i<strlen(adr_b); ++i){
				if(adr_b[i] == '0') adr_b[i] = '1';
				else adr_b[i] = '0';
			}
		int minus_adr = (int)strtol(adr_b, NULL, 2);
		minus_adr++;
		minus_adr *= -1;
		relativeadr = minus_adr;
		}

		//주소 계산
		int adr;
		if(pcrelative) adr = relativeadr + PC;
		else if(baserelative) adr = relativeadr + B;
		else if(basexrelative) adr = relativeadr + B + X;
		else adr = relativeadr;
		
		//OPCODE 실행
		/********************Load************************/
		//1. LDA 
		if(!strcmp(opcode_str, "00")){
			if(simpleadr){	
				int y = adr / line_size; int x = adr % line_size;
				int data[5];
				for(int k=0; k<3; ++k){
					data[k] = mem[y][x];
					x++;
					if( x >= line_size){ y++; x = 0;}
				}

				int finaldata = data[0] * 16 * 16 * 16 * 16 + data[1] * 16 * 16 + data[2];
				A = finaldata;
			}
			else if(immediateadr){
				A = adr;				
			}
		}

		//2. LDB
		if(!strcmp(opcode_str, "68")){
			if(simpleadr){
				int y = adr / line_size; int x = adr % line_size;
				int data[5];
				for(int k=0; k<3; ++k){
					data[k] = mem[y][x];
					x++;
					if( x >= line_size){ y++; x = 0;}
				}

				int finaldata = data[0] * 16 * 16 * 16 * 16 + data[1] * 16 * 16 + data[2];
				B = finaldata;
			}
			else if(immediateadr){
				B = adr;				
			}
		}

		//3. LDT
		if(!strcmp(opcode_str, "74")){
			if(simpleadr){
				int y = adr / line_size; int x = adr % line_size;
				int data[5];
				for(int k=0; k<3; ++k){
					data[k] = mem[y][x];
					x++;
					if( x >= line_size){ y++; x = 0;}
				}

				int finaldata = data[0] * 16 * 16 * 16 * 16 + data[1] * 16 * 16 + data[2];
				T = finaldata;
			}

			else if(immediateadr){
				T = adr;
			}
		}

		//4. LDCH
		if(!strcmp(opcode_str, "50")){
			A = mem[adr / line_size][adr % line_size];
		}
		/********************Load************************/
		
		/********************Store***********************/
		//1. STA
		if(!strcmp(opcode_str, "0C")){
			int data1, data2, data3;
			char datastr1[3]; char datastr2[3]; char datastr3[3];
			char datastr[10];
			sprintf(datastr, "%06X", A); datastr[6] = '\0';

			datastr1[0] = datastr[0]; datastr1[1] = datastr[1]; datastr1[2] = '\0';
			data1 = (int)strtol(datastr1, NULL, 16);

			datastr2[0] = datastr[2]; datastr2[1] = datastr[3]; datastr2[2] = '\0';
			data2 = (int)strtol(datastr2, NULL, 16);
			
			datastr3[0] = datastr[4]; datastr3[1] = datastr[5]; datastr1[2] = '\0';
			data3 = (int)strtol(datastr3, NULL, 16);

			edit(adr, data1);
			edit(adr + 1, data2);
			edit(adr + 2, data3);
		}
		
		//2. STX
		if(!strcmp(opcode_str, "10")){
			int data1, data2, data3;
			char datastr1[3]; char datastr2[3]; char datastr3[3];
			char datastr[10];
			sprintf(datastr, "%06X", X); datastr[6] = '\0';

			datastr1[0] = datastr[0]; datastr1[1] = datastr[1]; datastr1[2] = '\0';
			data1 = (int)strtol(datastr1, NULL, 16);

			datastr2[0] = datastr[2]; datastr2[1] = datastr[3]; datastr2[2] = '\0';
			data2 = (int)strtol(datastr2, NULL, 16);
			
			datastr3[0] = datastr[4]; datastr3[1] = datastr[5]; datastr1[2] = '\0';
			data3 = (int)strtol(datastr3, NULL, 16);

			edit(adr, data1);
			edit(adr + 1, data2);
			edit(adr + 2, data3);
		}

		//3. STL
		if(!strcmp(opcode_str, "14")){
			int data1, data2, data3;
			char datastr1[5]; char datastr2[5]; char datastr3[5];
			char datastr[10];
			sprintf(datastr, "%06X", L); datastr[6] = '\0';

			datastr1[0] = datastr[0]; datastr1[1] = datastr[1]; datastr1[2] = '\0';
			data1 = (int)strtol(datastr1, NULL, 16);

			datastr2[0] = datastr[2]; datastr2[1] = datastr[3]; datastr2[2] = '\0';
			data2 = (int)strtol(datastr2, NULL, 16);
			
			datastr3[0] = datastr[4]; datastr3[1] = datastr[5]; datastr1[2] = '\0';
			data3 = (int)strtol(datastr3, NULL, 16);

			edit(adr, data1);
			edit(adr + 1, data2);
			edit(adr + 2, data3);
		}

		/********************Store***********************/

		/********************Comp************************/
		//1. COMP
		//2. COMPR
		if(!strcmp(opcode_str, "A0")){
			CC = 0;
		}
		//3. TD
		if(!strcmp(opcode_str, "E0")){
			CC = -1;
		}
		//4. RD
		/********************Comp************************/

		/*******************CHANGE***********************/
		//1. TIXR
		if(!strcmp(opcode_str, "B8")){
			X++;
			//T: left[1] == 5 
			if(X < T) CC = -1;
			if(X == T) CC = 0;
		}
		
		//2. CLEAR
		if(!strcmp(opcode_str, "B4")){
			if(left[1] == 1) X = 0;
			else if(left[1] == 0) A = 0;
			else if(left[1] == 4) S = 0;
			else { }	
		}	
		/*******************CHANGE************************/
		
		/********************JUMP************************/
		//1. JSUB
		if(!strcmp(opcode_str, "48")){
			L = PC;
			PC = adr;
		}
		//2. JEQ
		if(!strcmp(opcode_str, "30")){
			if(CC == 0) PC = adr;
		}
		//3. J
		if(!strcmp(opcode_str, "3C")){
			if(indirectadr){
			    PC = mem[adr / line_size][adr % line_size] * 16 * 16 * 16 * 16 + mem[(adr + 1) / line_size][(adr + 1) % line_size] * 16 * 16 + mem[(adr + 2) / line_size][(adr + 2) % line_size];
			}
		       else PC = adr;
		}
		//4. JLT
		if(!strcmp(opcode_str, "38")){
			if(CC < 0) PC = adr;
		}
		//5. RSUB
		if(!strcmp(opcode_str, "4C")){
			PC = L;
		}
		/********************JUMP************************/
		
		//breaking point 만나면 break
		for(int b = 0; b < Bpcount; ++b){
			bp = (int)strtol(Breakingpoint[b], NULL, 16);
			if(bp == PC) {runEXECADDR = PC; actingflag = 1; break_flag = 1; break;}
		}
		if(break_flag) break;
	}


	printf("A: %06X  X: %06X\nL: %06X PC: %06X\nB: %06X  S: %06X\nT: %06X\n", A, X, L, PC, B, S, T);
	//ENDADDR에 PC가 아직 도달하지 않았을 경우
	if(PC < ENDADDR) printf("      Stop at checkpoint[%X]\n", bp);		
	//ENDADDR에 PC가 도달했을 경우
	else printf("      End Program\n");
}

void addbp(char string[]){
//breaking point를 추가하는 함수
	char datastr[10]; int data;
	data = (int)strtol(string, NULL, 16);
	sprintf(datastr, "%04X", data);
	datastr[4] = '\0';

	strcpy(Breakingpoint[Bpcount++], datastr);
	printf("      [ok] create breakpoint %X\n", data);
}

void printbp(void){
//지금까지 저장한 breaking point를 출력하는 함수
	printf("      breakpoint\n      ---------\n");
	for(int c = 0; c < Bpcount; ++c){
		int data = (int)strtol(Breakingpoint[c], NULL, 16);
		printf("      %X\n", data);
	}
}
void clearbp(void){
//지금까지 저장한 breaking point를 삭제하는 함수
	printf("      [ok] clear all breakpoints\n");
	for(int c = 0; c < Bpcount; ++c){Breakingpoint[c][0] = '\0';}
	Bpcount = 0;
}

void progaddr(char address[]){
//progaddr을 지정하는 함수
	PROGADDR = (int)strtol(address, NULL, 16);
}

void loader(int parametercount, char file[][20])
{
	totallength = 0;

	int CSN_sym_count[3];
	int EXTAB_SYM_FLAG = 0;

	//PASS1
	for(int f=0; f<parametercount; ++f){
		int line = 1;
		
		FILE *ip;
	
		int i = 0;
		while((file[f][i] >= 'a' && file[f][i] <= 'z') || file[f][i] == '.'){
			i++;
		}
		file[f][i] = '\0';	

		if(!(ip = fopen(file[f], "r"))){
		fputs("file open error\n", stderr);
		}
		//해당 이름을 가진 파일이 디렉토리에 존재할 경우
		else{
			char buffer[100];
			while(!feof(ip)){
				if(fgets(buffer, sizeof(buffer), ip) == NULL) {break;}
				else {
				//Header Record
				if(buffer[0] == 'H'){
					int blank_index = 0;
					for(int i=1; i<=6; ++i){
						int j = i - 1;
						if(buffer[i] == ' ') {blank_index = j; break;}
						else EXTAB_CSN[f][j] = buffer[i];
					}
					EXTAB_CSN[f][blank_index] = '\0';

					for(int i=7; i<=12; ++i){
						int j = i - 7;
						EXTAB_CSN_ADR[f][j] = buffer[i];
					}
					for(int i=13; i<=18; ++i){
						int j = i - 13;
						EXTAB_CSN_LEN[f][j] = buffer[i];
						}
					}
				//Define Record
				else if(buffer[0] == 'D'){
					EXTAB_SYM_FLAG = 1;
					CSN_sym_count[f] = ((int)strlen(buffer) - 2) / 12;
					int T = CSN_sym_count[f];

					int c = 0;
					int blank_index = 0;
					for(int t = 0; t < T; ++t){
						for(int i=1 + 12*t; i<=6 + 12*t; ++i){
							int j = i - (1 + 12*t);
							if(buffer[i] == ' ') {blank_index = j; break;}
							EXTAB_SYM[f][c][j] = buffer[i];
						}
						EXTAB_SYM[f][c][blank_index] = '\0';
	
						for(int i=7 + 12*t; i<=12 + 12*t; ++i){
							int j = i - (7 + 12*t);
							EXTAB_ADR[f][c][j] = buffer[i];
							}
						c++;
						}
					}
				}
			}
		fclose(ip);
		}
	}
	//가져온 정보를 이전에 저장한 PROGADDR에 알맞게 계산한다.
	for(int f=0; f<parametercount; ++f){	
	if(f == 0) sprintf(EXTAB_CSN_ADR[0], "%X", (int)strtol(EXTAB_CSN_ADR[0], NULL, 16) + PROGADDR);	
	else sprintf(EXTAB_CSN_ADR[f], "%X", (int)strtol(EXTAB_CSN_ADR[f-1], NULL, 16) + (int)strtol(EXTAB_CSN_LEN[f-1], NULL, 16) + (int)strtol(EXTAB_CSN[f], NULL, 16));
	}
	
	for(int f=0; f<parametercount; ++f){
		for(int c=0; c<CSN_sym_count[f]; ++c){
		sprintf(EXTAB_ADR[f][c], "%04X", (int)strtol(EXTAB_ADR[f][c], NULL, 16) + (int)strtol(EXTAB_CSN_ADR[f], NULL, 16));	
		}
	}
	//저장한 정보 출력
	printf("control symbol address length\n");
	printf("section name\n----------------------------------\n");

	int EXTAB_CSN_ADR_int = (int)strtol(EXTAB_CSN_ADR[0], NULL, 16);

	for(int i=0; i<parametercount; ++i)
	{
		if(parametercount > 1)
		printf("%-20s%-10s%-10s\n", EXTAB_CSN[i], EXTAB_CSN_ADR[i], EXTAB_CSN_LEN[i] + 2);
		else
		printf("%-20s  %04X    %-10s\n", EXTAB_CSN[i], EXTAB_CSN_ADR_int, EXTAB_CSN_LEN[i] + 2);
		if(EXTAB_SYM_FLAG){
			for(int j=0; j<2; ++j){
				printf("%15s%9s\n", EXTAB_SYM[i][j], EXTAB_ADR[i][j]);
			}
		}
	}

	for(int i=0; i<parametercount; ++i){
		int len = (int)strtol(EXTAB_CSN_LEN[i], NULL, 16);
		totallength += len;
	}
	printf("----------------------------------\n");
	printf("           total length: %04X\n", totallength);
	
	int CSADDR = PROGADDR;
	int EXECADDR = PROGADDR;
	
	//PASS2
	for(int f=0; f<parametercount; ++f){
		int line = 1;
			
		FILE *ip;
		if(!(ip = fopen(file[f], "r"))){
		fputs("file open error\n", stderr);
		}
		//해당 이름을 가진 파일이 존재할 경우
		else{
			char buffer[100];
			char ref[10][10];

			while(!feof(ip)){
				if(fgets(buffer, sizeof(buffer), ip) == NULL) {break;}
				else {
					//현재 다루고 있는 Control Section의 정보를 저장한다.
					int CS_code;
					char ProgramNameStr[10];

					//Header Record
					if(buffer[0] == 'H'){
						int blank_index;
						char ProgramNameStr[10];
						//Program 이름 저장
						for(int i=0; i<6; ++i){
							if(buffer[i+1] == ' ') {blank_index = i; break;}
							else ProgramNameStr[i] = buffer[i+1];
						}
						ProgramNameStr[blank_index] = '\0';
						strcpy(ref[1], ProgramNameStr);
						
						//현재 다루고 있는 Control Section 정보 저장
						for(int i=0; i<3; ++i){
							if(!strcmp(ProgramNameStr, EXTAB_CSN[i])) CS_code = i;
						}
						
						//현재 다루고 있는 Control Section의 주소 저장
						CSADDR = (int)strtol(EXTAB_CSN_ADR[CS_code], NULL, 16);
					}
					
					//Reference Record
					else if(buffer[0] == 'R'){
						strcpy(ref[1], ProgramNameStr);
						int i = 2; 	
						char *temp_ptr = strtok(buffer, " ");
						strcpy(ref[i++], temp_ptr + 3);
						ref[i - 1][strlen(ref[i-1])] = '\0'; 
						
						//token 하나하나에 접근한다.
						//Refer되는 Operand의 정보를 저장한다.
						while (temp_ptr != NULL) {
							temp_ptr = strtok(NULL, " ");
							if(temp_ptr == NULL) {ref[i-1][strlen(ref[i-1])-1] = '\0'; break;}
							strcpy(ref[i++], temp_ptr + 2);
							ref[i - 1][strlen(ref[i-1])] = '\0';
							}
						}				

					//Text Record	
					else if(buffer[0] == 'T'){
						
						char SpectifiedAddrStr[10]; int SpectifiedAddr;
						for(int i=1; i<=6; ++i) SpectifiedAddrStr[i-1] = buffer[i];			
						SpectifiedAddrStr[6] = '\0';
						SpectifiedAddr = (int)strtol(SpectifiedAddrStr, NULL, 16);
						
						char LineSizeStr[10]; int LineSize;
						for(int i=7; i<=8; ++i) LineSizeStr[i-7] = buffer[i];
						LineSizeStr[2] = '\0';
						LineSize = (int)strtol(LineSizeStr, NULL, 16);

						//Text Record에 있는 Object code들을 저장할 위치를 저장한다.
						EXECADDR = CSADDR + SpectifiedAddr;
						for(int i=9; i <= 9 + (LineSize * 2 - 1) - 1; i = i + 2){
							//Text Record의 Object code들을 적절한 위치에 load해준다.
							char unitdatastr[5]; int unitdata;
							unitdatastr[0] = buffer[i]; unitdatastr[1] = buffer[i+1]; unitdatastr[2] = '\0';
							unitdata = (int)strtol(unitdatastr, NULL, 16);
							edit(EXECADDR, unitdata);
							EXECADDR++;							
						} 
					}
					else if(buffer[0] == 'M'){
						
						//수정해야 할 주소 지정
						EXECADDR = (int)strtol(EXTAB_CSN_ADR[CS_code], NULL, 16);
						
						char RelativeAddrStr[10]; int RelativeAddr;
						for(int i=1; i<=6; ++i) RelativeAddrStr[i-1] = buffer[i];			
						RelativeAddrStr[6] = '\0';
						RelativeAddr = (int)strtol(RelativeAddrStr, NULL, 16);

						EXECADDR += RelativeAddr;
						
						//Replace = Operand(기존 mem에 저장된 값) + Operator(ex)LISTB)
						int Replace; char temp_Replace_Str[10]; char Replace_Str[10];
						//1. Operand
						char OperandStr[10]; int Operand; int OperandSize;
						if(buffer[8] == '5') OperandSize = 5;
						else if(buffer[8] == '6') OperandSize = 6;
						
						if(OperandSize == 5){
							int j=0;
							for(int i = EXECADDR; i < EXECADDR + 3; ++i){
								int data;
								if(i == EXECADDR) data = mem[i / line_size][i % line_size] % 16;
								else data = mem[i / line_size][i % line_size];
								
								char data_str[3];
								sprintf(data_str, "%02X", data);
								if(i == EXECADDR){OperandStr[j] = data_str[0]; j++;}
								else {OperandStr[j] = data_str[0]; OperandStr[j+1] = data_str[1]; j = j + 2;}
							}
							OperandStr[5] = '\0';
							Operand = (int)strtol(OperandStr, NULL, 16);
						}
						else if(OperandSize == 6){
							int j=0;
							for(int i = EXECADDR; i < EXECADDR + 3; ++i){
								int data = mem[i / line_size][i % line_size];
								char data_str[3];
								sprintf(data_str, "%02X", data);
								OperandStr[j] = data_str[0]; OperandStr[j+1] = data_str[1];
								j = j + 2; 
							}	
							OperandStr[6] = '\0';
							Operand = (int)strtol(OperandStr, NULL, 16);
						}

						//2. Operator						
						int plus_flag = 0, minus_flag = 0;
						if(buffer[9] == '+') plus_flag = 1;
						else minus_flag = 1;
						
						char REF_pos_str[5]; int REF_pos;
						REF_pos_str[0] = buffer[10]; REF_pos_str[1] = buffer[11]; REF_pos_str[2] = '\0';
						REF_pos = (int)strtol(REF_pos_str, NULL, 16);
						
						int code1, code2;
						int FIND = 0;
						int REFposIs1 = 0;

						//Operator의 정보를 찾아서 저장한다.
						for(code1 = 0; code1 < 3; ++code1){
							for(code2 = 0; code2 < 2; ++code2){
								if(!strcmp(ref[REF_pos], EXTAB_SYM[code1][code2])) {FIND = 1; break;}
								}
							if(FIND) break;
							}
			
						if(!strcmp(ref[REF_pos], ProgramNameStr)) {REFposIs1 = 1;}

						char OperatorStr[10]; int Operator;
						if(REFposIs1) strcpy(OperatorStr, EXTAB_CSN_ADR[CS_code]);
						else strcpy(OperatorStr, EXTAB_ADR[code1][code2]);
						Operator = (int)strtol(OperatorStr, NULL, 16);
	
						//지금까지 과정으로 구한 Operand와 Operator을 더하거나 뺌으로써 대체해야할 값을 계산한다.
						if(plus_flag) Replace = Operand + Operator;
						if(minus_flag) Replace = Operand - Operator;
							if(OperandSize == 5){
								sprintf(temp_Replace_Str, "%010X", Replace);
								strcpy(Replace_Str, temp_Replace_Str + 5);
								int Str_data = (int)strtol(Replace_Str, NULL, 16);
								sprintf(Replace_Str, "%05X", Str_data);
								Replace_Str[5] = '\0';
							}
							if(OperandSize == 6){
								sprintf(temp_Replace_Str, "%010X", Replace);
								strcpy(Replace_Str, temp_Replace_Str + 4);
								int Str_data = (int)strtol(Replace_Str, NULL, 16);
								sprintf(Replace_Str, "%06X", Str_data);
								Replace_Str[6] = '\0';
							}
	
						//새로운 data(Replace)을 load한다.
						if(OperandSize == 5){
							int j = 0;
							for(int i = EXECADDR; i < EXECADDR + 3; ++i){
								
								char newdata_Str[3]; 
								if(i == EXECADDR){
									newdata_Str[0] = Replace_Str[j]; newdata_Str[1] = '\0';
									j++;
								}
								else{
									newdata_Str[0] = Replace_Str[j]; newdata_Str[1] = Replace_Str[j+1]; 
									newdata_Str[2] = '\0';
									j = j + 2;
								}
								int newdata = 0;

								if(i == EXECADDR){ 
									int olddata = mem[i / line_size][i % line_size];
									newdata = (int)strtol(newdata_Str, NULL, 16) + olddata - olddata %  line_size;
									edit(i, newdata);
								}
								else {newdata = (int)strtol(newdata_Str, NULL, 16); edit(i, newdata);}
							}
						}
	
						else if(OperandSize == 6){
							int j = 0;
							for(int i = EXECADDR; i < EXECADDR + 3; ++i){
								char newdata_Str[3]; 
								newdata_Str[j] = Replace_Str[j]; newdata_Str[j+1] = Replace_Str[j+1]; 
								newdata_Str[j+2] = '\0';
								int newdata = (int)strtol(newdata_Str, NULL, 16);
								edit(i, newdata);
								j = j + 2;
							}	
						}						
					}	
				}
			}
		fclose(ip);
		}
	}
	//EXTAB 초기화
	for(int i=0; i<3; ++i) {EXTAB_CSN[i][0] = '\0'; EXTAB_CSN_ADR[i][0] = '\0'; EXTAB_CSN_LEN[i][0] = '\0';}

	for(int i=0; i<3; ++i){
		for(int j=0; j<10; ++j){
			EXTAB_SYM[i][j][0] = '\0';
			EXTAB_ADR[i][j][0] = '\0';
		}
	}
}

void symbol(void)
{
	
	SYMTAB* tempsymtab = (SYMTAB*)malloc(sizeof(SYMTAB));
	tempsymtab = symtab->next;
	
	char str1[100][100]; int loc[100];
	int i=0, j; char temp[100]; int temp_loc;
	//symtab의 정보를 str1(LABEL의 이름), loc(위치)에 옮긴다.
	while(tempsymtab != NULL){
			strcpy(str1[i], tempsymtab->name);
			loc[i++] = tempsymtab->loc;
			tempsymtab = tempsymtab->next;
		}
	int count = i;
	//알파벳 순으로 정렬
	for(i=0;i<count;i++)
    	  for(j=i+1;j<count;j++){
    	     if(strcmp(str1[i],str1[j])>0){
    	        strcpy(temp,str1[i]);
    	        strcpy(str1[i],str1[j]);
    	        strcpy(str1[j],temp);
		
		temp_loc = loc[i];
		loc[i] = loc[j];
		loc[j] = temp_loc;
    	     }
    	  }
	for(i=0; i < count; i++){
		printf("       %-10s %04X\n", str1[i], loc[i]);
		}
	
}
void type(char* filename)
{
	filename[strlen(filename)] = '\0';
	
	FILE* ip;
	//현재 디렉토리에 filename의 이름을 가진 file이 존재하지 않을 경우
	if(!(ip = fopen(filename, "r"))){
		fputs("file open error\n", stderr);
	}
	//존재할 경우
	else{
	char buffer[100];
	while(!feof(ip)){
		if(fgets(buffer, sizeof(buffer), ip) == NULL) {break;}
		else printf("%s", buffer);
		}
	fclose(ip);
	}
}
		
int assemble(char* filename)
{
	//이전에 이미 assemble한 파일이 있을 경우 symtab이 존재한다.
	//따라서 free해준다.
	if(symtabEXIST) free(symtab);

	symtab = malloc(sizeof(SYMTAB));
	symtab->next = NULL;
	//파일명 생성
	//op1_filename: filename.lst
	//op2_filename: filename.obj
	char temp_filename[100]; char op1_filename[100]; char op2_filename[100];
	char op1_str[10] = {"lst"}; char op2_str[10] = {"obj"};
	
	strcpy(temp_filename, filename);
	temp_filename[strlen(temp_filename) - 3] = '\0';

	strcpy(op1_filename, temp_filename);
	strcpy(op2_filename, temp_filename);

	strcat(op1_filename, op1_str);
	strcat(op2_filename, op2_str);
	
	op1_filename[strlen(op1_filename)] = '\0';
	op2_filename[strlen(op1_filename)] = '\0';
	
	//ip: *.asm을 다루는 file pointer
	FILE* ip;
	if(!(ip = fopen(filename, "r"))){
		fputs("file open error\n", stderr);
		fclose(ip);
		return 0;
	}
	//op1: *.lst를 다루는 file pointer
	FILE* op1 = fopen(op1_filename, "w");
	//op2: *.obj를 다루는 file pointer
	FILE* op2 = fopen(op2_filename, "w");
	
	//string(pass 과정에서 읽히는 한 문장)을 다루는 parameter
	char par1[100]; char par2[100]; char par3[100];
	char temp_LABEL[100]; char temp_OPCODE[100]; char temp_OPERAND[100];
	char temp_OPERAND1[100]; char temp_OPERAND2[100];
	//locctr 다루는 변수	
	int starting_address = 1; int starting_address_flag = 0;
	int line_number; int locctr;
	//string(pass 과정에서 읽는 한 문장)을 저장하는 공간 
	char buffer[100]; char temp_buffer[100];
	//각 line의 locctr
	int line_locctr[1000];
	//임시변수들
	char temp[100];
	//Header Record	에 쓰이는 변수들
	char program_name[6];
	char starting_address_str[6];
	char program_size[6];
	//Text Record에 쓰이는 변수들
	char ObjectCodeStr[61];
	ObjectCodeStr[0] = '\0';
	int Start_str;
	//Modified Record에 쓰이는 변수들
	char Modified_Start[100][100];
	char Modified_Size[100][100];
	int m=0; 
	//pass1
	//read first input line
	fgets(buffer, sizeof(buffer), ip);
	char *ptr = strtok(buffer, " ");
	//program의 정보 저장. Header Record에 쓰인다.
	strcpy(program_name, ptr);
	//assemble하는 도중 ERROR가 발생할 경우 FLAG = 1
	int ERROR_FLAG = 0;
	//Header Record의 프로그램 이름 저장
	int name_size = strlen(program_name);
	program_name[6] = '\0';
	for(int i = name_size; i<6; ++i) program_name[i] = ' ';
	//program의 시작 주소 찾기
	while (ptr != NULL) {
		strcpy(temp, ptr);
		//첫 줄에 시작 주소가 지정되어 있을 경우 
		if (starting_address_flag) starting_address = strtol(temp, NULL, 16);
		if (!strcmp("START", temp)) {
			starting_address_flag = 1;
		}		
		ptr = strtok(NULL, " ");
	}
	//첫 줄에 시작 주소가 지정되어 있지 않을 경우
	if (!starting_address_flag) starting_address = 0;
	//pass1에 쓰일 변수들 초기화
	line_locctr[0] = starting_address;
	Start_str = starting_address;
	locctr = starting_address;
	line_number = 1;
	//program의 시작 주소 저장
	sprintf(starting_address_str, "%06x", starting_address);
	//한 줄의 parameter의 개수
	int token_count = 0;
	//base의 주소 저장하는 string
	char base_address_key[10];
	//pass1
	while (1)
	{
		int flag = 0;
		fgets(buffer, sizeof(buffer), ip);
		
		strcpy(temp_buffer, buffer);
		
		//공백을 기준으로 문장을 나눈다.
		line_locctr[line_number++] = locctr;
		char *temp_ptr = strtok(temp_buffer, " ");
		//token 하나하나에 접근한다.
		while (temp_ptr != NULL) {
			temp_ptr = strtok(NULL, " ");
			token_count++;
		}

		//token_count가 n개일 때: n 덩어리
		char *ptr = strtok(buffer, " ");
		
		if (token_count == 3) {
			
			strcpy(par1, ptr);
			ptr = strtok(NULL, " ");
			strcpy(par2, ptr);
			ptr = strtok(NULL, " ");
			strcpy(par3, ptr);
			//두 번째 parameter의 마지막에 ','이 들어갈 경우
			//ex) STCH BUFFER, X
			if(par2[strlen(par2) - 1] == ','){
				strcpy(temp_OPCODE, par1);
				strcpy(temp_OPERAND1, par2);
				strcpy(temp_OPERAND2, par3); 
				}
			else{
				strcpy(temp_LABEL, par1);
				strcpy(temp_OPCODE, par2);
				strcpy(temp_OPERAND, par3);
				//Label일 경우 Symtab에 저장
				//Symtab에 이미 해당 Label의 정보가 저장되어 있을 경우
				if(FindSymtab(temp_LABEL) != NotInSymtab){
					printf("Line %04d: ERROR - 변수명이 중복으로 사용되었음.\n", line_number*5);
					ERROR_FLAG = 1;
					break;
				}
				//저장되어 있지 않을 경우 Label의 정보를  새로 추가해준다
				else AddSymtab(locctr, temp_LABEL); 
				}		
		}

		else if (token_count == 2) {
			strcpy(temp_OPCODE, ptr);
			ptr = strtok(NULL, " ");
			strcpy(temp_OPERAND, ptr);

		}
		else {
			//ex) . SUBROUTINE TO ...
			if (token_count > 3) flag = 1;
			else if (token_count == 1) {
				strcpy(temp_OPERAND, ptr);
				//ex) .
				if (!strcmp(".\n", temp_OPERAND)) flag = 1;
				else {
					//RSUB
				}
			}
		}
		//OPCODE에 따라 적절하게 locctr을 update한다.
		if (!strcmp("WORD", temp_OPCODE))
		{
			locctr += 3;
		}
		else if (!strcmp("RESW", temp_OPCODE))
		{
			//-1하는 이유는 '\0'을 고려하기 때문이다
			locctr += 3 * (strlen(temp_OPERAND) - 1);
		}
		else if (!strcmp("RESB", temp_OPCODE))
		{
			locctr += strtol(temp_OPERAND, NULL, 10);
		}
		else if (!strcmp("BYTE", temp_OPCODE))
		{
			//-1하는 이유는 '\0'을 고려하기 때문이다
			if(temp_OPERAND[0] == 'C') locctr += (strlen(temp_OPERAND) - 4);
			else if(temp_OPERAND[0] == 'X') locctr += (strlen(temp_OPERAND) - 4) / 2;
		}
		else if (!strcmp("BASE", temp_OPCODE))
		{
			temp_OPERAND[strlen(temp_OPERAND) - 1] = '\0';
			strcpy(base_address_key, temp_OPERAND);
		}
		//flag = 1인 상황에는 locctr을 더해줄 필요가 없다.
		else if(flag == 1){
			flag = 0;
		}
		else
		{	
			if (opcode_format(temp_OPCODE) == 0){
				if(temp_OPCODE[0] == '+') locctr += 4;
				}
			else if (opcode_format(temp_OPCODE) == 1){locctr += 1;}
			else if (opcode_format(temp_OPCODE) == 2){locctr += 2;}
			else if (opcode_format(temp_OPCODE) == 3){locctr += 3;}
		}
		//OPCODE에 END가 입력될 경우 PASS1 종료
		if(!strcmp("END", temp_OPCODE)){
			sprintf(program_size, "%06X", locctr);
			break;
			}
		//다음 buffer을 위하여 token_count는 0으로 초기화
		token_count = 0;
	}
	fclose(ip);
	//symtab을 만들었을 경우 symtabEXIST = 1
	symtabEXIST = 1;
	//Header Record: 프로그램 이름 program_name (3byte) + 프로그램의 시작주소starting_address_str (6byte) + 프로그램의 길이program_size (6byte)
	fprintf(op2, "H%s%s%s\n", program_name, starting_address_str, program_size);

	FILE* ip2 = fopen(filename, "r");
	//string의 parameter
	//pass2에서 현재 다루고 있는 line
	int presentline = 1;
	//Base Relative로 상대 주소를 지정할 때 base의 주소
	int base_address = FindSymtab(base_address_key);
	//.lst파일 작성
	fgets(buffer, sizeof(buffer), ip2);
	fprintf(op1, "%-4d %04X %s", 5, line_locctr[0], buffer);
	//Text Record 저장 관련 flag
	int FindStart_str = 0;
	//입력된 TEMP_OPCODE가 RESB나 RESW인 경우 FLAG = 1
	int OPCODE_RESB_RESW_FLAG = 0;
	//pass2
	
	while(1){
		//pass1에서 ERROR가 발생했을 경우
		if(ERROR_FLAG) break;

		int token_count = 0;
		//pass2에서 다룰 필요가 없는 문장을 입력했을 경우 해당 flag = 1
		int toNextFlag = 0;
		//X Register을 사용하고 Base Realtive일 경우flag = 1
		int BaseRelativeFlagwithX = 0;
		//Base Relative일 경우 flag = 1
		//int BaseRelativeFlag = 0;
		//operand가 없을 경우 flag = 1
		int NoOperandFlag = 0;

		fgets(buffer, sizeof(buffer), ip2);
		buffer[strlen(buffer) - 1] = '\0';
		//.lst 작성
		//line이 넘겨져도 locctr이 증가하지 않는 경우
		if(line_locctr[presentline] == line_locctr[presentline + 1]) fprintf(op1, "%-4d     %s", (presentline+1) * 5, buffer);
		else{ 
			if(presentline != line_number-1) fprintf(op1, "%-4d %04X %-30s", (presentline+1) * 5, line_locctr[presentline], buffer);
		}
		//line_number는 pass1의 끝의 locctr, 즉 .asm의 마지막 loine의 locctr을 가지고 있다.
		if(presentline == line_number-1) fprintf(op1, "%-4d      %s\n", (presentline+1)*5, buffer);
		
		strcpy(temp_buffer, buffer);
		
		//공백을 기준으로 문장을 나눈다.
		char *temp_ptr = strtok(temp_buffer, " ");
		//단어 하나하나에 접근한다.
		while (temp_ptr != NULL) {
			temp_ptr = strtok(NULL, " ");
			token_count++;
		}
		//token_count가 n개일 때: n 덩어리
		char *ptr = strtok(buffer, " ");
		if (token_count == 3) {

			strcpy(par1, ptr);
			ptr = strtok(NULL, " ");
			strcpy(par2, ptr);
			ptr = strtok(NULL, " ");
			strcpy(par3, ptr);
			
			//두 번째 parameter의 마지막에 ','이 들어갈 경우
			//ex) STCH BUFFER, X
			if(par2[strlen(par2) - 1] == ','){
				strcpy(temp_OPCODE, par1);
				strcpy(temp_OPERAND1, par2);
				temp_OPERAND1[strlen(temp_OPERAND1) - 1] = '\0';
				strcpy(temp_OPERAND2, par3);
				if(temp_OPERAND1[0] == 'X' || temp_OPERAND2[0] == 'X'){
					BaseRelativeFlagwithX= 1;
					} 
				}
			else{
				strcpy(temp_LABEL, par1);
				strcpy(temp_OPCODE, par2);
				strcpy(temp_OPERAND, par3); 
		
			}	

			if(!strcmp(temp_OPCODE, "RESW") || !strcmp(temp_OPCODE, "RESB") || !strcmp(temp_OPCODE, "BASE")) toNextFlag = 1;
		}

		else if (token_count == 2) {
			strcpy(temp_OPCODE, ptr);
			ptr = strtok(NULL, " ");
			strcpy(temp_OPERAND, ptr);
		}
		else {
			if (token_count > 3) {toNextFlag = 1;}
			else if (token_count == 1) {
				strcpy(temp_OPCODE, ptr);
				if (!strcmp(".", temp_OPCODE)) { toNextFlag = 1;}
				else {
					//RSUB
					NoOperandFlag = 1;
				}
			}	
		}
		if(!strcmp(temp_OPCODE, "RESW") || !strcmp(temp_OPCODE, "RESB") || !strcmp(temp_OPCODE, "BASE")) 
		{toNextFlag = 1;
		}
	//OPCODE에 END가 입력될 경우 PASS2 종료
	if (!strcmp(temp_OPCODE, "END")) {

			//Text Record 출력
			fprintf(op2,"T%06X%02X%s\n", Start_str, (int)strlen(ObjectCodeStr)/2, ObjectCodeStr);
			OPCODE_RESB_RESW_FLAG = 1;
			//Modified Record 출력
			for(int M=0; M<m; ++M){
				fprintf(op2, "M%s%s\n", Modified_Start[M], Modified_Size[M]);
			}
			//End Record 출력
			fprintf(op2, "E%06X\n", line_locctr[1]);
			break;
		}
	//objectcode를 생성할 필요가 없는 line일 경우
	if(toNextFlag) {
		fprintf(op1, "\n");
		presentline++;
		//Text Record 출력
		if((!strcmp(temp_OPCODE, "RESB") || !strcmp(temp_OPCODE, "RESW")) && OPCODE_RESB_RESW_FLAG == 0){
			fprintf(op2, "T%06X%02X%s\n", Start_str, (int)strlen(ObjectCodeStr)/2, ObjectCodeStr);
			memset(ObjectCodeStr, 0, sizeof(ObjectCodeStr));
			FindStart_str = 1;
			OPCODE_RESB_RESW_FLAG = 1;
		}	
		continue;
	}
	//flag 초기화
	OPCODE_RESB_RESW_FLAG = 0;

	char objectcode1_B[10]; //opcode(2진수)
	char objectcode1_X[10];

	char objectcode2_B[10]; //nixbpe(2진수)
	//objectcode1_B와 objectcode2_B가 합해진 string(Binary)
	char objectcode12_B[20];
	//objectcode12_B가 16진수로 변환된 것
	char objectcode12_X[20];

	char objectcode3_B[12]; //displayment or address(2진수)
	//objectcode1,2,3이 합해진 string
	char objectcode123_B[50];
	//objectcode123_B가 16진수로 변환된 것
	char objectcode123_X[20];

	int opcode_data;

	//Findstart_str: RESB || RESW 이후 TextRecord의 Starting Address를 찾는 중	
	if(FindStart_str){
		Start_str = line_locctr[presentline];
		FindStart_str = 0;
	}

	/******************objectcode1_B 입력 과정**********************/
	//objectcode1_B: opcode 정보(6bit)
	if(temp_OPCODE[0] == '+') opcode_data = opcode(temp_OPCODE + 1);
	else opcode_data = opcode(temp_OPCODE); //14(X)
	//ERROR: 정의되지 않은 instruction이 사용되었을 경우	
	if(opcode_data == ERROR_WrongOpcode && !(!strcmp("BYTE", temp_OPCODE)) && !(!strcmp("RESW", temp_OPCODE)) && !(!strcmp("RESB", temp_OPCODE))){
		printf("Line %04d: ERROR - 정의되지 않은 instruction이 사용되었음.\n", (presentline + 1)*5);
		ERROR_FLAG = 1;
		break;
	}
	//ex)opcode_data = 20일 경우 objectcode1_B = {"10100"} 
	itoa(opcode_data, objectcode1_B, 2);
	//objectcode의 형식에 맞게 0을 채워준다.
	//ex)10100 -> 000 10100
	int Z = 8 - strlen(objectcode1_B);
	char Zero[13][20] = { "", "0", "00", "000","0000","00000","000000","0000000", "00000000", "000000000", "0000000000", "00000000000", "000000000000"};
	char zero[13][20] = { "", "0", "00", "000","0000","00000","000000","0000000", "00000000", "000000000", "0000000000", "00000000000", "000000000000" };

	strcat(zero[Z], objectcode1_B);
	strcpy(objectcode1_B, zero[Z]);

	int format = opcode_format(temp_OPCODE);
	//opcode의 정보 8bit에서 마지막 2bit는 삭제한다.
	if(format != 2)	objectcode1_B[strlen(objectcode1_B) - 2] = '\0';
	
	strcpy(zero[Z], Zero[Z]);
	strcpy(objectcode12_B, objectcode1_B);
	/****************************************************************/

	/******************objectcode2_B 입력 과정**********************/
	//opcode format이 2일 때 objectcode2_B: Register의 정보
	//opcode format이 3,4일 때 objectcode2_B: n, i, x, b, p, e(6bit)
	
	if(format == 1){
		presentline++;
		sprintf(objectcode1_X, "%X", (int)strtol(objectcode1_B, NULL, 2));
		fprintf(op1, "%s\n", objectcode1_X);
	}
	//opcode가 다루는 format이 2일 경우
	else if(format == 2){
		presentline++;
		//ex)CLEAR X
		if(!strcmp("CLEAR", temp_OPCODE) || !strcmp("TIXR", temp_OPCODE)){
			//register의 정보
			int data = regis_to_data(temp_OPERAND[0]);
		
		itoa(data, objectcode2_B, 2);
		Z = 4 - strlen(objectcode2_B);
		strcat(zero[Z], objectcode2_B);
		strcpy(objectcode2_B, zero[Z]);
		strcpy(zero[Z], Zero[Z]);
		strcat(objectcode2_B, zero[4]);
		}
		//ex)COMPR A,S
		else{
			int data1 = regis_to_data(temp_OPERAND1[0]);
			int data2 = regis_to_data(temp_OPERAND2[0]);
			//첫번째 register의 정보
			itoa(data1, objectcode2_B, 2);
			Z = 4 - strlen(objectcode2_B);
			strcat(zero[Z], objectcode2_B);
			strcpy(objectcode2_B, zero[Z]);
			strcpy(zero[Z], Zero[Z]);
			
			//두번째 register의 정보
			char objectcode2_B2[10];
			itoa(data2, objectcode2_B2, 2);
			Z = 4 - strlen(objectcode2_B2);
			strcat(zero[Z], objectcode2_B2);
			strcpy(objectcode2_B2, zero[Z]);
			strcpy(zero[Z], Zero[Z]);

			strcat(objectcode2_B, objectcode2_B2);
		}
	
	strcat(objectcode12_B, objectcode2_B);
	sprintf(objectcode12_X, "%X", (int)strtol(objectcode12_B, NULL, 2));	
	//Text Record 작성
	fprintf(op1, "%s\n", objectcode12_X);	
	}
	//opcode format이 3,4일 경우
	else{

	int n=0, i=1, x=2, b=3, p=4, e=5;
	//Operand의 앞에 #가 불을 경우(Immediate Addressing) 
	if(temp_OPERAND[0] == '#'){objectcode2_B[n] = '0'; objectcode2_B[i] = '1';}
	//Operand의 앞에 @가 붙을 경우(Indirect Addressing)
	else if(temp_OPERAND[0] == '@'){objectcode2_B[n] = '1'; objectcode2_B[i] = '0';}
	//Simple Addressing
	else {objectcode2_B[n] = '1'; objectcode2_B[i] = '1';}
	//X register을 이용하고, Base Relative로 상대 주소를 지정할 경우
	if(BaseRelativeFlagwithX){
		objectcode2_B[x] = '1';
		objectcode2_B[b] = '1';
		objectcode2_B[p] = '0';
	}
	//Program Counter로 상대 주소를 지정할 경우
	else{
		objectcode2_B[x] = '0'; 
		objectcode2_B[b] = '0'; 
		objectcode2_B[p] = '1'; 
	}
	//opcode format이 4일 경우
	if(temp_OPCODE[0] == '+'){objectcode2_B[e] = '1'; objectcode2_B[p] = '0';}
	else objectcode2_B[e] = '0';
	//RSUB
	if(NoOperandFlag) {
		objectcode2_B[n] = '1'; objectcode2_B[i] = '1'; objectcode2_B[x] = '0'; objectcode2_B[b] = '0'; objectcode2_B[p] = '0'; objectcode2_B[e] = '0';
	}

	/***************************************************************/

	/******************objectcode3_B 입력 과정**********************/
	//objectcode3_B: 주소를 저장(format 3: 12bit format 4: 20bit)
	
	//presentline의 다음 line의 Program Counter
	int PC = line_locctr[presentline++ + 1];
	//operand의 주소 저장하는 변수
	int operand_address; 
	//immediate addressing일 경우 1
	int immediate = 0;

	//Simple Addressing 일 경우
	if(objectcode2_B[n] == '1' && objectcode2_B[i] == '1'){
		operand_address = FindSymtab(temp_OPERAND);
		//정의되지 않은 변수명이 사용되었을 경우
		if(!(!strcmp(temp_OPCODE, "BYTE"))){
			if(FindSymtab(temp_OPERAND) == NotInSymtab){
				printf("Line %d: ERROR - 정의되지 않은 변수명이 사용되었음. %s\n", presentline*5, temp_OPERAND);
				ERROR_FLAG = 1;
				break;
			}
		}
	}
	else {
		//Immediate Addressing일 경우
		if(temp_OPERAND[0] == '#' && FindSymtab(temp_OPERAND + 1) == NotInSymtab) {operand_address = strtol(temp_OPERAND+1, NULL, 10); objectcode2_B[p] = '0'; immediate = 1;}
		else {operand_address = FindSymtab(temp_OPERAND + 1);
		     
		//정의되지 않은 변수명이 사용되었을 경우
		if(FindSymtab(temp_OPERAND + 1) == NotInSymtab){
			printf("Line %d: ERROR - 정의되지 않은 변수명이 사용되었음.\n", (presentline - 1) * 5);
			ERROR_FLAG = 1;
			break;
			}
		}
	}
	//X register을 사용하고 Base Relative로 주소를 지정하는 경우
	if(BaseRelativeFlagwithX) {
			if(temp_OPERAND2[0] == 'X')
			operand_address = FindSymtab(temp_OPERAND1);
			else
			operand_address = FindSymtab(temp_OPERAND2);
			//정의되지 않은 변수명이 사용되었을 경우
		       if(FindSymtab(temp_OPERAND1) == NotInSymtab){
			printf("Line %d: ERROR - 정의되지 않은 변수명이 사용되었음.\n", (presentline - 1) * 5);
			ERROR_FLAG = 1;
			break;
			}
		}


	//상대주소 저장
	int address;
	//Base Relative로 주소를 저장하는 경우(이 때는 X register를 사용하는 경우)
	if(BaseRelativeFlagwithX) address = operand_address - base_address;
	//operand가 없을 경우 ex)RSUB
	else if(NoOperandFlag) address = 0;
	//Program Counter Relative로 주소를 저장하는 경우
	else address = operand_address - PC;
	//Base Relative로 주소를 저장하는 경우
	if((address < -2048 || address > 2047) && objectcode2_B[e] == '0')
	{
		//BaseRelativeFlag = 1;
		//Base Relative에 맞게 objectcode2_B 변환
		objectcode2_B[x] = '0'; objectcode2_B[b] = '1'; objectcode2_B[p] = '0';
		address = operand_address - base_address;
	}
	objectcode2_B[6] = '\0';

	strcat(objectcode12_B, objectcode2_B);
	strcpy(objectcode123_B, objectcode12_B);
	
	if(objectcode2_B[e] == '1'){
		//CASE: immediate addressing && format 4
		if(immediate){
			address = operand_address;
			itoa(address, objectcode3_B, 2);

			Z = 20 - strlen(objectcode3_B);
			strcat(zero[Z], objectcode3_B);
			strcpy(objectcode3_B, zero[Z]);
			strcpy(zero[Z], Zero[Z]);
		}
		//CASE: format 4
		else{
			address = operand_address;
			itoa(address, objectcode3_B, 2);
			//Modified Record 생성
			int M_start = line_locctr[presentline - 1] + (strlen(objectcode12_B)+ 1) / 8;
		
			Z = 20 - strlen(objectcode3_B);
			strcat(zero[Z], objectcode3_B);
			strcpy(objectcode3_B, zero[Z]);
			strcpy(zero[Z], Zero[Z]);

			//Modified Record 저장	
			int M_size = strlen(objectcode3_B) / 4;
			
			char M_start_str[100]; char M_size_str[100];
			sprintf(M_start_str, "%06X", M_start);
			sprintf(M_size_str, "%02X", M_size);
			
			strcpy(Modified_Start[m], M_start_str);
			strcpy(Modified_Size[m++], M_size_str);
		}
	}
		
	//CASE: format3 && immediate addressing
	else if(immediate){
		if(objectcode2_B[e] == '0'){
			address = operand_address;
			itoa(address, objectcode3_B, 2);
			
			Z = 12 - strlen(objectcode3_B);
			strcat(zero[Z], objectcode3_B);
			strcpy(objectcode3_B, zero[Z]);
			strcpy(zero[Z], Zero[Z]);
			}
	}

	else{
		//CASE: relative address >= 0
		if(address >= 0){
			itoa(address, objectcode3_B, 2);
	
			Z = 12 - strlen(objectcode3_B);
			strcat(zero[Z], objectcode3_B);
			strcpy(objectcode3_B, zero[Z]);
			strcpy(zero[Z], Zero[Z]);
			}
		//CASE: relative address < 0
		else{
			address *= -1;
			itoa(address, objectcode3_B, 2);
			
			Z = 12 - strlen(objectcode3_B);
			strcat(zero[Z], objectcode3_B);
			strcpy(objectcode3_B, zero[Z]);
			strcpy(zero[Z], zero[Z]);
		
			for (int i = 0; i < strlen(objectcode3_B); ++i){
				if (objectcode3_B[i] == '0') objectcode3_B[i] = '1';
				else objectcode3_B[i] = '0';
			}

			int converted = strtol(objectcode3_B, NULL, 2);
			converted++;
			itoa(converted, objectcode3_B, 2);
			}
	}

	strcat(objectcode123_B, objectcode3_B);
	sprintf(objectcode123_X, "%06X", (int)strtol(objectcode123_B, NULL, 2));	
	/***************************************************************/
	
	//OPCODE가 BYTE일 경우
	if(!strcmp(temp_OPCODE, "BYTE")){
		memset(objectcode123_X, 0, sizeof(objectcode123_X));

		char buffer[100]; char temp[100];
		//입력받는 DATA의 TYPE을 고려하여 buffer에 objectcode123 저장	
		if(temp_OPERAND[0] == 'X'){
			for(int i = 2; i < strlen(temp_OPERAND) - 1; ++i){
				buffer[i-2] = temp_OPERAND[i];
			}
		}
		else if(temp_OPERAND[0] == 'C'){
			for (int i = 2; i < strlen(temp_OPERAND) - 1; ++i) {
				int k = temp_OPERAND[i];
				itoa(k, temp, 16);
					if (i == 2) strcpy(buffer, temp);
					else strcat(buffer, temp);
				memset(temp, 0, sizeof(temp));
			}
		}
		strcpy(objectcode123_X, buffer);
		for(int i=0; i<strlen(objectcode123_X); i++){
			if(objectcode123_X[i] >= 'a' && objectcode123_X[i] <= 'z')
				objectcode123_X[i] = (objectcode123_X[i] - 'a') + 'A';
				}
		memset(buffer, 0, sizeof(buffer));
		}
	//Text Record 저장
	fprintf(op1, "%s\n", objectcode123_X);	
	}	
	//Text Record 저장
	//format 2가 아닐 경우 format = 0일 때 4형식이다.
	if(format == 3 || format == 0){
		//Text Record의 size가 기준을 넘지 않았을 경우
		if(strlen(ObjectCodeStr) + strlen(objectcode123_X) <= 60){
		//ObjectCodeStr(Text Record에 들어가 있는 Objectcode)에 생성된 objectcode를 저장한다.
		strcat(ObjectCodeStr, objectcode123_X);
		}
		else{
			//Text Record의 size가 기준을 넘을 경우
			//기존의 Text Record를 출력한다.
			//Text Record: 시작 주소: Start_str(3byte) + objectcode의 길이 (int)strlen(ObjectCodeStr)/2(2byte) + objectcode(30byte)
			fprintf(op2, "T%06X%02X%s\n", Start_str, (int)strlen(ObjectCodeStr)/2, ObjectCodeStr);
			//ObjectCodeStr 초기화
			memset(ObjectCodeStr, 0, sizeof(ObjectCodeStr));
			strcat(ObjectCodeStr, objectcode123_X);
			//Start_str 초기화(Text Record의 시작 주소)
			Start_str = line_locctr[presentline - 1];
		}
	}
	//format 2일 경우
	else if(format == 2){
		//line 938 ~ 946과 설명 동일
		if(strlen(ObjectCodeStr) + strlen(objectcode12_X) <= 60 && !(!strcmp("RESB", temp_OPERAND) || !strcmp("RESW", temp_OPERAND))){
		strcat(ObjectCodeStr, objectcode12_X);
		}

		else{
			fprintf(op2, "T%06X%02X%s\n", Start_str, (int)strlen(ObjectCodeStr)/2, ObjectCodeStr);
			memset(ObjectCodeStr, 0, sizeof(ObjectCodeStr));
			strcat(ObjectCodeStr, objectcode12_X);

			Start_str = line_locctr[presentline - 1];		
		}
	}
	
	else{
		//line 938 ~ 946과 설명 동일
		if(strlen(ObjectCodeStr) + strlen(objectcode1_X) <= 60 && !(!strcmp("RESB", temp_OPERAND) || !strcmp("RESW", temp_OPERAND))){
		strcat(ObjectCodeStr, objectcode1_X);
		}

		else{
			fprintf(op2, "T%06X%02X%s\n", Start_str, (int)strlen(ObjectCodeStr)/2, ObjectCodeStr);
			memset(ObjectCodeStr, 0, sizeof(ObjectCodeStr));
			strcat(ObjectCodeStr, objectcode1_X);

			Start_str = line_locctr[presentline - 1];		
			}
		}
	}
	fclose(ip); fclose(op1); fclose(op2);
	if(!ERROR_FLAG) printf("Successfully assemble %s.\n", filename);
	return 0;
}

int opcode_format(char* Mnemonic)
{
	int Find = 0;
	int flag = 0;	
	hash* temphash = (hash*)malloc(sizeof(hash));

	//Mnemonic의 예로 Add가 있다.
	//입력받은 opcode에 해당하는 format을 return한다.
	for (int h = 0; h < tablesize; ++h) {
		temphash = hashtable[h];
		while(temphash != NULL){
			if (!strcmp(temphash->order, Mnemonic))
			{
				Find = 1;
				if(!strcmp(temphash->format, "1")) flag = 1;
				else if(!strcmp(temphash->format, "2")) flag = 2;
				else {flag = 3;}
			}
			if (Find) break;
			else temphash = temphash->next;
		}
		if (Find) break;
	}
	if(!Find) return 0;
	else return flag;
}

void AddSymtab(int loc, char* name)
{
	//hashtable[key]에 char* order, char* format을 저장한다.
	SYMTAB* newsymtab = (SYMTAB*)malloc(sizeof(SYMTAB));
	newsymtab->next = symtab->next;

	strcpy(newsymtab->name, name);
	newsymtab->loc = loc;

	symtab->next = newsymtab;
}

int FindSymtab(char* name)
{
	SYMTAB* tempsymtab = (SYMTAB*)malloc(sizeof(SYMTAB));
	tempsymtab = symtab;
	int Find = 0;

	while(tempsymtab != NULL){
			//Mnemonic을 찾았을 경우
			if (!strcmp(tempsymtab->name, name))
			{
				Find = 1;
				//return tempsymtab->loc;
			}
			if (Find) break;
			else tempsymtab = tempsymtab->next;
		}
	if(!Find) return NotInSymtab;
	else return tempsymtab->loc;
}

char* itoa(int num, char* str, int base)
{
	int i = 0;
	int isNegative = 0;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with  
	// base 10. Otherwise numbers are considered unsigned. 
	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}

	// Process individual digits 
	while (num != 0)
	{
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// If number is negative, append '-' 
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator 

	// Reverse the string 
	//reverse(str, i);
	i = 0;
	int j = strlen(str) - 1; char temp;

	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}

	return str;
}

int regis_to_data(char c)
{
	if(c == 'A') return 0;
	else if(c == 'X') return 1;
	else if(c == 'S') return 4;
	else if(c == 'T') return 5;	
	else return -1;		
}

int dump(int start, int end)
{
//start번지부터 end번지까지의 정보를 출력한다.
//첫번째 column은 출력하는 메모리의 주소, 두번째 column은 메모리 내용을 16진수로 보여준다.
//세번째 column은 메모리 내용을 byte 별로 대응하는 ASCII code 형태로 보여준다.

int y_line_start = start / line_size; //시작점의 y좌표
int y_line_end = end / line_size;     //종점의 y좌표
int x_line_start = start % line_size; //시작점의 x좌표
int x_line_end = end % line_size;   //종점의 x좌표
int x, y;

	for(y = y_line_start; y <= y_line_end; ++y){
		//첫번째 column
		printf("%04X0 ", y); 
		//두번째 column
		for(x = 0; x < line_size; ++x){
			if(y == y_line_start && x < x_line_start) printf("   ");
			else if(y == y_line_end && x > x_line_end) printf("   ");
			else printf("%02X ", (unsigned char)mem[y][x]);
			}		
		printf("; ");
		//세번째 column
		for(x = 0; x < line_size; ++x) {
		
			if(y == y_line_start && x < x_line_start) printf(".");
			else if(y == y_line_end && x > x_line_end) printf(".");
			else{
				//value 영역에 속할 경우
				if(mem[y][x] >= 32 && mem[y][x] <= 126){
					printf("%c", mem[y][x]);
				}
				//안 속할 경우
				else printf(".");
			}
		}
	printf("\n");
	}
	return 0;
}

void edit(int address, int value)
{
//함수의 parameter은 10진수이다. 
//메모리의 address번지 값을 value로 변경한다.
//address의 위치를 2차원 배열 char** mem에 맞게 변환한다.
int y = address / line_size;
int x = address % line_size;
mem[y][x] =  value;
}

void fill(int start, int end, int value){
	//start부터 end까지 edit한다.
	for(int address = start; address <= end; ++address) 
		edit(address, value);
}

void reset(void){
	//메모리의 전부를 0으로 초기화한다.
	for(int y=0; y<line_max; ++y)
		memset(mem[y], 0, sizeof(char)*line_size);
}

void AddCommand(char* command) 
//실행한 명령어를 ActivatedCommands_head 혹은  ActivatedCommands_tail에 넣는다.
{
	node* newNode = (node*)malloc(sizeof(node));
	strcpy(newNode->name, command);
	newNode->next = NULL;

	if(ActivatedCommands_head == NULL){
		ActivatedCommands_head = newNode;
		ActivatedCommands_tail = ActivatedCommands_head;
		}
	else{
		ActivatedCommands_tail->next = newNode;
		ActivatedCommands_tail = newNode;
	}	
}
	
void AddHash(int key, char* sym, char* order, char* format)
{
//hashtable[key]에 char* order, char* format을 저장한다.
	hash* newHash = (hash*)malloc(sizeof(hash));
	newHash->next = hashtable[key]->next;

	strcpy(newHash->sym, sym);
	strcpy(newHash->order, order);
	strcpy(newHash->format, format);

	hashtable[key]->next = newHash;
}

void opcodelist(void)
{
//opcode.txt에서 받아온 opcode의 정보들을 나열한다.
	hash* temphash = (hash*)malloc(sizeof(hash));

	for (int h = 0; h < tablesize; ++h) {
		temphash = hashtable[h]->next;
		printf("%d : ", h);
		while (temphash != NULL) {
			if(temphash->next!=NULL) printf("[%s,%s] ->", temphash->order, temphash->sym);
			else printf("[%s,%s]", temphash->order, temphash->sym);
			temphash = temphash->next;
		}
		printf("\n");
	}
}
int opcode(char* Mnemonic)
{
//opcode의 정보가 저장되어 있는 hashtable에서 char* Mnemonic을 검색한다.
	int Find = 0;
	hash* temphash = (hash*)malloc(sizeof(hash));

	//Mnemonic의 예로 Add가 있다.
	for (int h = 0; h < tablesize; ++h) {
		temphash = hashtable[h];
		while(temphash != NULL){
			//Mnemonic을 찾았을 경우
			if (!strcmp(temphash->order, Mnemonic))
			{
				Find = 1;
				//return (int)strtol(temphash->sym, NULL, 16);
			}
			if (Find) break;
			temphash = temphash->next;
		}
		if (Find) break;
	}
	if(!Find) return ERROR_WrongOpcode;
	else return (int)strtol(temphash->sym, NULL, 16);
}
int opcode1(char* Mnemonic)
{
//opcode의 정보가 저장되어 있는 hashtable에서 char* Mnemonic을 검색한다.
	int Find = 0;
	hash* temphash = (hash*)malloc(sizeof(hash));

	//Mnemonic의 예로 Add가 있다.
	for (int h = 0; h < tablesize; ++h) {
		temphash = hashtable[h];
		while(temphash != NULL){
			//Mnemonic을 찾았을 경우
			if (!strcmp(temphash->order, Mnemonic))
			{
				Find = 1;
				printf("opcode is %s\n", temphash->sym);
				//return (int)strtol(temphash->sym, NULL, 16);
			}
			if (Find) break;
			temphash = temphash->next;
		}
		if (Find) break;
	}
	if(!Find) return ERROR_WrongOpcode;
	else return (int)strtol(temphash->sym, NULL, 16);
}

int key(int opcode)
{
	int key = opcode % tablesize;
	//key를 return한다.
	return key;
}

int Input_Data(void)
{

	FILE* ip;
	if (!(ip = fopen("./20161580.c", "rb"))) {
		fputs("ERROR: File NOT OPEN\n", stderr);
		return 0;
	}

	//Memory 초기화
	int i;
	//1MB를 만들기 위해 mem의 data type을 1byte인 unsigned char로 하였다.
	mem = (unsigned char**)malloc(sizeof(unsigned char*)*line_max);
	for(i=0; i<line_max; ++i){
		mem[i] = (unsigned char*)malloc(sizeof(unsigned char) * line_size);
		}
	for(i=0; i<line_max; ++i){
		fread(mem[i], sizeof(unsigned char), line_size, ip);
		}
	fclose(ip);
	
	//opcode.txt를 기반으로 Hash table 초기화
	int h;
	for(h = 0; h < tablesize; ++h){
		hashtable[h] = malloc(sizeof(hash));
		hashtable[h]->next = NULL;
	}
	if (!(ip = fopen("./opcode.txt", "r"))) {
		fputs("ERROR: opcode.txt NOT OPEN\n", stderr);
		return 0;
	}
	hash* temphash = malloc(sizeof(hash));

	while(1){
		//opcode.txt의 한 line을 읽는다.
		fscanf(ip, "%s %s %s", temphash->sym, temphash->order, temphash->format);
		if(feof(ip) != 0) break;
		//hash table에 값을 저장한다. 방법은 다음과 같다.
		//1. sym에 알맞는 key를 배정한다.
		int THISopcode = (int)strtol(temphash->sym, NULL, 16);
		int THISkey = key(THISopcode);
		//2. key에 해당하는 linked list에 값을 저장한다.	
		//저장해야 하는 값은 1. symbol 2. order 3. format
		AddHash(THISkey, temphash->sym, temphash->order, temphash->format);
	}
	return 0;
}

int main (void)
{	
	Input_Data();
	
	char* command = malloc(sizeof(char) * max_command_size);
	while(1){
		printf("sicsim>");
		
		fgets(command, max_command_size, stdin);
		//fgets로 string을 입력할 경우 string 마지막에 enter, 즉 '\n' 값이 저장된다.
		//따라서 string의 마지막을 NULL로 저장해준다.
		command[strlen(command) - 1] = '\0';
		//q를 입력할 경우 프로그램 종료
		if(!strcmp("q", command) || !strcmp("quit", command)) break;
		else {
		//입력받은 명령어(command)를 실행한다.
		int flag = Input_Order(command);
			//history를 실행할 때 history 역시 링크드리스트에 저장이 되어야 한다.
			//따라서 다른 명령어들과 달리 history는 main에서 AddCommand를 안한다.
			//대신 Activate 함수에서 AddCommand를 한다. 
			if(!strcmp("hi", command) || !strcmp("history", command)){} 
			//입력한 명령어에 ERROR가 있을 경우 해당 ERROR의 정보를 출력한다.
			else if(flag == ERROR_OutofRange) printf("ERROR: Out of Range\n");
			else if(flag == ERROR_StartIsLargerThanEnd) printf("ERROR: Start is larger than End\n");
			else if(flag == ERROR_WrongCommand) printf("ERROR: Wrong Command\n");
			else if(flag == ERROR_WrongOpcode) printf("ERROR: Wrong Opcode\n");
			//입력한 명령어가 ERROR가 없을 경우 해당 명령어를 저장한다.
			else AddCommand(command);
		}
	}	
}
