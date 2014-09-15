pg_rrule
========

Synopsis
--------

  RRULE field type for PostgreSQL

Description
-----------



Usage
-----

 SELECT 'FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z;BYDAY=SA;BYHOUR=10;BYMINUTE=51;BYSECOND=2'::rrule;
                                    rrule
 ------------------------------------------------------------------------------
  FREQ=WEEKLY;UNTIL=20200101T045102Z;BYSECOND=2;BYMINUTE=51;BYHOUR=10;BYDAY=SA
 (1 row)

 SELECT * FROM
     unnest(
         rrule_get_occurrences('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z;BYDAY=SA;BYHOUR=10;BYMINUTE=51;BYSECOND=2'::rrule,
             '2019-12-07 10:51:02+00'::timestamp with time zone)
     );
          unnest
 ------------------------
  2019-12-07 10:51:02+00
  2019-12-14 10:51:02+00
  2019-12-21 10:51:02+00
  2019-12-28 10:51:02+00
 (4 rows)

 SELECT * FROM
     unnest(
         rrule_get_occurrences('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z;BYDAY=SA;BYHOUR=10;BYMINUTE=51;BYSECOND=2'::rrule,
             '2019-12-07 10:51:02'::timestamp)
     );
        unnest
 ---------------------
  2019-12-07 10:51:02
  2019-12-14 10:51:02
  2019-12-21 10:51:02
  2019-12-28 10:51:02
 (4 rows)

Support
-------

  There is issues tracker? Github? Put this information here.

Author
------

petropavel

Copyright and License
---------------------

Copyright (c) 2014 petropavel.

