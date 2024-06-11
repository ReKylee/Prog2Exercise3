#include <stdio.h>
#include "pr2hrm.h"

int main() {
    // Create HR management system
    HrMgmt hr_mgmt = HrMgmtCreate();
    if (hr_mgmt == NULL) {
        printf("Error creating HR management system.\n");
        return 1;
    }

    // Add some workers
    HrmResult result = HrMgmtAddWorker(hr_mgmt, "John Doe", 1, MANAGER, 15.0, 5);
    if (result != HRM_SUCCESS) {
        printf("Error adding worker1: %d\n", result);
        return 1;
    }

    result = HrMgmtAddWorker(hr_mgmt, "Jane Smith", 2, WAITER, 12.0, 4);
    if (result != HRM_SUCCESS) {
        printf("Error adding worker2: %d\n", result);
        return 1;
    }

    // Add shifts to workers
    result = HrMgmtAddShiftToWorker(hr_mgmt, 1, MONDAY, MORNING);
    if (result != HRM_SUCCESS) {
        printf("Error adding shift to worker: %d\n", result);
        return 1;
    }

    result = HrMgmtAddShiftToWorker(hr_mgmt, 2, TUESDAY, EVENING);
    if (result != HRM_SUCCESS) {
        printf("Error adding shift to worker: %d\n", result);
        return 1;
    }

    // Report workers
    printf("All workers:\n");
    HrMgmtReportWorkers(hr_mgmt, ALL_ROLES, stdout);

    // Report shifts of a specific worker
    printf("\nShifts for worker with ID 1:\n");
    HrMgmtReportShiftsOfWorker(hr_mgmt, 1, stdout);

    // Report workers assigned to a specific shift
    printf("\nWorkers assigned to Monday morning shift:\n");
    HrMgmtReportWorkersInShift(hr_mgmt, MONDAY, MORNING, stdout);

    // Clean up
    HrMgmtDestroy(hr_mgmt);

    return 0;
}
