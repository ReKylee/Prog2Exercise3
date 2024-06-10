#ifndef __PR2HRM_H__
#define __PR2HRM_H__

#include "pr2ex3.h"

/*
* ADT module interface for managing an HR Management of workers and shifts.
* 
* Below you'll find the functions you need to implement for this module.
* Each function has a description of what it should do and what it's input and output are.
*
* You should not change the function prototypes or the defined types. 
*/

/* *** NOTE! *** */
/* Each function that returns HrmResult may return one of the following in addition to the description:
 * HRM_SUCCESS, HRM_OUT_OF_MEMORY or HRM_NULL_ARGUMENT
 *
 */

/* The ADT main structure type definition */
typedef struct HrMgmt_s *HrMgmt;

/*
* Function: HrMgmtCreate
* Parameters:
*           None
* Returns:
*           A new HrMgmt instance
* Description:
*           Creates a new HR Management instance.
*/
HrMgmt HrMgmtCreate();
/*****************************************************************************/

/*
* Function: HrMgmtDestroy
* Parameters:
*           HrMgmt hrm - The HR Management instance to destroy
* Returns:
*           None
* Description:
*           Destroys the given HR Management instance and frees all of its allocated memory.
*/
void HrMgmtDestroy(HrMgmt hrm);
/*****************************************************************************/

/*
* Function: HrMgmtAddWorker
* Parameters:
*           hrm - The HR Management instance to add the worker to
*           name - The worker's name
*           id - The worker's ID (integer > 0 ; must be unique)
*           role - The worker's role
*           wage - The worker's wage
*           numOfShifts - The number of shifts the worker is available for (integer > 0)
* Returns:
*           HRM_INVALID_WORKER_ID - The worker's ID is invalid
*           HRM_INVALID_WAGE - The worker's wage is invalid
*           HRM_INVALID_NUM_OF_SHIFTS - The number of shifts is invalid
*           HRM_WORKER_ALREADY_EXISTS - The worker already exists
* Description:
*           Adds a new worker to the HR Management instance.
*/
HrmResult HrMgmtAddWorker(HrMgmt hrm, const char *name, int id, HrmWorkerRole role, float wage, int numOfShifts);
/*****************************************************************************/

/*
* Function: HrMgmtRemoveWorker
* Parameters:
*           hrm - The HR Management instance to remove the worker from
*           id - The worker's ID
* Returns:
*           HRM_INVALID_WORKER_ID - The worker's ID is invalid
*           HRM_WORKER_DOES_NOT_EXIST - The worker does not exist
* Description:
*           Removes a worker from the HR Management instance.
*/
HrmResult HrMgmtRemoveWorker(HrMgmt hrm, int id);
/*****************************************************************************/

/*
* Function: HrMgmtAddShiftToWorker
* Parameters:
*           hrm - The HR Management instance to add the shift to
*           id - The worker's ID
*           day - The day of the shift
*           type - The type of the shift
* Returns:
*           HRM_INVALID_WORKER_ID - The worker's ID is invalid
*           HRM_WORKER_DOES_NOT_EXIST - The worker does not exist
*           HRM_SHIFTS_OVERFLLOW - The worker has no more shifts available
*           HRM_SHIFT_ALREADY_EXISTS - The shift already exists
* Description:
*           Adds a shift to a worker in the HR Management instance.
*/
HrmResult HrMgmtAddShiftToWorker(HrMgmt hrm, int id, HrmShiftDay day, HrmShiftType type);
/*****************************************************************************/

/*
* Function: HrMgmtRemoveShiftFromWorker
* Parameters:
*           hrm - The HR Management instance to remove the shift from
*           id - The worker's ID
*           day - The day of the shift
*           type - The type of the shift
* Returns:
*           HRM_INVALID_WORKER_ID - The worker's ID is invalid
*           HRM_WORKER_DOES_NOT_EXIST - The worker does not exist
*           HRM_SHIFT_DOES_NOT_EXIST - The shift does not exist
* Description:
*           Removes a shift from a worker in the HR Management instance.
*/
HrmResult HrMgmtRemoveShiftFromWorker(HrMgmt hrm, int id, HrmShiftDay day, HrmShiftType type);
/*****************************************************************************/

/*
* Function: HrMgmtTransferShift
* Parameters:
*           hrm - The HR Management instance to transfer the shift in
*           fromId - The worker's ID to transfer the shift from
*           toId - The worker's ID to transfer the shift to
*           day - The day of the shift
*           type - The type of the shift
* Returns:
*           HRM_INVALID_WORKER_ID - The worker's ID is invalid
*           HRM_WORKER_DOES_NOT_EXIST - The worker does not exist
*           HRM_SHIFT_DOES_NOT_EXIST - The shift does not exist
*           HRM_SHIFT_ALREADY_EXISTS - The shift already exists
*           HRM_SHIFTS_OVERFLLOW - The worker has no more shifts available
* Description:
*           Transfers a shift from one worker to another in the HR Management instance.
*/
HrmResult HrMgmtTransferShift(HrMgmt hrm, int fromId, int toId, HrmShiftDay day, HrmShiftType type);
/*****************************************************************************/

/*
* Function: HrMgmtReportWorkers
* Parameters:
*           hrm - The HR Management instance to report the workers from
*           role - The role of the workers to report (ALL_ROLES for all roles)
*           output - The output file to write the report to
* Returns:
*           HRM_NO_WORKERS - There are no workers to report
* Description:
*           Reports the workers of the requested role in the HR Management instance
*           to the output file.
*/
HrmResult HrMgmtReportWorkers(HrMgmt hrm, HrmWorkerRole role, FILE* output);
/*****************************************************************************/

/*
* Function: HrMgmtReportShiftsOfWorker
* Parameters:
*           hrm - The HR Management instance to report the shifts from
*           id - The worker's ID to report the shifts of
*           output - The output file to write the report to
* Returns:
*           HRM_INVALID_WORKER_ID - The worker's ID is invalid
*           HRM_WORKER_DOES_NOT_EXIST - The worker does not exist
*           HRM_NO_SHIFTS - There are no shifts to report
* Description:
*           Reports the shifts of the requested worker in the HR Management instance
*           to the output file.
*/
HrmResult HrMgmtReportShiftsOfWorker(HrMgmt hrm, int id, FILE* output);
/*****************************************************************************/

/*
* Function: HrMgmtReportWorkersInShift
* Parameters:
*           hrm - The HR Management instance to report the workers from
*           day - The day of the shift to report the workers of
*           type - The type of the shift to report the workers of
*           output - The output file to write the report to
* Returns:
*           HRM_NO_WORKERS - There are no workers to report
* Description:
*           Reports the workers that are working in the requested shift in the HR Management instance
*           to the output file.
*/
HrmResult HrMgmtReportWorkersInShift(HrMgmt hrm, HrmShiftDay day, HrmShiftType type, FILE* output);
/*****************************************************************************/

#endif