#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdint>

struct Code {
	std::string instruction;
	std::vector<int> address;
};

//memory
std::vector<int> memory;

//stack
std::vector<int> stack(16, 0);

// registradores
int r0 = 0;
int r1 = 0;
int r2 = 0;
int r3 = 0;
int r4 = 0;
int r5 = 0;
int r6 = 0;
int r7 = 0;
int cp = 0;
int sp = 0x8200;

//flags
int C = 0;
int Ov = 0;
int Z = 0;
int S = 0;

int transDec(char hexa);

int *transBin(int dec);

std::string transHex(int val);

int immed(int *decode, int sel);

int* refRd(int val);
int* refRm(int val);
int* refRn(int val);
int* refCp();
int* refSp();

struct Code readFile();

void printReg();

void printStack();

void printMemory();

void deCode(Code code);

void nopFunc();

void haltFunc();

void movFunc(int *decode);

void storeFunc(int *decode);

void loadFunc(int *decode);

void stackFunc(int *decode);

void jumpFunc(int *decode);

void ulaFunc(int *decode);

int main(void) {
	cp = 0;
	Code code = readFile();
	deCode(code);
	printMemory();
	printStack();
	printReg();
	return 0;
}


int transDec(char hexa) {
    if(hexa >= 97) hexa -= 39;
    if(hexa >= 65) hexa -= 7;
    hexa -= 48;
    return int(hexa);
}

int* transBin(int dec) {
    int *aux = new int[4];

    aux[0] = dec / 8;
    aux[1] = ((dec % 8) / 4);
    aux[2] = ((dec % 8) % 4) / 2;
    aux[3] = ((dec % 8) % 4) % 2;

    return aux;
}

std::string transHex(int dec) {
    std::string hex = "";
    int aux;
   
    if(dec >= 0) {
        for(int i = 0; i < 4; i++) {
            aux = dec % 16;
	    aux = aux + 48;
	    if (aux >= 58) aux = aux + 7;
            hex += (char)aux;
            dec = dec / 16;
        }
    } else {
        for(int i = 0; i < 4; i++) {
            aux = dec % 16;
            aux = aux + 70;
	    if(aux < 65) aux = aux - 8;
	    if(aux < 0) aux = -aux;
            hex += (char)aux;
            dec = dec / 16;
        }
    }
    
    for(int i = 0; i < 2; i++) {
      aux = (int)hex[i];
      hex[i] = hex[3-i];
      hex[3-i] = (char)(aux);
    }
    
    return (hex);
}

int immed(int val, int sel) {
	int aux;
	if(sel == 0) {
		 aux = val & 255;
		 if(aux > 127) aux = 255 - aux;
	}
	else if(sel == 1) {
		aux = val & 31;
		val &= 1792;
		val = val >> 3;
		aux += val;
		if(aux > 127) aux = 255 - aux;
	}
	else if(sel == 2) {
		aux = val & 31;
		if(aux > 15) aux = 31 - aux;
	}
	else {
		val &= 2044;
		val = val >> 2;
		aux = val;
		if(aux > 255) aux = 255 - aux;
	}
	return aux;
}

int *refRd(int val) {
	int *aux;
	switch((val & 1792) >> 8) {
		case 0: return &r0;
		case 1: return &r1;
		case 2: return &r2;
		case 3: return &r3;
		case 4: return &r4;
		case 5: return &r5;
		case 6: return &r6;
		case 7: return &r7;
	}
	return aux;
}

int* refRm(int val) {
	int* aux;
	switch((val & 224) >> 5) {
		case 0: return &r0;
		case 1: return &r1;
		case 2: return &r2;
		case 3: return &r3;
		case 4: return &r4;
		case 5: return &r5;
		case 6: return &r6;
		case 7: return &r7;
	}
	return aux;
}

