#include <stdbool.h>
#include "erl_nif.h"
#include "lz4.h"
#include "lz4hc.h"

static ERL_NIF_TERM nif_compress(ErlNifEnv* env, int argc,
    const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM nif_uncompress(ErlNifEnv* env, int argc,
    const ERL_NIF_TERM argv[]);

static ERL_NIF_TERM atom_ok;
static ERL_NIF_TERM atom_error;
static ERL_NIF_TERM atom_high;
static ERL_NIF_TERM atom_compress_failed;
static ERL_NIF_TERM atom_uncompress_failed;

static ERL_NIF_TERM
nif_compress(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  ERL_NIF_TERM opts_term, head_term, tail_term, ret_term;
  ErlNifBinary src_bin, res_bin;
  bool high = false;
  int real_size;
  size_t res_size;
  int tuple_size,high_level;
  const ERL_NIF_TERM *tuple_array;

  if (!enif_inspect_binary(env, argv[0], &src_bin) ||
      !enif_is_list(env, argv[1]))
    return 0;

  opts_term = argv[1];
  while (enif_get_list_cell(env, opts_term, &head_term, &tail_term)) {
    if(enif_is_tuple(env,head_term)){
      enif_get_tuple(env,head_term,&tuple_size,&tuple_array);
      if(tuple_size == 2 && enif_is_identical((*tuple_array), atom_high)){
        enif_get_int(env,(*(tuple_array + 1)), &high_level);
       //high_level = 12;
        high = true;
      }
    }
    opts_term = tail_term;
  }

  res_size = LZ4_compressBound(src_bin.size);
  enif_alloc_binary(res_size, &res_bin);

  if (high){
     real_size = LZ4_compress_HC((char *)src_bin.data,
        (char *)res_bin.data, src_bin.size,res_size,high_level);
  }else{
    real_size = LZ4_compress_default((char *)src_bin.data,
        (char *)res_bin.data, src_bin.size,res_size);
  }


  if (real_size >= 0) {
    enif_realloc_binary(&res_bin, real_size);
    ret_term = enif_make_tuple2(env, atom_ok,enif_make_binary(env, &res_bin));
    enif_release_binary(&res_bin);
    return ret_term;
  } else {
    enif_release_binary(&res_bin);
    return enif_make_tuple2(env, atom_error, atom_compress_failed);
  }
}

static ERL_NIF_TERM
nif_uncompress(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  ERL_NIF_TERM ret_term;
  ErlNifBinary src_bin, res_bin;
  long res_size;

  if (!enif_inspect_binary(env, argv[0], &src_bin) ||
      !enif_get_long(env, argv[1], &res_size))
    return 0;

  enif_alloc_binary((size_t)res_size, &res_bin);

  int ret_size = LZ4_decompress_safe((char *)src_bin.data,
      (char *)res_bin.data, src_bin.size, res_bin.size);
  if (ret_size >= 0) {
    if (ret_size != res_bin.size) {
      if (!enif_realloc_binary(&res_bin, ret_size))
        goto error;
    }
    ret_term = enif_make_tuple2(env, atom_ok,
        enif_make_binary(env, &res_bin));
    enif_release_binary(&res_bin);
    return ret_term;
  }

error:
  enif_release_binary(&res_bin);
  return enif_make_tuple2(env, atom_error, atom_uncompress_failed);
}

static int on_load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
  atom_ok = enif_make_atom(env, "ok");
  atom_error = enif_make_atom(env, "error");
  atom_high = enif_make_atom(env, "high");
  atom_compress_failed = enif_make_atom(env, "compress_failed");
  atom_uncompress_failed = enif_make_atom(env, "uncompress_failed");
  return 0;
}
static ErlNifFunc nif_funcs[] =
{
    {"compress", 2, nif_compress,ERL_NIF_DIRTY_JOB_CPU_BOUND},
    {"uncompress", 2, nif_uncompress,ERL_NIF_DIRTY_JOB_CPU_BOUND}
};

ERL_NIF_INIT(ailz4, nif_funcs, &on_load, NULL, NULL, NULL);

