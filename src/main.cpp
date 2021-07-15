#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <stack>
using namespace std;

enum class BfCommand: unsigned char {
	Right = '>',
	Left = '<',
	Increment = '+',
	Decrement = '-',
	LoopBegin = '[',
	LoopEnd = ']',
	Output = '.',
	Input = ',',
};
enum class NkoCommand : unsigned short {
	Right = u'こ',
	Left = u'う',
	Increment = u'ち',
	Decrement = u'ん',
	LoopBegin = u'お',
	LoopEnd = u'ま',
	Output = u'ぽ',
	Input = u'！',
};
constexpr unsigned char ORDER_LEN = 2;

int run(string file_path, unsigned short memory_size) {

	unsigned char memory[memory_size];
	unsigned int ptr = 0;
	unsigned int code_ptr = 0;
	unsigned int code_len = 0;
	memset(memory, 0, sizeof(memory));

	stack<int> loops;

	ifstream file(file_path);
	if (!file) {
		cerr << "Error: " << file_path << " cannot be opened." << endl;
		return -1;
	}

	stringstream buffer;
	buffer << file.rdbuf();
	string code(buffer.str());
	code_len = code.size();

	while (code_ptr < code_len) {
		unsigned char upper_char = code[code_ptr + 1];
		unsigned char lower_char = code[code_ptr];
		switch (upper_char << 8 | lower_char) {
			case static_cast<unsigned short>(NkoCommand::Increment):
				memory[ptr]++;
				break;
			case static_cast<unsigned short>(NkoCommand::Decrement):
				memory[ptr]--;
				break;
			case static_cast<unsigned short>(NkoCommand::Right):
				ptr = (ptr >= memory_size - 1) ? 0 : ptr + 1;
				break;
			case static_cast<unsigned short>(NkoCommand::Left):
				ptr = (ptr <= 0) ? memory_size - 1 : ptr - 1;
				break;
			case static_cast<unsigned short>(NkoCommand::LoopBegin):
				loops.push(code_ptr);
				if (memory[ptr] == 0) {
					int depth = 1;
					while (depth > 0) {
						code_ptr += ORDER_LEN;
						if (code_ptr >= code_len){
							cerr << "Error: Lack loop-end." << endl;
							return -1;
						}
						unsigned char _upper_char = code[code_ptr + 1];
						unsigned char _lower_char = code[code_ptr];
						switch (_upper_char << 8 | _lower_char) {
							case static_cast<unsigned short>(NkoCommand::LoopBegin):
								depth++;
								break;
							case static_cast<unsigned short>(NkoCommand::LoopEnd):
								depth--;
								break;
						}
					}
					loops.pop();
				}
				break;
			case static_cast<unsigned short>(NkoCommand::LoopEnd):
				if (loops.empty()) {
					cerr << "Error: Lack loop-begin." << endl;
					return -1;
				}
				code_ptr = loops.top() - ORDER_LEN;
				loops.pop();
				break;
			case static_cast<unsigned short>(NkoCommand::Output):
				putchar(memory[ptr]);
				break;
			case static_cast<unsigned short>(NkoCommand::Input):
				memory[ptr] = getchar();
				break;
		}
		code_ptr += ORDER_LEN;
	}

	return 0;
}

int bf2nko(string bf_path, string nko_path) {
	ifstream bf_file(bf_path);
	if (!bf_file) {
		cerr << "Error: " << bf_path << " cannot be opened." << endl;
		return -1;
	}
	ofstream nko_file(nko_path, ios::binary | ios::out);
	if (!nko_file) {
		cerr << "Error: " << nko_path << " cannot be created." << endl;
		return -1;
	}

	stringstream bf_buffer;
	bf_buffer << bf_file.rdbuf();
	string code(bf_buffer.str());
	int code_len = code.size();
	int code_ptr = 0;
	nko_file << (char)0xff << (char)0xfe;
	while (code_ptr < code_len) {
		char current = code[code_ptr++];
		if (current < 0x80) {
			unsigned short wchar = current;
			switch (current) {
				case static_cast<unsigned char>(BfCommand::Right):
					wchar = static_cast<unsigned short>(NkoCommand::Right);
					break;
				case static_cast<unsigned char>(BfCommand::Left):
					wchar = static_cast<unsigned short>(NkoCommand::Left);
					break;
				case static_cast<unsigned char>(BfCommand::Increment):
					wchar = static_cast<unsigned short>(NkoCommand::Increment);
					break;
				case static_cast<unsigned char>(BfCommand::Decrement):
					wchar = static_cast<unsigned short>(NkoCommand::Decrement);
					break;
				case static_cast<unsigned char>(BfCommand::LoopBegin):
					wchar = static_cast<unsigned short>(NkoCommand::LoopBegin);
					break;
				case static_cast<unsigned char>(BfCommand::LoopEnd):
					wchar = static_cast<unsigned short>(NkoCommand::LoopEnd);
					break;
				case static_cast<unsigned char>(BfCommand::Output):
					wchar = static_cast<unsigned short>(NkoCommand::Output);
					break;
				case static_cast<unsigned char>(BfCommand::Input):
					wchar = static_cast<unsigned short>(NkoCommand::Input);
					break;
			}
			nko_file << (char)(wchar & 0xff) << (char)(wchar >> 8);
		}
	}
	nko_file.close();
	cout << "Completed transpile: " << bf_path << " to " << nko_path << endl;

	return 0;
}