int* refRn(int val) {
	int* aux;
	switch((val & 28) >> 2) {
		case 0: return &r0;
		case 1: return &r1;
		case 2: return &r2;
		case 3: return &r3;
		case 4: return &r4;
		case 5: return &r5;
		case 6: return &r6;
		case 7: return &r7;
	}
	return aux;
}

int* refCp() { return &cp; }
int* refSp() { return &sp; }

struct Code readFile() {
	Code code;
	code.instruction = "";
	std::string file, line;
	std::fstream fIn;
	
	std::cout << "nome do arquivo ou diretorio: ";
	std::cin >> file;
	fIn.open(file, std::ios::in);
	
	while(fIn >> line) {
		code.address.push_back(transDec(line[0]) * 256 + transDec(line[1]) * 64 + transDec(line[2]) * 16 + transDec(line[3]));
		
		fIn >> line;
		code.instruction += line + " " ;
	}
	
	fIn.close();
	return code;
}

void printReg() {
	std::cout 
	<< "r0: 0x" << transHex(r0) << std::endl
	<< "r1: 0x" << transHex(r1) << std::endl
	<< "r2: 0x" << transHex(r2) << std::endl
	<< "r3: 0x" << transHex(r3) << std::endl
	<< "r4: 0x" << transHex(r4) << std::endl
	<< "r5: 0x" << transHex(r5) << std::endl
	<< "r6: 0x" << transHex(r6) << std::endl
	<< "r7: 0x" << transHex(r7) << std::endl
	<< "cp: 0x" << transHex(cp) << std::endl
	<< "sp: 0x" << transHex(sp) << std::endl
	<< std::endl
	<< "Flags" << std::endl
	<< "C=" << C << " | Z=" << Z << " | Ov=" << Ov << " | S=" << S << std::endl;
}

void printStack() {
	std::cout << "Stack" << std::endl;
	for (int i = 0; i < stack.size(); i++) {
        	std::cout << transHex(sp - i) << ": 0x" << transHex(stack[i]) << std::endl;
    	}
	std::cout << std::endl;
}

void printMemory() {
	for(int i = 0; i < memory.size(); i = i + 2) {
		//if(memory[i] % 2 == 1) std::cout << transHex(memory[i] - 1) << ": "; else std::cout << transHex(memory[i]) << ": ";
		std::cout << transHex(memory[i]) << ": 0x" << transHex(memory[i + 1]) << std::endl;
	}
	std::cout << std::endl;
}

void deCode(Code code){
	std::string aux;
	std::stringstream ss(code.instruction);
	int decode[4];
	
	while(cp/2 != code.address.size()){
		ss >> aux;
		aux = aux.substr(2,4);
		
		if(cp != code.address[cp/2]) break;
		
		decode[0] = transDec(aux[0]);
		decode[1] = transDec(aux[1]);
		decode[2] = transDec(aux[2]);
		decode[3] = transDec(aux[3]);
		
		switch(decode[0]) {
			case 0: 
				if(decode[1] > 7) jumpFunc(decode);
				else {
					switch(decode[3] & 3) {
						case 0:
							nopFunc();
							break;
						case 3:
							ulaFunc(decode);
							break;
						default:
							stackFunc(decode);
							break;
					}
				}
				break;
			case 1: 
				movFunc(decode); 
				break;
			case 2: 
				storeFunc(decode); 
				break;
			case 3: 
				loadFunc(decode); 
				break;
			case 15:
				return;
			default: 
				ulaFunc(decode); 
				break;
		}
		
		cp += 2;
	}
}

void nopFunc(){
	printMemory();
	printStack();
	printReg();
}

void movFunc(int *decode){
	int *Rd = refRd(decode[1]*256 + decode[2]*16 + decode[3]);
	int *Rm = refRm(decode[1]*256 + decode[2]*16 + decode[3]);
	
	if(decode[1] < 7) *Rd = *Rm;
	else *Rd = immed(decode[1]*256 + decode[2]*16 + decode[3], 0);
}

