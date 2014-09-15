#include "pg_rrule.h"

#include <utils/timestamp.h>
#include <utils/array.h>
#include <catalog/pg_type.h>
#include <utils/lsyscache.h>

Datum rrule_in(PG_FUNCTION_ARGS) {
    const char* const rrule_str = PG_GETARG_CSTRING(0);

    struct icalrecurrencetype recurrence = icalrecurrencetype_from_string(rrule_str);

    const icalerrorenum err = icalerrno;

    if (err != ICAL_NO_ERROR) {
        icalerror_clear_errno();

        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("String \"%s\" is not correct RRULE. iCal error: %s", rrule_str, icalerror_strerror(err)),
                 errhint("You need to omit \"RRULE:\" part of expression(if present)")));
    }

    struct icalrecurrencetype* recurrence_ref = palloc(sizeof(struct icalrecurrencetype));

    (*recurrence_ref) = recurrence;

    PG_RETURN_POINTER(recurrence_ref);
}


Datum rrule_out(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);

    char* const rrule_str = icalrecurrencetype_as_string(recurrence_ref);

    const icalerrorenum err = icalerrno;

    if (err != ICAL_NO_ERROR) {
        icalerror_clear_errno();

        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("Error when converting RRULE to string. iCal error: %s", icalerror_strerror(err)),
                 errhint("Please create new issue here: https://github.com/petropavel13/pg_rrule/issues/new")));
    }

    const size_t str_bytes = sizeof(char) * (strlen(rrule_str) + 1);

    char* const rrule_str_copy = palloc(str_bytes);
    memcpy(rrule_str_copy, rrule_str, str_bytes);

    PG_RETURN_CSTRING(rrule_str_copy);
}


Datum rrule_get_occurrences_rrule_timestamptz(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);
    TimestampTz ts = PG_GETARG_TIMESTAMPTZ(1);

    long int gmtoff = 0;
    pg_get_timezone_offset(session_timezone, &gmtoff);

    pg_time_t ts_pg_time_t = timestamptz_to_time_t(ts);

    icaltimezone* ical_tz = icaltimezone_get_builtin_timezone_from_offset(gmtoff, pg_get_timezone_name(session_timezone));

    if (ical_tz == NULL) {
        elog(WARNING, "Can't get timezone from current session! Fallback to UTC.");
        ical_tz = icaltimezone_get_utc_timezone();
    }

    struct icaltimetype dtstart = icaltime_from_timet_with_zone((time_t)ts_pg_time_t, 0, ical_tz); // it's safe ? time_t may be double, float, etc...

    return rrule_get_occurrences_rrule(*recurrence_ref, dtstart, true);
}


Datum rrule_get_occurrences_rrule_timestamp(PG_FUNCTION_ARGS) {
    struct icalrecurrencetype* recurrence_ref = (struct icalrecurrencetype*)PG_GETARG_POINTER(0);
    Timestamp ts = PG_GETARG_TIMESTAMP(1);

    pg_time_t ts_pg_time_t = timestamptz_to_time_t(ts);

    struct icaltimetype dtstart = icaltime_from_timet_with_zone((time_t)ts_pg_time_t, 0, icaltimezone_get_utc_timezone()); // it's safe ? time_t may be double, float, etc...

    return rrule_get_occurrences_rrule(*recurrence_ref, dtstart, false);
}


Datum rrule_get_occurrences_rrule(struct icalrecurrencetype recurrence,
                                  struct icaltimetype dtstart,
                                  bool use_tz) {
    time_t* times_array = NULL;
    unsigned int cnt = 0;

    rrule_to_time_t_array(recurrence, dtstart, &times_array, &cnt);
    pg_time_t* pg_times_array = palloc(sizeof(pg_time_t) * cnt);

    unsigned int i;

    for (i = 0; i < cnt; ++i) {
        pg_times_array[i] = (pg_time_t)times_array[i]; // it's safe ? time_t may be double, float, etc...
    }

    free(times_array);

    Datum* datum_elems = palloc(sizeof(Datum) * cnt);

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


void rrule_to_time_t_array(struct icalrecurrencetype recurrence,
                           struct icaltimetype dtstart,
                           time_t** const out_array,
                           unsigned int* const out_count) {

    icalrecur_iterator* const recur_iterator = icalrecur_iterator_new(recurrence, dtstart);

    icalarray* const icaltimes_list = icalarray_new(sizeof(icaltimetype), 32);

    struct icaltimetype ical_time = icalrecur_iterator_next(recur_iterator);

    while (!icaltime_is_null_time(ical_time)) {
        icalarray_append(icaltimes_list, &ical_time);
        ical_time = icalrecur_iterator_next(recur_iterator);
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
