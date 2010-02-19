package cc.lmaa.yajl {
  import cmodule.yajl.CLibInit;
  import flash.utils.ByteArray;

  public class Yajl {
    private static var _yajl : Object = new CLibInit().init();

    public static function decode(data : *) : * {
      return _yajl.decode(data);
    }

    public static function decodeAsync(callback : Function, str : String) : void {
      _yajl.decodeAsync(callback, str);
    }
  }
}
