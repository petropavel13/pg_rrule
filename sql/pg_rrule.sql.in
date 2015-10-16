/*
 * Author: petropavel
 * Created at: 2014-09-14 23:36:11 +0600
 *
 */

--
-- This is a example code genereted automaticaly
-- by pgxn-utils.

SET client_min_messages = warning;


CREATE TYPE rrule;

CREATE OR REPLACE FUNCTION rrule_in(cstring)
    RETURNS rrule
    AS 'MODULE_PATHNAME', 'pg_rrule_in'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION rrule_out(rrule)
    RETURNS cstring
    AS 'MODULE_PATHNAME', 'pg_rrule_out'
    LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE rrule (
   input = rrule_in,
   output = rrule_out,
   internallength = 2760 -- TODO: check sizeof(icalrecurrencetype) on 32-bit machine
);


CREATE CAST (text AS rrule)
    WITH INOUT;

CREATE CAST (varchar AS rrule)
    WITH INOUT;


CREATE OR REPLACE FUNCTION get_occurrences(rrule, timestamp with time zone)
    RETURNS timestamp with time zone[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_occurrences_dtstart_tz'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION get_occurrences(rrule, timestamp with time zone, timestamp with time zone)
    RETURNS timestamp with time zone[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_occurrences_dtstart_until_tz'
    LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION get_occurrences(rrule, timestamp)
    RETURNS timestamp[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_occurrences_dtstart'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION get_occurrences(rrule, timestamp, timestamp)
    RETURNS timestamp[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_occurrences_dtstart_until'
    LANGUAGE C IMMUTABLE STRICT;


/* FREQ */
CREATE OR REPLACE FUNCTION get_freq(rrule)
    RETURNS text
    AS 'MODULE_PATHNAME', 'pg_rrule_get_freq_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* UNTIL */
CREATE OR REPLACE FUNCTION get_until(rrule)
    RETURNS timestamp
    AS 'MODULE_PATHNAME', 'pg_rrule_get_until_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* UNTIL TZ */
CREATE OR REPLACE FUNCTION get_untiltz(rrule)
    RETURNS timestamp with time zone
    AS 'MODULE_PATHNAME', 'pg_rrule_get_untiltz_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* COUNT */
CREATE OR REPLACE FUNCTION get_count(rrule)
    RETURNS int4
    AS 'MODULE_PATHNAME', 'pg_rrule_get_count_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* INTERVAL */
CREATE OR REPLACE FUNCTION get_interval(rrule)
    RETURNS int2
    AS 'MODULE_PATHNAME', 'pg_rrule_get_interval_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* BYSECOND */
CREATE OR REPLACE FUNCTION get_bysecond(rrule)
    RETURNS int2[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_bysecond_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* BYMINUTE */
CREATE OR REPLACE FUNCTION get_byminute(rrule)
    RETURNS int2[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_byminute_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* BYHOUR */
CREATE OR REPLACE FUNCTION get_byhour(rrule)
    RETURNS int2[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_byhour_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* BYDAY */
CREATE OR REPLACE FUNCTION get_byday(rrule)
    RETURNS int2[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_byday_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* BYMONTHDAY */
CREATE OR REPLACE FUNCTION get_bymonthday(rrule)
    RETURNS int2[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_bymonthday_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* BYYEARDAY */
CREATE OR REPLACE FUNCTION get_byyearday(rrule)
    RETURNS int2[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_byyearday_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* BYWEEKNO */
CREATE OR REPLACE FUNCTION get_byweekno(rrule)
    RETURNS int2[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_byweekno_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* BYMONTH */
CREATE OR REPLACE FUNCTION get_bymonth(rrule)
    RETURNS int2[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_bymonth_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* BYSETPOS */
CREATE OR REPLACE FUNCTION get_bysetpos(rrule)
    RETURNS int2[]
    AS 'MODULE_PATHNAME', 'pg_rrule_get_bysetpos_rrule'
    LANGUAGE C IMMUTABLE STRICT;


/* WKST */
CREATE OR REPLACE FUNCTION get_wkst(rrule)
    RETURNS text
    AS 'MODULE_PATHNAME', 'pg_rrule_get_wkst_rrule'
    LANGUAGE C IMMUTABLE STRICT;



