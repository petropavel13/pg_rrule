set -e
cd /usr/src/app

if test -f ./pg_rrule.so; then
	rm ./pg_rrule.so
fi
if test -f ./regression.diffs; then
	rm ./regression.diffs
fi
if test -f ./regression.out; then
	rm ./regression.out
fi
if test -f ./src/pg_rrule.bc; then
	rm ./src/pg_rrule.bc
fi
if test -f ./src/pg_rrule.o; then
	rm ./src/pg_rrule.o
fi
if test -f ./sql/pg_rrule--0.2.0.sql; then
	rm ./sql/pg_rrule--0.2.0.sql
fi
if test -f ./sql/pg_rrule.sql; then
	rm ./sql/pg_rrule.sql
fi
if test -f ./results/base.out; then
	rm ./results/base.out
fi

if test -f ./$CUSTOM_PGXS; then
	rm ./$CUSTOM_PGXS
fi