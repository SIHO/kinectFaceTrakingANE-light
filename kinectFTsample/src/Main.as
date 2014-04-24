package 
{
	import flash.display.Sprite;
	import flash.events.Event;
	import jp.vivamambo.KinectFaceTrakingLight;
	import flash.text.TextField;
	import flash.geom.Rectangle;
	import flash.geom.Point;
	import mx.events.CloseEvent;
	import flash.desktop.NativeApplication;
	import flash.display.Graphics;
	
	/**
	 * ...
	 * @author SIHO from viva mambo,inc.
	 */
	[SWF(width=600,height=400,frameRate=60)]
	public class Main extends Sprite 
	{
		private var ext:KinectFaceTrakingLight;
		private var str:String;
		
		private var tf:TextField = new TextField();
		private var tf2:TextField = new TextField();
		private var tf3:TextField = new TextField();
		private var tfArray:Array = [];
		private var tiltMotor:Boolean = false;
		private var nearMode:Boolean = true;
		
		private var _cnt:int = 0;
		
		private var KINECT_INIT:Boolean = false; 
		
		//Rect描画用
		private var rectDrawArea:Sprite = new Sprite();
		
		//顔の識別点描画用
		private var pointDrawArea:Sprite = new Sprite();
		
		public function Main():void 
		{
			addChild(new Stats());
			ext = new KinectFaceTrakingLight();
			kinectInit();
			
			/*drawAreaをステージに追加
			 * */
			addChild(rectDrawArea);
			addChild(pointDrawArea);
			
			tf.width = tf2.width = tf3.width = 300;
			tf.y = 100 ; tf2.y = 110; tf3.y = 120;
			addChild(tf);
			addChild(tf2);
			addChild(tf3);
			
			//addEventListener(Event.ENTER_FRAME, enterFrameHandler);
			//stage.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownHandler);
			//stage.nativeWindow.addEventListener(Event.CLOSING, onClosingHandler);
			
			setTFnoView();
			
			// exit
			NativeApplication.nativeApplication.addEventListener(Event.EXITING, onExit);
		}
		public function kinectInit():void { 
			
			/**
			 * Kinectの初期化/引数はNearMode, trueでNearMode ON ,falseでOFF　起動時のみ有効。
			 * #NearMode ON はKinect For Windowsでのみ利用可能。 Xboxでは常にfalseにすること。
			 * 戻り値はkinectが有効かどうか
			 */
			var res:Boolean = ext.initKinect(nearMode);
			trace("initKinect:res ", res);
			
			/**
			 * 簡易テスト用　戻り値「Hello World」
			 */
			str = ext.say(); 
			trace(str);
			KINECT_INIT = true;

			addEventListener(Event.ENTER_FRAME, enterFrameHandler);
			//Tween24.serial(Tween24.func(addEventListener,Event.ENTER_FRAME,enterFrameHandler)).delay(4).play();
			
			//Tween24.serial(Tween24.func(mouseDownHandler)).delay(25).play();
		}

		private function setTFnoView():void 
		{
			for (var i:int = 0; i < 100; i++) 
			{
				var tf:TextField = new TextField();
				tf.width = 300;
				addChild(tf);
				tfArray.push(tf);
			}
		}

		private function enterFrameHandler(e:Event):void 
		{
			_cnt++;
			if(_cnt<5)return
			var num:int = int(tf.text) + 1;
			tf.text = String(num);
			tf2.text = "tiltMotorTraking:" + String(tiltMotor);
			
			/**
			 * KinectのTrakingデータの更新メソッド
			 * dllの中でwhile()処理で行うとfocusが戻ってこないのでFlash側から呼ぶ
			 * */
			//引数のBool値は TiltMotorの有効化を示しています。trueにするとKinectが鼻のポイントをセンターに追従します。
			//ただ、この処理は重いので、適当なタイミングでfalseにすることをおすすめします。
			var res:Boolean = ext.startUpdate(tiltMotor);
			if(res ==true)getKinectFaceTrakingData();
			_cnt = 0;
		}
		
		public function getKinectFaceTrakingData():void 
		{
			var res:Vector.<Point>  = new Vector.<Point>();
			var rect:Rectangle = new Rectangle();
			
			res = ext.doFaceTraking();
			
			tf3.text = "doFaceTraking:" + String(res.length);
			if (res.length > 1) {
				writeFacePoint(res);
			}

			rect = ext.getRectArray();
			if (rect.width +rect.height > 0 ) {
				drawFaceRect(rect);
			}
			
			res = null;
			rect = null;
		}
		//FaceRectを描画します
		private function drawFaceRect(rect:Rectangle):void 
		{
			//trace("drawFaceRect",rect);
			clear(rectDrawArea);
			rectDrawArea.graphics.lineStyle(1, 0xff0000);
			rectDrawArea.graphics.drawRect(rect.x*2-300, rect.y*2-300, rect.width*2, rect.height*2);
			rectDrawArea.graphics.endFill();	
		}
		//FacePointを描画します
		private function writeFacePoint(res:Vector.<Point>):void 
		{
			clear(pointDrawArea);
			var len:int = res.length;
			for (var i:int = 0; i <len ; i++) 
			{
				pointDrawArea.graphics.lineStyle(1, 0xff0000);
				pointDrawArea.graphics.drawCircle(res[i].x*2-300, res[i].y*2-300,1);
				pointDrawArea.graphics.endFill();
				tfArray[i].x = res[i].x * 2-1-300;
				tfArray[i].y = res[i].y * 2 - 1-300;
				tfArray[i].text = String(i);
			}
		}
		
		private function clear(dArea:Sprite):void
		{
			var sp:Sprite = dArea;
			sp.graphics.clear();
		}
		
		/**
		 * Exit
		 * 
		 */
		public function onExit(e:Event):void
		{
			trace("onExit");
			removeEventListener(Event.ENTER_FRAME, enterFrameHandler);
			if (KINECT_INIT) {
				ext.dispose();
				ext = null;
				trace(str) ;
			}
		}
	}
	
}