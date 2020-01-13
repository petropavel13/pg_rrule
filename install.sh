# set -e

# if test -f /usr/local/lib/postgresql/pg_rrule.so; then
# 	rm /usr/local/lib/postgresql/pg_rrule.so
# fi
# if test -f /usr/local/share/postgresql/extension/pg_rrule.control; then
# 	rm /usr/local/share/postgresql/extension/pg_rrule.control
# fi
# if test -f /usr/local/share/postgresql/extension/pg_rrule--$EXTVERSION.sql; then
# 	rm /usr/local/share/postgresql/extension/pg_rrule--$EXTVERSION.sql
# fi

# cp pg_rrule.so /usr/local/lib/postgresql/pg_rrule.so
# cp pg_rrule.control /usr/local/share/postgresql/extension/
# cp sql/pg_rrule.sql /usr/local/share/postgresql/extension/pg_rrule--$EXTVERSION.sql
make install

# https://git.postgresql.org/gitweb/?p=pldebugger.git;a=snapshot;h=HEAD;sf=tgz

# cp pg_rrule.so /usr/local/pgsql/share/extension/pg_rrule.control \
cp pg_rrule.so /usr/local/lib/postgresql/pg_rrule.so
\
&& cp pg_rrule.control /usr/local/share/postgresql/extension \
&& cp sql/pg_rrule.sql /usr/local/share/postgresql/extension/pg_rrule--0.2.0.sql


cp pg_rrule.control /usr/local/pgsql/share/extension/