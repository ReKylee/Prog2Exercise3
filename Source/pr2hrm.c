// ReSharper disable CppDFAMemoryLeak
//Includes
#include "pr2hrm.h"
#include "set.h"
#include <stdlib.h>
#include <string.h>


/* ******************************************************************************************************* */
/* Structs Definitions					*
 *  HrMgmt_s -							*
 *		LinkedList: A List of Workers	*
 *	Shift_s -                           *
 *	    day: HrmShiftDay                *
 *	    type: HrmShiftType              *
 *	Worker_s -							*
 *		char*: name						*
 *		int: ID							*
 *		HrmWorkerRole: Role				*
 *		float: Wage						*
 *		int: Number of Shifts			*
 *		                                */

typedef struct HrMgmt_s
{
    Set worker_set;
} HrMgmt_t, *HrMgmt;


typedef struct Shift_s
{
    HrmShiftDay day;
    HrmShiftType type;
}Shift_t, *Shift;

typedef struct Worker_s
{
    char* name;
    unsigned long id;
    HrmWorkerRole role;
    float wage;
    Set shift_set;
    int num_available_shifts;
} Worker_t, *Worker;

/* ******************************************************************************************************* */
/* Required Set functions for an element of type Shift	*
 * functions here -                                     *
 *      copyShift                                       *
 *      freeShift                                       *
 *      printShift                                      *
 *      sortByDay   - for cmpSetElemFunc                *
 *      matchByDay  - for matchSetElemFunc              *
 *      matchByType - for matchSetElemFunc              *
 *                                                      */
SetElement copyShift(SetElement Elem /*Shift*/)
{
    Shift this_shift = (Shift)Elem;
    Shift new_shift = malloc(sizeof(Shift_t));
    if (new_shift != NULL)
    {
        new_shift->day = this_shift->day;
        new_shift->type = this_shift->type;
    }
    return (SetElement)new_shift;
}

void freeShift(SetElement Elem /*Shift*/)
{
    free(Elem);
}

void printShift(FILE* out, SetElement Elem /*Shift*/)
{
   prog2_report_shift(out, ((Shift)Elem)->day, ((Shift)Elem)->type);
}


/*	return -1 if first element should be placed							*
 *	ahead of 2nd , return 1 if vice versa , return 0 if can't decide	*/
int sortByDay(SetElement ElemA /*Shift*/, SetElement ElemB /*Shift*/)
{
    HrmShiftDay dayA = ((Shift)ElemA)->day;
    HrmShiftDay dayB = ((Shift)ElemB)->day;

    return (dayA > dayB) - (dayA < dayB);
}

/* return 1 if the element matches the key, 0 if not */
int matchByDay(SetElement Elem /*Shift*/, KeyForSetElement key /*Shift Day*/)
{
    return (((Shift)Elem)->day == *(HrmShiftDay*)key);
}
/* return 1 if the element matches the key, 0 if not */
int matchByType(SetElement Elem /*Shift*/, KeyForSetElement key /*Shift Type*/)
{
    return (((Shift)Elem)->type == *(HrmShiftType*)key);
}

/* ******************************************************************************************************* */
/* Required Set functions for an element of type Worker	*
 * functions here -                                     *
 *      freeWorker                                      *
 *      copyWorker                                      *
 *      printWorker                                     *
 *      sortByID     -  for cmpSetElemFunc              *
 *      matchByID    -  for matchSetElemFunc            *
 *      matchByRole  -  for matchSetElemFunc            *
 *      matchByShift -  for matchSetElemFunc            *
 *                                                      */
void freeWorker(SetElement Elem /*Worker*/)
{
    if (Elem == NULL)
        return;

    Worker worker = (Worker)Elem;
    if (worker->shift_set != NULL) {
        setDestroy(worker->shift_set);
    }
    if (worker->name != NULL) {
        free(worker->name);
    }
    free(worker);
}

int filterSetCopyFunc(SetElement Elem, KeyForSetElement key)
{
    return 1;
}
SetElement copyWorker(SetElement Elem /*Worker*/)
{
    Worker new_worker = malloc(sizeof(Worker_t));
    if (new_worker != NULL)
    {
        Worker this_worker = (Worker)Elem;
        new_worker->id = this_worker->id;
        new_worker->name = strdup(this_worker->name);
        if (new_worker->name == NULL)
        {
            freeWorker(new_worker);
            return NULL;
        }
        new_worker->role = this_worker->role;
        new_worker->wage = this_worker->wage;
        new_worker->num_available_shifts = this_worker->num_available_shifts;

        SetResult result = setFilter(this_worker->shift_set, &(new_worker->shift_set), 0,filterSetCopyFunc);
        if(result != SET_SUCCESS)
            freeWorker(new_worker);
        if (result == SET_OUT_OF_MEMORY)
            return NULL;
    }
    return (SetElement)new_worker;
}

