#include <stdio.h>
#include <assert.h>
#include "pr2hrm.h"

int main() {
    // Create HR management system
    HrMgmt hrm = HrMgmtCreate();
    assert(hrm != NULL);

    // Add a worker
    assert(HrMgmtAddWorker(hrm, "John", 1, MANAGER, 20.0, 6) == HRM_SUCCESS);

    // Try adding a worker with duplicate ID
    assert(HrMgmtAddWorker(hrm, "Alice", 1, CHEF, 15.0, 3) == HRM_WORKER_ALREADY_EXISTS);
    assert(HrMgmtAddWorker(hrm, "Alice", 2, CHEF, 15.0, 3) == HRM_SUCCESS);

    // Add shifts to a worker until their available shifts are full
    for (int i = 0; i < 6; i++) {
        assert(HrMgmtAddShiftToWorker(hrm, 1, i, MORNING) == HRM_SUCCESS);
    }
    // Try adding one more shift
    assert(HrMgmtAddShiftToWorker(hrm, 1, TUESDAY, AFTERNOON) == HRM_SHIFTS_OVERFLLOW);
    assert(HrMgmtAddShiftToWorker(hrm, 2, TUESDAY, AFTERNOON) == HRM_SUCCESS);

    // Try removing a shift from a worker with no shifts
    assert(HrMgmtRemoveShiftFromWorker(hrm, 2, MONDAY, MORNING) == HRM_SHIFT_DOES_NOT_EXIST);

    // Try transferring a shift to a worker with full schedule
    assert(HrMgmtTransferShift(hrm, 1, 1, TUESDAY, MORNING) == HRM_SHIFTS_OVERFLLOW);

    // Report workers for each role individually
    FILE *output = fopen("output.txt", "w");
    assert(HrMgmtReportWorkers(hrm, MANAGER, output) == HRM_SUCCESS);
    fclose(output);

    output = fopen("output.txt", "a");
    assert(HrMgmtReportWorkers(hrm, CHEF, output) == HRM_SUCCESS);
    fclose(output);

    // Report workers for all roles combined
    output = fopen("output.txt", "a");
    assert(HrMgmtReportWorkers(hrm, ALL_ROLES, output) == HRM_SUCCESS);
    fclose(output);

    // Report shifts for each worker individually
    output = fopen("output.txt", "a");
    assert(HrMgmtReportShiftsOfWorker(hrm, 1, output) == HRM_SUCCESS);
    fclose(output);

    // Try reporting workers for a non-existent shift
    output = fopen("output.txt", "a");
    assert(HrMgmtReportWorkersInShift(hrm, SATURDAY, MORNING, output) == HRM_NO_WORKERS);
    fclose(output);

    // Remove a worker with shifts
    assert(HrMgmtRemoveWorker(hrm, 1) == HRM_SUCCESS);

    // Try removing a worker from an empty HR management system
    assert(HrMgmtRemoveWorker(hrm, 1) == HRM_WORKER_DOES_NOT_EXIST);

    // Try transferring a shift from a non-existent worker
    assert(HrMgmtTransferShift(hrm, 1, 2, MONDAY, MORNING) == HRM_WORKER_DOES_NOT_EXIST);

    // Cleanup
    HrMgmtDestroy(hrm);

    printf("All tests passed successfully!\n");

    return 0;
}
