#include "pr2hrm.h"

int main() {
    printf("Starting up...\n");

    HrMgmt hrm = HrMgmtCreate();
    HrMgmtAddWorker(hrm, "John", 1212, BARTENDER, 3, 3);

    return 0;
}