void printWorker(FILE* out, SetElement Elem /*Worker*/)
{
    Worker worker = ((Worker)Elem);
    Set shifts = worker->shift_set;
    float total_payment = setGetSize(shifts) * HOURS_PER_SHIFT * worker->wage;
    prog2_report_worker(out, worker->id, worker->name,
        worker->wage, worker->role, total_payment);
}

/*	return -1 if first element should be placed							*
 *	ahead of 2nd , return 1 if vice versa , return 0 if can't decide	*/
int sortByID(SetElement ElemA /*Worker*/, SetElement ElemB /*Worker*/)
{
    int idA = ((Worker)ElemA)->id;
    int idB = ((Worker)ElemB)->id;

    return (idA > idB) - (idA < idB);
}

/* return 1 if the element matches the key, 0 if not */
int matchByID(SetElement Elem /*Worker*/, KeyForSetElement key /*Worker ID*/)
{
    return (((Worker)Elem)->id == *(int*)key);
}

/* return 1 if the element matches the key, 0 if not */

int matchByRole(SetElement Elem /*Worker*/, KeyForSetElement key /*Worker Role*/)
{
    return (((Worker)Elem)->role == *(HrmWorkerRole*)key);
}

/* return 1 if the element matches the key, 0 if not */

int matchByShift(SetElement Elem /*Worker*/, KeyForSetElement key /*Shift*/)
{
    return (
        setIsIn(
            ((Worker)Elem)->shift_set, (Shift)key)
            == SET_ELEMENT_EXISTS);
}


HrmResult FilterByShift(Set set, HrmShiftDay day, HrmShiftType type, /*OUT*/ Set* workers_in_shift)
{
    /* Create a Shift to be used for setIsIn*/
    Shift shift = malloc(sizeof(Shift_t));
    if(shift == NULL)
        return HRM_OUT_OF_MEMORY;

    shift->day = day;
    shift->type = type;

    SetResult result = setFilter(set, workers_in_shift, shift, matchByShift);
    int size = setGetSize(*workers_in_shift);
    freeShift(shift);
    if(result != SET_SUCCESS || size <= 0)
    {
        return HRM_NO_WORKERS;
    }
    return HRM_SUCCESS;
}

/* ******************* HR Management Functions ******************* */

HrMgmt HrMgmtCreate()
{

    HrMgmt hrMgmt = malloc(sizeof(HrMgmt_t));
    if (hrMgmt != NULL)
    {
        SetResult result = setCreate(&(hrMgmt->worker_set),sortByID, copyWorker, freeWorker, printWorker);

        if (result != SET_SUCCESS)
        {
            free(hrMgmt);
            return NULL;
        }
    }
    return hrMgmt;
}

void HrMgmtDestroy(HrMgmt hrm)
{
    setDestroy(hrm->worker_set);
    free(hrm);
}

HrmResult HrMgmtAddWorker(HrMgmt hrm,
                          const char* name, int id,
                          HrmWorkerRole role, float wage,
                          int num_available_shifts)
{

    // Check for null pointers and invalid state
    if (hrm == NULL || name == NULL || hrm->worker_set == NULL)
        return HRM_NULL_ARGUMENT;

    if (id <= 0)
        return HRM_INVALID_WORKER_ID;

    if (wage <= 0)
        return HRM_INVALID_WAGE;

    if (num_available_shifts <= 0)
        return HRM_INVALID_NUM_OF_SHIFTS;

    // Allocate memory for new worker
    Worker new_worker = malloc(sizeof(Worker_t));
    if (new_worker == NULL) {
        return HRM_OUT_OF_MEMORY;
    }
    Set set = hrm->worker_set;

    new_worker->id = id;
    new_worker->name = strdup(name);
    if (new_worker->name == NULL)
    {
        /* Worker Hasn't been assigned memory for name; free it by itself*/
        freeWorker(new_worker);
        return HRM_OUT_OF_MEMORY;
    }
    new_worker->role = role;
    new_worker->wage = wage;
    new_worker->num_available_shifts = num_available_shifts;
    /* New shift set for our new worker */
    SetResult result = setCreate(&(new_worker->shift_set), sortByDay, copyShift, freeShift, printShift);
    if(result != SET_SUCCESS)
        freeWorker(new_worker);
    if(result == SET_OUT_OF_MEMORY)
        return HRM_OUT_OF_MEMORY;
    if(result == SET_BAD_ARGUMENTS)
        return HRM_NULL_ARGUMENT;

    /* Add our worker to the worker_set */
    result = setAdd(set, new_worker);
    if(result != SET_SUCCESS)
        freeWorker(new_worker);
    if (result == SET_OUT_OF_MEMORY)
        return HRM_OUT_OF_MEMORY;
    if (result == SET_ELEMENT_EXISTS)
        return HRM_WORKER_ALREADY_EXISTS;

    /* Since Set just clones the data, we can free it */
    freeWorker(new_worker);
    return HRM_SUCCESS;
}

