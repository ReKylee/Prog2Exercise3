// ReSharper disable CppDFAMemoryLeak
//Includes
#include "pr2hrm.h"
#include "set.h"
#include <stdlib.h>
#include <string.h>


/* MACROS: ERROR  */
#define EVALUATE_ERROR(newError, currentError) \
    ((newError) < (currentError)) ? (newError) : (currentError)

/* MACROS: EXPECT  */
#define EXPECT_GT(i, e, ...) \
    if (i <= e) { __VA_ARGS__ }

#define EXPECT_NOT_NULL(i, ...) \
    if (i == NULL) { __VA_ARGS__ }

/* ******************************************************************************************************* */

/**
 * HrMgmt struct
 *  Hold a set of workers
 */
typedef struct HrMgmt_s
{
    Set worker_set;
} HrMgmt_t, *HrMgmt;


/**
 * Shift struct
 *  Holds a shift day and a shirt type
 */
typedef struct Shift_s
{
    HrmShiftDay day;
    HrmShiftType type;
} Shift_t, *Shift;


/**
 * Worker struct
 *  Holds a name, an id, a worker role, a wage, a shift set and the number of available shifts
 */
typedef struct Worker_s
{
    char* name;
    unsigned int id;
    HrmWorkerRole role;
    float wage;
    Set shift_set;
    int num_available_shifts;
} Worker_t, *Worker;

/* ******************************************************************************************************* */
/*      Error helper function    */

/**
 * setResultToHrmResult
 * @param set_result The set result to convert
 * @param element_exists_result what should be returned if set element exists
 * @param element_not_exists_result what should be returned if set element does not exist
 * @return returns the matching hrm result
 */
HrmResult setResultToHrmResult(SetResult set_result, HrmResult element_exists_result,
                               HrmResult element_not_exists_result)
{
    switch (set_result)
    {
    case SET_BAD_ARGUMENTS:
        return HRM_NULL_ARGUMENT;
    case SET_OUT_OF_MEMORY:
        return HRM_OUT_OF_MEMORY;
    case SET_ELEMENT_EXISTS:
        return element_exists_result;
    case SET_ELEMENT_DOES_NOT_EXIST:
        return element_not_exists_result;
    default:
        return HRM_SUCCESS;
    }
}

/* ******************************************************************************************************* */
/* Required Set functions for an element of type Shift	*/

/**
 * 
 * @param Elem Shift element to copy
 * @return a pointer to a duplicate of the Shift
 */
