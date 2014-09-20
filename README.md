pg_rrule
========

Usage

Get RRULE parameter get_PARAMNAME.

Example. Get freq param:
```sql
 SELECT get_freq('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z'::rrule);
  get_freq
 ----------
  WEEKLY
 (1 row)
```
Example. Get byday param:
```sql
 SELECT get_byday('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z;BYDAY=MO,TH,SU'::rrule);
  get_byday
 -----------
  {2,5,1}
 (1 row)
```
Example. Expand RRULE with timezone:
```sql
 SELECT * FROM
     unnest(
         get_occurrences('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z;BYDAY=SA;BYHOUR=10;BYMINUTE=51;BYSECOND=2'::rrule,
             '2019-12-07 10:51:02+00'::timestamp with time zone)
     );

          unnest
 ------------------------
  2019-12-07 10:51:02+00
  2019-12-14 10:51:02+00
  2019-12-21 10:51:02+00
  2019-12-28 10:51:02+00
 (4 rows)
```
Example. Expand RRULE without timezone:
```sql
 SELECT * FROM
     unnest(
         get_occurrences('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z;BYDAY=SA;BYHOUR=10;BYMINUTE=51;BYSECOND=2'::rrule,
             '2019-12-07 10:51:02'::timestamp)
     );

        unnest
 ---------------------
  2019-12-07 10:51:02
  2019-12-14 10:51:02
  2019-12-21 10:51:02
  2019-12-28 10:51:02
 (4 rows)
```
 Simple building:
 --------
 ```sh
 cd src/
 qmake pg_rrule.pro
 make
 ```
  Simple install (Linux):
 ```sh
 cp libpg_rrule.so /usr/lib/postgresql/pg_rrule.so
 cp pg_rrule.control /usr/share/postgresql/extension/
 cp sql/pg_rrule.sql /usr/share/postgresql/extension/pg_rrule--0.1.0.sql
 ```
 Simple install (OS X):
 ```sh
 #TODO
 ```
 Install extension (Postgresql):
 ```sql
 CREATE EXTENSION pg_rrule;
 ```

Building (A long description (not tested))

To build it, just do this:

    make
    make installcheck
    make install

If you encounter an error such as:

    "Makefile", line 8: Need an operator

You need to use GNU make, which may well be installed on your system as
`gmake`:

    gmake
    gmake install
    gmake installcheck

If you encounter an error such as:

    make: pg_config: Command not found

Be sure that you have `pg_config` installed and in your path. If you used a
package management system such as RPM to install PostgreSQL, be sure that the
`-devel` package is also installed. If necessary tell the build process where
to find it:

    env PG_CONFIG=/path/to/pg_config make && make installcheck && make install

And finally, if all that fails (and if you're on PostgreSQL 8.1 or lower, it
likely will), copy the entire distribution directory to the `contrib/`
subdirectory of the PostgreSQL source tree and try it there without
`pg_config`:

    env NO_PGXS=1 make && make installcheck && make install

If you encounter an error such as:

    ERROR:  must be owner of database regression

You need to run the test suite using a super user, such as the default
"postgres" super user:

    make installcheck PGUSER=postgres

Once pg_rrule is installed, you can add it to a database. If you're running
PostgreSQL 9.1.0 or greater, it's a simple as connecting to a database as a
super user and running:

    CREATE EXTENSION pg_rrule;

If you've upgraded your cluster to PostgreSQL 9.1 and already had pg_rrule
installed, you can upgrade it to a properly packaged extension with:

    CREATE EXTENSION pg_rrule FROM unpackaged;

For versions of PostgreSQL less than 9.1.0, you'll need to run the
installation script:

    psql -d mydb -f /path/to/pgsql/share/contrib/pg_rrule.sql

If you want to install pg_rrule and all of its supporting objects into a specific
schema, use the `PGOPTIONS` environment variable to specify the schema, like
so:

    PGOPTIONS=--search_path=extensions psql -d mydb -f pg_rrule.sql

Dependencies
------------
The `pg_rrule` has libical as dependency.

Copyright and License
---------------------

Copyright (c) 2014 petropavel.

