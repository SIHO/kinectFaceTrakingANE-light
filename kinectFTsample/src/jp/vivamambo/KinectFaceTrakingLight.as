package jp.vivamambo{
  import flash.external.ExtensionContext;
  import flash.events.EventDispatcher;
  import flash.geom.Rectangle;
  import flash.geom.Point;
  
	/**
	 * ...
	 * @author SIHO from viva mambo,inc.
	 */
  public class KinectFaceTrakingLight extends EventDispatcher{
    private var ctx:ExtensionContext;
    
    public function KinectFaceTrakingLight() {
      ctx = ExtensionContext.createExtensionContext("jp.vivamambo", "type");
    }
	/**
	 * テスト用
	 * @return Hello World 
	 */
    public function say():String {
		return ctx.call("hello") as String;
    }
	/**
	 *  FaceTraking の識別点を返します
	 * @return Vector.<Point>
	 */
	public function doFaceTraking():Vector.<Point> {
		var vecP:Vector.<Point> = new Vector.<Point>();
		vecP.push(new Point(0.0, 0.0));
		vecP = ctx.call("aneDoFaceTraking") as Vector.<Point>;
		return vecP;
	}
	/**
	 *  FaceTraking で得られるRectを返します
	 * @return Rectangle
	 */
	public function getRectArray():Rectangle {
		var arr:Array = [0,0,0,0];
		var rect:Rectangle;
		arr = ctx.call("aneRectArray") as Array;
		rect = new Rectangle(arr[0], arr[1], arr[2], arr[3]);
		return rect;
	}
	////////life cycle ////////
	/**
	 * Kinectを初期化します。
	 * @param	bool true だとnearMode ON false だとなし。
	 * @return 戻り値は常にfalse
	 */
	public function initKinect(bool:Boolean):Boolean {
		return ctx.call("aneInitKinect",bool) as Boolean;
	}
	/**
	 * FaceTrakingのインスタンスを作り、顔追従を始めます。
	 * @param	bool trueだとKinectの角度を追従します。重いのでお勧めは出来ない。
	 * @return Boolean 戻りがtrueならFaceTrakingしている。falseならしていない。
	 */
	public function startUpdate(bool:Boolean):Boolean {
		return ctx.call("aneStartUpdate",bool) as Boolean;
	}
	/**
	 * kinectをインスタンスを全て破棄して閉じます。
	 * 正しい順序でコマンドを投げないとプロセス残りを起します。
	 */
    public function dispose():void {
		trace("disposeKinect");
      return ctx.dispose();
    }
  }
}