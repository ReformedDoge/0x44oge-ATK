// DeviceInfo.h
#pragma once

#include <vector>
#include <string>

struct ReceiverInfo {
    std::wstring type;
    unsigned short vendorId;
    unsigned short productId;
};

// DeviceInfo Struct
struct DeviceInfo {
    std::wstring displayName;
    UINT commandId;
    unsigned short vendorId;
    unsigned short wiredPid;
    std::wstring sensor;
    std::wstring firmwareMark;
    std::vector<ReceiverInfo> receivers;
};

extern std::vector<DeviceInfo> g_deviceDatabase;
extern UINT g_selectedDeviceId;

void LoadDeviceDatabase();