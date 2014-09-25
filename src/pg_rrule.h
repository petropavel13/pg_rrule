#ifndef PG_RRULE_H
#define PG_RRULE_H

#include <libical/ical.h>

#include <postgres.h>
#include <fmgr.h>

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_rrule_in);
Datum pg_rrule_in(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pg_rrule_out);
Datum pg_rrule_out(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pg_rrule_get_occurrences_dtstart_tz);
Datum pg_rrule_get_occurrences_dtstart_tz(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pg_rrule_get_occurrences_dtstart_until_tz);
Datum pg_rrule_get_occurrences_dtstart_until_tz(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pg_rrule_get_occurrences_dtstart);
Datum pg_rrule_get_occurrences_dtstart(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pg_rrule_get_occurrences_dtstart_until);
Datum pg_rrule_get_occurrences_dtstart_until(PG_FUNCTION_ARGS);

/* FREQ */
PG_FUNCTION_INFO_V1(pg_rrule_get_freq_rrule);
Datum pg_rrule_get_freq_rrule(PG_FUNCTION_ARGS);

/* UNTIL */
PG_FUNCTION_INFO_V1(pg_rrule_get_until_rrule);
Datum pg_rrule_get_until_rrule(PG_FUNCTION_ARGS);

/* UNTIL TZ */
PG_FUNCTION_INFO_V1(pg_rrule_get_untiltz_rrule);
Datum pg_rrule_get_untiltz_rrule(PG_FUNCTION_ARGS);

/* COUNT */
PG_FUNCTION_INFO_V1(pg_rrule_get_count_rrule);
Datum pg_rrule_get_count_rrule(PG_FUNCTION_ARGS);

/* INTERVAL */
PG_FUNCTION_INFO_V1(pg_rrule_get_interval_rrule);
Datum pg_rrule_get_interval_rrule(PG_FUNCTION_ARGS);

/* BYSECOND */
PG_FUNCTION_INFO_V1(pg_rrule_get_bysecond_rrule);
Datum pg_rrule_get_bysecond_rrule(PG_FUNCTION_ARGS);

/* BYMINUTE */
PG_FUNCTION_INFO_V1(pg_rrule_get_byminute_rrule);
Datum pg_rrule_get_byminute_rrule(PG_FUNCTION_ARGS);

/* BYHOUR */
PG_FUNCTION_INFO_V1(pg_rrule_get_byhour_rrule);
Datum pg_rrule_get_byhour_rrule(PG_FUNCTION_ARGS);

/* BYDAY */
PG_FUNCTION_INFO_V1(pg_rrule_get_byday_rrule);
Datum pg_rrule_get_byday_rrule(PG_FUNCTION_ARGS);

/* BYMONTHDAY */
PG_FUNCTION_INFO_V1(pg_rrule_get_bymonthday_rrule);
Datum pg_rrule_get_bymonthday_rrule(PG_FUNCTION_ARGS);

/* BYYEARDAY */
PG_FUNCTION_INFO_V1(pg_rrule_get_byyearday_rrule);
Datum pg_rrule_get_byyearday_rrule(PG_FUNCTION_ARGS);

/* BYWEEKNO */
PG_FUNCTION_INFO_V1(pg_rrule_get_byweekno_rrule);
Datum pg_rrule_get_byweekno_rrule(PG_FUNCTION_ARGS);

/* BYMONTH */
PG_FUNCTION_INFO_V1(pg_rrule_get_bymonth_rrule);
Datum pg_rrule_get_bymonth_rrule(PG_FUNCTION_ARGS);

/* BYSETPOS */
PG_FUNCTION_INFO_V1(pg_rrule_get_bysetpos_rrule);
Datum pg_rrule_get_bysetpos_rrule(PG_FUNCTION_ARGS);

/* WKST */
PG_FUNCTION_INFO_V1(pg_rrule_get_wkst_rrule);
Datum pg_rrule_get_wkst_rrule(PG_FUNCTION_ARGS);


Datum pg_rrule_get_occurrences_rrule(struct icalrecurrencetype recurrence,
                                     struct icaltimetype dtstart,
                                     bool use_tz);

Datum pg_rrule_get_occurrences_rrule_until(struct icalrecurrencetype recurrence,
                                           struct icaltimetype dtstart,
                                           struct icaltimetype until,
                                           bool use_tz);


void pg_rrule_rrule_to_time_t_array(struct icalrecurrencetype recurrence,
                                    struct icaltimetype dtstart,
                                    time_t** const out_array,
                                    unsigned int* const out_count);

void pg_rrule_rrule_to_time_t_array_until(struct icalrecurrencetype recurrence,
                                          struct icaltimetype dtstart,
                                          struct icaltimetype until,
                                          time_t** const out_array,
                                          unsigned int* const out_count);

#endif // PG_RRULE_H
