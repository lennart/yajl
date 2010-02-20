#include <AS3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "yajl/yajl_parse.h"
#include "yajl/yajl_gen.h"

//void as3_(void* ctx, const char* str, unsigned int len) {
//  yail_gen wrapper = (yail_gen)ctx;
//  
//  sztrace("DO NOTHING");
//  AS3_Trace(AS3_StringN(str, len));
//
//}
typedef enum {
  yajl_gen_start,
  yajl_gen_map_start,
  yajl_gen_map_key,
  yajl_gen_map_val,
  yajl_gen_array_start,
  yajl_gen_in_array,
  yajl_gen_complete,
  yajl_gen_error
} yajl_gen_state;

struct yajl_gen_t {
  unsigned int depth;
  unsigned int pretty;
  const char * indentString;
  yajl_gen_state state[YAJL_MAX_DEPTH];
  AS3_Val stack;
  int nested_hash_level;
  int nested_array_level;
  void * ctx; /* yajl_buf */
  /* memory allocation routines */
  yajl_alloc_funcs alloc;
};

yajl_gen as3_gen_alloc2(const yajl_print_t callback,
    const yajl_gen_config * config,
    const yajl_alloc_funcs * afs,
    void * ctx) {
  yajl_gen g = NULL;
  yajl_alloc_funcs afsBuffer;

  /* first order of business is to set up memory allocation routines */
  if (afs != NULL) {
    if (afs->malloc == NULL || afs->realloc == NULL || afs->free == NULL)
    {
      return NULL;
    }
  }
  else {
    yajl_set_default_alloc_funcs(&afsBuffer);
    afs = &afsBuffer;
  }

  g = (yajl_gen)malloc(sizeof(struct yajl_gen_t));
  memset((void *) g, 0, sizeof(struct yajl_gen_t));
  /* copy in pointers to allocation routines */
  memcpy((void *) &(g->alloc), (void *) afs, sizeof(yajl_alloc_funcs));

  if (config) {
    g->pretty = config->beautify;
    g->indentString = config->indentString ? config->indentString : "  ";
  }

  g->ctx = AS3_Undefined();
  g->nested_hash_level = 0;
  g->nested_array_level = 0;
  g->stack = AS3_Array("");

  return g;
}


yajl_gen as3_gen_alloc(const yajl_gen_config * config,
    const yajl_alloc_funcs * afs) {
  return as3_gen_alloc2(NULL, config, afs, NULL);
}

#ifdef VERBOSE
#define LOG_C(cvalue) sztrace(cvalue); 
#define LOG_AS3(as3value) AS3_Trace(as3value);
#else
#define LOG_C(cvalue)
#define LOG_AS3(cvalue)
#endif

#ifdef DEBUG_LOG
#define DEBUG_LOG_C(cvalue) LOG_C(cvalue)
#define DEBUG_LOG_AS3(as3value) LOG_AS3(as3value)
#else
#define DEBUG_LOG_C(cvalue)
#define DEBUG_LOG_AS3(cvalue)
#endif

#define IS_ARRAY(value) \
  AS3_InstanceOf(value, AS3_NSGetS(NULL, "Array"))

#define IS_OBJECT(val) \
  (!IS_ARRAY(val) && !IS_BOOLEAN(val) && !IS_NULL(val) && !IS_NUMBER(val) && !IS_INT(val) && !IS_STRING(val) && !IS_UNDEFINED(val))

#define IS_BOOLEAN(value) \
  AS3_InstanceOf(value, AS3_NSGetS(NULL, "Boolean"))

#define IS_UNDEFINED(value) \
  (value == AS3_Undefined())

#define IS_NULL(value) \
  (value == AS3_Null())

#define IS_NUMBER(value) \
  AS3_InstanceOf(value, AS3_NSGetS(NULL, "Number"))

#define IS_INT(value) \
  AS3_InstanceOf(value, AS3_NSGetS(NULL, "int"))


#define IS_STRING(value) \
  AS3_InstanceOf(value, AS3_NSGetS(NULL, "String"))

static void yajl_check_end(void * ctx) {
  yajl_gen wrapper = (yajl_gen) ctx;
  int stack_length = AS3_IntValue(AS3_GetS(wrapper->stack,"length"));
  if ((stack_length == 1) && (wrapper->nested_hash_level == 0) && (wrapper->nested_array_level == 0)) {
    wrapper->ctx = AS3_CallS("pop",wrapper->stack,AS3_Array(""));
    LOG_C("This is the end")
    LOG_AS3(wrapper->ctx)
  }
  else {
    DEBUG_LOG_C("Continue")
    DEBUG_LOG_AS3(AS3_Int(stack_length))
    DEBUG_LOG_AS3(AS3_Int(wrapper->nested_hash_level))
    DEBUG_LOG_AS3(AS3_Int(wrapper->nested_array_level))
  }
}

