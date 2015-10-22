#include "pg_rrule.h"

#include <utils/timestamp.h>
#include <utils/array.h>
#include <catalog/pg_type.h> // oids
#include <utils/lsyscache.h> // get_typlenbyvalalign
#include "utils/builtins.h" // cstring_to_text

const char* icalrecur_freq_to_string(icalrecurrencetype_frequency kind); // no public definition in ical.h
const char* icalrecur_weekday_to_string(icalrecurrencetype_weekday kind);  // no public definition in ical.h

Datum pg_rrule_in(PG_FUNCTION_ARGS) {
    const char* const rrule_str = PG_GETARG_CSTRING(0);

    struct icalrecurrencetype recurrence = icalrecurrencetype_from_string(rrule_str);

    const icalerrorenum err = icalerrno;

    if (err != ICAL_NO_ERROR) {
        icalerror_clear_errno();

        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Can't parse RRULE. iCal error: %s. RRULE \"%s\".", icalerror_strerror(err), rrule_str),
                 errhint("You need to omit \"RRULE:\" part of expression (if present)")));
    }

    if (recurrence.freq == ICAL_NO_RECURRENCE) {
        // libical 1.0 won't round trip this, so we treat it as an error.
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Invalid RRULE frequency. RRULE \"%s\".", rrule_str)));
    }

    struct icalrecurrencetype* recurrence_ref = palloc(sizeof(struct icalrecurrencetype));

    (*recurrence_ref) = recurrence;

    PG_RETURN_POINTER(recurrence_ref);
}


Datum pg_rrule_out(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    char* const rrule_str = icalrecurrencetype_as_string(recurrence_ref);

    const icalerrorenum err = icalerrno;

    if (err != ICAL_NO_ERROR) {
        icalerror_clear_errno();

        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Can't convert RRULE to string. iCal error: %s", icalerror_strerror(err)),
                 errhint("Please create new issue here: https://github.com/petropavel13/pg_rrule/issues/new")));
    }

    const size_t str_bytes = sizeof(char) * (strlen(rrule_str) + 1);

    char* const rrule_str_copy = palloc(str_bytes);
    memcpy(rrule_str_copy, rrule_str, str_bytes);

    PG_RETURN_CSTRING(rrule_str_copy);
}


Datum pg_rrule_get_occurrences_dtstart_tz(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);
    TimestampTz dtstart_ts = PG_GETARG_TIMESTAMPTZ(1);

    long int gmtoff = 0;
    icaltimezone* ical_tz = NULL;
    if (pg_get_timezone_offset(session_timezone, &gmtoff)) {
        ical_tz = icaltimezone_get_builtin_timezone_from_offset(gmtoff, pg_get_timezone_name(session_timezone));
    }

    if (ical_tz == NULL) {
        elog(WARNING, "Can't get timezone from current session! Fallback to UTC.");
        ical_tz = icaltimezone_get_utc_timezone();
    }

    pg_time_t dtstart_ts_pg_time_t = timestamptz_to_time_t(dtstart_ts);

    struct icaltimetype dtstart = icaltime_from_timet_with_zone((time_t)dtstart_ts_pg_time_t, 0, ical_tz); // it's safe ? time_t may be double, float, etc...

    return pg_rrule_get_occurrences_rrule(*recurrence_ref, dtstart, true);
}

Datum pg_rrule_get_occurrences_dtstart_until_tz(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);
    TimestampTz dtstart_ts = PG_GETARG_TIMESTAMPTZ(1);
    TimestampTz until_ts = PG_GETARG_TIMESTAMPTZ(2);

    long int gmtoff = 0;
    icaltimezone* ical_tz = NULL;
    if (pg_get_timezone_offset(session_timezone, &gmtoff)) {
        ical_tz = icaltimezone_get_builtin_timezone_from_offset(gmtoff, pg_get_timezone_name(session_timezone));
    }

    if (ical_tz == NULL) {
        elog(WARNING, "Can't get timezone from current session! Fallback to UTC.");
        ical_tz = icaltimezone_get_utc_timezone();
    }

    pg_time_t dtstart_ts_pg_time_t = timestamptz_to_time_t(dtstart_ts);
    pg_time_t until_ts_pg_time_t = timestamptz_to_time_t(until_ts);

    struct icaltimetype dtstart = icaltime_from_timet_with_zone((time_t)dtstart_ts_pg_time_t, 0, ical_tz); // it's safe ? time_t may be double, float, etc...
    struct icaltimetype until = icaltime_from_timet_with_zone((time_t)until_ts_pg_time_t, 0, ical_tz); // it's safe ? time_t may be double, float, etc...

    return pg_rrule_get_occurrences_rrule_until(*recurrence_ref, dtstart, until, true);
}