SetElement copyShift(SetElement Elem /*Shift*/)
{
    Shift this_shift    = (Shift)Elem;
    Shift new_shift     = malloc(sizeof(Shift_t));

    EXPECT_NOT_NULL(new_shift, return NULL; )

    new_shift->day  = this_shift->day;
    new_shift->type = this_shift->type;
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


/**
 * 
 * @param ElemA First Shift to compare
 * @param ElemB Second Shift to compare
 * @return -1 if first ahead of second, 1 if vice versa, 0 if they're the same
 */
int sortByDayAndThenType(SetElement ElemA /*Shift*/, SetElement ElemB /*Shift*/)
{
    HrmShiftDay dayA    = ((Shift)ElemA)->day;
    HrmShiftDay dayB    = ((Shift)ElemB)->day;
    HrmShiftType typeA  = ((Shift)ElemA)->type;
    HrmShiftType typeB  = ((Shift)ElemB)->type;

    int compare_days    = (dayA > dayB) - (dayA < dayB);
    int compare_types   = (typeA > typeB) - (typeA < typeB);
    return (compare_days == 0) ? compare_types : compare_days;
}

/**
 *
 * @param Elem Shift to compare
 * @param key HrmShiftDay to compare against
 * @return 1 if matches, 0 if not
 */
int matchByDay(SetElement Elem /*Shift*/, KeyForSetElement key /*Shift Day*/)
{
    return (((Shift)Elem)->day == *(HrmShiftDay*)key);
}

/**
 *
 * @param Elem Shift to compare
 * @param key HrmShiftType to compare against
 * @return 1 if matches, 0 if not
 */
int matchByType(SetElement Elem /*Shift*/, KeyForSetElement key /*Shift Type*/)
{
    return (((Shift)Elem)->type == *(HrmShiftType*)key);
}


/**
 *
 * @param Elem Shift to compare
 * @param key An array of KeyForSetElement [&day, &type] to compare against
 * @return 1 if matches, 0 if not
 */
int matchByDayAndType(SetElement Elem /*Shift*/, KeyForSetElement key /*Shift Day*/)
{
    KeyForSetElement* keys = (KeyForSetElement*)key;
    return matchByDay(Elem, keys[0]) && matchByType(Elem, keys[1]);
}

/**
 * 
 * @param day HrmShiftDay to find
 * @param type HrmShiftType to find
 * @param shift_set Shift Set to search
 * @param shift_element The found shift element
 * @return the result of setFind matched to a HrmResult
 */
HrmResult findShift(HrmShiftDay day, HrmShiftType type, Set shift_set, SetElement* shift_element)
{
    KeyForSetElement keys[] = {&day, &type};
    SetResult result        = setFind(shift_set, (shift_element), &keys, matchByDayAndType);
    return setResultToHrmResult(result, HRM_SHIFT_ALREADY_EXISTS, HRM_SHIFT_DOES_NOT_EXIST);
}

/* ******************************************************************************************************* */

/* Required Set functions for an element of type Worker	*/

void freeWorker(SetElement Elem /*Worker*/)
{
    setDestroy(((Worker)Elem)->shift_set);
    free(((Worker)Elem)->name);
    free(Elem);
}

int filterSetCopyFunc(SetElement Elem, KeyForSetElement key)
{
    return 1;
}

/**
 *
 * @param name Name
 * @param id ID
 * @param role HrmWorkerRole
 * @param wage Wage
 * @param numOfShifts Number of available shifts
 * @param new_worker A pointer to where the new worker will be stored
 * @param shift_set_to_copy If supplied, will copy the set to the new worker's shift set.
 *                          set to NULL to create an empty one.
 * @return The result of adding a new worker.
 */
HrmResult initiateNewWorker(const char* name, unsigned int id, HrmWorkerRole role, float wage, int numOfShifts,
                            const Worker* new_worker, const Set* shift_set_to_copy)
{
    SetResult result;
    (*new_worker)->id                   = id;
    (*new_worker)->name                 = strdup(name);

    EXPECT_NOT_NULL((*new_worker)->name, return HRM_OUT_OF_MEMORY;)

    (*new_worker)->role                 = role;
    (*new_worker)->wage                 = wage;
    (*new_worker)->num_available_shifts = numOfShifts;
    if (shift_set_to_copy == NULL)
    {
        result = setCreate(&(*new_worker)->shift_set,
            sortByDayAndThenType, copyShift, freeShift, printShift);
    }
    else
    {
        result = setFilter(*shift_set_to_copy,
            &(*new_worker)->shift_set, 0, filterSetCopyFunc);
    }
    return setResultToHrmResult(result, HRM_WORKER_ALREADY_EXISTS, HRM_WORKER_DOES_NOT_EXIST);
}


/**
 * 
 * @param Elem Worker element to copy
 * @return a pointer to a duplicate of the Worker
 */
SetElement copyWorker(SetElement Elem /*Worker*/)
{
    Worker new_worker   = malloc(sizeof(Worker_t));
    EXPECT_NOT_NULL(new_worker, return NULL; )

    Worker this_worker  = (Worker)Elem;
    HrmResult result    = initiateNewWorker(
                            this_worker->name,
                            this_worker->id,
                            this_worker->role,
                            this_worker->wage,
                            this_worker->num_available_shifts,
                            &new_worker,
                            &this_worker->shift_set);
    if (result != HRM_SUCCESS)
        freeWorker(new_worker);
    return new_worker;
}

void printWorker(FILE* out, SetElement Elem /*Worker*/)
{
    Worker worker       = Elem;
    Set shifts          = worker->shift_set;
    float total_payment = setGetSize(shifts) * HOURS_PER_SHIFT * worker->wage;
    prog2_report_worker(out, worker->id, worker->name,
                        worker->wage, worker->role, total_payment);
}

/**
 *
 * @param ElemA First Worker to compare
 * @param ElemB Second Worker to compare
 * @return -1 if first ahead of second, 1 if vice versa, 0 if they're the same
 */
int sortByID(SetElement ElemA /*Worker*/, SetElement ElemB /*Worker*/)
{
    unsigned int idA = ((Worker)ElemA)->id;
    unsigned int idB = ((Worker)ElemB)->id;

    return (idA > idB) - (idA < idB);
}

/**
 *
 * @param Elem Worker to compare
 * @param key unsigned int ID to compare against
 * @return 1 if matches, 0 if not
 */
int matchByID(SetElement Elem /*Worker*/, KeyForSetElement key /*Worker ID*/)
{
    return (((Worker)Elem)->id == *(unsigned int*)key);
}

/**
 *
 * @param Elem Worker to compare
 * @param key HrmWorkerRole to compare against
 * @return 1 if matches, 0 if not
 */
int matchByRole(SetElement Elem /*Worker*/, KeyForSetElement key /*Worker Role*/)
{
    return (((Worker)Elem)->role == *(HrmWorkerRole*)key);
}

/**
 *
 * @param Elem Worker to compare
 * @param key pointer to a Shift
 * @return 1 if matches, 0 if not
 */
int matchByShift(SetElement Elem /*Worker*/, KeyForSetElement key /*Shift*/)
{
    return (
        setIsIn(
            ((Worker)Elem)->shift_set, *(Shift*)key)
        == SET_ELEMENT_EXISTS);
}


/**
 *
 * @param set Set to filter
 * @param day HrmShiftDay to find
 * @param type HrmShiftType to find
 * @param workers_in_shift Pointer to a new set to store the workers in
 * @return The result of filtering for workers in this shift
 */
HrmResult filterWorkersByShift(const Set* set, HrmShiftDay day, HrmShiftType type, /*OUT*/ Set* workers_in_shift)
{
    EXPECT_NOT_NULL(set,                return HRM_NULL_ARGUMENT;)

    EXPECT_NOT_NULL(workers_in_shift,   return HRM_NULL_ARGUMENT;)

    /* Create a Shift to be used for setIsIn*/
    Shift shift         = malloc(sizeof(Shift_t));

    EXPECT_NOT_NULL(shift,              return HRM_OUT_OF_MEMORY;)

    shift->day          = day;
    shift->type         = type;

    SetResult result    = setFilter(*set, workers_in_shift, &shift, matchByShift);
    freeShift(shift);
    return setResultToHrmResult(result, HRM_WORKER_ALREADY_EXISTS, HRM_WORKER_DOES_NOT_EXIST);
}

/* ******************* HR Management Functions ******************* */
HrMgmt HrMgmtCreate()
{
    HrMgmt hrMgmt       = malloc(sizeof(HrMgmt_t));
    EXPECT_NOT_NULL(hrMgmt, return NULL; )

    SetResult result    = setCreate(&(hrMgmt->worker_set), sortByID, copyWorker, freeWorker, printWorker);

    if (result != SET_SUCCESS)
    {
        free(hrMgmt);
        return NULL;
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
                          int numOfShifts)
{
    /* THE ERROR GATES!! */
    EXPECT_NOT_NULL(name,               return HRM_NULL_ARGUMENT; )
    EXPECT_NOT_NULL(hrm,                return HRM_NULL_ARGUMENT; )
    EXPECT_NOT_NULL(hrm->worker_set,    return HRM_NULL_ARGUMENT; )

    EXPECT_GT(id, 0,                    return HRM_INVALID_WORKER_ID; )

    EXPECT_GT(wage, 0,                  return HRM_INVALID_WAGE; )

    EXPECT_GT(numOfShifts, 0,           return HRM_INVALID_NUM_OF_SHIFTS; )

    Worker new_worker   = malloc(sizeof(Worker_t));

    EXPECT_NOT_NULL(new_worker,         return HRM_OUT_OF_MEMORY; )

    Set set              = hrm->worker_set;

    HrmResult hrm_result = initiateNewWorker(name, id, role, wage, numOfShifts, &new_worker, NULL);
    if (hrm_result != HRM_SUCCESS)
    {
        freeWorker(new_worker);
        return hrm_result;
    }

    /* Add our worker to the worker_set */
    SetResult set_result = setAdd(set, new_worker);

    /* Since Set just clones the data, we can free it */
    freeWorker(new_worker);
    return setResultToHrmResult(set_result, HRM_WORKER_ALREADY_EXISTS, HRM_WORKER_DOES_NOT_EXIST);
}

HrmResult HrMgmtRemoveWorker(HrMgmt hrm, int id)
{
    /* THE ERROR GATES!! */
    EXPECT_NOT_NULL(hrm,                return HRM_NULL_ARGUMENT; )
    EXPECT_NOT_NULL(hrm->worker_set,    return HRM_NULL_ARGUMENT; )

    EXPECT_GT(id, 0,                    return HRM_INVALID_WORKER_ID; )

    /* Find the worker by ID and remove him */
    SetElement worker_element   = NULL;
    SetResult result            = setFind(hrm->worker_set, &worker_element, &id, matchByID);
    setRemove(hrm->worker_set, (Worker)worker_element);
    return setResultToHrmResult(result, HRM_WORKER_ALREADY_EXISTS, HRM_WORKER_DOES_NOT_EXIST);
}

HrmResult HrMgmtAddShiftToWorker(HrMgmt hrm,
                                 int id, HrmShiftDay day, HrmShiftType type)
{
    /* THE ERROR GATES!! */
    Shift new_shift = malloc(sizeof(Shift_t));
    EXPECT_NOT_NULL(new_shift, return HRM_OUT_OF_MEMORY; )

    EXPECT_NOT_NULL(hrm, {
        freeShift(new_shift);
        return HRM_NULL_ARGUMENT;
    })

    Set set = hrm->worker_set;
    EXPECT_GT(setGetSize(set), 0, {
        freeShift(new_shift);
        return HRM_WORKER_DOES_NOT_EXIST;
    })

    EXPECT_GT(id, 0, {
        freeShift(new_shift);
        return HRM_INVALID_WORKER_ID;
    })


    /* Find worker by ID */
    SetElement worker_element   = NULL;
    SetResult result            = setFind(set, &worker_element, &id, matchByID);
    if (result != SET_SUCCESS)
    {
        freeShift(new_shift);
        return HRM_WORKER_DOES_NOT_EXIST;
    }

    Worker this_worker  = (Worker)worker_element;

    new_shift->day      = day;
    new_shift->type     = type;


    /* Add shift to set and check for errors */
    HrmResult result_add_or_overflow = setResultToHrmResult(
        setAdd(this_worker->shift_set, new_shift),
        HRM_SHIFT_ALREADY_EXISTS,
        HRM_WORKER_DOES_NOT_EXIST);

    /* If his shift count is greater than what he reported to be available */
    if (setGetSize(this_worker->shift_set) > this_worker->num_available_shifts)
    {
        setRemove(this_worker->shift_set, new_shift);
        result_add_or_overflow = HRM_SHIFTS_OVERFLLOW;
    }
    freeShift(new_shift);

    /* if the result_add error is smaller(i.e higher priority), then return that instead of the overflow(in case we had one) */
    return result_add_or_overflow;
    ;
}

HrmResult HrMgmtRemoveShiftFromWorker(HrMgmt hrm, int id, HrmShiftDay day, HrmShiftType type)
{
    /* THE ERROR GATES!! */
    EXPECT_NOT_NULL(hrm,            return HRM_NULL_ARGUMENT; )

    Set set = hrm->worker_set;

    EXPECT_GT(setGetSize(set), 0,   return HRM_WORKER_DOES_NOT_EXIST;)

    EXPECT_GT(id, 0,                return HRM_INVALID_WORKER_ID; )

    /* Find worker by ID */
    SetElement worker_element   = NULL;
    SetResult result            = setFind(set, &worker_element, &id, matchByID);
    if (result != SET_SUCCESS)
        return HRM_WORKER_DOES_NOT_EXIST;

    Set shift_set = ((Worker)worker_element)->shift_set;

    SetElement shift_element    = NULL;
    HrmResult hrm_result        = findShift(day, type, shift_set, &shift_element);
    if (hrm_result != HRM_SUCCESS)
        return HRM_SHIFT_DOES_NOT_EXIST;

    setRemove(shift_set, shift_element);
    return HRM_SUCCESS;
}


HrmResult HrMgmtTransferShift(HrMgmt hrm, int fromId, int toId, HrmShiftDay day, HrmShiftType type)
{
    /* THE ERROR GATES!! */
    EXPECT_NOT_NULL(hrm,            return HRM_NULL_ARGUMENT; )

    Set set = hrm->worker_set;

    EXPECT_GT(setGetSize(set), 0,   return HRM_WORKER_DOES_NOT_EXIST; )

    EXPECT_GT(fromId, 0,            return HRM_INVALID_WORKER_ID; )
    EXPECT_GT(toId, 0,              return HRM_INVALID_WORKER_ID; )


    /* Find both workers by their ID */
    SetElement worker_from_element  = NULL;
    SetElement worker_to_element    = NULL;
    SetResult result_from           = setFind(set, &worker_from_element, &fromId, matchByID);
    SetResult result_to             = setFind(set, &worker_to_element, &toId, matchByID);
    if (result_from != SET_SUCCESS || result_to != SET_SUCCESS)
        return HRM_WORKER_DOES_NOT_EXIST;

    Worker worker_from  = (Worker)worker_from_element;
    Worker worker_to    = (Worker)worker_to_element;

    Set shift_set_from  = worker_from->shift_set;
    Set shift_set_to    = worker_to->shift_set;


    /* Find required shift in Worker A */
    SetElement shift_element    = NULL;
    HrmResult hrm_result        = findShift(day, type, shift_set_from, &shift_element);
    if (hrm_result != HRM_SUCCESS)
        return hrm_result;

    /* Transfer shift */
    SetResult result = setAdd(shift_set_to, shift_element);
    if (result != SET_SUCCESS)
        return HRM_SHIFT_ALREADY_EXISTS;


    /* Check availability of Worker B for a new shift */
    if (setGetSize(shift_set_to) > worker_to->num_available_shifts)
    {
        setRemove(shift_set_to, shift_element);
        return HRM_SHIFTS_OVERFLLOW;
    }


    setRemove(shift_set_from, shift_element);
    return HRM_SUCCESS;
}


HrmResult HrMgmtReportWorkers(HrMgmt hrm, HrmWorkerRole role, FILE* output)
{
    /* THE ERROR GATES!! */
    EXPECT_NOT_NULL(hrm,    return HRM_NULL_ARGUMENT; )
    EXPECT_NOT_NULL(output, return HRM_NULL_ARGUMENT; )

    Set worker_set  = hrm->worker_set;
    int set_size    = setGetSize(worker_set);
    EXPECT_GT(set_size, 0,  return HRM_NO_WORKERS; )

    if (role >= ALL_ROLES)
    {
        setPrintSorted(worker_set, output, set_size, sortByID);
        return HRM_SUCCESS;
    }

    /* If the role isn't ALL_ROLES, we filter based on role */
    Set filteredSet     = NULL;
    SetResult result    = setFilter(worker_set, &filteredSet, &role, matchByRole);
    if (result != SET_SUCCESS)
    {
        setDestroy(filteredSet);
        return setResultToHrmResult(result, HRM_WORKER_ALREADY_EXISTS, HRM_WORKER_DOES_NOT_EXIST);
    }

    EXPECT_GT(setGetSize(filteredSet), 0, return HRM_NO_WORKERS;)

    setPrintSorted(filteredSet, output, setGetSize(filteredSet), sortByID);
    setDestroy(filteredSet);
    return HRM_SUCCESS;
}


HrmResult HrMgmtReportShiftsOfWorker(HrMgmt hrm, int id, FILE* output)
{
    /* THE ERROR GATES!! */
    EXPECT_NOT_NULL(hrm,    return HRM_NULL_ARGUMENT; )
    EXPECT_NOT_NULL(output, return HRM_NULL_ARGUMENT; )

    EXPECT_GT(id, 0,        return HRM_INVALID_WORKER_ID;)

    Set set             = hrm->worker_set;
    int worker_set_size = setGetSize(set);

    EXPECT_GT(worker_set_size, 0, return HRM_NO_WORKERS; )

    SetElement worker_element = NULL;
    SetResult result = setFind(set, &worker_element, &id, matchByID);
    if (result != SET_SUCCESS || setGetSize(((Worker)worker_element)->shift_set) <= 0)
        return HRM_NO_SHIFTS;

    printWorker(output, (Worker)worker_element);
    setPrintSorted(((Worker)worker_element)->shift_set, output, setGetSize(((Worker)worker_element)->shift_set),
                   sortByDayAndThenType);
    return HRM_SUCCESS;
}

HrmResult HrMgmtReportWorkersInShift(HrMgmt hrm, HrmShiftDay day, HrmShiftType type, FILE* output)
{
    /* THE ERROR GATES!! */
    EXPECT_NOT_NULL(hrm,            return HRM_NULL_ARGUMENT; )

    Set set             = hrm->worker_set;
    int worker_set_size = setGetSize(set);
    EXPECT_GT(worker_set_size, 0,   return HRM_NO_WORKERS; )

    Set workers_in_shift    = NULL;
    HrmResult filter_result = filterWorkersByShift(&set, day, type, &workers_in_shift);
    if (filter_result != HRM_SUCCESS || setGetSize(workers_in_shift) <= 0)
        return HRM_NO_WORKERS;

    setPrintSorted(workers_in_shift, output, worker_set_size, sortByID);
    setDestroy(workers_in_shift);
    return filter_result;
}