static void yajl_set_static_value(void * ctx, AS3_Val val) {
  yajl_gen wrapper = (yajl_gen) ctx;
  DEBUG_LOG_AS3(val)
  int stack_length = AS3_IntValue(AS3_GetS(wrapper->stack,"length"));
  if (stack_length > 0) {
    AS3_Val lastEntry = AS3_CallS("pop",wrapper->stack,AS3_Array(""));
    if(IS_STRING(lastEntry) || IS_BOOLEAN(lastEntry) || IS_NULL(lastEntry) || IS_NUMBER(lastEntry) || IS_INT(lastEntry) || IS_UNDEFINED(lastEntry))  {
      AS3_Val obj = AS3_CallS("pop",wrapper->stack,AS3_Array(""));
      if(IS_OBJECT(obj)) {
        AS3_Set(obj,lastEntry,val);
        AS3_CallS("push",wrapper->stack,AS3_Array("AS3ValType", obj));
        if(IS_ARRAY(val) || IS_OBJECT(val)) {
          AS3_CallS("push",wrapper->stack,AS3_Array("AS3ValType", val));
        }
      }
      else {
        AS3_CallS("push",wrapper->stack,AS3_Array("AS3ValType", obj));
        AS3_CallS("push",wrapper->stack,AS3_Array("AS3ValType", lastEntry));
      }
      AS3_Release(obj);
    }
    else if(IS_ARRAY(lastEntry)) {
      AS3_CallS("push", lastEntry, AS3_Array("AS3ValType",val));
      AS3_CallS("push", wrapper->stack, AS3_Array("AS3ValType",lastEntry));
      if(IS_ARRAY(val) || IS_OBJECT(val)) {
        AS3_CallS("push", wrapper->stack, AS3_Array("AS3ValType", val));
      }
    }
    else if(IS_OBJECT(lastEntry)) {
      AS3_SetS(lastEntry, val, AS3_Null());
      AS3_CallS("push",wrapper->stack,AS3_Array("AS3ValType", lastEntry));
      AS3_CallS("push",wrapper->stack,AS3_Array("AS3ValType", val));
    }
    else {
      LOG_C("Something else on the Stack")
      AS3_CallS("push",wrapper->stack,AS3_Array("AS3ValType", lastEntry));
    }
    AS3_Release(lastEntry);
  }
  else {
    AS3_CallS("push",wrapper->stack,AS3_Array("AS3ValType", val));
  }
}

static int yajl_found_null(void * ctx) {
  yajl_set_static_value(ctx,AS3_Null());
  yajl_check_end(ctx);
  return 1;
}

static int yajl_found_boolean(void * ctx, int boolean) {
  if(boolean) {
    yajl_set_static_value(ctx, AS3_True());
  }
  else {
    yajl_set_static_value(ctx, AS3_False());
  }
  yajl_check_end(ctx);
  return 1;
}

static int yajl_found_number(void * ctx, const char * s, unsigned int l) {
  AS3_Val string = AS3_StringN(s,l);
  AS3_Val string_class = AS3_NSGet(NULL, "String");

  yajl_set_static_value(ctx, AS3_CallS("parseFloat",NULL,AS3_Array("StrType",string)) );
  AS3_Release(string);
  AS3_Release(string_class);
  yajl_check_end(ctx);
  return 1;
}

static int yajl_found_string(void * ctx, const unsigned char * stringVal,
    unsigned int stringLen) {
  yajl_set_static_value(ctx, AS3_StringN(stringVal, stringLen));
  yajl_check_end(ctx);
  return 1;
}

static int yajl_found_map_key(void * ctx, const unsigned char * stringVal,
    unsigned int stringLen) {
  yajl_set_static_value(ctx,AS3_StringN(stringVal, stringLen)); 
  yajl_check_end(ctx);
  return 1;
}

static int yajl_found_start_map(void * ctx) {    
  yajl_gen wrapper = (yajl_gen)ctx;
  wrapper->nested_hash_level++;
  yajl_set_static_value(ctx,AS3_Object(""));
  return 1;
}


static int yajl_found_end_map(void * ctx) {
  yajl_gen wrapper = (yajl_gen)ctx;
  wrapper->nested_hash_level--;
  if(AS3_IntValue(AS3_GetS(wrapper->stack,"length")) > 1) {
    AS3_CallS("pop",wrapper->stack,AS3_Array(""));
  }
  yajl_check_end(ctx);
  return 1;
}

static int yajl_found_start_array(void * ctx) {
  yajl_gen wrapper = (yajl_gen)ctx;
  wrapper->nested_array_level++;
  yajl_set_static_value(ctx,AS3_Array(""));
  return 1;
}

static int yajl_found_end_array(void * ctx) {
  yajl_gen wrapper = (yajl_gen)ctx;
  wrapper->nested_array_level--;
  if(AS3_IntValue(AS3_GetS(wrapper->stack,"length")) > 1) {
    AS3_CallS("pop",wrapper->stack,AS3_Array(""));
  }
  yajl_check_end(ctx);
  return 1;
}

