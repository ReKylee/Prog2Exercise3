// ReSharper disable CppDFAMemoryLeak
#include "pr2hrm.h"
#include "set.h"
#include <stdlib.h>
#include <string.h>


/* Structs Definitions					*
 * HrMgmt_s -							*
 *		LinkedList: A List of Workers	*
 *	Worker_s -							*
 *		char*: name						*
 *		int: ID							*
 *		HrmWorkerRole: Role				*
 *		float: Wage						*
 *		int: Number of Shifts			*/

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
 * functions here -
 *      copyShift
 *      freeShift
 *      printShift
 *      sortByDay - for cmpSetElemFunc
 *      matchByDay - for matchSetElemFunc
 */
SetElement copyShift(SetElement Elem)
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

void freeShift(SetElement Elem)
{
    free(Elem);
}

void printShift(FILE* out, SetElement Elem)
{
    Shift this_shift = (Shift)Elem;
    prog2_report_shift(out, this_shift->day, this_shift->type);
}


/*	return -1 if first element should be placed							*
 *	ahead of 2nd , return 1 if vice versa , return 0 if can't decide	*/
int sortByDay(SetElement ElemA, SetElement ElemB)
{
    if (((Shift)ElemA)->day > ((Shift)ElemB)->day)
    {
        return -1;
    }
    if (((Shift)ElemA)->day < ((Shift)ElemB)->day)
    {
        return 1;
    }
    return 0;
}

/* return 1 if the element matches the key, 0 if not */
int matchByDay(SetElement Elem, KeyForSetElement key)
{
    return (((Shift)Elem)->day == *(int*)key);
}
/* return 1 if the element matches the key, 0 if not */
int matchByType(SetElement Elem, KeyForSetElement key)
{
    return (((Shift)Elem)->type == *(int*)key);
}

/* ******************************************************************************************************* */
/* Required Set functions for an element of type Worker	*
 * functions here -
 *      freeWorker
 *      copyWorker
 *      printWorker
 *      sortByID - for cmpSetElemFunc
 *      matchByID - for matchSetElemFunc
 *
 */
void freeWorker(SetElement Elem)
{
    Worker worker = (Worker)Elem;
    setDestroy(worker->shift_set);
    free(worker->name);
    free(worker);
}

SetElement copyWorker(SetElement Elem)
{
    Worker new_worker = malloc(sizeof(Worker_t));
    if (new_worker != NULL)
    {
        Worker this_worker = (Worker)Elem;
        new_worker->id = this_worker->id;
        new_worker->name = strdup(this_worker->name);
        if (new_worker->name == NULL)
        {
            free(new_worker);
            return NULL;
        }
        new_worker->role = this_worker->role;
        new_worker->wage = this_worker->wage;
        new_worker->num_available_shifts = this_worker->num_available_shifts;
        SetResult result = setCreate(&(new_worker->shift_set),sortByDay, copyShift, freeShift, printShift);
        if (result == SET_OUT_OF_MEMORY)
        {
            free(new_worker->name);
            free(new_worker);
            return NULL;
        }
    }
    return (SetElement)new_worker;
}

void printWorker(FILE* out, SetElement Elem)
{
    Worker worker = ((Worker)Elem);
    Set shifts = worker->shift_set;
    float total_payment = setGetSize(shifts) * HOURS_PER_SHIFT * worker->wage;
    prog2_report_worker(out, worker->id, worker->name,
        worker->wage, worker->role, total_payment);
}

/*	return -1 if first element should be placed							*
 *	ahead of 2nd , return 1 if vice versa , return 0 if can't decide	*/
int sortByID(SetElement ElemA, SetElement ElemB)
{
    if (((Worker)ElemA)->id > ((Worker)ElemB)->id)
    {
        return -1;
    }
    if (((Worker)ElemA)->id < ((Worker)ElemB)->id)
    {
        return 1;
    }
    return 0;
}

