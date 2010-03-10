package cc.lmaa.yajl {

	import asunit.framework.TestCase;

	public class YajlTest extends TestCase {
		private var yajl:Yajl;

		public function YajlTest(methodName:String=null) {
			super(methodName);
		}

		override protected function setUp():void {
			super.setUp();
			//yajl = new Yajl();
		}

		override protected function tearDown():void {
			super.tearDown();
			//yajl = null;
		}

    public function testArray() : void {
      var json : String = '["bla","blubb","test"]';
      var decoded : * = Yajl.decode(json);
      assertTrue("Decoded is Array", decoded is Array);
      assertTrue("Decoded has 3 Elements", decoded.length == 3);
    }

		public function testIntegers():void {
      var json : String = '100000001';
      var decoded : * = Yajl.decode(json);
      assertTrue("Decoded is Number",decoded == 100000001);
      assertFalse("Decoded is not 'Not a Number'", isNaN(decoded));
//      assertTrue("Number Array contains Numbers", );
          
		}

		public function testDoubles():void {
      var json : String = "[ 0.1e2, 1e1, 3.141569, 10000000000000e-10]";
      var decoded : Array = Yajl.decode(json) as Array;
      assertTrue("Number Array contains 4 Elements",decoded.length == 4);
      for(var i in decoded) {
        assertTrue("Number Array contains Numbers", i is Number);
        assertFalse("Number Array contains real Numbers", isNaN(i));
      }
          
		}

	}
}