Datum pg_rrule_get_occurrences_dtstart(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);
    Timestamp dtstart_ts = PG_GETARG_TIMESTAMP(1);

    pg_time_t dtstart_ts_pg_time_t = timestamptz_to_time_t(dtstart_ts);

    struct icaltimetype dtstart = icaltime_from_timet_with_zone((time_t)dtstart_ts_pg_time_t, 0, icaltimezone_get_utc_timezone()); // it's safe ? time_t may be double, float, etc...

    return pg_rrule_get_occurrences_rrule(*recurrence_ref, dtstart, false);
}


Datum pg_rrule_get_occurrences_dtstart_until(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);
    Timestamp dtstart_ts = PG_GETARG_TIMESTAMP(1);
    Timestamp until_ts = PG_GETARG_TIMESTAMPTZ(2);

    pg_time_t dtstart_ts_pg_time_t = timestamptz_to_time_t(dtstart_ts);
    pg_time_t until_ts_pg_time_t = timestamptz_to_time_t(until_ts);

    struct icaltimetype dtstart = icaltime_from_timet_with_zone((time_t)dtstart_ts_pg_time_t, 0, icaltimezone_get_utc_timezone()); // it's safe ? time_t may be double, float, etc...
    struct icaltimetype until = icaltime_from_timet_with_zone((time_t)until_ts_pg_time_t, 0, icaltimezone_get_utc_timezone()); // it's safe ? time_t may be double, float, etc...

    return pg_rrule_get_occurrences_rrule_until(*recurrence_ref, dtstart, until, false);
}

/* FREQ */
Datum pg_rrule_get_freq_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    if (recurrence_ref->freq == ICAL_NO_RECURRENCE) {
        PG_RETURN_NULL();
    }

    const char* const freq_string = icalrecur_freq_to_string(recurrence_ref->freq);

    PG_RETURN_TEXT_P(cstring_to_text(freq_string));
}


/* UNTIL */
Datum pg_rrule_get_until_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    if (icaltime_is_null_time(recurrence_ref->until)) {
        PG_RETURN_NULL();
    }

    pg_time_t until_pg_time_t = (pg_time_t)icaltime_as_timet_with_zone(recurrence_ref->until, icaltimezone_get_utc_timezone()); // it's safe ? time_t may be double, float, etc...

    PG_RETURN_TIMESTAMP(time_t_to_timestamptz(until_pg_time_t));
}


/* UNTIL TZ */
Datum pg_rrule_get_untiltz_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    if (icaltime_is_null_time(recurrence_ref->until)) {
        PG_RETURN_NULL();
    }

    long int gmtoff = 0;
    icaltimezone* ical_tz = NULL;
    if (pg_get_timezone_offset(session_timezone, &gmtoff)) {
        ical_tz = icaltimezone_get_builtin_timezone_from_offset(gmtoff, pg_get_timezone_name(session_timezone));
    }

    if (ical_tz == NULL) {
        elog(WARNING, "Can't get timezone from current session! Fallback to UTC.");
        ical_tz = icaltimezone_get_utc_timezone();
    }

    pg_time_t until_pg_time_t = (pg_time_t)icaltime_as_timet_with_zone(recurrence_ref->until, ical_tz); // it's safe ? time_t may be double, float, etc...

    PG_RETURN_TIMESTAMP(time_t_to_timestamptz(until_pg_time_t));
}


/* COUNT */
Datum pg_rrule_get_count_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);
    PG_RETURN_INT32(recurrence_ref->count);
}


/* INTERVAL */
Datum pg_rrule_get_interval_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);
    PG_RETURN_INT16(recurrence_ref->interval);
}


/* BYSECOND */
Datum pg_rrule_get_bysecond_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    unsigned int cnt = 0;
    for (; cnt < ICAL_BY_SECOND_SIZE && recurrence_ref->by_second[cnt] != ICAL_RECURRENCE_ARRAY_MAX; ++cnt);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    unsigned int i = 0;
    for (i = 0; i < cnt; ++i) {
        datum_elems[i] = Int16GetDatum(recurrence_ref->by_second[i]);
    }

    int16 typlen;
    bool typbyval;
    char typalign;

    get_typlenbyvalalign(INT2OID, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, INT2OID, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}

