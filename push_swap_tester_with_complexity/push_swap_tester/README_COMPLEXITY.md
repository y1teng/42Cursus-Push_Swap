# push_swap 手数計算量テスター

`complexity_tester.py` は、入力サイズ `n` を段階的に増やし、`push_swap` が出力した命令数の増え方を測定します。
すべての命令列を内部シミュレーターで実行し、stack A が昇順、stack B が空になることも確認します。

## 先に重要な点

有限回の実験だけでは、Big-O の上界を数学的に証明できません。
このテスターができるのは次の2点です。

1. 実測値が主張する計算量と整合している証拠を作る。
2. 実測値が明らかに速く増えすぎる場合、主張への反例候補を見つける。

Defense では、テスターのグラフ・表に加えて、コードから手数の上界を説明してください。

## 基本実行

```bash
./run_complexity.sh /path/to/push_swap
```

既定では以下を実行します。

- 入力サイズ: `16, 32, 64, 128, 256`
- 各サイズ7回
- `--simple`, `--medium`, `--complex`, `--adaptive`
- 強制戦略: random、reverse
- adaptive: disorder 0.10、0.35、0.75
- 各サイズで観測した最大手数を計算量判定に使用
- `--bench` の戦略名・複雑性表記も確認

実行後、カレントディレクトリの `complexity_report/` にレポートが生成されます。

## 推奨コマンド

### まず短く動作確認

```bash
./run_complexity.sh . \
  --sizes 8,16,32,64 \
  --runs 3 \
  --out-dir complexity_quick
```

### Defense 用の比較

```bash
./run_complexity.sh . \
  --sizes 16,32,64,128,256,512 \
  --runs 15 \
  --out-dir complexity_defense
```

`--simple` が遅すぎる場合は、戦略ごとにサイズを分けます。

```bash
./run_complexity.sh . \
  --strategies simple \
  --sizes 16,32,64,128,256 \
  --runs 15 \
  --out-dir complexity_simple

./run_complexity.sh . \
  --strategies medium,complex \
  --sizes 32,64,128,256,512,1024 \
  --runs 15 \
  --out-dir complexity_fast
```

### adaptive の3領域だけ確認

```bash
./run_complexity.sh . \
  --strategies adaptive \
  --adaptive-scenarios low,medium,high \
  --sizes 16,32,64,128,256,512 \
  --runs 15 \
  --out-dir complexity_adaptive
```

### bench未実装の途中段階

```bash
./run_complexity.sh . --no-bench
```

## 何を比較しているか

| 戦略 | 主張する手数 | `n` を2倍にしたときの目安 |
|---|---:|---:|
| simple | `O(n²)` | 約4倍 |
| medium | `O(n√n)` | 約`2√2 ≈ 2.828`倍 |
| complex | `O(n log₂n)` | 約2倍より少し大きい |

単純に倍率だけを見ると、小さい `n`、固定コスト、特殊入力で誤判定しやすいため、次も出力します。

- `operations / n²`
- `operations / (n√n)`
- `operations / (n log₂n)`
- log-log 回帰の傾き
- 主張する基底に対するR²
- 大きいサイズ側での正規化値の広がり
- 正答性失敗数
- benchの戦略・複雑性不一致数

正規化した値が、入力サイズを増やしても一定範囲に収まることが、主張する上界と整合する証拠になります。

## adaptive の入力

テスターは反転数を指定した permutation を生成します。

- low: disorder ≈ `0.10` → `O(n²)` 領域
- medium: disorder ≈ `0.35` → `O(n√n)` 領域
- high: disorder ≈ `0.75` → `O(n log n)` 領域

反転数から生成しているため、単なる「少しシャッフルした配列」よりも、狙った disorder を再現しやすくなっています。

## 出力ファイル

- `complexity_report.md`: Defenseで読みやすい表と判定
- `complexity_report.json`: 全データ
- `complexity_samples.csv`: 1実行ごとの生データ
- `complexity_summary.csv`: サイズごとの集計
- `chart_<strategy>_<scenario>.svg`: log-log グラフ

## 判定ラベル

- `SUPPORTED`: テスト範囲では主張する増加率と整合
- `INCONCLUSIVE`: サイズや試行数が不足、または傾向が曖昧
- `CONTRADICTED`: 正答性に失敗、または手数が主張より明確に速く増加

`SUPPORTED` は証明済みという意味ではありません。

## テスター自身の確認

```bash
python3 complexity_tester.py --self-test
```

反転数付き入力生成、命令シミュレーター、複雑性文字列解析を確認します。