HrmResult HrMgmtRemoveWorker(HrMgmt hrm, int id)
{
    if (hrm == NULL || hrm->worker_set == NULL)
        return HRM_NULL_ARGUMENT;

    Set set = hrm->worker_set;

    if (id <= 0)
        return HRM_INVALID_WORKER_ID;

    /* Find the worker by ID and remove him */
    SetElement worker_element = NULL;
    SetResult result = setFind(set, &worker_element, &id, matchByID);
    if(result != SET_SUCCESS)
        return HRM_WORKER_DOES_NOT_EXIST;

    setRemove(set, (Worker)worker_element);
    return HRM_SUCCESS;
}

HrmResult HrMgmtAddShiftToWorker(HrMgmt hrm,
                                 int id, HrmShiftDay day, HrmShiftType type)
{
    /* THE ERROR GATES!! */
    Shift new_shift = malloc(sizeof(Shift_t));
    if (new_shift == NULL)
        return HRM_OUT_OF_MEMORY;
    if (hrm == NULL)
    {
        freeShift(new_shift);
        return HRM_NULL_ARGUMENT;
    }

    Set set = hrm->worker_set;
    if (setGetSize(set) <= 0)
    {
        freeShift(new_shift);
        return HRM_WORKER_DOES_NOT_EXIST;
    }

    if (id <= 0)
    {
        freeShift(new_shift);
        return HRM_INVALID_WORKER_ID;
    }

    /* Find worker by ID */
    SetElement worker_element = NULL;
    SetResult result = setFind(set, &worker_element, &id, matchByID);
    if (result != SET_SUCCESS || worker_element == NULL)
    {
        freeShift(new_shift);
        return HRM_WORKER_DOES_NOT_EXIST;
    }

    Worker this_worker = (Worker)worker_element;
    /* If his shift count is greater than what he reported to be available */
    if(setGetSize(this_worker->shift_set)+1 > this_worker->num_available_shifts)
    {
        freeShift(new_shift);
        return HRM_SHIFTS_OVERFLLOW;
    }

    new_shift->day = day;
    new_shift->type = type;
    /* Add shift to set and check for errors */
    result = setAdd(this_worker->shift_set, new_shift);

    if(result != SET_SUCCESS)
    {
        fprintf(stderr, "%d\t", id);
        freeShift(new_shift);
    }
    if(result == SET_OUT_OF_MEMORY)
        return HRM_OUT_OF_MEMORY;
    if(result == SET_ELEMENT_EXISTS)
        return HRM_SHIFT_ALREADY_EXISTS;


    /* Set clones the data so we can free the new shift */
    freeShift(new_shift);
    return HRM_SUCCESS;
}

HrmResult HrMgmtRemoveShiftFromWorker(HrMgmt hrm, int id, HrmShiftDay day, HrmShiftType type)
{
    /* THE ERROR GATES!! */
    if (hrm == NULL)
        return HRM_NULL_ARGUMENT;

    Set set = hrm->worker_set;
    if (setGetSize(set) <= 0)
        return HRM_WORKER_DOES_NOT_EXIST;

    if (id <= 0)
        return HRM_INVALID_WORKER_ID;

    /* Find worker by ID */
    SetElement worker_element = NULL;
    SetResult result = setFind(set, &worker_element, &id, matchByID);
    if(result != SET_SUCCESS)
        return HRM_WORKER_DOES_NOT_EXIST;

    Set shift_set = ((Worker)worker_element)->shift_set;

    /* Find shift by day, if the shift that day doesn't match the type then no shifts */
    SetElement shift_element = NULL;
    result = setFind(shift_set, &shift_element, &day, matchByDay);
    if(result != SET_SUCCESS || ((Shift)shift_element)->type != type)
        return HRM_SHIFT_DOES_NOT_EXIST;

    setRemove(shift_set, shift_element);
    return HRM_SUCCESS;
}

