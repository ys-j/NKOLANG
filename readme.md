# NKOLANG
Brainfuck風プログラミング言語（インタープリタ）

Brainfuck-like Programming Language (Interpreter)

ゲーム「NKODICE」に感銘を受け製作しました。

## Brainfuckとの対応

| Brainfuck | NKOLANG | 説明 |
| --------- | ------- | ---- |
| > | こ | ポインタをインクリメントする |
| < | う | ポインタをデクリメントする |
| + | ち | ポインタの指す値をインクリメントする |
| - | ん | ポインタの指す値をデクリメントする |
| [ | お | ポインタの指す値が0ならば対応する「ま」の直後へ移動する |
| ] | ま | 対応する「お」へ移動する |
| . | ぽ | ポインタの指す値を文字として出力する |
| , | ！ | 入力した文字をポインタの指す値へ格納する |

普通に書く分にはおそらく不適切な単語は出現しません（せいぜい💩こぐらい）。

## 使い方

```
nko run hello.nko [memory_capacity]
- memory_capacity = (number) * 1000 Byte  (Default is 30)

Exmaple: nko run chi.nko 18cm
  Allocate 18KB memory capacity, and run "chi.nko".

nko bf2nko hello.bf [output_name.nko]
nko nko2bf hello.nko [output_name.bf]
```

nkoファイルはUTF-16(LE)で作成してください。変換するbfファイルは、ASCII互換で作成してください。

## Webインタープリタ

[GitHub Page](//ys-j.github.io/NKOLANG/web/)

※画面制御は ␛[`r`;`c`H （カーソルを`r`行`c`列に移動）のみ確認しています。他は不具合があると思います。