/*
	NKO-BF Map
*/
const bfNkoMap = new Map([
	['>', 'こ'],
	['<', 'う'],
	['+', 'ち'],
	['-', 'ん'],
	['[', 'お'],
	[']', 'ま'],
	['.', 'ぽ'],
	[',', '！'],
]);
const nkoChars = Array.from(bfNkoMap.values());
const bfChars = Array.from(bfNkoMap.keys());

/** @type {?Worker} */
let worker = null;

/*
	Collapse table
*/
const sidebar = /** @type {HTMLElement} */ (document.getElementById("sidebar"));
const table = /** @type {HTMLTableElement} */ (document.getElementById("table"));
const checkboxes = /** @type {HTMLCollectionOf<HTMLInputElement>} */ (document.getElementsByClassName("toggle"));

for (const checkbox of checkboxes) {
	checkbox.addEventListener("change", updateCollapse, { passive: true });
	updateCollapse({ target: checkbox });
}

/**
 * Update table layout: collapsed or not.
 * @param { Event | { target: HTMLInputElement } } e onchange event
 */
function updateCollapse(e) {
	const elem = /** @type {HTMLInputElement} */ (e.target);
	sidebar.classList[elem.checked ? "remove" : "add"](elem.value);
}

/*
	Textbox
*/
//@ts-ignore
const form = /** @type {HTMLFormElement} */ (document.forms.run);
/** @type { { [key:string]: HTMLTextAreaElement } } */
const { code, output } = form;
const assist = /** @type {HTMLInputElement} */ (document.getElementById("assist"));
assist.addEventListener("change", initAssistant, { passive: true });
initAssistant({ target: assist });
/**
 * Initialize assistant
 * @param {Event | { target: HTMLInputElement }} e 
 */
function initAssistant(e) {
	const checkbox = /** @type {HTMLInputElement} */ (e.target)
	// @ts-ignore
	code[checkbox.checked ? "addEventListener": "removeEventListener"]("keydown", assistInput);
}

/**
 * Replace inputted character with correspondence kana.
 * @param {KeyboardEvent} e 
 */
function assistInput(e) {
	if (bfChars.includes(e.key)) {
		e.preventDefault();
		const textarea = /** @type {HTMLTextAreaElement} */ (e.target);
		const cursor = textarea.selectionStart;
		const before = textarea.value.substr(0, cursor);
		const after = textarea.value.substr(cursor);
		textarea.value = before + bfNkoMap.get(e.key) + after;
	}
}
form.addEventListener("submit", e => {
	e.preventDefault();
	const button = /** @type {HTMLButtonElement} */ (e.submitter);
	switch (button.value) {
		case "run":
			if (worker) return false;
			try {
				return runCode();
			} catch (err) {
				output.value = err;
				return false;
			}
		case "reset":
			if (worker) {
				worker.terminate();
				worker = null;
			} else {
				output.value = '';
			}
			break;
		case "2nko": return convert2nko();
		case "2bf": return convert2bf();
	}
	return false;
});

