#pragma once
#include "framework.h"

struct hid_device_;

class HIDController
{
public:
    HIDController();
    ~HIDController();

    bool findAndOpenDevice(unsigned short vendorId, const std::vector<unsigned short>& productIds);

    int getBatteryLevel();
    void runDiagnostics();

private:
    hid_device_* m_device = nullptr;
    std::wstring m_devicePath;

    bool probeForBattery();
    bool executeBatteryQuery(std::vector<unsigned char>& response);
    void calculateAtkChecksum(std::vector<uint8_t>& buffer);
    static constexpr unsigned char REPORT_ID = 0x08;
};
