yajl for ActionScript 3
=======================
_(we need a flashy name)_

This is just a quick hack to bring one of the nicest JSON-parsers [__yajl__](http://github.com/lloyd/yajl) to ActionScript 3.  
The currently available JSON-Parser from the corelib wasn't what I expected and definitely not _crash-proof_. So I decided to hack yajl to ActionScript 3 through alchemy.

This quick hack is inspired by the reformatter (included in the original source) and [Brian Mario's yajl-ruby](http://github.com/brianmario/yajl-ruby).

Compilation
===========
Sorry, no swc yet.  
You have to make sure, that your `ALCHEMY_HOME` environment variable is set correctly before running any of the following commands.
You have to build the swc yourself by running

    rake lib

to build `libyajl.a`, followed by

    rake swc

to build the AS3-Binding

This places the `yajl.swc` into `build/`


Usage
=====
Currently there is just one simple function:

`decode(str : String) : *`

Example
-------

    import cmodule.yajl.CLibInit;

    public function decodeJSON(str : String) : * {
      var yajl : Object = CLibInit().init();
      return yajl.decode(str);
    }

TODO
====

Encoding! Handling of Numbers, Boolean and Null-Values. Testing.
