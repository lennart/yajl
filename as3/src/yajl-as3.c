#include <AS3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "yajl/yajl_parse.h"
#include "yajl/yajl_gen.h"
 


static int reformat_null(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    yajl_gen_null(g);
    return 1;
}

static int reformat_boolean(void * ctx, int boolean)
{
    yajl_gen g = (yajl_gen) ctx;
    yajl_gen_bool(g, boolean);
    return 1;
}

static int reformat_number(void * ctx, const char * s, unsigned int l)
{
    yajl_gen g = (yajl_gen) ctx;
    yajl_gen_number(g, s, l);
    return 1;
}

static int reformat_string(void * ctx, const unsigned char * stringVal,
                           unsigned int stringLen)
{
    yajl_gen g = (yajl_gen) ctx;
    yajl_gen_string(g, stringVal, stringLen);
    return 1;
}

static int reformat_map_key(void * ctx, const unsigned char * stringVal,
                            unsigned int stringLen)
{
    yajl_gen g = (yajl_gen) ctx;
    yajl_gen_string(g, stringVal, stringLen);
    return 1;
}

static int reformat_start_map(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    yajl_gen_map_open(g);
    return 1;
}


static int reformat_end_map(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    yajl_gen_map_close(g);
    return 1;
}

static int reformat_start_array(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    yajl_gen_array_open(g);
    return 1;
}

static int reformat_end_array(void * ctx)
{
    yajl_gen g = (yajl_gen) ctx;
    yajl_gen_array_close(g);
    return 1;
}

static yajl_callbacks callbacks = {
    reformat_null,
    reformat_boolean,
    NULL,
    NULL,
    reformat_number,
    reformat_string,
    reformat_start_map,
    reformat_map_key,
    reformat_end_map,
    reformat_start_array,
    reformat_end_array
};

static void
usage(const char * progname)
{
    exit(1);

}

AS3_Val  
decode(void * data, AS3_Val args) 
{
    yajl_handle hand;
    /* generator config */
    yajl_gen_config conf = { 1, "  " };
	yajl_gen g;
    yajl_status stat;
    size_t rd;
    /* allow comments */
    yajl_parser_config cfg = { 1, 1 };
    int retval = 0, done = 0;
    
    /* ActionScript Argument setup */
    AS3_Val result = AS3_Undefined();
    unsigned char* input;
    int length;
    AS3_ArrayValue(args, "StrType",&input);
    length = strlen(input);
    /* check arguments.*/
    /*int a = 1;
    while ((a < argc) && (argv[a][0] == '-') && (strlen(argv[a]) > 1)) {
        unsigned int i;
        for ( i=1; i < strlen(argv[a]); i++) {
            switch (argv[a][i]) {
                case 'm':
                    conf.beautify = 0;
                    break;
                case 'u':
                    cfg.checkUTF8 = 0;
                    break;
                default:
                    fprintf(stderr, "unrecognized option: '%c'\n\n", argv[a][i]);
                    usage(argv[0]);
            }
        }
        ++a;
    }
    if (a < argc) {
        usage(argv[0]);
    }*/
    
    g = yajl_gen_alloc(&conf, NULL);

    /* ok.  open file.  let's read and parse */
    hand = yajl_alloc(&callbacks, &cfg, NULL, (void *) g);
        
            stat = yajl_parse(hand, input, length);

        if (stat != yajl_status_ok &&
            stat != yajl_status_insufficient_data)
        {
            unsigned char * str = yajl_get_error(hand, 1, input, length);
            sztrace(str);
            yajl_free_error(hand, str);

        } else {
            const unsigned char * buf;
            unsigned int len;
            yajl_gen_get_buf(g, &buf, &len);
            result = AS3_StringN(buf, len);
            yajl_gen_clear(g);
        }

    yajl_gen_free(g);
    yajl_free(hand);
    
    return result;
}
int main()
{
  AS3_Val decode_ = AS3_Function( NULL, decode );
  AS3_Val airobj = AS3_Object("decode: AS3ValType", 
      decode_ );
  AS3_Release( decode_ );
  AS3_LibInit( airobj );
  return 0;
};
