#include "framework.h"
#include "HIDController.h"
#include "DeviceInfo.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cwchar>
#include <hidsdi.h>
#pragma comment(lib, "hid.lib")

// hidapi C linkage
extern "C" {
#include <hidapi.h>
}

//  Debug logging
static void dbg(const std::string& s) {
    OutputDebugStringA((s + "\n").c_str());
}

bool HIDController::executeBatteryQuery(std::vector<unsigned char>& response)
{
    if (!m_device || m_devicePath.empty()) {
        return false;
    }

    // Build the 16-byte battery command packet
    std::vector<uint8_t> cmd(16, 0);
    cmd[0] = 0x04;
    calculateAtkChecksum(cmd);

    std::vector<unsigned char> report(1 + cmd.size());
    report[0] = REPORT_ID;
    std::copy(cmd.begin(), cmd.end(), report.begin() + 1);

    // Send the command via a raw handle using HidD_SetOutputReport (Only one that works)
    HANDLE rawHandle = CreateFileW(
        m_devicePath.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (rawHandle == INVALID_HANDLE_VALUE) {
        dbg("executeBatteryQuery: Failed to create raw handle.");
        return false;
    }

    bool success = true;
    if (!HidD_SetOutputReport(rawHandle, (PVOID)report.data(), (ULONG)report.size())) {
        dbg("executeBatteryQuery: HidD_SetOutputReport failed.");
        success = false;
    }
    CloseHandle(rawHandle);

    if (!success) {
        return false;
    }

    // Read the response, polling for a short time to allow the device to reply.
    // handles cases where other HID reports might be in the queue.
    for (int attempt = 0; attempt < 30; ++attempt) {
        int r = hid_read_timeout(m_device, response.data(), (int)response.size(), 50);
        if (r > 0) {
            // Check if this is the correct report (ID=0x08, Command=0x04)
            if (r >= 9 && response[0] == REPORT_ID && response[1] == 0x04) {
                dbg("executeBatteryQuery: Got a valid battery response.");
                return true; // Success!
            }
        }
    }

    dbg("executeBatteryQuery: Timed out waiting for a valid battery response.");
    return false; // Failure
}


HIDController::HIDController()
{
    dbg("HIDController ctor: calling hid_init()");
    hid_init();
}

HIDController::~HIDController()
{
    dbg("HIDController dtor: closing and exiting hid");
    if (m_device) {
        hid_close(m_device);
        m_device = nullptr;
    }
    hid_exit();
}

bool HIDController::findAndOpenDevice(unsigned short vendorId, const std::vector<unsigned short>& productIds)
{
    if (m_device) {
        hid_close(m_device);
        m_device = nullptr;
    }

    dbg("Starting hidapi enumeration...");
    struct hid_device_info* devs = hid_enumerate(vendorId, 0x0);
    struct hid_device_info* cur = devs;
    std::vector<std::string> candidate_paths;

    while (cur) {
        for (unsigned short pid : productIds) {
            if (cur->product_id == pid) {
                if (cur->path) {
                    candidate_paths.emplace_back(cur->path);
                }
                break;
            }
        }
        cur = cur->next;
    }
    hid_free_enumeration(devs);

    for (auto const& path : candidate_paths) {
        dbg(std::string("Trying path: ") + path);
        m_device = hid_open_path(path.c_str());
        if (m_device) {
            m_devicePath = std::wstring(path.begin(), path.end());
            if (probeForBattery()) {
                dbg("  probeForBattery succeeded, using this device");
                return true;
            }
            hid_close(m_device);
            m_device = nullptr;
        }
    }

    dbg("FAILED: could not open any of the target devices");
    return false;
}


bool HIDController::probeForBattery()
{
    std::vector<unsigned char> response(17);
    return executeBatteryQuery(response);
}

int HIDController::getBatteryLevel()
{
    // If we aren't connected, we can't get a battery level.
    if (!m_device) {
        return -1;
    }

    // Try to get the battery level using the existing connection.
    std::vector<unsigned char> response(17);
    if (executeBatteryQuery(response)) {
        // Success! Return the parsed value.
        return response[6];
    }
    else {
        // If the query failed, the device was likely disconnected.
        // Clean up the invalid handle..
        dbg("getBatteryLevel: executeBatteryQuery failed. Assuming device disconnected.");
        hid_close(m_device);
        m_device = nullptr;
        return -1;
    }
}


void HIDController::calculateAtkChecksum(std::vector<uint8_t>& buffer)
{
    if (buffer.size() < 16) return;
    uint16_t sum = REPORT_ID;
    for (int i = 0; i < 15; ++i) sum += buffer[i];
    buffer[15] = static_cast<uint8_t>((0x55 - (sum & 0xFF)) & 0xFF);
}

void HIDController::runDiagnostics(const DiagnosticStats& stats)
{
    wchar_t* desktopPath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &desktopPath))) {
        std::wstring filePath = std::wstring(desktopPath) + L"\\ATK_Battery_Report.txt";
        CoTaskMemFree(desktopPath);

        std::wofstream logFile(filePath);
        if (!logFile.is_open()) return;

        SYSTEMTIME now;
        GetLocalTime(&now);

        logFile << L"==============================================\n";
        logFile << L"       ATK BATTERY MONITOR DIAGNOSTICS        \n";
        logFile << L"==============================================\n\n";

        logFile << L"[APPLICATION STATE]\n";
        logFile << L"Report Generated: " << now.wYear << L"-" << now.wMonth << L"-" << now.wDay << L" " << now.wHour << L":" << now.wMinute << L"\n";
        logFile << L"App Started:      " << stats.startTime.wYear << L"-" << stats.startTime.wMonth << L"-" << stats.startTime.wDay << L"\n";
        logFile << L"Selected Device:  " << stats.selectedDeviceName << L"\n";
        logFile << L"Poll Interval:    " << stats.pollingIntervalMinutes << L" Minute(s)\n";
        logFile << L"Total Triggers:   " << stats.totalTriggers << L"\n";
        logFile << L"Success Count:    " << stats.successfulRefreshes << L"\n";
        logFile << L"Last Result:      " << stats.lastStatus << L" (Level: " << stats.lastBatteryLevel << L"%)\n\n";

        logFile << L"[HID ENUMERATION]\n";
        logFile << L"Scanning system for database matches...\n\n";

        struct hid_device_info* devs = hid_enumerate(0x0, 0x0);
        struct hid_device_info* cur = devs;

        while (cur) {
            std::wstring matchNote = L"";

            // CHECK DATABASE FOR MATCHES
            for (const auto& dbEntry : g_deviceDatabase) {
                // Check if it matches a Wired PID
                if (cur->vendor_id == dbEntry.vendorId && cur->product_id == dbEntry.wiredPid) {
                    matchNote = L">>> [MATCH: " + dbEntry.displayName + L" (Wired Mode)] <<<";
                    break;
                }

                // Check if it matches any Receiver PID
                for (const auto& receiver : dbEntry.receivers) {
                    if (cur->vendor_id == receiver.vendorId && cur->product_id == receiver.productId) {
                        matchNote = L">>> [MATCH: " + dbEntry.displayName + L" (" + receiver.type + L" Receiver)] <<<";
                        break;
                    }
                }
                if (!matchNote.empty()) break;
            }

            if (!matchNote.empty()) {
                logFile << matchNote << L"\n";
            }

            logFile << L"Product:   " << (cur->product_string ? cur->product_string : L"Unknown") << L"\n";
            logFile << L"VID/PID:   0x" << std::hex << cur->vendor_id << L" / 0x" << cur->product_id << std::dec << L"\n";
            logFile << L"Interface: " << cur->interface_number << L"\n";
            logFile << L"Usage Page: 0x" << std::hex << cur->usage_page << L"\n";
            logFile << L"Usage ID:   0x" << cur->usage << std::dec << L"\n";

            if (cur->path) {
                logFile << L"Path:      " << cur->path << L"\n";
            }
            logFile << L"----------------------------------------------\n";
            cur = cur->next;
        }

        hid_free_enumeration(devs);
        logFile << L"\n[EOF]\n";
        logFile.close();

        std::wstring msg = L"Detailed diagnostics saved to Desktop:\nATK_Battery_Report.txt";
        MessageBoxW(NULL, msg.c_str(), L"Diagnostics Complete", MB_OK | MB_ICONINFORMATION);
    }
}
