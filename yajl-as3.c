#include <AS3.h>
#include <stdio.h>
#include <setjmp.h>
#include "yajl/yajl_parse.h"
#include "yajl/yajl_gen.h"
 

int main()
{
  AS3_Val decode_ = AS3_Function( NULL, decode );
  AS3_Val airobj = AS3_Object("decode: AS3ValType", 
      decode_ );
  AS3_Release( decode_ );
  AS3_LibInit( airobj );
  return 0;
};
