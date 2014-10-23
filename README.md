procon2014_ut
=============

高専プロコン2014競技部門の東大チームの公開用レポジトリです．このプロジェクトは MIT License で公開されています．詳しくは LICENSE.md を見てください．

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

### options for CMake

cmake に渡すことの出来る主なオプションは以下です．以下のオプションの設定はキャッシュされ，変更しない限り持続します．

* -DCMAKE\_BUILD\_TYPE=&lt;build\_type&gt; : ビルドの種類を &lt;build\_type&gt; に設定します．
	* &lt;build\_type&gt; = Debug : デバッグビルドを行います．様々なデバッグオプションが ON になり，バグの発見をし易くなりますが，実行速度が極端に落ちます．全ての assert は有効になります．
	* &lt;build\_type&gt; = RelWithDebInfo（デフォルト） : デバッグ情報を残しつつ，最適化をかけます．全ての assert は有効になります．
	* &lt;build\_type&gt; = Release : 最適化を完全に ON にします．最も実行速度が早くなります．全ての assert は無効になります．
* -DUSE\_TC\_MALLOC=&lt;value&gt; : ON/1 に設定すると，TC Malloc がインストールされている場合にそれをリンクします．OFF/0 に設定すると，TC Malloc がインストールされている場合でもそれをリンクしません．デフォルトは ON です．
* -DUSE\_CPU\_PROFILER=&lt;value&gt; : ON/1 に設定すると，gperftool がインストールされている場合に，CPU プロファイラをリンクします．OFF/0 に設定すると，gperftools がインストールされている場合でも CPU プロファイラをリンクしません．デフォルトは OFF です．
* -DUSE\_FAST\_BRANCH=&lt;value&gt; : ON/1 に設定すると，ビルドが高速になりますが，解ける問題のサイズが特定の十数種類に限られます（slide/branch.hpp を参照してください）．OFF/0 に設定すると，ビルドが非常に低速になりますが，2×2 以上 16×16 以下の任意のサイズの問題を解けるようになります．デフォルトは OFF です．