/* return 1 if the element matches the key, 0 if not */
int matchByID(SetElement Elem, KeyForSetElement key)
{
    return (((Worker)Elem)->id == *(int*)key);
}

/* return 1 if the element matches the key, 0 if not */
int matchByRole(SetElement Elem, KeyForSetElement key)
{
    return (((Worker)Elem)->role == *(int*)key);
}

/* return 1 if the element matches the key, 0 if not */
int matchByShift(SetElement Elem, KeyForSetElement key)
{
    Worker worker = (Worker)Elem;
    Set shift_set = worker->shift_set;
    if(setGetSize(shift_set) <= 0)
        return 0;

    SetElement shift_element = NULL;
    SetResult result = setFind(shift_set, shift_element, key, matchByDay);
    if(result != SET_SUCCESS)
        return 0;
    return 1;
}

/* ******************* HR Management Functions ******************* */
HrMgmt HrMgmtCreate()
{

    HrMgmt hrMgmt = malloc(sizeof(HrMgmt_t));
    if (hrMgmt != NULL)
    {
        printf("pp\n");
        SetResult result = setCreate(&(hrMgmt->worker_set),sortByID, copyWorker, freeWorker, printWorker);
        if (result != SET_SUCCESS)
        {
            printf("pp2\n");
            free(hrMgmt);
            return NULL;
        }
        printf("pp3\n");
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
    /* Get the worker and list*/
    Worker new_worker = malloc(sizeof(Worker_t));


    /* THE ERROR GATES!! */
    if (new_worker == NULL)
        return HRM_OUT_OF_MEMORY;

    if (hrm == NULL || name == NULL)
    {
        free(new_worker);
        return HRM_NULL_ARGUMENT;
    }
    Set set = hrm->worker_set;
    if (setGetSize(set) <= 0)
    {
        //TODO: FIX THIS MEMORY LEAK HAHAHAHAHAHHAHA
    printf("pp4\n");
        free(new_worker);
        return HRM_NULL_ARGUMENT;
    }

    if (id <= 0)
    {
        free(new_worker);
        return HRM_INVALID_WORKER_ID;
    }
    if (wage <= 0)
    {
        free(new_worker);
        return HRM_INVALID_WAGE;
    }
    if (num_available_shifts <= 0)
    {
        free(new_worker);
        return HRM_INVALID_NUM_OF_SHIFTS;
    }

    new_worker->id = id;
    new_worker->name = strdup(name);
    if (new_worker->name == NULL)
    {
        /* Worker Hasn't been assigned memory for name; free it by itself*/
        free(new_worker);
        return HRM_OUT_OF_MEMORY;
    }
    new_worker->role = role;
    new_worker->wage = wage;
    new_worker->num_available_shifts = num_available_shifts;

    SetResult result = setCreate(&(new_worker->shift_set), sortByDay, copyShift, freeShift, printShift);
    if(result == SET_OUT_OF_MEMORY)
    {
        free(new_worker->name);
        free(new_worker);
        return HRM_OUT_OF_MEMORY;
    }

    result = setAdd(set, new_worker);
    if (result == SET_OUT_OF_MEMORY)
    {
        free(new_worker->name);
        free(new_worker);
        return HRM_OUT_OF_MEMORY;
    }
    if (result == SET_ELEMENT_EXISTS)
    {
        free(new_worker->name);
        free(new_worker);
        return HRM_WORKER_ALREADY_EXISTS;
    }
    /* Since Set just clones the data, we can free it */
    freeWorker(new_worker);

    return HRM_SUCCESS;
}

HrmResult HrMgmtRemoveWorker(HrMgmt hrm, int id)
{
    if (hrm == NULL)
        return HRM_NULL_ARGUMENT;

    Set set = hrm->worker_set;
    if (setGetSize(set) <= 0)
        return HRM_NULL_ARGUMENT;

    if (id <= 0)
        return HRM_INVALID_WORKER_ID;

    SetElement worker_element = NULL;
    SetResult result = setFind(set, worker_element, &id, matchByID);
    if(result == SET_ELEMENT_DOES_NOT_EXIST)
        return HRM_WORKER_DOES_NOT_EXIST;

    setRemove(set, (Worker)worker_element);
    return HRM_SUCCESS;
}

HrmResult HrMgmtAddShiftToWorker(HrMgmt hrm,
                                 int id, HrmShiftDay day, HrmShiftType type)
{
    if (hrm == NULL)
        return HRM_NULL_ARGUMENT;

    Set set = hrm->worker_set;
    if (setGetSize(set) <= 0)
        return HRM_WORKER_DOES_NOT_EXIST;

    if (id <= 0)
        return HRM_INVALID_WORKER_ID;


    SetElement worker_element = NULL;
    SetResult result = setFind(set, worker_element, &id, matchByID);
    if (result != SET_SUCCESS)
        return HRM_WORKER_DOES_NOT_EXIST;

    Worker this_worker = (Worker)worker_element;
    Shift new_shift = malloc(sizeof(Shift_t));
    if (new_shift == NULL)
        return HRM_OUT_OF_MEMORY;

    if(setGetSize(this_worker->shift_set)+1 > this_worker->num_available_shifts)
    {
        free(new_shift);
        return HRM_SHIFTS_OVERFLLOW;
    }

    new_shift->day = day;
    new_shift->type = type;

    result = setAdd(this_worker->shift_set, new_shift);
    if(result == SET_OUT_OF_MEMORY)
    {
        freeShift(new_shift);
        return HRM_OUT_OF_MEMORY;
    }
    if(result == SET_ELEMENT_EXISTS)
    {
        freeShift(new_shift);
        return HRM_SHIFT_ALREADY_EXISTS;
    }

    freeShift(new_shift);
    return HRM_SUCCESS;
}

HrmResult HrMgmtRemoveShiftFromWorker(HrMgmt hrm, int id, HrmShiftDay day, HrmShiftType type)
{
    if (hrm == NULL)
        return HRM_NULL_ARGUMENT;

    Set set = hrm->worker_set;
    if (setGetSize(set) <= 0)
        return HRM_WORKER_DOES_NOT_EXIST;

    if (id <= 0)
        return HRM_INVALID_WORKER_ID;

    SetElement worker_element = NULL;
    SetResult result = setFind(set, worker_element, &id, matchByID);
    if(result == SET_ELEMENT_DOES_NOT_EXIST)
        return HRM_WORKER_DOES_NOT_EXIST;

    Set shift_set = ((Worker)worker_element)->shift_set;
    SetElement shift_element = NULL;
    result = setFind(shift_set, shift_element, &day, matchByDay);
    if(result == SET_ELEMENT_DOES_NOT_EXIST || ((Shift)shift_element)->type != type)
        return HRM_SHIFT_DOES_NOT_EXIST;

    setRemove(shift_set, shift_element);
    return HRM_SUCCESS;
}

HrmResult HrMgmtTransferShift(HrMgmt hrm, int fromId, int toId, HrmShiftDay day, HrmShiftType type)
{
    if (hrm == NULL)
        return HRM_NULL_ARGUMENT;

    Set set = hrm->worker_set;
    if (setGetSize(set) <= 0)
        return HRM_WORKER_DOES_NOT_EXIST;

    if (fromId <= 0 || toId <= 0)
        return HRM_INVALID_WORKER_ID;

    SetElement worker_from_element = NULL;
    SetElement worker_to_element = NULL;
    SetResult result_from = setFind(set, worker_from_element, &fromId, matchByID);
    SetResult result_to = setFind(set, worker_to_element, &toId, matchByID);
    if(result_from == SET_ELEMENT_DOES_NOT_EXIST || result_to == SET_ELEMENT_DOES_NOT_EXIST)
        return HRM_WORKER_DOES_NOT_EXIST;

    Worker worker_from = (Worker)worker_from_element;
    Worker worker_to = (Worker)worker_to_element;

    Set shift_set_from = worker_from->shift_set;
    Set shift_set_to = worker_to->shift_set;

    SetElement shift_element = NULL;
    SetResult result = setFind(shift_set_from, shift_element, &day, matchByDay);
    if (shift_set_from == NULL || result == SET_ELEMENT_DOES_NOT_EXIST || ((Shift)shift_element)->type != type)
        return HRM_SHIFT_DOES_NOT_EXIST;

    if(setGetSize(shift_set_to)+1 > worker_to->num_available_shifts)
        return HRM_SHIFTS_OVERFLLOW;

    result = setAdd(shift_set_to, shift_element);
    if(result == SET_ELEMENT_DOES_NOT_EXIST)
    {
        return HRM_SHIFT_ALREADY_EXISTS;
    }

    return HRM_SUCCESS;

}

HrmResult HrMgmtReportWorkers(HrMgmt hrm, HrmWorkerRole role, FILE* output)
{
    if(hrm == NULL)
        return HRM_NULL_ARGUMENT;
    int worker_set_size = setGetSize(hrm->worker_set);
    if(worker_set_size <= 0)
        return HRM_NO_WORKERS;

    if(role <= ALL_ROLES)
    {
        Set filtered_set = NULL;
        SetResult result = setFilter(hrm->worker_set, &filtered_set,&role, matchByRole);

        int filtered_set_size = setGetSize(filtered_set);
        if(result == SET_OUT_OF_MEMORY)
        {
            free(filtered_set);
            return HRM_OUT_OF_MEMORY;
        }
        if(filtered_set_size <= 0)
            return HRM_NO_WORKERS;


        setPrintSorted(filtered_set,output,filtered_set_size+1,sortByID);
        setDestroy(filtered_set);
        return HRM_SUCCESS;
    }
    setPrintSorted(hrm->worker_set,output,worker_set_size+1,sortByID);
    return HRM_SUCCESS;
}

HrmResult HrMgmtReportShiftsOfWorker(HrMgmt hrm, int id, FILE* output)
{
    if(hrm == NULL)
        return HRM_NULL_ARGUMENT;
    if(id <= 0)
        return HRM_INVALID_WORKER_ID;
    Set set = hrm->worker_set;
    int worker_set_size = setGetSize(set);
    if(worker_set_size <= 0)
        return HRM_NO_SHIFTS;

    SetElement worker_element = NULL;
    SetResult result = setFind(set, worker_element,&id, matchByID);
    if(result != SET_SUCCESS)
        return HRM_NO_SHIFTS;

    printWorker(output, worker_element);

    Set shift_set = ((Worker)worker_element)->shift_set;

    setPrintSorted(shift_set,output,setGetSize(shift_set),sortByDay);
    return HRM_SUCCESS;
}

HrmResult HrMgmtReportWorkersInShift(HrMgmt hrm, HrmShiftDay day, HrmShiftType type, FILE* output)
{
    if(hrm == NULL)
        return HRM_NULL_ARGUMENT;

    Set set = hrm->worker_set;
    int worker_set_size = setGetSize(set);
    if(worker_set_size <= 0)
        return HRM_NO_WORKERS;

    Set workers_in_day = NULL;
    SetResult result = setFilter(set, &workers_in_day,&day, matchByShift);
    if(result != SET_SUCCESS)
        return HRM_NO_WORKERS;

    Set workers_in_shift = NULL;
    result = setFilter(workers_in_day, &workers_in_shift, &type, matchByShift);
    if(result != SET_SUCCESS)
        return HRM_NO_WORKERS;
    
    setPrintSorted(workers_in_shift,output,setGetSize(workers_in_shift)+1, sortByID);
    return HRM_SUCCESS;
}
