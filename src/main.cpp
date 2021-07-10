#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <stack>
using namespace std;

constexpr unsigned short MEMORY_SIZE = 0xffff;

constexpr char16_t INCREMENT = u'ち';
constexpr char16_t DECREMENT = u'ん';
constexpr char16_t RIGHT = u'う';
constexpr char16_t LEFT = u'お';
constexpr char16_t LOOP_BEGIN = u'こ';
constexpr char16_t LOOP_END = u'ま';
constexpr char16_t OUTPUT = u'ぽ';
constexpr char16_t INPUT = u'！';

constexpr unsigned char order_len = 2;

int main(int argc, char* argv[]) {	
	unsigned char memory[MEMORY_SIZE];
	unsigned int ptr = 0;
	unsigned int code_ptr = 0;
	unsigned int code_len = 0;
	memset(memory, 0, sizeof(memory));

	stack<int> loops;

	if (argc < 2){
		cerr << "Error: No argument." << endl;
		return -1;
		// for debug: argv[1] = "examples\\hello.nko";
	}
	ifstream file(argv[1]);
	if (!file) {
		cerr << "Error: " << argv[1] << " cannot be opened." << endl;
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
			case INCREMENT:
				memory[ptr]++;
				break;
			case DECREMENT:
				memory[ptr]--;
				break;
			case RIGHT:
				ptr = (ptr >= MEMORY_SIZE - 1) ? 0 : ptr + 1;
				break;
			case LEFT:
				ptr = (ptr <= 0) ? MEMORY_SIZE - 1 : ptr - 1;
				break;
			case LOOP_BEGIN:
				loops.push(code_ptr);
				if (memory[ptr] == 0) {
					int depth = 1;
					while (depth > 0) {
						code_ptr += order_len;
						if (code_ptr >= code_len){
							cerr << "Error: Lack loop-end." << endl;
							return -1;
						}
						unsigned char _upper_char = code[code_ptr + 1];
						unsigned char _lower_char = code[code_ptr];
						switch (_upper_char << 8 | _lower_char) {
							case LOOP_BEGIN:
								depth++;
								break;
							case LOOP_END:
								depth--;
								break;
						}
					}
					loops.pop();
				}
				break;
			case LOOP_END:
				if (loops.empty()) {
					cerr << "Error: Lack loop-begin." << endl;
					return -1;
				}
				code_ptr = loops.top() - order_len;
				loops.pop();
				break;
			case OUTPUT:
				putchar(memory[ptr]);
				break;
			case INPUT:
				memory[ptr] = getchar();
				break;
		}
		code_ptr += order_len;
	}

	return 0;
}