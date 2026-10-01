# plugin-snapshot

JUCEプラグインのエディタを、ウィンドウを開かずにPNGで書き出すツール。READMEに載せるスクショ用。
ウィンドウ枠やStandaloneのOptionsバーは写らず、エディタ部分だけが撮れる。

## 使い方

```bash
# パラメータID・範囲・デフォルト値の一覧
./snap.sh ../../plugins/Saturator-1 --list

# 撮影(値はパラメータの実単位。Bool/Choiceは0,1,2...のインデックス)
./snap.sh ../../plugins/Saturator-1 docs/images/main.png --scale 1.5 drive=0.62 mix=80 volume=-3
```

| オプション | 内容 |
| --- | --- |
| `--scale S` | エディタのデフォルトサイズに対する倍率(READMEには1.5くらいがきれい) |
| `--width W --height H` | サイズを直接指定 |
| `paramID=value` | 撮影前にパラメータを設定(複数指定可) |

## 仕組み

- 対象プラグインの`CMakeLists.txt`を`add_subdirectory`で取り込み、`juce_add_plugin`のターゲット
  (共有コードの静的ライブラリ)にリンクする。ターゲット名は`CMakeLists.txt`から自動検出
- `createPluginFilter()` → `createEditorIfNeeded()` → `createComponentSnapshot()`でPNGに書き出す
- ビルドは`build/<ターゲット名>/`にキャッシュされる。初回はJUCEごとビルドするので数分かかる

## 注意

- 音声を流していないので、アナライザーやメーターは空になる
- Timerで遅れて反映されるUIは、撮影時点ではまだ更新されていないことがある
- プラグインのディレクトリは`../JUCE`を参照できる配置(`plugins/`配下)である必要がある