static yajl_callbacks callbacks = {
  yajl_found_null,
  yajl_found_boolean,
  NULL,
  NULL,
  yajl_found_number,
  yajl_found_string,
  yajl_found_start_map,
  yajl_found_map_key,
  yajl_found_end_map,
  yajl_found_start_array,
  yajl_found_end_array
};

struct as3_yajl_wrapper_t {
  yajl_gen generator;
  yajl_handle handle;
  yajl_gen_config config;
  yajl_parser_config parser_config;
};


typedef struct as3_yajl_wrapper_t * as3_yajl_wrapper; 


as3_yajl_wrapper streamDecoder(void) {

  as3_yajl_wrapper wrapper = (as3_yajl_wrapper)malloc(sizeof(struct as3_yajl_wrapper_t));
  yajl_gen_config config = { 1, "  " };
  wrapper->config = config;
  yajl_parser_config parser_config = { 1, 1 };
  wrapper->parser_config = parser_config;
  wrapper->generator = as3_gen_alloc(&wrapper->config, NULL);
  wrapper->handle = yajl_alloc(&callbacks, &wrapper->parser_config, NULL, (void *) wrapper->generator);
  return wrapper;
}

AS3_Val setupStreamDecoder(void * data, AS3_Val args) {
  return AS3_Number((long)streamDecoder());
}

AS3_Val decodeStream(void * data, AS3_Val args) {
  yajl_status stat;

  /* ActionScript Argument setup */
  AS3_Val result = AS3_Undefined();
  unsigned char* input;
  int length;
  unsigned int wrapper_address;
  unsigned char * str;
  AS3_ArrayValue(args, "IntType, StrType",&wrapper_address, &input);
  length = strlen(input);

  as3_yajl_wrapper wrapper = (as3_yajl_wrapper)wrapper_address;
  //LOG_C("The Current Stack:")
  //LOG_AS3(wrapper->generator->stack)
  LOG_C("Parsing Started")
  LOG_AS3(AS3_Int(length))

  DEBUG_LOG_C("PARSING: ")

  yajl_gen_clear(wrapper->generator);
  stat = yajl_parse(wrapper->handle, input, length);

  LOG_C("Parsing Complete");

  switch(stat) {
    case yajl_status_insufficient_data:
      LOG_C("Insufficient Data, running Data Callback")
      DEBUG_LOG_C("Continue")
      DEBUG_LOG_AS3(AS3_Int(wrapper->generator->nested_hash_level))
      DEBUG_LOG_AS3(AS3_Int(wrapper->generator->nested_array_level))
      DEBUG_LOG_AS3(wrapper->generator->stack)

      //yajl_free(wrapper->handle);
      return AS3_Undefined();
      break;
    case yajl_status_ok:
      result = (AS3_Val)wrapper->generator->ctx;
      LOG_AS3(result)
      yajl_gen_free(wrapper->generator);
      return AS3_Object("result: AS3ValType",result);
      break;
    case yajl_status_client_canceled:
    case yajl_status_error:
      str = yajl_get_error(wrapper->handle, 1, input, length);
      LOG_C(str)
      yajl_free_error(wrapper->handle, str);
      yajl_gen_free(wrapper->generator);
      yajl_free(wrapper->handle);
      return AS3_Undefined();
  }


  return result;
}

AS3_Val decode(void * data, AS3_Val args) {
  as3_yajl_wrapper wrapper = streamDecoder();
  yajl_status stat;

  /* ActionScript Argument setup */
  AS3_Val result = AS3_Undefined();
  unsigned char* input;
  int length;
  AS3_ArrayValue(args, "StrType",&input);
  length = strlen(input);

  LOG_C("Parsing Started");
  LOG_AS3(AS3_Int(length))
  stat = yajl_parse(wrapper->handle, input, length);
  LOG_C("Parsing Complete");


  if (stat != yajl_status_ok &&
      stat != yajl_status_insufficient_data) {
    unsigned char * str = yajl_get_error(wrapper->handle, 1, input, length);
    LOG_C(str)
    yajl_free_error(wrapper->handle, str);
  } 
  else {
    result = (AS3_Val)wrapper->generator->ctx;
    LOG_AS3(result)
    yajl_gen_clear(wrapper->generator);
  }

  yajl_gen_free(wrapper->generator);
  yajl_free(wrapper->handle);

  LOG_C("After gen free");
  LOG_AS3(result)
  return result;
}

int main()
{
  AS3_Val decode_ = AS3_Function( NULL, decode );
  AS3_Val decodeAsync_ = AS3_FunctionAsync( NULL, decode );
  AS3_Val decodeStreamAsync_ = AS3_FunctionAsync( NULL, decodeStream );
  AS3_Val setupStreamDecoder_  = AS3_Function( NULL, setupStreamDecoder );
  AS3_Val airobj = AS3_Object("decode: AS3ValType, decodeAsync: AS3ValType, decodeStreamAsync: AS3ValType, setupStreamDecoder: AS3ValType",
      decode_, decodeAsync_, decodeStreamAsync_, setupStreamDecoder_ );
  AS3_Release( decode_ );
  AS3_LibInit( airobj );
  return 0;
};