HrmResult HrMgmtTransferShift(HrMgmt hrm, int fromId, int toId, HrmShiftDay day, HrmShiftType type)
{
    /* THE ERROR GATES!! */
    if (hrm == NULL)
        return HRM_NULL_ARGUMENT;

    Set set = hrm->worker_set;
    if (setGetSize(set) <= 0)
        return HRM_WORKER_DOES_NOT_EXIST;

    if (fromId <= 0 || toId <= 0)
        return HRM_INVALID_WORKER_ID;

    /* Find both workers by their ID */
    SetElement worker_from_element = NULL;
    SetElement worker_to_element = NULL;
    SetResult result_from = setFind(set, &worker_from_element, &fromId, matchByID);
    SetResult result_to = setFind(set, &worker_to_element, &toId, matchByID);
    if(result_from != SET_SUCCESS || result_to != SET_SUCCESS)
        return HRM_WORKER_DOES_NOT_EXIST;

    Worker worker_from = (Worker)worker_from_element;
    Worker worker_to = (Worker)worker_to_element;

    Set shift_set_from = worker_from->shift_set;
    Set shift_set_to = worker_to->shift_set;


    /* Find required shift in Worker A */
    SetElement shift_element = NULL;
    SetResult result = setFind(shift_set_from, &shift_element, &day, matchByDay);
    if (result != SET_SUCCESS || ((Shift)shift_element)->type != type)
        return HRM_SHIFT_DOES_NOT_EXIST;

    /* Check availability of Worker B for a new shift */
    if(setGetSize(shift_set_to)+1 > worker_to->num_available_shifts)
        return HRM_SHIFTS_OVERFLLOW;

    /* Transfer shift */
    result = setAdd(shift_set_to, shift_element);
    if(result != SET_SUCCESS)
    {
        return HRM_SHIFT_ALREADY_EXISTS;
    }

    setRemove(shift_set_from, shift_element);
    return HRM_SUCCESS;

}

HrmResult HrMgmtReportWorkers(HrMgmt hrm, HrmWorkerRole role, FILE* output)
{
    /* THE ERROR GATES!! */
    if (hrm == NULL || output == NULL)
        return HRM_NULL_ARGUMENT;

    Set setToPrint = hrm->worker_set;
    int setSize = setGetSize(setToPrint);

    if (setSize <= 0)
        return HRM_NO_WORKERS;
    /* If the role isn't ALL_ROLES, we filter based on role */
    if (role < ALL_ROLES)
    {
        Set filteredSet = NULL;
        SetResult result = setFilter(hrm->worker_set, &filteredSet, &role, matchByRole);
        int filteredSize = setGetSize(filteredSet);
        if (result == SET_OUT_OF_MEMORY)
        {
            setDestroy(filteredSet);
            return HRM_OUT_OF_MEMORY;
        }
        if (filteredSize <= 0)
        {
            setDestroy(filteredSet);
            return HRM_NO_WORKERS;
        }

        setToPrint = filteredSet;
        setSize = filteredSize;
    }

    setPrint(setToPrint, output, setSize + 1);
    if (setToPrint != hrm->worker_set)
        setDestroy(setToPrint);

    return HRM_SUCCESS;
}


HrmResult HrMgmtReportShiftsOfWorker(HrMgmt hrm, int id, FILE* output)
{
    /* THE ERROR GATES!! */
    if(hrm == NULL)
        return HRM_NULL_ARGUMENT;
    if(id <= 0)
        return HRM_INVALID_WORKER_ID;
    Set set = hrm->worker_set;
    int worker_set_size = setGetSize(set);
    if(set == NULL || worker_set_size <= 0)
        return HRM_NO_SHIFTS;

    SetElement worker_element = NULL;
    SetResult result = setFind(set, &worker_element,&id, matchByID);
    if(result != SET_SUCCESS || worker_element == NULL)
        return HRM_NO_SHIFTS;

    printWorker(output, (Worker)worker_element);
    setPrint(((Worker)worker_element)->shift_set,output,setGetSize(((Worker)worker_element)->shift_set));
    return HRM_SUCCESS;
}

HrmResult HrMgmtReportWorkersInShift(HrMgmt hrm, HrmShiftDay day, HrmShiftType type, FILE* output)
{
    /* THE ERROR GATES!! */
    if(hrm == NULL)
        return HRM_NULL_ARGUMENT;

    Set set = hrm->worker_set;
    int worker_set_size = setGetSize(set);
    if(worker_set_size <= 0)
        return HRM_NO_WORKERS;

    Set workers_in_shift = NULL;
    HrmResult filter_result = FilterByShift(set, day, type, &workers_in_shift);

    setPrint(workers_in_shift,output,worker_set_size);
    setDestroy(workers_in_shift);
    return filter_result;
}