void storeFunc(int *decode){
	int aux = 0;
	int i;
	int *Rm = refRm(decode[1]*256 + decode[2]*16 + decode[3]);
	int *Rn = refRn(decode[1]*256 + decode[2]*16 + decode[3]);
	
	for(i = 0; i < memory.size(); i = i + 2) {
		if(*Rm == memory[i]) aux = 1; break;
	}
	
	if(aux == 0){
		if(decode[1] < 7) {
			memory.push_back(*Rm);
			memory.push_back(*Rn);
		} else {
			memory.push_back(*Rm);
			memory.push_back(immed(decode[1]*256 + decode[2]*16 + decode[3], 1));
		}
	} else {
		if(decode[1] < 7) memory[i+1] = *Rn; 
		else memory[i+1] = immed(decode[1]*256 + decode[2]*16 + decode[3], 1);
	}
	
}

void loadFunc(int *decode){
	int *Rd = refRd(decode[1]*256 + decode[2]*16 + decode[3]);
	int *Rm = refRm(decode[1]*256 + decode[2]*16 + decode[3]);
	
	for(int i = 0; i < memory.size(); i = i + 2) {
		if (*Rm == memory[i]) *Rd = memory[i + 1];
	}
}

void stackFunc(int *decode){
	int *Rd = refRd(decode[1]*256 + decode[2]*16 + decode[3]);
	int *Rn = refRn(decode[1]*256 + decode[2]*16 + decode[3]);
	
	if((decode[3] & 3) == 1){
		if (sp > 0x81F1) {
			stack[0x8200 - sp] = *Rn;
			sp = sp - 1;
		}
	}
	else {
		if(sp < 0x8200) {
			sp = sp + 1;
			*Rd = stack[0x8200 - sp];
		}
	}
}

void jumpFunc(int *decode){
	switch(decode[3] & 3) {
		case 0: 
			cp += immed(decode[1]*256 + decode[2]*16 + decode[3], 3) - 2; 
			break;
		case 1: 
			if(Z == 1 && C == 0) cp += immed(decode[1]*256 + decode[2]*16 + decode[3], 3) - 2; 
			break;
		case 2: 
			if(Z == 0 && C == 1) cp += immed(decode[1]*256 + decode[2]*16 + decode[3], 3) - 2; 
			break;
		case 3: 
			if(Z == 0 && C == 0) cp += immed(decode[1]*256 + decode[2]*16 + decode[3], 3) - 2;
			break;
	}
}

void ulaFunc(int *decode){
	int *Rd = refRd(decode[1]*256 + decode[2]*16 + decode[3]);
	int *Rm = refRm(decode[1]*256 + decode[2]*16 + decode[3]);
	int *Rn = refRn(decode[1]*256 + decode[2]*16 + decode[3]);
	
	switch(decode[0]) {
		case 0: 
			if(*Rm == *Rn) Z = 1;
			else Z = 0;
			if(*Rm < *Rn) C = 1;
			else C = 0;
			if(*Rm - *Rn < 0) S = 1;
			else S = 0;
			if(*Rm + *Rn > 0xFFFF) Ov = 1;
			else Ov = 0;
			break;
		case 4: *Rd = *Rm + *Rn; break;
		case 5: *Rd = *Rm - *Rn; break;
		case 6: *Rd = *Rm * *Rn; break;
		case 7: *Rd = *Rm & *Rn; break;
		case 8: *Rd = *Rm | *Rn; break;
		case 9: *Rd = ~*Rm; break;
		case 10: *Rd = *Rm ^ *Rn; break;
		case 11: *Rd = *Rm >> immed(decode[1]*256 + decode[2]*16 + decode[3], 2); break;
		case 12: *Rd = *Rm << immed(decode[1]*256 + decode[2]*16 + decode[3], 2); break;
		case 13: *Rd = ((*Rm >> 1) + ((*Rm & 1) << 15)); break;
		case 14: *Rd = ((*Rm << 1) + ((*Rm & 1) >> 15)); break;
	}
	
}
