-module(ailz4).

-export([compress/1, compress/2, uncompress/2,
    pack/1, pack/2, unpack/1]).


-define(APPNAME,ailz4).
-define(LIBNAME,ailz4).

-on_load(init/0).

-type option() :: high.
-type pack() :: binary().

init() ->
  LibName =
  	case code:priv_dir(?APPNAME) of
    	{error,bad_name} ->
            case filelib:is_dir(filename:join(["..",priv])) of
            	true -> filename:join(["..",priv,?LIBNAME]);
            	_ -> filename:join([priv,?LIBNAME])
            end;
        Dir -> filename:join(Dir,?LIBNAME)
    end,
  erlang:load_nif(LibName,0).

not_loaded(Line) -> exit({not_loaded,[{module,?MODULE},{line,Line}]}).


%% @doc Equals `compress(Binary, [])'.
%% @see compress/2
-spec compress(binary()) -> {ok, binary()} | {error, term()}.
compress(Binary) ->compress(Binary, []).

%% @doc Returns an compressed binary. Note that the compressed binary
%%      does not include original size to be needed at uncompressing.
%% @see uncompress/2
%% @see pack/2
-spec compress(binary(), [option()]) -> {ok, binary()} | {error, term()}.
compress(_Binary, _Options) ->not_loaded(?LINE).

%% @doc Returns an uncompressed binary.
%%      You need to specify original size as `OrigSize'.
%% @see compress/2
-spec uncompress(binary(), integer()) -> {ok, binary()} | {error, term()}.
uncompress(_Binary, _OrigSize) ->not_loaded(?LINE).

%% @doc Equals `pack(Binary, [])'.
%% @see pack/2
-spec pack(binary()) -> {ok, pack()} | {error, term()}.
pack(Binary) -> pack(Binary, []).

%% @doc Returns a binary included compressed data and original size.
%%      Use `unpack/1' to uncompress the returned binary.
%% @see compress/2
%% @see unpack/1
-spec pack(binary(), [option()]) -> {ok, pack()} | {error, term()}.
pack(Binary, Options) ->
    case compress(Binary, Options) of
        {ok, Compressed} ->
            OrigSize = byte_size(Binary),
            {ok, <<OrigSize:4/little-unsigned-integer-unit:8,
                Compressed/binary>>};
        Error ->
            Error
    end.

%% @doc Return a uncompressed binary compressed with `pack/2'.
%% @see pack/2
-spec unpack(pack()) -> {ok, binary()} | {error, term()}.
unpack(<<OrigSize:4/little-unsigned-integer-unit:8, Binary/binary>>=_Binary) ->
    uncompress(Binary, OrigSize).

