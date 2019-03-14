# BoneUtil (Shade3D Plugin)

Shade3Dでボーンを制御するユーティリティプラグインです。     
Shade3Dマーケットプレイスで公開している無料プラグインのソースコードになります。    

## Shade3Dマーケットプレイスでの公開場所

https://shade3d.jp/store/marketplace/ft-lab/boneutil/boneutil.html

## 開発環境

### 使用SDK

Shade 15.1 Plugin SDK (481137)    
https://github.com/shadedev/pluginsdk    

本プラグインのプロジェクトは、Visual Studio 2017でビルドできるようにしています。    

### Windows

Visual Studio 2017 

### Mac OS X

まだ。    

## 動作確認したShade3Dのバージョン

ver.16/17/18    

## ビルド方法

GitHubからダウンロードしたソースのprojectsフォルダ内の「BoneUtil」一式を、    
Shade 15.1 Plugin SDKの「projects」フォルダにコピーします。     

### Windows

Visual Studio 2017で「/projects/BoneUtil/win/Template.sln」を開き、ビルドします。    
Debug 64bit/Release 64bitでビルドするようにします。     
ビルドで生成された「BoneUtil64.dll」をShade3DのpluginsフォルダにコピーしてShade3Dを起動します。    

### Mac OS X

まだ。     


