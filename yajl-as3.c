/***
 
	asynchronous jpeg encoding
 
	see it in action at : 
		http://segfaultlabs.com/blog/post/asynchronous-jpeg-encoding
 
	author: Mateusz Malczak ( http://segfaultlabs.com )
 
 ***/
#include <AS3.h>
#include <stdio.h>
#include <setjmp.h>
#include "yajl/yajl_parse.h"
#include "yajl/yajl_gen.h"
 

struct as3_node_t {
  AS3_Val ctx;
  AS3_Val this;
};

typedef struct as3_node_t * as3_node;



static void generate(as3_node ctx, AS3_Val value) {

//  switch(
  sztrace("Generating");
  
  AS3_Trace(ctx->ctx);
  AS3_Trace(value);
  AS3_Trace(AS3_CallS("callback",ctx->this,AS3_Array("AS3ValType AS3ValType", ctx->ctx, value)));
}

static int as3_null(void *ctx)
{
    sztrace("null\n");
    generate((as3_node)ctx,AS3_Null());
    
    
    return 1;
}

static int as3_boolean(void * ctx, int boolVal)
{
  sztrace("bool: \n");
  if(boolVal) {
    generate((as3_node)ctx, AS3_True());
  }
  else {
    generate((as3_node)ctx, AS3_False());
  }
  return 1;
}

static int as3_integer(void *ctx, long integerVal)
{
  sztrace("integer: \n");
  generate((as3_node)ctx, AS3_Int(integerVal));
  return 1;
}

static int as3_double(void *ctx, double doubleVal)
{
  sztrace("double: \n");
  generate((as3_node)ctx, AS3_Number(doubleVal));
  return 1;
}

static int as3_string(void *ctx, const unsigned char * stringVal,
    unsigned int stringLen)
{
  sztrace("string: '");
  generate((as3_node)ctx, AS3_StringN(stringVal,stringLen));
  return 1;
}

static int as3_map_key(void *ctx, const unsigned char * stringVal,
    unsigned int stringLen)
{
  char * str = (char *) malloc(stringLen + 1);
  str[stringLen] = 0;
  memcpy(str, stringVal, stringLen);
  sztrace("key: \n");
  free(str);
  return 1;
}

static int as3_start_map(void *ctx)
{
  sztrace("map open '{'\n");
  return 1;
}


static int as3_end_map(void *ctx)
{
  sztrace("map close '}'\n");
  return 1;
}

static int as3_start_array(void *ctx)
{
  sztrace("array open '['\n");
  return 1;
}

static int as3_end_array(void *ctx)
{
  sztrace("array close ']'\n");
  return 1;
}

//static yajl_callbacks callbacks = {
//  as3_null,
//  as3_boolean,
//  as3_integer,
//  as3_double,
//  NULL,
//  as3_string,
//  as3_start_map,
//  as3_map_key,
//  as3_end_map,
//  as3_start_array,
//  as3_end_array
//};
//
/* begin parsing callback routines */
#define BUF_SIZE 2048

static int test_yajl_null(void *ctx)
{
    sztrace("null\n");
    return 1;
}

static int test_yajl_boolean(void * ctx, int boolVal)
{
    sztrace("bool: %s\n");
    return 1;
}

static int test_yajl_integer(void *ctx, long integerVal)
{
    sztrace("integer: %ld\n");
    return 1;
}

static int test_yajl_double(void *ctx, double doubleVal)
{
    sztrace("double: %g\n");
    return 1;
}

static int test_yajl_string(void *ctx, const unsigned char * stringVal,
                            unsigned int stringLen)
{
    sztrace("string: '");
    //fwrite(stringVal, 1, stringLen, stdout);
    sztrace("'\n");    
    return 1;
}

static int test_yajl_map_key(void *ctx, const unsigned char * stringVal,
                             unsigned int stringLen)
{
    char * str = (char *) malloc(stringLen + 1);
    str[stringLen] = 0;
    memcpy(str, stringVal, stringLen);
    sztrace("key: '%s'\n");
    free(str);
    return 1;
}

static int test_yajl_start_map(void *ctx)
{
    sztrace("map open '{'\n");
    return 1;
}


static int test_yajl_end_map(void *ctx)
{
    sztrace("map close '}'\n");
    return 1;
}

static int test_yajl_start_array(void *ctx)
{
    sztrace("array open '['\n");
    return 1;
}

static int test_yajl_end_array(void *ctx)
{
    sztrace("array close ']'\n");
    return 1;
}

static yajl_callbacks callbacks = {
    test_yajl_null,
    test_yajl_boolean,
    test_yajl_integer,
    test_yajl_double,
    NULL,
    test_yajl_string,
    test_yajl_start_map,
    test_yajl_map_key,
    test_yajl_end_map,
    test_yajl_start_array,
    test_yajl_end_array
};

AS3_Val decode( void *data, AS3_Val args )
{
  yajl_handle hand;
  yajl_status stat;
  AS3_Val this;

  unsigned char * string;
  int length;
  AS3_Val parsedJSON = AS3_Undefined();
  AS3_Trace(args);
  AS3_ArrayValue( args, "AS3ValType StrType IntType", &this, &string, &length);
  as3_node ctx = (as3_node)malloc(sizeof(struct as3_node_t));
//  ctx->state = gen_start;
  ctx->ctx = parsedJSON;
  ctx->this = this;
  sztrace("TesT");
  generate(ctx, AS3_String("Something"));
  hand = yajl_alloc(&callbacks, NULL, NULL, NULL);
  sztrace("Parsing String ");
  stat = yajl_parse(hand, string, length);
  if (stat != yajl_status_insufficient_data &&
      stat != yajl_status_ok)
  {
    sztrace("Error done");
    unsigned char * str = yajl_get_error(hand, 0, string, length);
    sztrace(str);
    yajl_free_error(hand, str);
  }

  sztrace("Parsing done");
  yajl_free(hand); 



  return parsedJSON;
};

int main()
{
  //	AS3_Val encode_ = AS3_Function( NULL, fileTest );
  AS3_Val decode_ = AS3_Function( NULL, decode );
  AS3_Val airobj = AS3_Object("decode: AS3ValType", 
      decode_ );
  AS3_Release( decode_ );
  AS3_LibInit( airobj );
  return 0;
};
