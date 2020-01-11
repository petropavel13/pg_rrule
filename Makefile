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
CUSTOM_PGXS  = pgxs.mk 

ifeq ($(PG91),yes)
all: sql/$(EXTENSION)--$(EXTVERSION).sql

sql/$(EXTENSION)--$(EXTVERSION).sql: sql/$(EXTENSION).sql
	cp $< $@

DATA = $(wildcard sql/*--*.sql) sql/$(EXTENSION)--$(EXTVERSION).sql
EXTRA_CLEAN = sql/$(EXTENSION)--$(EXTVERSION).sql
endif

PGXS := $(shell $(PG_CONFIG) --pgxs)
ifneq ("$(wildcard $(PGXS))","")
    include ${PGXS}
endif

src/pg_rrule.o: CFLAGS += $(shell pkg-config --cflags libical)
pg_rrule.so: SHLIB_LINK += $(shell pkg-config --libs libical)

sql/pg_rrule.sql: sql/pg_rrule.sql.in
	sed 's,MODULE_PATHNAME,$$libdir/$(@:sql/%.sql=%),g' $< >$@

_install_packages:
	docker-compose exec --user=root ${DOCKER_NAME} /bin/sh -c "apk add tar qt5-qtbase-dev g++ clang libical-dev postgresql-dev llvm8 make"

_compile:
	docker-compose exec --user=root ${DOCKER_NAME} /bin/sh -c "cp ${PGXS} ./${CUSTOM_PGXS} && make"

_install:
	docker-compose exec --user=root ${DOCKER_NAME} /bin/sh -c "export EXTVERSION=${EXTVERSION} && ./install.sh"

_tests:
	docker-compose exec --user=postgres ${DOCKER_NAME} /bin/sh -c "make installcheck"

_build: _install_packages _compile _install _tests

_clean_up:
	docker-compose exec --user=root ${DOCKER_NAME} /bin/sh -c "export CUSTOM_PGXS=${CUSTOM_PGXS} && ./clean_up.sh"