int nko2bf(string nko_path, string bf_path) {
	ifstream nko_file(nko_path);
	if (!nko_file) {
		cerr << "Error: " << nko_path << " cannot be opened." << endl;
		return -1;
	}
	ofstream bf_file(bf_path, ios::binary | ios::out);
	if (!bf_file) {
		cerr << "Error: " << bf_path << " cannot be created." << endl;
		return -1;
	}
	stringstream nko_buffer;
	nko_buffer << nko_file.rdbuf();
	string code(nko_buffer.str());
	int code_len = code.size();
	int code_ptr = 0;
	while (code_ptr < code_len) {
		unsigned short current = code[code_ptr++] | code[code_ptr] << 8;
		switch (current) {
			case static_cast<unsigned short>(NkoCommand::Right):
				bf_file << static_cast<unsigned char>(BfCommand::Right);
				break;
			case static_cast<unsigned short>(NkoCommand::Left):
				bf_file << static_cast<unsigned char>(BfCommand::Left);
				break;
			case static_cast<unsigned short>(NkoCommand::Increment):
				bf_file << static_cast<unsigned char>(BfCommand::Increment);
				break;
			case static_cast<unsigned short>(NkoCommand::Decrement):
				bf_file << static_cast<unsigned char>(BfCommand::Decrement);
				break;
			case static_cast<unsigned short>(NkoCommand::LoopBegin):
				bf_file << static_cast<unsigned char>(BfCommand::LoopBegin);
				break;
			case static_cast<unsigned short>(NkoCommand::LoopEnd):
				bf_file << static_cast<unsigned char>(BfCommand::LoopEnd);
				break;
			case static_cast<unsigned short>(NkoCommand::Output):
				bf_file << static_cast<unsigned char>(BfCommand::Output);
				break;
			case static_cast<unsigned short>(NkoCommand::Input):
				bf_file << static_cast<unsigned char>(BfCommand::Input);
				break;
			case (unsigned short)'\n':
				bf_file << endl;
				break;
		}
	}
	bf_file.close();
	cout << "Completed transpile: " << nko_path << " to " << bf_path << endl;

	return 0;
}


int main(int argc, char* argv[]) {	

	int rslt = 0;

	if (argc < 2){
		cerr << "Error: No argument." << endl;
		return -1;
	}
	string command = argv[1];
	if (command == "run") {
		if (argc < 3) {
			cerr << "Error: No file." << endl;
			return -1;
		} else {
			string file_path = argv[2];
			unsigned short memory_size = argv[3] ? atoi(argv[3]) * 1000 : 30000;
			rslt = run(file_path, memory_size);
		} 
	} else if (command == "bf2nko" || command == "nko2bf") {
		if (argc < 3) {
			cerr << "Error: No input file." << endl;
			return -1;
		} else {
			string input_path = argv[2];
			string output_path;
			if (argv[3]) {
				output_path = argv[3];
			} else {
				char dot_pos = input_path.find_last_of('.');
				output_path = input_path.substr(0, dot_pos + 1) + ((command == "bf2nko") ? "nko" : "bf");
			}
			rslt = (command == "bf2nko") ? bf2nko(input_path, output_path): nko2bf(input_path, output_path);
		}
	} else {
		cerr << "Error: Wrong command." << endl;
	}

	return rslt;
}