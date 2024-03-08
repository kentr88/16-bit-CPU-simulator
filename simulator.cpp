#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <string>
#include <bitset>

using namespace std;



class BinaryConversions {
public:
    static uint16_t binaryToDecimal(bool * binary, int length){
        uint16_t decimal = 0;
        for(int i = 0; i < length; i++){
            decimal += binary[i] * (1 << (length - 1 - i));
        }
        return decimal;
    }

    static int signedBinaryToDecimal(bool binary[4]){
        int decimal = 0;
        bool negative = binary[0]; // Check if the number is negative

        // Convert the binary number to decimal
        for(int i = 1; i < 4; i++){
            decimal += binary[i] * (1 << (3 - i));
        }

        // If the number is negative, apply the 2's complement
        if(negative){
            decimal = -((1 << 3) - decimal);
        }

        return decimal;
    }

    static void decimalToBinary(uint16_t decimal, bool * binary, int length){
        for(int i = 0; i < length; i++){
            binary[i] = (decimal & (1 << (length - 1 - i))) >> (length - 1 - i);
        }
    }



    

    static void signedDecimalToBinary(int decimal, bool binary[16]){
        bool negative = decimal < 0;
        if(negative){
            decimal = -decimal;
            decimal = (1 << 3) - decimal;
        }

        for(int i = 0; i < 16; i++){
            binary[i] = (decimal & (1 << (15 - i))) >> (15 - i);
        }

        if(negative){
            binary[0] = 1;
        }
    }


    static void printBinary(bool binary[16], int length = 16){
        for(int i = 0; i < length; i++){
            cout << binary[i];
        }
        cout << endl;
    }


    static void transferBinary(bool * source, bool * dest, int num, int sourceOffset, int destOffset){
        try {
            for(int i = 0; i < num; i++){
                dest[i + destOffset] = source[i + sourceOffset];
            }
        } catch (exception e){
            cout << "Error with transferBinary" << endl;
        }
        
    }

    static uint16_t hexToInt(char hex[4]){
        uint16_t decimal = 0;
        bool binary[16];

        for(int i = 0; i < 4; i++){
            // Convert each hex character to its corresponding 4-bit binary string
            uint16_t hexValue;
            if (hex[i] >= '0' && hex[i] <= '9') {
                hexValue = hex[i] - '0';
            } else if (hex[i] >= 'A' && hex[i] <= 'F') {
                hexValue = hex[i] - 'A' + 10;
            } else {
                // Handle invalid hex characters here
                // For example, you can set hexValue to a default value or throw an exception
            }
            for(int j = 0; j < 4; j++){
                binary[i * 4 + j] = (hexValue >> (3 - j)) & 1;
            }
        }

        // Convert the 16-bit binary string back to a number
        for(int i = 0; i < 16; i++){
            decimal += binary[i] * (1 << (15 - i));
        }

        return decimal;
    }

    

};


class Register {
    uint16_t data;
public:
    Register() {
        data = 0;
    }

    Register(uint16_t _data) {
        data = _data;
    }

    virtual void write(uint16_t data) {
        this->data = data;
    }

    virtual uint16_t read() {
        return data;
    }
};

class ConstRegister : public Register {
public:
    ConstRegister() : Register() {}
    ConstRegister(uint16_t data) : Register(data) {}
    void write(uint16_t data) override {
        // Do nothing
    }
};


class RamModule {
    uint16_t * data;
    uint32_t slots;
public:
    RamModule(uint32_t _slots){
        slots = _slots;
        
        //2^24 ram slots
        data = new uint16_t[slots];
        for(int i = 0; i < (int) slots; i++){
            data[i] = uint16_t(0);
        }
    }

    //load instructions from file
    //instructions
    void loadInstructions(string filename){
        //open file
        ifstream file;
        file.open(filename);
        if(!file.is_open()){
            cout << "Error opening file" << endl;
            return;
        }

        //read file
        string line;
        int i = 0;
        int arrcounter = 0;
        while (getline(file, line)) {
            // Ignore first line
            if (i == 0) {
                i++;
                continue;
            }

            for (int j = 0; j < line.length(); j += 4) {
                char hex[4];
                hex[0] = line[j];
                hex[1] = line[j + 1];
                hex[2] = line[j + 2];
                hex[3] = line[j + 3];
                j++;

                data[arrcounter] = BinaryConversions::hexToInt(hex);
                arrcounter++;
                i++;
            }
        }
        file.close();
    }


    void write(uint16_t data, uint32_t address){
        this->data[address] = data;
    }

    uint16_t read(uint32_t address){
        return this->data[address];
    }


    void print(int num = 128) {
        for (int i = 0; i < min(num, (int) slots); i++) {
            cout << bitset<16>(data[i]) << " ";
            if ((i + 1) % 4 == 0) {
                cout << endl;
            }
        }
    }

    ~RamModule(){
        delete[] data;
    }
};



