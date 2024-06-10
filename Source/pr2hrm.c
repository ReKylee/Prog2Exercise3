// ReSharper disable CppDFAMemoryLeak
#include "pr2hrm.h"
#include "linked_list.h"
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
	LinkedList worker_linked_list;
} HrMgmt_s;

typedef struct Worker_s
{
	char* name;
	unsigned long id;
	HrmWorkerRole role;
	float wage;
	int numOfShifts;
} Worker;

/* Required Linked List Functions	*
 * For an element of type Worker	*/
void freeWorker(ListElement Elem)
{
	Worker* worker = (Worker*)Elem;
	free(worker->name);
	free(worker);
}

ListElement copyWorker(ListElement Elem)
{
	Worker* new_worker = malloc(sizeof(Worker));
	if(new_worker != NULL)
	{
		Worker* this_worker = (Worker*)Elem;
		new_worker->id = this_worker->id;
		new_worker->name = strdup(this_worker->name);
		if(new_worker->name == NULL)
		{
			freeWorker(new_worker);
			return NULL;
		}
		new_worker->role = this_worker->role;
		new_worker->wage = this_worker->wage;
		new_worker->numOfShifts = this_worker->numOfShifts;
	}
	return (ListElement)new_worker;
}

void printWorker(FILE * out, ListElement Elem)
{
	fprintf(out, "Not Implemented Yet");
}

/*	return -1 if first element should be placed							*
 *	ahead of 2nd , return 1 if vica versa , return 0 if can't decide	*/
int sortByID(ListElement ElemA, ListElement ElemB)
{
	if(((Worker*)ElemA)->id > ((Worker*)ElemB)->id)
	{
		return -1;
	}
	if(((Worker*)ElemA)->id < ((Worker*)ElemB)->id)
	{
		return 1;
	}
	return 0;
}



/* HR Management Functions */
HrMgmt HrMgmtCreate() {
	HrMgmt hrMgmt = malloc(sizeof(struct HrMgmt_s));
	if (hrMgmt != NULL) {
		ListResult result = linkedListCreate(&hrMgmt->worker_linked_list,copyWorker, freeWorker, printWorker);
		if(result == LIST_OUT_OF_MEMORY)
		{
			free(hrMgmt);
			return NULL;
		}
	}
	return hrMgmt;
}

void HrMgmtDestroy(HrMgmt hrm)
{
	linkedListDestroy(hrm->worker_linked_list);
	free(hrm);
}

HrmResult HrMgmtAddWorker(HrMgmt hrm,
	const char *name, int id,
	HrmWorkerRole role, float wage,
	int numOfShifts)
{
	Worker* new_worker = malloc(sizeof(Worker));
	if(new_worker == NULL)
		return HRM_OUT_OF_MEMORY;

	if(name == NULL)
		return HRM_NULL_ARGUMENT;
	if(id <= 0)
		return HRM_INVALID_WORKER_ID;
	if(wage <= 0)
		return HRM_INVALID_WAGE;
	if(numOfShifts <= 0)
		return HRM_INVALID_NUM_OF_SHIFTS;
	new_worker->id = id;
	new_worker->name = strdup(name);
	if(new_worker->name == NULL)
	{
		free(new_worker);
		return HRM_OUT_OF_MEMORY;
	}
	new_worker->role = role;
	new_worker->wage = wage;
	new_worker->numOfShifts = numOfShifts;

	LinkedList list = hrm->worker_linked_list;
	ListResult result =  linkedListInsertLast(list, new_worker);
	if(result == LIST_OUT_OF_MEMORY)
		return HRM_OUT_OF_MEMORY;
	/* linkedListSortElements(list, sortByID);		*/

	/* Since Linked List already clones the data,	*
	 * we can free it afterwards					*/
	freeWorker(new_worker);

	return HRM_SUCCESS;
}
