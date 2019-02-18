include env.mk

CURDIR := $(shell pwd)
BASEDIR := $(abspath $(CURDIR)/..)

C_SRC_DIR = $(CURDIR)
C_SRC_ENV ?= $(C_SRC_DIR)/env.mk
C_SRC_OUTPUT ?= $(CURDIR)/../priv/ailz4.so

OBJECTS = ailz4_nif.o

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S),Darwin)
	PLAT=macosx
	CFLAGS ?= -O3 -std=c99 -arch x86_64 -finline-functions -Wall -Wmissing-prototypes
	CXXFLAGS ?= -O3 -arch x86_64 -finline-functions -Wall
	LDFLAGS ?= -arch x86_64 -flat_namespace -undefined suppress
else
ifeq ($(uname_S),Linux)
	PLAT=linux
	CFLAGS ?= -O3 -std=c99 -finline-functions -Wall -Wmissing-prototypes
	CXXFLAGS ?= -O3 -finline-functions -Wall
else
ifeq ($(uname_S),FreeBSD)
	PLAT=freebsd
	CFLAGS ?= -O3 -std=c99 -finline-functions -Wall -Wmissing-prototypes
	CXXFLAGS ?= -O3 -finline-functions -Wall
else
ifeq ($(uname_S),OpenBSD)
	PLAT=bsd
	CFLAGS ?= -O3 -std=c99 -finline-functions -Wall -Wmissing-prototypes
	CXXFLAGS ?= -O3 -finline-functions -Wall
else
	PLAT=posix
	CFLAGS ?= -O3 -std=c99 -finline-functions -Wall -Wmissing-prototypes
	CXXFLAGS ?= -O3 -finline-functions -Wall
endif
endif
endif
endif

LZ4_STATICLIB := $(C_SRC_DIR)/3rd/lz4/lib/liblz4.a
LZ4_SRC :=  $(C_SRC_DIR)/3rd/lz4/lib
LZ4_DIR ?= $(C_SRC_DIR)/3rd/lz4
undefine PLAT

CFLAGS += -I"$(ERTS_INCLUDE_DIR)" -I"$(ERL_INTERFACE_INCLUDE_DIR)" -I"$(LZ4_SRC)" -fPIC 
CXXFLAGS += -I"$(ERTS_INCLUDE_DIR)" -I"$(ERL_INTERFACE_INCLUDE_DIR)" -I"$(LZ4_SRC)" -fPIC 

LDLIBS += -shared -L"$(ERL_INTERFACE_LIB_DIR)" -lerl_interface -lei 




all: $(LZ4_STATICLIB) $(C_SRC_OUTPUT)

$(LZ4_STATICLIB) :
	@cd $(LZ4_DIR) && $(MAKE) CC='$(CC) -std=gnu99 -fPIC' $(PLAT)

$(C_SRC_OUTPUT): $(OBJECTS)
	@mkdir -p $(BASEDIR)/priv/
	 $(CC) $(OBJECTS) $(LZ4_STATICLIB) $(LDFLAGS) $(LDLIBS) -o $(C_SRC_OUTPUT)

%.o: %.c
	$(CC) -c $(CFLAGS)  $<

clean:
	@rm -f $(C_SRC_OUTPUT) $(OBJECTS); rm -f $(C_SRC_ENV)
	@cd $(LZ4_DIR) && $(MAKE) clean