class ALU {
public:
    //operations:
    // 0 - add
    // 1 - subtract
    // 2 - mul
    // 3 - div
    // 4 - and
    // 5 - or
    // 6 - nor
    // 7 - xor
    // 8 - comp
    // 9 - shift
    // 10 - addi (add immediate)


    static uint16_t use(bool opcode[4], uint16_t a, uint16_t b, bool bconst[4]){
        uint16_t opint = BinaryConversions::binaryToDecimal(opcode, 4);
        int shift;

        switch(opint){
            case 0:
                return a + b;
                break;
            case 1:
                return a - b;
                break;
            case 2:
                return a * b;
                break;
            case 3:
                return a / b;
                break;
            case 4:
                return a & b;
                break;
            case 5:
                return a | b;
                break;
            case 6:
                return ~(a | b);
                break;
            case 7:
                return a ^ b;
                break;
            case 8:
                if(a == b) return 0;
                if(a > b) return 1;
                if(a < b) return -1;
                break;
            case 9:
                shift = BinaryConversions::binaryToDecimal(bconst, 4);
                if(shift > 0){
                    return a >> shift;
                } else {
                    return a << abs(shift);
                }
                break;
            case 10:
                return a + BinaryConversions::binaryToDecimal(bconst, 4);
                break;
            default:
                cout << "Error with instruction passed to ALU" << endl;
                break;
        }
        return 0;
    }
};


class CPU {
    Register * registers;
    //register 0 is a constant
    //register 13 is data address extension
    //register 14 is instruction address extension
    //register 15 is the user input/output

    uint32_t counter = 0;   //stop overflow miscounting
public:
    CPU(){
        registers = new Register[16];
        registers[0] = ConstRegister(0);
        for(int i = 1; i < 16; i++){
            registers[i] = Register();
        }
    }


