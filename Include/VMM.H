
#define   VMStat_Exclusive          0x0001L    // VM is exclusive mode
#define   VMStat_Exclusive_Bit      0
#define   VMStat_Background         0x0002L    // VM runs in background
#define   VMStat_Background_Bit     1
#define   VMStat_Creating           0x0004L    // In process of creating
#define   VMStat_Creating_Bit       2
#define   VMStat_Suspended          0x0008L    // VM not scheduled
#define   VMStat_Suspended_Bit      3
#define   VMStat_Not_Executeable    0x0010L    // VM partially destroyed
#define   VMStat_Not_Executeable_Bit 4
#define   VMStat_PM_Exec            0x0020L    // Currently in PM app
#define   VMStat_PM_Exec_Bit        5
#define   VMStat_PM_App             0x0040L    // PM app present in VM
#define   VMStat_PM_App_Bit         6
#define   VMStat_PM_Use32           0x0080L    // PM app is 32-bit
#define   VMStat_PM_Use32_Bit       7
#define   VMStat_VxD_Exec           0x0100L    // Call from VxD
#define   VMStat_VxD_Exec_Bit       8
#define   VMStat_High_Pri_Back      0x0200L    // High pri background
#define   VMStat_High_Pri_Back_Bit  9
#define   VMStat_Blocked            0x0400L    // Blocked on semaphore
#define   VMStat_Blocked_Bit        10
#define   VMStat_Awakening          0x0800L    // Woke up after blocked
#define   VMStat_Awakening_Bit      11
#define   VMStat_PageableV86        0x1000L    // part of V86 is pageable (PM app)
#define   VMStat_PageableV86Bit     12
#define   VMStat_V86IntsLocked      0x2000L    // Rest of V86 is locked
#define   VMStat_V86IntsLockedBit   13         //  regardless of pager type
#define   VMStat_TS_Sched           0x4000L    // Scheduled by time-slicer
#define   VMStat_TS_Sched_Bit       14
#define   VMStat_Idle               0x8000L    // VM has released time
#define   VMStat_Idle_Bit           15         // slice
#define   VMStat_Closing            0x10000L   // Close_VM called for VM
#define   VMStat_Closing_Bit        16

