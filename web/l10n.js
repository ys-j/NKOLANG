const supports = ["ja"];
const l10n = {
	"NKOLANG_Interpreter": {
		"ja": "NKOLANGインタープリタ"
	},
	"Input": {
		"ja": "入力"
	},
	"Assistant": {
		"ja": "アシスト"
	},
	"InputAssistant_Description": {
		"ja": "（記号を入力すると対応文字に自動変換されます）"
	},
	"Desc": {
		"ja": "説明"
	},
	"show": {
		"ja": "を表示する"
	},
	"Quick_Reference": {
		"ja": "早見表"
	},
	"Char": {
		"ja": "文字"
	},
	"Sym": {
		"ja": "入力"
	},
	"Description": {
		"ja": "説明"
	},
	"Increment_pointer": {
		"ja": "ポインタをインクリメントする"
	},
	"Decrement_pointer": {
		"ja": "ポインタをデクリメントする"
	},
	"Increment_byte": {
		"ja": "ポインタの指す値をインクリメントする"
	},
	"Decrement_byte": {
		"ja": "ポインタの指す値をデクリメントする"
	},
	"Begin_loop": {
		"ja": "ポインタの指す値が0ならば対応する「ま」の直後へ移動する"
	},
	"End_loop": {
		"ja": "対応する「お」へ移動する"
	},
	"Output_byte": {
		"ja": "ポインタの指す値を文字として出力する"
	},
	"Input_byte": {
		"ja": "入力した文字をポインタの指す値へ格納する"
	},
	"Run": {
		"ja": "実行"
	},
	"Reset": {
		"ja": "リセット"
	},
	"Convert_to_NKO": {
		"ja": "NKOLANG形式に変換"
	},
	"Convert_to_BF": {
		"ja": "Brainfuck形式に変換"
	}
};

const acceptables = new Set(navigator.languages.map(l => l.slice(0, 2)));
/** @type {string?} */
let target = null;
for (const acceptable of acceptables) {
	if (supports.includes(acceptable)) {
		target = acceptable;
		break;
	}
}
if (target) {
	const elements = /** @type {NodeListOf<HTMLElement>} */ (document.querySelectorAll('[data-l10n]'));
	for (const elem of elements) {
		const l10nId = elem.dataset.l10n;
		if (l10nId) {
			const msg = l10n[l10nId];
			if (msg && msg[target]) {
				elem.textContent = msg[target];
			}
		}
	}
}