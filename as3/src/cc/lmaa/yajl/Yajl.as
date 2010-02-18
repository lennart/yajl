package cc.lmaa.yajl {
  import cmodule.yajl.CLibInit;

  public class Yajl {
    private var _yajl : Object = new CLibInit().init();
    public static function decode(str : String) : * {
      _yajl.decode(str);
    }

    public static function decodeAsync(callback : Function, str : String) : void {
      _yajl.decodeAsync(callback, str);
    }
  }
}
