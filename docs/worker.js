onmessage = function (e) {
	const capacity = 30000;
	const memory = new Uint8Array(capacity);
	/** @type {string} */
	const code = e.data;
	const stack = [];
	let ptr = 0, codePtr = 0, codeLen = code.length;
	while (codePtr < codeLen) {
		switch (code[codePtr]) {
			case 'こ':
				ptr = (ptr >= capacity - 1) ? 0 : ptr + 1;
				break;
			case 'う':
				ptr = (ptr <= 0) ? capacity - 1 : ptr - 1;
				break;
			case 'ち':
				memory[ptr]++;
				break;
			case 'ん':
				memory[ptr]--;
				break;
			case 'お':
				stack.push(codePtr);
				if (memory[ptr] == 0) {
					let depth = 1;
					while (depth > 0) {
						codePtr++;
						if (codePtr >= codeLen){
							this.postMessage({
								type: "error",
								data: "Error: Lack loop-end."
							});
							return;
						}
						switch (code[codePtr]) {
							case 'お':
								depth++;
								break;
							case 'ま':
								depth--;
								break;
						}
					}
					stack.pop();
				}
				break;
			case 'ま':
				if (stack.length == 0) {
					this.postMessage({
						type: "error",
						data: "Error: Lack loop-begin."
					});
					return;
				} else {
					// @ts-ignore
					codePtr = stack.pop() - 1;
				}
				break;
			case 'ぽ':
				this.postMessage({
					type: "char",
					data: memory[ptr]
				});
				break;
		}
		codePtr++;
	}
	this.postMessage({
		type: "complete",
		data: this.performance.now() - e.timeStamp
	});
};