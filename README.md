procon2014_ut
=============

高専プロコン2014競技部門の東大チームの公開用レポジトリです．このプロジェクトは MIT License で公開されています．詳しくは LICENSE.md を見てください．

質問・要望・意見等は [@natrium11321](https://twitter.com/natrium11321) の方までお気軽にどうぞ．

### environment

このプロジェクトは，以下の環境で動作することを確認しています．

* OS: Mac OS X 10.9 Mavericks, コンパイラ: apple clang 6.0
* OS: Ubuntu 14.04 LTS, コンパイラ: gcc 4.8

このプロジェクトをビルドするためには，以下がインストールされている必要があります．Mac なら全て homebrew でインストールすることが出来ます．

* [CMake](http://www.cmake.org/)
* [boost](http://www.boost.org/)
* [Intel TBB](https://www.threadingbuildingblocks.org/)

以下がインストールされていると，GUI アプリケーションををビルドすることが出来ます．

* [Qt 5](http://qt-project.org/qt5)

以下がインストールされていると，動作が速くなることがあります．

* [gperftools](https://code.google.com/p/gperftools/)

### build

solver/ に移動します．

```
cd solver
```

solver/ にビルド用のディレクトリ（build など）を作り，そこに移動します．

```
mkdir build
cd build
```

cmake を実行し，Makefile を作成します．

```
cmake ..
```

make します．USE_FAST_BRANCH（後述）を ON にしていない場合は，1時間程度かかります．

```
make -j4
```

### run console application

ビルドを行ったら，`bin/run_slide` を用いることでスライドパズルを解くことが出来ます．例えば，盤面サイズ 16×16，交換コスト 5, 選択コスト 200, 最大選択回数 8 の問題をランダムに生成して dragon solver に解かせるには，ビルドしたディレクトリから以下のコマンドを実行します．

```
bin/run_slide -w16 -h16 -a5 -b200 -l8 -s dragon
```

詳しい使い方は，`bin/run_slide —-help` を見てください．

### run GUI application

Qt 5 がインストールされている場合は，`bin/procon2014-qt-slide` 又は `bin/procon2014-qt-slide.app` がビルドされます．これらはGUI アプリケーションであり，直接実行するか，または Mac なら open の引数に渡すことで起動することが出来ます．  
GUIアプリケーションを起動後，右側にあるサイズと選択上限，各コストを適切に設定し，Reloadボタンを押す事でその設定の盤面が読み込まれます．Shuffleボタンを押すと盤面がシャッフルされます．問題を解く場合は右下にあるSolverから適切なソルバーを選択し，Solveボタンを押します．問題によっては数秒から数分以上かかります（例えばExact Solverでは最適解を求めようとするため盤面のサイズが大きいと数時間経っても解けません）．Solverは別スレッドで動作させていますが，終了手段を用意していないため，強制終了させる場合には該当プロセスをKILLする必要があります．GUIのメインウィンドウを終了しただけではスレッドが残っている可能性があることに注意してください．問題が解き終わると右下のPlayボタンを押す事で求めた手順が再生されます．再生時間は上のAnimation Timeで調節できます．再生をすぐに終わらせる場合はAnimation Timeを0にします．手順の再生が終了後，右下のRestoreボタンを押せば問題の盤面が復元されるので，別のソルバーで問題を解き直すことや，再び解答を再生することができます．  
GUIアプリケーションはメニューバーの`ファイル > 開く`から以下の形式の問題を読み込むこともできます．

```
<幅> <高さ> <選択上限> <選択コスト> <交換コスト>
[スライド盤面 (16進数)]
```

ここで，16進数の値は1桁目がx座標，2桁目がy座標に対応します．
例えば以下の用なファイルに対応しています．

```
3 3 3 1 1
00 21 20
22 10 01
02 11 12
```

また，GUIアプリケーションを起動したディレクトリにresources/があるとそこから練習問題を読み込みます．  
盤面はマウスで操作することも可能です． なお，左下にあるネットワーク機能は大会用で，単体で使用する際には意味がありません．

### network module

`modules/network`以下には通信用モジュールのソースがあります．本番ではこれを利用してスライドパズルの問題を3台のパソコンに渡して（3台別々のソルバーで）解きました．
`exe/post_client.cpp`，`exe/post_server.cpp`，および`exe/post_client.cpp`，`exe/post_server.cpp`は通信部分のテスト用プログラムです．

### options for CMake

cmake に渡すことの出来る主なオプションは以下です．以下のオプションの設定はキャッシュされ，変更しない限り持続します．

* -DCMAKE\_BUILD\_TYPE=&lt;build\_type&gt; : ビルドの種類を &lt;build\_type&gt; に設定します．
	* &lt;build\_type&gt; = Debug : デバッグビルドを行います．様々なデバッグオプションが ON になり，バグの発見をし易くなりますが，実行速度が極端に落ちます．全ての assert は有効になります．
	* &lt;build\_type&gt; = RelWithDebInfo（デフォルト） : デバッグ情報を残しつつ，最適化をかけます．全ての assert は有効になります．
	* &lt;build\_type&gt; = Release : 最適化を完全に ON にします．最も実行速度が早くなります．全ての assert は無効になります．
* -DUSE\_TC\_MALLOC=&lt;value&gt; : ON/1 に設定すると，TC Malloc がインストールされている場合にそれをリンクします．OFF/0 に設定すると，TC Malloc がインストールされている場合でもそれをリンクしません．デフォルトは ON です．
* -DUSE\_CPU\_PROFILER=&lt;value&gt; : ON/1 に設定すると，gperftool がインストールされている場合に，CPU プロファイラをリンクします．OFF/0 に設定すると，gperftools がインストールされている場合でも CPU プロファイラをリンクしません．デフォルトは OFF です．
* -DUSE\_FAST\_BRANCH=&lt;value&gt; : ON/1 に設定すると，ビルドが高速になりますが，解ける問題のサイズが特定の十数種類に限られます（slide/branch.hpp を参照してください）．OFF/0 に設定すると，ビルドが非常に低速になりますが，2×2 以上 16×16 以下の任意のサイズの問題を解けるようになります．デフォルトは OFF です．
