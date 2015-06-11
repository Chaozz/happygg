## Happygg

happygg是一个中国象棋引擎程序。名字意为该引擎的结局--欢声笑语中打出gg。

### 引擎协议

Happygg支持UCCI 3.0协议。

#### 支持的UCCI命令

ucci
position {fen <fen_str> | startpos} [moves <move_list>]
banmoves <move_list>
go [ponder | draw] ...
quit

#### 可以返回的UCCI信息

id {name <engine_name> | version <version_name> | copyright <copyright_info> | author <author_name> | user <user_name>}
ucciok
{nobestmove | bestmove <best_move> [ponder <ponder_move>] [draw | resign]}
bye

### Build

#### Linux and Mac OSX

```sh
git clone https://github.com/Chaozz/happygg.git
cd src
make
```

#### Windows

开发环境为vs2010，项目文件和源码在/win-src文件夹。

### 使用

用户可以通过支持UCCI的中国象棋界面程序调用该引擎。
作者在开发与测试中使用的是跨平台中国象棋界面程序"ChessQ"[(https://github.com/walker8088/ChessQ)](https://github.com/walker8088/ChessQ)。

