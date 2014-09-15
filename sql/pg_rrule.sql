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
    AS '$libdir/pg_rrule'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION rrule_out(rrule)
    RETURNS cstring
    AS '$libdir/pg_rrule'
    LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE rrule (
   input = rrule_in,
   output = rrule_out
);


CREATE OR REPLACE FUNCTION rrule_get_occurrences(rrule, timestamp with time zone)
    RETURNS timestamp with time zone[]
    AS '$libdir/pg_rrule', 'rrule_get_occurrences_rrule_timestamptz'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION rrule_get_occurrences(rrule, timestamp)
    RETURNS timestamp[]
    AS '$libdir/pg_rrule', 'rrule_get_occurrences_rrule_timestamp'
    LANGUAGE C IMMUTABLE STRICT;


CREATE CAST (text AS rrule)
    WITH INOUT;

CREATE CAST (varchar AS rrule)
    WITH INOUT;
