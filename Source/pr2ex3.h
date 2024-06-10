#ifndef __PROG2_EX3_H__
#define __PROG2_EX3_H__

#include <stdio.h>

/*
 * This file contains the function prototypes for some types and functions you
 * need to use in home assignment.
 * 
 * You should not change this file.
 */

#define HOURS_PER_SHIFT 8
#define ALL_ROLES 9999

#define FALSE 0
#define TRUE 1

/* Results enum */
typedef enum {
    HRM_SUCCESS,
    HRM_OUT_OF_MEMORY,
    HRM_NULL_ARGUMENT,
    HRM_INVALID_WORKER_ID,
    HRM_INVALID_WAGE,
    HRM_INVALID_NUM_OF_SHIFTS,
    HRM_WORKER_ALREADY_EXISTS,
    HRM_WORKER_DOES_NOT_EXIST,
    HRM_SHIFT_ALREADY_EXISTS,
    HRM_SHIFT_DOES_NOT_EXIST,
    HRM_SHIFTS_OVERFLLOW,
    HRM_NO_WORKERS,
    HRM_NO_SHIFTS,
} HrmResult;

/* Worker role enum */
typedef enum {
    BARTENDER,
    WAITER,
    CHEF,
    CLEANER,
    MANAGER,
    CASHIER,
}HrmWorkerRole;

/* Shift day enum */
typedef enum {
    SUNDAY,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
}HrmShiftDay;

/* Shift type enum */
typedef enum {
    MORNING,
    AFTERNOON,
    EVENING,
    NIGHT,
} HrmShiftType;

/*
* Function: prog2_report_worker
* Parameters:
*           out - The output file to write the report to
*           id - The worker's ID
*           name - The worker's name
*           wage - The worker's wage
*           role - The worker's role
*           total_payment - The total payment of the worker
* Description:
*           Reports a worker to the output stream.
*/
 void prog2_report_worker(FILE *out, long int id, const char *name, float wage, HrmWorkerRole role, float total_payment);
 /******************************************************************************/

/*
* Function: prog2_report_shift
* Parameters:
*           out - The output file to write the report to
*           day - The day of the shift
*           type - The type of the shift
* Description:
*           Reports a shift to the output stream.
*/
 void prog2_report_shift(FILE *out, HrmShiftDay day, HrmShiftType type);
 /******************************************************************************/

#endif /* __PROG2_EX3_H__ */
