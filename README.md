# Manosaba Plugin Loader
Pre-Release

---

## Description
名前の通り、ゲーム「魔法少女ノ魔女裁判」のPluginローダです。現状は、ハードコードされたPluginしかパッチできません。ただ、ストーリー読破するまでPluginとか作りたくないので、先にコードを公開してしまいます。  
まだ2章1話までしかやってないのでネタバレしないでください。

---

## 動作確認環境
|OS|Loader Version|Game Version|
|:--|:--|:--|
| Windows 11 (x64) | 0.0 | 1.1.0, 1.1.1 |

---

## Usage
1. manosaba.exeと同じ階層にある **UnityPlayer.dll** を **UnityPlayer_original.dll** にリネーム。
2. Releaseページからダウンロードした **UnityPlayer.dll** を上と同じ階層に配置。

---

## Build
このプロジェクトは **MinHook** を使用しています。コンパイルにはincludeフォルダ内に **[MinHook.h](https://github.com/TsudaKageyu/minhook/blob/256a4a83df6344ccbddb6c0e15ad89260a3cd279/include/MinHook.h)** 、libフォルダに **[libMinHook.x64.lib](https://github.com/TsudaKageyu/minhook/releases/tag/v1.3.4)** を配置してください。  
また、C++17で開発しているため、他のバージョンでは動作しない可能性があります。
```
cl UnityPlayerProxy.cpp /std:c++17 /EHsc /LD /Fe:UnityPlayer.dll /I .\include /link user32.lib .\lib\libMinHook.x64.lib /LTCG /MACHINE:X64
```
**Visual Studio 2022 Developer Command Prompt** 推奨です。他のコンパイラ、コンソールでのコンパイルは未確認です。
というか、PC向けにC++の開発したの初めてなのでこの辺よくわかってません。

---

## References
[MinHook](https://github.com/TsudaKageyu/minhook)  
[UnityPlayer (DDLCPlus)](https://github.com/DDLCPlus/UnityPlayer/blob/main/dllmain.cpp)
