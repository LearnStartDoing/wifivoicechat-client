
typedef struct {
  char ID[16];
  char ServerName[128];
  u32 ServerPort;
  u32 Latency;
  u32 dummy00,dummy01;
  u32 dummy10,dummy11,dummy12,dummy13;
} TNetworkSetting;

extern TNetworkSetting NetworkSetting;