/* BYMINUTE */
Datum pg_rrule_get_byminute_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    unsigned int cnt = 0;
    for (; cnt < ICAL_BY_MINUTE_SIZE && recurrence_ref->by_minute[cnt] != ICAL_RECURRENCE_ARRAY_MAX; ++cnt);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    unsigned int i = 0;
    for (i = 0; i < cnt; ++i) {
        datum_elems[i] = Int16GetDatum(recurrence_ref->by_minute[i]);
    }

    int16 typlen;
    bool typbyval;
    char typalign;

    get_typlenbyvalalign(INT2OID, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, INT2OID, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}

/* BYHOUR */
Datum pg_rrule_get_byhour_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    unsigned int cnt = 0;
    for (; cnt < ICAL_BY_HOUR_SIZE && recurrence_ref->by_hour[cnt] != ICAL_RECURRENCE_ARRAY_MAX; ++cnt);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    unsigned int i = 0;
    for (i = 0; i < cnt; ++i) {
        datum_elems[i] = Int16GetDatum(recurrence_ref->by_hour[i]);
    }

    int16 typlen;
    bool typbyval;
    char typalign;

    get_typlenbyvalalign(INT2OID, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, INT2OID, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}

/* BYDAY */
Datum pg_rrule_get_byday_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    unsigned int cnt = 0;
    for (; cnt < ICAL_BY_DAY_SIZE && recurrence_ref->by_day[cnt] != ICAL_RECURRENCE_ARRAY_MAX; ++cnt);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    unsigned int i = 0;
    for (i = 0; i < cnt; ++i) {
        datum_elems[i] = Int16GetDatum(recurrence_ref->by_day[i]);
    }

    int16 typlen;
    bool typbyval;
    char typalign;

    get_typlenbyvalalign(INT2OID, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, INT2OID, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}

/* BYMONTHDAY */
Datum pg_rrule_get_bymonthday_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    unsigned int cnt = 0;
    for (; cnt < ICAL_BY_MONTHDAY_SIZE && recurrence_ref->by_month_day[cnt] != ICAL_RECURRENCE_ARRAY_MAX; ++cnt);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    unsigned int i = 0;
    for (i = 0; i < cnt; ++i) {
        datum_elems[i] = Int16GetDatum(recurrence_ref->by_month_day[i]);
    }

    int16 typlen;
    bool typbyval;
    char typalign;

    get_typlenbyvalalign(INT2OID, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, INT2OID, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}

/* BYYEARDAY */
Datum pg_rrule_get_byyearday_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    unsigned int cnt = 0;
    for (; cnt < ICAL_BY_YEARDAY_SIZE && recurrence_ref->by_year_day[cnt] != ICAL_RECURRENCE_ARRAY_MAX; ++cnt);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    unsigned int i = 0;
    for (i = 0; i < cnt; ++i) {
        datum_elems[i] = Int16GetDatum(recurrence_ref->by_year_day[i]);
    }

    int16 typlen;
    bool typbyval;
    char typalign;

    get_typlenbyvalalign(INT2OID, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, INT2OID, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}

/* BYWEEKNO */
Datum pg_rrule_get_byweekno_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    unsigned int cnt = 0;
    for (; cnt < ICAL_BY_WEEKNO_SIZE && recurrence_ref->by_week_no[cnt] != ICAL_RECURRENCE_ARRAY_MAX; ++cnt);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    unsigned int i = 0;
    for (i = 0; i < cnt; ++i) {
        datum_elems[i] = Int16GetDatum(recurrence_ref->by_week_no[i]);
    }

    int16 typlen;
    bool typbyval;
    char typalign;

    get_typlenbyvalalign(INT2OID, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, INT2OID, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}

/* BYMONTH */
Datum pg_rrule_get_bymonth_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    unsigned int cnt = 0;
    for (; cnt < ICAL_BY_MONTH_SIZE && recurrence_ref->by_month[cnt] != ICAL_RECURRENCE_ARRAY_MAX; ++cnt);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    unsigned int i = 0;
    for (i = 0; i < cnt; ++i) {
        datum_elems[i] = Int16GetDatum(recurrence_ref->by_month[i]);
    }

    int16 typlen;
    bool typbyval;
    char typalign;

    get_typlenbyvalalign(INT2OID, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, INT2OID, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}

/* BYSETPOS */
Datum pg_rrule_get_bysetpos_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    unsigned int cnt = 0;
    for (; cnt < ICAL_BY_SETPOS_SIZE && recurrence_ref->by_set_pos[cnt] != ICAL_RECURRENCE_ARRAY_MAX; ++cnt);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    unsigned int i = 0;
    for (i = 0; i < cnt; ++i) {
        datum_elems[i] = Int16GetDatum(recurrence_ref->by_set_pos[i]);
    }

    int16 typlen;
    bool typbyval;
    char typalign;

    get_typlenbyvalalign(INT2OID, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, INT2OID, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}