    void run(RamModule* ram, uint16_t& input, uint16_t& output, bool& halt){
        //jump variable
        bool ifJump = false;

        //create opcode
        bool instructionAddress[24] = {0};
        //add in 16 bits from register
        bool instructionExtensionAddress[16];
        BinaryConversions::decimalToBinary(registers[14].read(), instructionExtensionAddress, 16);
        BinaryConversions::transferBinary(instructionExtensionAddress, instructionAddress, 16, 0, 0);

        //add in 8 bits from counter
        bool counterAddress[8] = {0};
        BinaryConversions::decimalToBinary(counter, counterAddress, 8);
        
        BinaryConversions::transferBinary(counterAddress, instructionAddress, 8, 0, 16);

        //read instruction from ram
        //get opcode first and then remainding paramaters later depending on instruction type

        //not updating
        cout << "into ram sel: " << BinaryConversions::binaryToDecimal(instructionAddress, 24) << endl;
        uint16_t instruction = ram->read(BinaryConversions::binaryToDecimal(instructionAddress, 24));
        bool bInstruction[16];
        BinaryConversions::decimalToBinary(instruction, bInstruction, 16);
        cout << "instruction: ";
        BinaryConversions::printBinary(bInstruction);
        
        bool opcode[4] = {0};
        BinaryConversions::transferBinary(bInstruction, opcode, 4, 0, 0);
        int opcodeInt = BinaryConversions::binaryToDecimal(opcode, 4);

        //execute instruction
        if(opcodeInt >= 0 && opcodeInt <= 10){
            cout << "ALU\n";
            //use ALU
            bool outAddress[4];
            bool aAddress[4];
            bool bAddress[4];
            cout << "binstruction: ";
            BinaryConversions::printBinary(bInstruction);
            BinaryConversions::transferBinary(bInstruction, outAddress, 4, 4, 0);
            BinaryConversions::transferBinary(bInstruction, aAddress, 4, 8, 0);
            BinaryConversions::transferBinary(bInstruction, bAddress, 4, 12, 0);
            cout << "a: " << BinaryConversions::binaryToDecimal(aAddress, 4) << " b: " << BinaryConversions::binaryToDecimal(bAddress, 4) << " out: " << BinaryConversions::binaryToDecimal(outAddress, 4) << endl;

            uint16_t out = ALU::use(opcode, registers[BinaryConversions::binaryToDecimal(aAddress, 4)].read(), registers[BinaryConversions::binaryToDecimal(bAddress, 4)].read(), bAddress);
            cout << "out: " << out << endl;
            registers[BinaryConversions::binaryToDecimal(outAddress, 4)].write(out);
            cout << "write address: " << BinaryConversions::binaryToDecimal(outAddress, 4) << endl;
            cout << "write: " << registers[BinaryConversions::binaryToDecimal(outAddress, 4)].read() << endl;

        } else if(opcodeInt == 11){
            cout << "load\n";
            //load
            bool regAddress[4];
            bool opRamAddress[8];
            BinaryConversions::transferBinary(bInstruction, regAddress, 4, 4, 0);
            BinaryConversions::transferBinary(bInstruction, opRamAddress, 8, 8, 0);

            //get ram address
            bool ramAddress[24];
            bool ramExtensionAddress[16];
            BinaryConversions::decimalToBinary(registers[13].read(), ramExtensionAddress, 16);
            BinaryConversions::transferBinary(ramExtensionAddress, ramAddress, 16, 0, 0);

            //last 8 from opcode
            BinaryConversions::transferBinary(opRamAddress, ramAddress, 8, 0, 16);

            //read from ram
            uint16_t data = ram->read(BinaryConversions::binaryToDecimal(ramAddress, 24));
            registers[BinaryConversions::binaryToDecimal(regAddress, 4)].write(data);


        } else if(opcodeInt == 12){
            cout << "store\n";
            //store
            bool regAddress[4];
            bool opRamAddress[8];
            BinaryConversions::transferBinary(bInstruction, regAddress, 4, 4, 0);
            BinaryConversions::transferBinary(bInstruction, opRamAddress, 8, 8, 0);

            //get ram address
            bool ramAddress[24];
            bool ramExtensionAddress[16];
            BinaryConversions::decimalToBinary(registers[13].read(), ramExtensionAddress, 16);
            BinaryConversions::transferBinary(ramExtensionAddress, ramAddress, 16, 0, 0);

            //last 8 from opcode
            BinaryConversions::transferBinary(opRamAddress, ramAddress, 8, 0, 16);

            //write to ram
            cout << "ram address: " << BinaryConversions::binaryToDecimal(ramAddress, 24) << " reg address: " << BinaryConversions::binaryToDecimal(regAddress, 4) << endl;
            ram->write(registers[BinaryConversions::binaryToDecimal(regAddress, 4)].read(), BinaryConversions::binaryToDecimal(ramAddress, 24));

        } else if(opcodeInt == 13){
            cout << "jump address\n";
            //jump to address
            bool address[8];
            bool regAddress[4];
            BinaryConversions::transferBinary(bInstruction, address, 8, 8, 0);
            BinaryConversions::transferBinary(bInstruction, regAddress, 4, 4, 0);

            //do comparison if reg == 0
            if(registers[BinaryConversions::binaryToDecimal(regAddress, 4)].read() == 0){
                counter = BinaryConversions::binaryToDecimal(address, 8);
                ifJump = true;
            }
            cout << "cmp: " << registers[BinaryConversions::binaryToDecimal(regAddress, 4)].read() << endl;

        } else if(opcodeInt == 14){
            //jump amount
            cout << "jump number\n";
            bool addAddress[8];
            bool regAddress[4];
            BinaryConversions::transferBinary(bInstruction, addAddress, 8, 8, 0);
            BinaryConversions::transferBinary(bInstruction, regAddress, 4, 4, 0);

            //do comparison if reg == 0
            if(registers[BinaryConversions::binaryToDecimal(regAddress, 4)].read() == 0){
                counter += BinaryConversions::binaryToDecimal(addAddress, 8);
                ifJump = true;
            }
            cout << "cmp jump: " << BinaryConversions::binaryToDecimal(addAddress, 8) << " cmp reg: " << BinaryConversions::binaryToDecimal(regAddress, 4) << endl;
            //do nothing
            cout << "cmp: " << registers[BinaryConversions::binaryToDecimal(regAddress, 4)].read() << endl;

        } else if(opcodeInt == 15){
            //halt.
            halt = true;
        }


        //check instruction register overflow        
        if(counter >= pow(2, 8) - 1){
            //make counter = to remainder
            counter = counter - pow(2, 8);
        }

        //may need to modify this code
        // if(opcodeInt != 13 && opcodeInt != 14){
            
        // }
        if(ifJump == false){
            counter++;
        }

        cout << endl;

        return;

    }

    void printReg(){
        for(int i = 0; i < 16; i++){
            cout << "Register " << i << ": " << registers[i].read() << endl;
        }
    }
};


class Computer {
    CPU cpu;
    RamModule ram = RamModule(pow(2, 24));
    uint16_t input;
    uint16_t output;
    bool halt = false;
public:
    Computer(){
        input = 0;
        output = 0;
    }

    void run(){
        ram.loadInstructions("hexMachineCode");
        while(halt == false){
            cpu.run(&ram, input, output, halt);
        }
        
        cout << "Registers: \n";
        cpu.printReg();
        cout << "RAM: \n";
        ram.print(50);
    }

};

int main() {
    // // Create CPU and registers
    // MyCPU cpu;
    // GeneralPurposeRegister reg1, reg2, resultReg;

    // // Add registers to CPU
    // cpu.addRegister(&reg1);
    // cpu.addRegister(&reg2);
    // cpu.addRegister(&resultReg);

    // // Create instructions
    // AddInstruction addInstr(&reg1, &reg2, &resultReg);

    // // Add instructions to CPU
    // cpu.addInstruction(&addInstr);

    // // Run the CPU
    // cpu.run();

    // // Print the result
    // std::cout << "Result: " << resultReg.read() << std::endl;
    Computer comp;
    comp.run();


    return 0;
}