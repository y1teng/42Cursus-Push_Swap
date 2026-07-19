*This project has been created as part of the 42 curriculum by ayaito, ykomori.*

# Push_swap

## Description

<!-- 2〜4文で十分: 何をするプロジェクトか -->
2つのスタックと限られた操作（sa, pb, ra など）だけを使い、整数列を最小の操作数でソートする操作列を出力するプログラム。
入力の乱れ具合（disorder）に応じて4つの戦略を切り替える。

## Instructions

```bash
make            # push_swap をビルド
./push_swap 2 1 3 6 5 8                  # 操作列を stdout に出力
./push_swap --simple 5 4 3 2 1           # O(n^2) 戦略を強制
./push_swap --medium 5 4 3 2 1           # O(n√n) 戦略を強制
./push_swap --complex 5 4 3 2 1          # O(n log n) 戦略を強制
./push_swap --adaptive 5 4 3 2 1         # disorderで自動選択（デフォルト）
./push_swap --bench <args> 2> bench.txt  # ベンチマークを stderr に出力
make clean / fclean / re
```

検証例:

```bash
ARG="4 67 3 87 23"; ./push_swap $ARG | ./checker_linux $ARG   # → OK
```

## Algorithms


### Complex — O(n log n)
**数を2進数として見て、下の桁から1ビットずつ「0のグループ」と「1のグループ」に
仕分けることを繰り返すソート**。比較（どっちが大きい？）を一切せず、
各要素のビットだけを見て機械的に動くのが特徴。

処理は2段階：

1. **前処理（ランク化）**: 各値を「小さい方から何番目か」(0〜n-1) に置き換える
2. **本体（ビットごとの仕分け）**: bit 0 から最上位ビットまで、1ビットにつき1パス
---

#
## Resources

### AI利用について

<!-- 必須: どのタスク・どの部分で使ったかを具体的に -->
## Contributions

- **ayaito**: <担当箇所: 例 パース、スタック実装（循環バッファ）、
  操作関数、Simple/Medium戦略、sort_five、Makefile>
- **ykomori**: <担当箇所:Complex戦略（基数ソート）、sort_three、README>