/* WKST */
Datum pg_rrule_get_wkst_rrule(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    if (recurrence_ref->week_start == ICAL_NO_WEEKDAY) {
        PG_RETURN_NULL();
    }

    const char* const wkst_string = icalrecur_weekday_to_string(recurrence_ref->week_start);

    PG_RETURN_TEXT_P(cstring_to_text(wkst_string));
}


Datum pg_rrule_get_occurrences_rrule(struct icalrecurrencetype recurrence,
                                     struct icaltimetype dtstart,
                                     bool use_tz) {
    return pg_rrule_get_occurrences_rrule_until(recurrence, dtstart, icaltime_null_time(), use_tz);
}

Datum pg_rrule_get_occurrences_rrule_until(struct icalrecurrencetype recurrence,
                                           struct icaltimetype dtstart,
                                           struct icaltimetype until,
                                           bool use_tz) {
    time_t* times_array = NULL;
    unsigned int cnt = 0;

    pg_rrule_rrule_to_time_t_array_until(recurrence, dtstart, until, &times_array, &cnt);
    pg_time_t* pg_times_array = palloc(sizeof(pg_time_t) * cnt);

    unsigned int i;

    for (i = 0; i < cnt; ++i) {
        pg_times_array[i] = (pg_time_t)times_array[i]; // it's safe ? time_t may be double, float, etc...
    }

    free(times_array);

    Datum* const datum_elems = palloc(sizeof(Datum) * cnt);

    if (use_tz) {
        for (i = 0; i < cnt; ++i) {
            datum_elems[i] = TimestampTzGetDatum(time_t_to_timestamptz(pg_times_array[i]));
        }
    } else {
        for (i = 0; i < cnt; ++i) {
            datum_elems[i] = TimestampGetDatum(time_t_to_timestamptz(pg_times_array[i]));
        }
    }

    pfree(pg_times_array);

    int16 typlen;
    bool typbyval;
    char typalign;

    const Oid ts_oid = use_tz ? TIMESTAMPTZOID : TIMESTAMPOID;

    get_typlenbyvalalign(ts_oid, &typlen, &typbyval, &typalign);

    ArrayType* result_array = construct_array(datum_elems, cnt, ts_oid, typlen, typbyval, typalign);

    PG_RETURN_ARRAYTYPE_P(result_array);
}


void pg_rrule_rrule_to_time_t_array(struct icalrecurrencetype recurrence,
                                    struct icaltimetype dtstart,
                                    time_t** const out_array,
                                    unsigned int* const out_count) {
    pg_rrule_rrule_to_time_t_array_until(recurrence, dtstart, icaltime_null_time(), out_array, out_count);
}

void pg_rrule_rrule_to_time_t_array_until(struct icalrecurrencetype recurrence,
                                          struct icaltimetype dtstart,
                                          struct icaltimetype until,
                                          time_t** const out_array,
                                          unsigned int* const out_count) {

    icalrecur_iterator* const recur_iterator = icalrecur_iterator_new(recurrence, dtstart);

    icalarray* const icaltimes_list = icalarray_new(sizeof(icaltimetype), 32);

    struct icaltimetype ical_time = icalrecur_iterator_next(recur_iterator);

    if (icaltime_is_null_time(until)) {
        while (icaltime_is_null_time(ical_time) == false) {
            icalarray_append(icaltimes_list, &ical_time);
            ical_time = icalrecur_iterator_next(recur_iterator);
        }
    } else {
        while (icaltime_is_null_time(ical_time) == false
               && icaltime_compare(ical_time, until) != 1 ) { // while ical_time <= until
            icalarray_append(icaltimes_list, &ical_time);
            ical_time = icalrecur_iterator_next(recur_iterator);
        }
    }

    icalrecur_iterator_free(recur_iterator);

    const unsigned int cnt = (*out_count) = icaltimes_list->num_elements;

    time_t* times_array = (*out_array) = malloc(sizeof(time_t) * cnt);

    unsigned int i = 0;

    for (i = 0; i < cnt; ++i) {
        ical_time = (*(icaltimetype*)icalarray_element_at(icaltimes_list, i));
        times_array[i] = icaltime_as_timet_with_zone(ical_time, dtstart.zone);
    }

    icalarray_free(icaltimes_list);
}
