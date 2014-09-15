#ifndef PG_RRULE_H
#define PG_RRULE_H

#include <libical/ical.h>

#include <postgres.h>
#include <fmgr.h>

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(rrule_in);
Datum rrule_in(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(rrule_out);
Datum rrule_out(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(rrule_get_occurrences_rrule_timestamptz);
Datum rrule_get_occurrences_rrule_timestamptz(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(rrule_get_occurrences_rrule_timestamp);
Datum rrule_get_occurrences_rrule_timestamp(PG_FUNCTION_ARGS);

Datum rrule_get_occurrences_rrule(struct icalrecurrencetype recurrence,
                                  struct icaltimetype dtstart,
                                  bool use_tz);

void rrule_to_time_t_array(struct icalrecurrencetype recurrence,
                           struct icaltimetype dtstart,
                           time_t** const out_array,
                           unsigned int* const out_count);

#endif // PG_RRULE_H
