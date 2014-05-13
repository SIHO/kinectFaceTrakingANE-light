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


This code is a sample at the time of creating ANE for Windows. 

If there is ANE (ActionScript Native Extensions), the facial recognition information on Flash (AIR) to Kinect is utilizable. 

In order to acquire the information on Kinect from ANE, DLL is created as an agency library. 

Here I was allowed to refer to Mr. Kaoru Nakamura's code, and also had you carry out to correction. 
Thank you!! 

kaorun55 / KinectSensor.h

https://gist.github.com/kaorun55/c38970b82cb76d8f9b1f


It is written also about the data acquisition method of String, and not only Number but Vector which also have an interface section of ANE in an Adobe site, or a Rect type. 

The sample program and the source code were also recorded. 

Please try after improving the development environment of Kinect. 

* he also leaves that PATH passes to DLL within kinect RunTime in PATH of an environment variable -- Don't forget it! 



Kinectから得られる情報 / Information acquired from Kinect 
--------------
#. 顔のエリアを囲む4点をRectとして
#. 顔の中の100個の識別点をVector.<Point>として


#. Set four surrounding the area of a face to Rect.
#. Make 100 identifying points in a face into Vector.<Point>. 



サンプルプログラム / Sample program 
--------------
kinectFTsample\kinectFTsample.air内のkinectFTsample.exeを起動してください。

※キャプティブランタイムになっていますのでAIRのランタイムは不要です。


Please start kinectFTsample.exe in kinectFTsample\kinectFTsample.air. 

* Since CaptiveRunTime comes, the run time of AIR is unnecessary. 



ライセンス / License
--------------
ライセンスは｢MIT ライセンス｣とします。

http://ja.wikipedia.org/wiki/MIT_License


A license is taken as "the MIT license."
 
http://ja.wikipedia.org/wiki/MIT_License



免責事項 / Qualification
--------------
本ソフトウェアは使用者の責任において利用してください。
このプログラムによって発生した、いかなる障害・損害も作成者は一切責任を負わないものとします。


Please use this software in a user's responsibility. 
In any obstacle and damage which occurred by this program, a maker makes responsibility that of a negative trap potato entirely. 



開発環境 / Development Environment
---------------
#. Windows 7
#. VisualStudio2012
#. C++
#. FlashDevelop v4.5.2.5
#. ActionScript 3.0 
#. AIR 3.6
#. Kinect SDK v1.8　



補足：顔認識以外のKinectの情報取得について / Supplement:About information acquisition of Kinect(s) other than facial recognition 

---------------
顔認識以外のKinectの情報を取得するには、先人が開発した下記のANEが存在します。

こちらは開発がとまっているようですが、Kinectv1.5時点でのほぼ全ての機能を網羅しています。

AS3NUIに顔認識は含まれていませんでしたので、独自開発いたしました。

AS3NUIとの併用が可能かどうかについては未検証ですので、試された方は教えていただけると嬉しいです。

AS3NUI <AIRKinect>

http://as3nui.github.io/airkinect-2-core/

パッケージに関する情報は下記サイトを参考にどうぞ。

http://as3nui.github.io/airkinect-2-core/docs/


In order to obtain the information on Kinect(s) other than facial recognition, following ANE which predecessors developed exists. 

Although it seems that development has stopped, almost all the functions in Kinectv1.5 time are covered here. 

Since facial recognition was not included in AS3NUI, I developed originally. 

Since it has not verified about whether combined use with AS3NUI is possible, if you teach the tried direction, it is glad. 

AS3NUI <AIRKinect>

http://as3nui.github.io/airkinect-2-core/

the information about a package is reference about the following site -- pleasing . 

http://as3nui.github.io/airkinect-2-core/docs/