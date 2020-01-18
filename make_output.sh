set -e

if ! test -d ./output; then
	mkdir ./output
fi

if ! test -d ./output/sql; then
	mkdir ./output/sql
fi

if ! test -d ./output/src; then
	mkdir ./output/src
fi

if ! test -d ./output/doc; then
	mkdir ./output/doc
fi
cp ./pg_rrule.so ./output/pg_rrule.so
cp pg_rrule.control ./output/
cp sql/pg_rrule.sql ./output/sql/pg_rrule--$EXTVERSION.sql
cp ./doc/pg_rrule.md ./output/doc
cp ./src/pg_rrule.bc ./output/src/pg_rrule.bc


echo "/bin/mkdir -p '/usr/local/lib/postgresql'
/bin/mkdir -p '/usr/local/share/postgresql/extension'
/bin/mkdir -p '/usr/local/share/postgresql/extension'
/bin/mkdir -p '/usr/local/share/doc/postgresql/extension'
/usr/bin/install -c -m 755  pg_rrule.so '/usr/local/lib/postgresql/pg_rrule.so'
/usr/bin/install -c -m 644 .//pg_rrule.control '/usr/local/share/postgresql/extension/'
/usr/bin/install -c -m 644 .//sql/pg_rrule--$EXTVERSION.sql .//sql/pg_rrule--$EXTVERSION.sql  '/usr/local/share/postgresql/extension/'
/usr/bin/install -c -m 644 .//doc/pg_rrule.md '/usr/local/share/doc/postgresql/extension/'
/bin/mkdir -p '/usr/local/lib/postgresql/bitcode/pg_rrule'
/bin/mkdir -p '/usr/local/lib/postgresql/bitcode'/pg_rrule/src/
/usr/bin/install -c -m 644 src/pg_rrule.bc '/usr/local/lib/postgresql/bitcode'/pg_rrule/src/
cd '/usr/local/lib/postgresql/bitcode' && /usr/lib/llvm8/bin/llvm-lto -thinlto -thinlto-action=thinlink -o pg_rrule.index.bc pg_rrule/src/pg_rrule.bc" > ./output/install.sh