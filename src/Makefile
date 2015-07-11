#
# Latest Update: 2015/05/02
#

CC ?= gcc

PKG_DEPS = gtk+-3.0
PKG_CONFIG ?= pkg-config

#CFLAGS ?= -O2
#CFLAGS += -Wall -Wno-format -std=c99

CFLAGS += $(shell $(PKG_CONFIG) --cflags $(PKG_DEPS))
LDLIBS += $(shell $(PKG_CONFIG) --libs $(PKG_DEPS)) -lm

all: aclock

clean:
	rm aclock
