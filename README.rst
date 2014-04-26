KinectFaceTraking2D ANE light
================================================

このコードは Windows 用の ANE　を作成する際のサンプルです。

ANE（ActionScript Native Extensions）があればFlash ( AIR ) からKinectの顔認識情報を活用できます。


ANEからKinectの情報を得るために仲介ライブラリとしてDLLを作成しています。

こちらは 中村薫さんのコードを参考にさせていただいた上、添削までしていただきました。
ありがとうございます！！

kaorun55 / KinectSensor.h

https://gist.github.com/kaorun55/c38970b82cb76d8f9b1f


ANEのインターフェイス部分もAdobeサイトにあるString や Numberのみならず、VectorやRect型のデータ取得方法についても書かれています。

より実践的な内容になっています。

サンプルプログラムとソースコードも収録しました。

Kinectの開発環境を整えた上でお試しください。

※環境変数のPATHにkinectランタイム内のDLLへPATHが通っていることもお忘れなく！



Kinectから得られる情報
--------------
#. 顔のエリアを囲む4点をRectとして
#. 顔の中の100個の識別点をVector.<Point>として


サンプルプログラム
--------------
kinectFTsample\kinectFTsample.air

内のkinectFTsample.exeを起動してください。

※キャプティブランタイムになっていますのでAIRのランタイムは不要です。


ライセンス
--------------
ライセンスは｢MIT ライセンス｣とします。

http://ja.wikipedia.org/wiki/MIT_License



免責事項
--------------
本ソフトウェアは使用者の責任において利用してください。

このプログラムによって発生した、いかなる障害・損害も作成者は一切責任を負わないものとします。

環境
---------------
#. Windows 7
#. VisualStudio2012
#. C++
#. FlashDevelop v4.5.2.5
#. ActionScript 3.0 
#. AIR 3.6
#. Kinect SDK v1.8　