EXTENSION    = pg_rrule
EXTVERSION   = $(shell grep default_version $(EXTENSION).control | sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")

DATA         = $(filter-out $(wildcard sql/*--*.sql),$(wildcard sql/*.sql))
DOCS         = $(wildcard doc/*.md)
TESTS        = $(wildcard test/sql/*.sql)
REGRESS      = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --load-language=plpgsql
#
# Uncoment the MODULES line if you are adding C files
# to your extention.
#
MODULE_big   = pg_rrule
OBJS         = $(patsubst %.c,%.o,$(wildcard src/*.c))
PG_CONFIG    = pg_config
PG91         = $(shell $(PG_CONFIG) --version | grep -qE " 8\.| 9\.0" && echo no || echo yes)

DOCKER_NAME  = pg-rrule

ifeq ($(PG91),yes)
all: sql/$(EXTENSION)--$(EXTVERSION).sql

sql/$(EXTENSION)--$(EXTVERSION).sql: sql/$(EXTENSION).sql
	cp $< $@

DATA = $(wildcard sql/*--*.sql) sql/$(EXTENSION)--$(EXTVERSION).sql
EXTRA_CLEAN = sql/$(EXTENSION)--$(EXTVERSION).sql
endif

PGXS := $(shell $(PG_CONFIG) --pgxs)
include ${PGXS}

# override CPPFLAGS := -I. -I$(srcdir) $(CPPFLAGS) -I/usr/local/Cellar/libical/3.0.6/include

src/pg_rrule.o: CFLAGS += $(shell pkg-config --cflags libical)
pg_rrule.so: SHLIB_LINK += $(shell pkg-config --libs libical)

sql/pg_rrule.sql: sql/pg_rrule.sql.in
	sed 's,MODULE_PATHNAME,$$libdir/$(@:sql/%.sql=%),g' $< >$@

_install_packages:
	docker-compose exec --user=root ${DOCKER_NAME} /bin/sh -c "apk add g++ clang libical-dev postgresql-dev llvm8 make"

_compile:
	docker-compose exec --user=root ${DOCKER_NAME} /bin/sh -c "make"

_install:
	docker-compose exec --user=root ${DOCKER_NAME} /bin/sh -c "export EXTVERSION=${EXTVERSION} && ./install.sh"

_tests:
ifeq ("$(wildcard ./regression.out)","")
	echo >> ./regression.out && chmod 777 regression.out
endif
ifeq ("$(wildcard ./regression.diffs)","")
	echo >> ./regression.diffs && chmod 777 regression.diffs
endif
ifeq ("$(wildcard ./results)","")
	mkdir ./results && chmod 777 ./results
endif
	docker-compose exec --user=postgres ${DOCKER_NAME} /bin/sh -c "make installcheck"

_build: _install_packages _compile _install _tests

_clean_up:
	docker-compose exec --user=root ${DOCKER_NAME} /bin/sh -c "make clean && ./clean_up.sh"

_export:
ifeq ("$(wildcard ./pg_rrule_ext)","")
	mkdir ./pg_rrule_ext && chmod 777 ./pg_rrule_ext
endif

	cp ./pg_rrule.so ./pg_rrule_ext
	cp ./pg_rrule.control ./pg_rrule_ext
	cp ./sql/pg_rrule--0.2.0.sql ./pg_rrule_ext
	cp ./doc/pg_rrule.md ./pg_rrule_ext