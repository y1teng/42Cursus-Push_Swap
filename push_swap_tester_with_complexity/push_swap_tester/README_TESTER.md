# push_swap tester

`push_swap` subject v1.1 と Review 要件をまとめた、Python 3だけで動くテスターです。
`checker_linux` がなくても、11命令を内部で再現して正しくソートできたか検証します。

## 使い方

テスターをリポジトリ外に置いたまま実行できます。

```bash
python3 push_swap_tester.py /path/to/push_swap --mode quick
python3 push_swap_tester.py /path/to/push_swap --mode mandatory
python3 push_swap_tester.py /path/to/push_swap --mode all
```

同梱のラッパーを使う場合：

```bash
./run_tester.sh /path/to/push_swap --mode mandatory
```

終了コードは、全自動テストに失敗がなければ `0`、1件以上失敗すると `1` です。

## モード

- `quick`: ビルド、Norm、README、エラー処理、4戦略、3個・5個、bench、Valgrind。100個・500個は省略。
- `mandatory`: `quick` に加えて、100個を3回、500個を2回テスト。デフォルト。
- `performance`: 100個・500個の正答性と命令数を中心に確認。
- `bonus`: `make bonus` で `checker` を作り、エラー・OK・KO・連携を確認。checkerがない場合は失敗。
- `all`: mandatory と bonus。bonus未実装なら bonus はスキップ扱い。

## 主な検証内容

- Makefile: `NAME`, `all`, `clean`, `fclean`, `re`, `-Wall -Wextra -Werror`, 不要な再リンク
- README: 規定の先頭行、Description / Instructions / Resources、AI利用、4戦略、貢献記録、2人のlogin
- エラー: 非数、重複、INT範囲外、空文字、引数なし、stdout/stderrの分離
- 出力: `sa sb ss pa pb ra rb rr rra rrb rrr` 以外を拒否し、各行末の改行も確認
- 戦略: `--simple`, `--medium`, `--complex`, `--adaptive`, フラグなし
- 既ソート入力: 命令数0
- 3個: 全6順列、最大5命令
- 5個: Review記載の3ケース、最大15命令
- bench: stderr出力、disorder、戦略名、複雑性、総命令数、11命令別カウント
- 100個: 全試行で2000未満
- 500個: 全試行で12000未満
- Valgrind: インストール済みの場合、valid/error入力の definite/indirect leak を確認
- bonus checker: エラー、既ソートOK、既知のOK/KO、push_swap出力との連携

## 追加オプション

```bash
# 5個の全120順列も検証
./run_tester.sh . --mode quick --exhaustive-five

# Review当日のライブコーディング後に --count-only を検証
./run_tester.sh . --mode quick --include-live

# 再現可能なJSONレポート
./run_tester.sh . --mode mandatory --seed 42 --json-report report.json

# NorminetteやValgrindがない環境
./run_tester.sh . --mode mandatory --skip-norm --skip-valgrind
```

## 注意

- ビルド検証では `make fclean`, `make`, `make clean`, `make fclean`, `make re` を実行します。最後はビルド済みの状態になります。
- `--include-live` は通常の提出要件ではなく、Review中に追加する `--count-only` の確認用です。実装前には付けません。
- Git author数は登録学習者数と一致しない場合があるため、`git shortlog` は警告に留めています。Intra上の参加者2名は手動確認が必要です。
- 計算量の説明、両名が全コードを説明できるか、禁止関数・グローバル変数の有無は、最終的には人間による確認が必要です。

## 手数計算量の実測

計算量専用の `complexity_tester.py` と `run_complexity.sh` を同梱しています。

```bash
./run_complexity.sh /path/to/push_swap
```

入力サイズを増やしながら4戦略の手数を測定し、`n²`、`n√n`、`n log₂n` で正規化します。
adaptive は disorder 0.10、0.35、0.75 の入力を生成して3領域を別々に確認します。
CSV、JSON、Markdown、SVGグラフを `complexity_report/` に出力します。

詳細は [`README_COMPLEXITY.md`](README_COMPLEXITY.md) を参照してください。
有限回の測定だけでBig-Oを数学的に証明することはできないため、Defenseではコードからの上界説明も必要です。
