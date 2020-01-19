\set ECHO none
BEGIN;
\i sql/pg_rrule.sql
\set ECHO all

SELECT 'FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z';

SELECT get_byday('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z;BYDAY=MO,TH,SU');

SELECT get_freq('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z');

SELECT * FROM
    unnest(
        get_occurrences('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z;BYDAY=SA;BYHOUR=10;BYMINUTE=51;BYSECOND=2',
            '2019-12-07 10:51:02+00'::timestamp with time zone)
    );

SELECT * FROM
    unnest(
        get_occurrences('FREQ=WEEKLY;INTERVAL=1;WKST=MO;UNTIL=20200101T045102Z;BYDAY=SA;BYHOUR=10;BYMINUTE=51;BYSECOND=2',
            '2019-12-07 10:51:02'::timestamp)
    );

SELECT * FROM (SELECT UNNEST(t.occurrence) as occurence
               FROM get_occurrences('FREQ=WEEKLY;WKST=MO;BYDAY=MO,TU,WE,TH,FR,SA,SU;INTERVAL=1;UNTIL=20200108',
                                    '20200105'::timestamp) as t(occurrence)
               UNION
               SELECT UNNEST(t.occurrence) as occurence
               FROM get_occurrences('FREQ=WEEKLY;WKST=MO;BYDAY=MO,TU,WE,TH,FR,SA,SU;INTERVAL=1;UNTIL=20200113',
                                    '20200108'::timestamp) as t(occurrence)) as t
ORDER BY t.occurence;


-- TEST: we want repeat each second thuesday,
SELECT UNNEST(t.occurrence) as occurence
FROM get_occurrences('FREQ=WEEKLY;WKST=MO;BYDAY=TU;INTERVAL=2;UNTIL=20200129',
                     '20200114'::timestamp) as t(occurrence);
-- -> we are starting from day we want, and end day is set to +1 day

-- TEST: change WKST to wednesday and start in middle of week, repeatin until 01-29-2020
SELECT UNNEST(t.occurrence) as occurence
FROM get_occurrences('FREQ=WEEKLY;WKST=WE;BYDAY=MO,TU,WE,TH,FR,SA,SU;INTERVAL=2;UNTIL=20200130',
                     '20200106'::timestamp) as t(occurrence);
-- -> repeat until end of week and then start from the start of next two weeks

-- TEST: each send month, each second tuesday of month(BYSETPOS=2), 3 times(COUNT=3)
SELECT UNNEST(t.occurrence) as occurence
FROM get_occurrences('FREQ=MONTHLY;INTERVAL=2;WKST=MO;BYDAY=TU;BYSETPOS=2;COUNT=3',
                     '20200106'::timestamp) as t(occurrence);
-- 
ROLLBACK;
