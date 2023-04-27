#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <fstream>
#include <algorithm>
#include <sstream>

std::string pad(std::string in, int width, char pad_char) {
	if (in.size() < width){
		std::string padding((width - in.size()) / 2, pad_char);
		if (width - in.size() % 2 != 0 && padding.size() * 2 + in.size() != width){in.append(" ");}
		return padding + in + padding;
	} else {return in;}
}

std::string table_row(std::string label, unsigned int value){
	return "|" + pad(label, 10, ' ') + "|" + pad(std::bitset<8>(value).to_string(), 10, ' ') + pad("(" + std::to_string(value) + ")", 10, ' ') + "|\n";
}

std::vector<std::string> split(std::string in, char delim){
	std::string temp;
	std::vector<std::string> out;
	std::stringstream in_stream = std::stringstream(in);
	while (std::getline(in_stream,temp,delim)) {
		out.push_back(temp);
	}
	return out;
}

class CPU {
	public:
		CPU(std::string file_name_in, unsigned int reg_len);
		void display(std::string last_inst);
		void run();

		std::string file_name;
		std::vector<uint8_t> registers; // reg_len general purpouse registers
		uint8_t accumulator;
		unsigned int pc;
		std::vector<std::string> instructions;
};
	CPU::CPU(std::string file_name_in, unsigned int reg_len) {
		file_name = file_name_in;
		registers = std::vector<uint8_t>(reg_len, 0);
		accumulator = 0;
		pc = 0;
		instructions = {};
	}

	void CPU::display(std::string last_inst) {
		std::string output = "| Register | Contents (Decimal) |\n";
		for (int i = 0; i < registers.size(); i++){
			output.append(table_row(std::to_string(i), registers[i]));
		}
		output.append(table_row("ACC", accumulator));
		output.append(table_row("PC", pc));

		std::cout << "\x1b[H\x1b[0J" << output << last_inst << "\n" << "Press enter to step\n";
	}

	void CPU::run() {
		std::fstream file(file_name);

		if (!file.is_open()) {
			std::cout << "No file found\n";
			return;
		}

		std::string line;
		while (std::getline(file, line)){
			line.erase(remove(line.begin(), line.end(), ' '), line.end());
			instructions.push_back(line);
		}
		file.close();
		unsigned int inst_size = instructions.size();

		std::vector<std::string> inst;
		std::string inst_out;
		std::vector<std::string> mem_loc;
		std::string instruction;
		while (true) {
			inst = split(instructions[pc], ',');
			inst_out = "";
			// vvv Should probably use a map or hash table but meh
			if (!inst.empty()){
				instruction = inst[0];
				if (instruction == "LDR"){
					std::cout << inst[2] << "\n";
					mem_loc = split(instructions[std::stoi(inst[2])], ',');
					for (auto i: mem_loc){std::cout << i << "|";}
					std::cout << "\n";
					registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = std::stoi(mem_loc[1].substr(1,mem_loc[1].size()-1));
					inst_out = "Loaded value: " + mem_loc[1] + " into " + inst[1];
				}
				else if (instruction == "STR"){
					if (!instructions[std::stoi(inst[2])].empty()){
						mem_loc = split(instructions[std::stoi(inst[2])], ',');
					} else {
						mem_loc = {"VAL","#0"};
					}
					mem_loc[1] = "#" + std::to_string(registers[std::stoi(inst[1].substr(1,inst[1].size()-1))]);
					instructions[std::stoi(inst[2])] = mem_loc[0] + "," + mem_loc[1];
					inst_out = "Stored value: " + mem_loc[1] + " into " + inst[2];
				}
				else if (instruction == "ADD"){
					if (inst[3][0] == '#'){
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] + 
							std::stoi(inst[3].substr(1,inst[3].size()-1));
					} else {
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] + 
							registers[std::stoi(inst[3].substr(1,inst[3].size()-1))];
					}
					inst_out = "Added value: " + inst[2] + " to value: " + inst[3] + " and stored result in: " + inst[1];
				}
				else if (instruction == "SUB"){
					if (inst[3][0] == '#'){
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] - 
							std::stoi(inst[3].substr(1,inst[3].size()-1));
					} else {
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] - 
							registers[std::stoi(inst[3].substr(1,inst[3].size()-1))];
					}
					inst_out = "Subtracted value: " + inst[3] + " from value: " + inst[2] + " and stored result in: " + inst[1];
				}
				else if (instruction == "MOV"){
					if (inst[2][0] == '#'){
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = std::stoi(inst[2].substr(1,inst[2].size()-1));
					} else {
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))];
					}
					inst_out = "Copied value: " + inst[2] + " into " + inst[1];
				}
				else if (instruction == "CMP"){
					if (inst[2][0] == '#'){
						if (registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] == std::stoi(inst[2].substr(1,inst[2].size()-1))){
							accumulator = 1;
						} else if (registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] > std::stoi(inst[2].substr(1,inst[2].size()-1))){
							accumulator = 2;
						} else if (registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] < std::stoi(inst[2].substr(1,inst[2].size()-1))){
							accumulator = 3;
						} else {accumulator = 0;}
					} else {
						if (registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] == registers[std::stoi(inst[2].substr(1,inst[2].size()-1))]){
							accumulator = 1;
						} else if (registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] > registers[std::stoi(inst[2].substr(1,inst[2].size()-1))]){
							accumulator = 2;
						} else if (registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] < registers[std::stoi(inst[2].substr(1,inst[2].size()-1))]){
							accumulator = 3;
						} else {accumulator = 0;}
					}
					inst_out = "Compared value: " + inst[1] + " with value: " + inst[2] + ", giving " + std::to_string(accumulator);
				}
				else if (instruction == "B"){
					pc = std::stoi(inst[1]) - 1;
					inst_out = "Branched to memory location: " + inst[1];
				}
				else if (instruction == "BEQ"){
					if (accumulator == 1){
						pc = std::stoi(inst[1]) - 1;
						inst_out = "Branched to memory location: " + inst[1];
					} else {
						inst_out = "Did not branch";
					}
				}
				else if (instruction == "BNE"){
					if (accumulator == 2 || accumulator == 3){
						pc = std::stoi(inst[1]) - 1;
						inst_out = "Branched to memory location: " + inst[1];
					} else {
						inst_out = "Did not branch";
					}
				}
				else if (instruction == "BLT"){
					if (accumulator == 3){
						pc = std::stoi(inst[1]) - 1;
						inst_out = "Branched to memory location: " + inst[1];
					} else {
						inst_out = "Did not branch";
					}
				}
				else if (instruction == "BGT"){
					if (accumulator == 2){
						pc = std::stoi(inst[1]) - 1;
						inst_out = "Branched to memory location: " + inst[1];
					} else {
						inst_out = "Did not branch";
					}
				}
				else if (instruction == "AND"){
					if (inst[3][0] == '#'){
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] & 
							std::stoi(inst[3].substr(1,inst[3].size()-1));
					} else {
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] & 
							registers[std::stoi(inst[3].substr(1,inst[3].size()-1))];
					}
					inst_out = "Bitwise AND of values: " + inst[2] + " and " + inst[3] + " result stored in " + inst[1];
				}
				else if (instruction == "ORR"){
					if (inst[3][0] == '#'){
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] | 
							std::stoi(inst[3].substr(1,inst[3].size()-1));
					} else {
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] | 
							registers[std::stoi(inst[3].substr(1,inst[3].size()-1))];
					}
					inst_out = "Bitwise OR of values: " + inst[2] + " and " + inst[3] + " result stored in " + inst[1];
				}
				else if (instruction == "EOR"){
					if (inst[3][0] == '#'){
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] ^ 
							std::stoi(inst[3].substr(1,inst[3].size()-1));
					} else {
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] ^ 
							registers[std::stoi(inst[3].substr(1,inst[3].size()-1))];
					}
					inst_out = "Bitwise XOR of values: " + inst[2] + " and " + inst[3] + " result stored in " + inst[1];
				}
				else if (instruction == "MVN"){
					if (inst[3][0] == '#'){
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = ~ registers[std::stoi(inst[2].substr(1,inst[2].size()-1))];
					} else {
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = ~ registers[std::stoi(inst[2].substr(1,inst[2].size()-1))];
					}
					inst_out = "Bitwise NOT of value: " + inst[2] + " result stored in " + inst[1];
				}
				else if (instruction == "LSL"){
					if (inst[3][0] == '#'){
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] << 
							std::stoi(inst[3].substr(1,inst[3].size()-1));
					} else {
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] << 
							registers[std::stoi(inst[3].substr(1,inst[3].size()-1))];
					}
					inst_out = "Logical shift left " + inst[2] + " by value: " + inst[3] + " places, stored in " + inst[1];
				}
				else if (instruction == "LSR"){
					if (inst[3][0] == '#'){
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] >> 
							std::stoi(inst[3].substr(1,inst[3].size()-1));
					} else {
						registers[std::stoi(inst[1].substr(1,inst[1].size()-1))] = registers[std::stoi(inst[2].substr(1,inst[2].size()-1))] >> 
							registers[std::stoi(inst[3].substr(1,inst[3].size()-1))];
					}
					inst_out = "Logical shift right " + inst[2] + " by value: " + inst[3] + " places, stored in " + inst[1];
				}
				else if (instruction == "VAL"){
					inst_out = "Value: " + inst[1];
				}
				else if (instruction == "//"){
					inst_out = "Comment: " + inst[1];
				}
				else if (instruction == "HALT"){
					std::cout << "HALT reached" << "\n";
					break;
				}
				else {
					std::cout << "Invalid instruction -- line:" << std::to_string(pc) << "\n";
					break;
				}
			}
			display(inst_out);
			std::cin.get(); // Halts until enter is pressed
			if (pc >= inst_size - 1){
				std::cout << "End of file reached" << "\n";
				break;
			}
			pc += 1;
		}
	}

int main() {
	std::string fileName;
	std::cout << "Enter file name: \n";
	std::cin >> fileName;
	CPU cpu(fileName,8);

	cpu.run();
	std::cout << "End of program. Type \"exit\" to exit.\n";
	std::string exitInput;
	while (exitInput != "exit"){
		std::cin >> exitInput;
	}

	return 0;
}