function runCode() {
	output.value = '';
	worker = new Worker('worker.js');
	let isControl = false;
	let controls = '';
	worker.onmessage = e => {
		/** @type { { type: "char" | "complete" | "error", data: number | string } } */
		const { type, data } = e.data;
		switch (type) {
			case "char":
				const cursor = output.selectionStart;
				if (isControl === false) {
					if (data < 0x20) {
						switch (data) {
							case 0x0a:
								if (cursor === output.textLength) {
									output.value += '\n';
								}
								break;
							case 0x1b:
								isControl = true;
								break;
						}
					} else {
						const current = output.value.charAt(cursor);
						const char = String.fromCharCode(/** @type {number} */ (data));
						if (current !== char) {
							output.setRangeText(char, cursor, cursor + Number(current !== '\n'), 'end');
						}
					}
				} else {
					controls += String.fromCharCode(/** @type {number} */ (data));
					if ((0x40 < data && data < 0x5b) || data === 0x6d) {
						isControl = false;
						if (/^\[\d*;?\d*H$/.test(controls)) {
							const match = /^\[(\d*);?(\d*)H$/.exec(controls);
							if (match) {
								const [_, rowStr, colStr] = match;
								const row = parseInt(rowStr || '1') - 1, col = parseInt(colStr || '1') - 1;
								const lines = output.value.split('\n');
								for (let i = lines.length; i <= row; i++) {
									lines.push('');
								}
								if (col >= lines[row].length) {
									lines[row] += ' '.repeat(col - lines[row].length);
								}
								output.value = lines.join('\n');
								const pos = (row ? lines.slice(0, row).join('\n').length : 0) + col + 1;
								output.setSelectionRange(pos, pos);
							}
						} else if (/^\[\d*[JK]/.test(controls)) {
							const match = /^\[(\d*)([JK])/.exec(controls);
							if (match) {
								const int = parseInt(match[1] || '0');
								const char = match[2];
								if (char === 'J') {
									if (int === 0) {
										const before = output.value.substr(0, cursor);
										output.value = before;
									} else if (int === 1) {
										const after = output.value.substr(cursor);
										output.value = after;
									} else if (int === 2) {
										output.value = '';
									}
								} else {
									const prev = output.value.lastIndexOf('\n', cursor - 1);
									const next = output.value.indexOf('\n', cursor);
									if (int === 0) {
										output.setRangeText('', cursor, next < 0 ? output.textLength : next, "start");
									} else if (int === 1) {
										output.setRangeText('\n', prev < 0 ? 0 : prev, cursor, "end");
									} else if (int === 2) {
										output.setRangeText('\n', prev < 0 ? 0 : prev, next < 0 ? output.textLength : next, "end");
									}
								}
							}
						} else if (/^\[\d+[A-D]$/.test(controls)) {
							const match = /^\[(\d+)([A-D])$/.exec(controls);
							if (match) {
								const [_, deltaStr, dir] = match;
								const delta = parseInt(deltaStr);
								if (dir === 'A') {
									const before = output.value.substr(0, cursor);
									const lines = before.split('\n');
									const currentLine = lines.length;
									if (currentLine > 1) {
										const targetLines = lines.slice(0, currentLine - delta - 1);
										const pos = targetLines.join('\n').length;
										output.setSelectionRange(pos + 1, pos + 1);
									}
								} else if (dir === 'B') {
									const after = output.value.substr(cursor);
									const lines = after.split('\n');
									if (delta < lines.length) {
										const targetLines = lines.slice(0, delta - 1);
										const pos = cursor + targetLines.join('\n').length;
										output.setSelectionRange(pos, pos);
									}
								} else if (dir === 'C') {
									const pos = cursor + delta;
									output.setSelectionRange(pos, pos);
								} else if (dir === 'D') {
									const pos = cursor - delta;
									output.setSelectionRange(pos, pos);
								}
							}
						}
						controls = '';
					}
				}
				break;
			case "complete":
				if (worker) {
					worker.terminate();
					worker = null;
				}
				console.log(`NKOLANG Completed: ${data}ms`);
				return true;
			case "error":
				if (worker) {
					worker.terminate();
					worker = null;
				}
				output.value = /** @type {string} */ (data);
				return false;
		}
	};
	worker.postMessage(code.value);
	return false;
}

function convert2nko() {
	let str = code.value;
	for (const [bf, nko] of bfNkoMap.entries()) {
		str = str.replace(new RegExp('\\' + bf, 'g'), nko);
	}
	output.value = str;
	return true;
}
function convert2bf() {
	let str = code.value;
	for (const [bf, nko] of bfNkoMap.entries()) {
		str = str.replace(new RegExp(nko, 'g'), bf);
	}
	output.value = str;
	return true;
}