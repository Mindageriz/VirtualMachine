#include <iostream>
#include <fstream>
using namespace std;
int main() {
    unsigned char regs[16] = { 0 };  // 16 bendrosios paskirties registru 
    char prog_mem[256];            // 256 baitu atmintis
    char* prog_count = prog_mem;   // program counter realizuotas kaip adresas
    char flag_reg = 0b00000010;    // veliavu registras 0  0  0   EOF zero  0  1  0

    ifstream file;
    file.open("decryptor.bin", ios::in | ios::binary); // decryptor.bin  VM-reiskinys.bin
    char* read_count = prog_mem;  // read counter adresas nuskaitymui
    while (!file.eof()) {
        file.get(*read_count);
        read_count++;
    }
    file.close();
    file.open("q1_encr.txt"); // q1_encr.txt  TekstinisFailas.txt

    while (true) {
        switch (*prog_count) {
        case 0x01: // INC (Rx++) type: 1
            regs[*(prog_count + 1) & 0b00001111]++;
            if (regs[*(prog_count + 1) & 0b00001111] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x02: // DEC (Rx--) type: 1
            regs[*(prog_count + 1) & 0b00001111]--;
            if (regs[*(prog_count + 1) & 0b00001111] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x03: // MOV (Ry -> Rx) type: 1
            regs[*(prog_count + 1) & 0b00001111] = regs[*(prog_count + 1) >> 4];
            if (regs[*(prog_count + 1) & 0b00001111] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x04: // MOVC (Ry -> R0) type: 2
            regs[0] = *(prog_count + 1);
            if (regs[0] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x05: // LSL (Rx << 1) type: 1
            regs[*(prog_count + 1) & 0b00001111] = regs[*(prog_count + 1) & 0b00001111] << 1;
            if (regs[*(prog_count + 1) & 0b00001111] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x06: // LSR (Rx >> 1) type: 1
            regs[*(prog_count + 1) & 0b00001111] = regs[*(prog_count + 1) & 0b00001111] >> 1;
            if (regs[*(prog_count + 1) & 0b00001111] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x07: // JMP (prog_count + konstanta) type: 2        
            prog_count += *(prog_count + 1);
            flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            break;

        case 0x08: // JZ ( if(zeroFlag = 1){prog_count + konstanta} ) type: 2
            if ((flag_reg & 0b00001000) == 0b00001000) prog_count = prog_count + (*(prog_count + 1));
            else prog_count += 2;
            flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            break;

        case 0x09: // JNZ (if(zeroFlag = 0){prog_count + konstanta} ) type: 2
            if ((flag_reg & 0b00001000) == 0b00000000) prog_count = prog_count + (*(prog_count + 1));
            else prog_count += 2;
            flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            break;

        case 0x0A: // JFE (if(eofFlag = 1){prog_count + konstanta} ) type: 2
            if ((flag_reg & 0b00010000) == 0b00010000) prog_count = prog_count + (*(prog_count + 1));
            else prog_count += 2;
            flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            break;

        case 0x0B: // RET (STOP) type: -
            return 0;
            break;

        case 0x0C: // ADD (Rx = Rx + Ry) type: 1
            regs[*(prog_count + 1) & 0b00001111] = regs[*(prog_count + 1) & 0b00001111] + regs[*(prog_count + 1) >> 4];
            if (regs[*(prog_count + 1) & 0b00001111] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x0D: // SUB (Rx = Rx - Ry) type: 1
            regs[*(prog_count + 1) & 0b00001111] = regs[*(prog_count + 1) & 0b00001111] - regs[*(prog_count + 1) >> 4];
            if (regs[*(prog_count + 1) & 0b00001111] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x0E: // XOR (Rx = Rx XOR Ry) type: 1
            regs[*(prog_count + 1) & 0b00001111] = regs[*(prog_count + 1) & 0b00001111] ^ regs[*(prog_count + 1) >> 4];
            if (regs[*(prog_count + 1) & 0b00001111] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x0F: // OR (Rx = Rx OR Ry) type: 1
            regs[*(prog_count + 1) & 0b00001111] = regs[*(prog_count + 1) & 0b00001111] | regs[*(prog_count + 1) >> 4];
            if (regs[*(prog_count + 1) & 0b00001111] == 0x00)  flag_reg = flag_reg | 0b00001000;
            else  flag_reg = flag_reg & 0b11110111;
            flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x10: // IN (file >> Rx (1-byte), eofFlag check) type: 2
            file >> regs[*(prog_count + 1) & 0b00001111];
            if (file.eof())  flag_reg = flag_reg | 0b00010000;
            else flag_reg = flag_reg & 0b11101111;
            prog_count += 2;
            break;

        case 0x11: // OUT (print Rx) type: 1
            cout << regs[*(prog_count + 1) & 0b00001111];
            flag_reg = flag_reg & 0b11110111;
            prog_count += 2;
            break;
        }
    }
}
