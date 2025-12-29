#include "framework.h"
#include "0x44oge ATK.h"
#include "HIDController.h"
#include <resource.h>
#include "DeviceInfo.h"
#include <winreg.h> 
#include <atomic>
#include <mutex>

#define MAX_LOADSTRING 100
#define WM_TRAYICON (WM_APP + 1)
#define WM_BATTERY_UPDATE_READY (WM_APP + 2)

namespace {
    struct GdiObj {
        HGDIOBJ h;
        GdiObj(HGDIOBJ h) : h(h) {}
        ~GdiObj() { if (h) DeleteObject(h); }
        template<typename T>
        operator T() const { return (T)h; }
    };

    const WCHAR* REG_PATH_RUN = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const WCHAR* REG_PATH_APP = L"Software\\0x44oge\\ATK Battery Monitor";
    const WCHAR* REG_VAL_DEVICE = L"SelectedDevice";
    const WCHAR* STARTUP_VALUE_NAME = L"0x44oge_ATK_Battery_Monitor";
    const WCHAR* REG_VAL_INTERVAL = L"Interval";
}

class BatteryApp {
public:
    BatteryApp(HINSTANCE hInst) : m_hInst(hInst), m_isUpdating(false) {
        LoadStringW(hInst, IDS_APP_TITLE, m_szTitle, MAX_LOADSTRING);
        LoadStringW(hInst, IDC_MY0X44OGEATK, m_szWindowClass, MAX_LOADSTRING);
        GetLocalTime(&m_stats.startTime);
        m_stats.totalTriggers = 0;
        m_stats.successfulRefreshes = 0;
        m_stats.lastBatteryLevel = -1;
        m_stats.lastStatus = L"Initializing";
    }

    bool Init() {
        WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
        wcex.lpfnWndProc = BatteryApp::WndProcStatic;
        wcex.hInstance = m_hInst;
        wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.lpszClassName = m_szWindowClass;
        RegisterClassExW(&wcex);

        m_hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInst, this);

        return m_hWnd != NULL;
    }

    void Run() {
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

private:
    HINSTANCE m_hInst;
    HWND m_hWnd = nullptr;
    WCHAR m_szTitle[MAX_LOADSTRING];
    WCHAR m_szWindowClass[MAX_LOADSTRING];

    std::mutex m_hidMutex;
    HIDController m_hidController;
    NOTIFYICONDATAW m_nid = { sizeof(m_nid) };
    HICON m_hCurrentIcon = nullptr;

    UINT m_currentIntervalId = IDM_INTERVAL_5;
    std::atomic<bool> m_isUpdating; // Prevents overlapping threads
    DiagnosticStats m_stats;

    // Background Threading Logic
    void RequestBatteryUpdate() {
        if (m_isUpdating.exchange(true)) return;

        DeviceInfo selected;
        bool found = false;
        for (const auto& d : g_deviceDatabase) {
            if (d.commandId == g_selectedDeviceId) {
                selected = d;
                found = true;
                break;
            }
        }

        if (!found) {
            m_isUpdating = false;
            UpdateTrayIcon(-1);
            return;
        }

        m_stats.totalTriggers++; // Count attempts
        m_stats.selectedDeviceName = selected.displayName;
        // Run query in background thread
        std::thread([this, selected]() {
            std::vector<unsigned short> pids;
            if (selected.wiredPid != 0) pids.push_back(selected.wiredPid);
            for (const auto& r : selected.receivers) pids.push_back(r.productId);

            int level = -1;
            {
                std::lock_guard<std::mutex> lock(m_hidMutex); // LOCK
                if (m_hidController.findAndOpenDevice(selected.vendorId, pids)) {
                    level = m_hidController.getBatteryLevel();
                }
            }

            PostMessage(m_hWnd, WM_BATTERY_UPDATE_READY, (WPARAM)level, 0);
            }).detach();
    }

    void SaveInterval(UINT cid) {
        HKEY hKey;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_PATH_APP, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
            RegSetValueExW(hKey, REG_VAL_INTERVAL, 0, REG_DWORD, (BYTE*)&cid, sizeof(DWORD));
            RegCloseKey(hKey);
        }
    }

    UINT LoadInterval() {
        HKEY hKey;
        UINT cid = IDM_INTERVAL_5; // Default if nothing is found
        if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH_APP, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD val;
            DWORD sz = sizeof(val);
            if (RegQueryValueExW(hKey, REG_VAL_INTERVAL, NULL, NULL, (LPBYTE)&val, &sz) == ERROR_SUCCESS) {
                cid = (UINT)val;
            }
            RegCloseKey(hKey);
        }
        return cid;
    }

    void AutoDetectDevice() {
        struct hid_device_info* devs = hid_enumerate(0x0, 0x0);
        struct hid_device_info* cur = devs;
        bool foundMatch = false;

        while (cur) {
            for (const auto& dbEntry : g_deviceDatabase) {
                bool isWired = (cur->vendor_id == dbEntry.vendorId && cur->product_id == dbEntry.wiredPid);
                bool isReceiver = false;

                for (const auto& receiver : dbEntry.receivers) {
                    if (cur->vendor_id == receiver.vendorId && cur->product_id == receiver.productId) {
                        isReceiver = true;
                        break;
                    }
                }

                if (isWired || isReceiver) {
                    g_selectedDeviceId = dbEntry.commandId;
                    SaveSelectedDevice(dbEntry.displayName);
                    foundMatch = true;
                    break;
                }
            }
            if (foundMatch) break;
            cur = cur->next;
        }

        hid_free_enumeration(devs);

        if (foundMatch) {
            RequestBatteryUpdate(); // Refresh immediately with the new device
        }
        else {
            MessageBoxW(m_hWnd, L"No matching ATK/VGN device found.\nPlease ensure your mouse is turned on or plugged in.",
                L"Auto-Detect", MB_OK | MB_ICONINFORMATION);
        }
    }

    // UI & Tray Logic
    void UpdateTrayIcon(int batteryLevel) {
        std::wstring text = (batteryLevel >= 0) ? std::to_wstring(batteryLevel) : L"X";
        const int iconSize = GetSystemMetrics(SM_CXSMICON);

        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        GdiObj hBitmap = CreateCompatibleBitmap(hdcScreen, iconSize, iconSize);
        GdiObj hMonoMask = CreateBitmap(iconSize, iconSize, 1, 1, NULL);
        ReleaseDC(NULL, hdcScreen);

        {
            HGDIOBJ hOldBitmap = SelectObject(hdcMem, hBitmap);
            RECT rect = { 0, 0, iconSize, iconSize };

            // temporary scope for the brush
            {
                GdiObj hBrush = CreateSolidBrush(
                    (batteryLevel < 0) ? RGB(128, 128, 128) :
                    (batteryLevel <= 20) ? RGB(210, 43, 43) :
                    (batteryLevel > 95) ? RGB(0, 150, 255) : RGB(46, 139, 87));
                FillRect(hdcMem, &rect, hBrush);
            }

            SetBkMode(hdcMem, TRANSPARENT);

            HFONT hFontRaw = NULL;
            for (int pointSize = 15; pointSize >= 7; pointSize--) {
                int fontSize = -MulDiv(pointSize, GetDeviceCaps(hdcMem, LOGPIXELSY), 72);
                hFontRaw = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    NONANTIALIASED_QUALITY, DEFAULT_PITCH, L"Arial");

                HGDIOBJ hOldFont = SelectObject(hdcMem, hFontRaw);
                RECT textRect = { 0, 0, 0, 0 };
                DrawTextW(hdcMem, text.c_str(), -1, &textRect, DT_CALCRECT | DT_SINGLELINE);
                SelectObject(hdcMem, hOldFont);

                if ((textRect.right - textRect.left) < iconSize) break;
                DeleteObject(hFontRaw); hFontRaw = NULL;
            }

            if (hFontRaw) {
                GdiObj hFont = hFontRaw; // WRAP FOR AUTO-DELETE
                HGDIOBJ hOldFont = SelectObject(hdcMem, hFont);
                SetTextColor(hdcMem, RGB(0, 0, 0));
                RECT shadowRect = rect; OffsetRect(&shadowRect, 1, 1);
                DrawTextW(hdcMem, text.c_str(), -1, &shadowRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SetTextColor(hdcMem, RGB(255, 255, 255));
                DrawTextW(hdcMem, text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(hdcMem, hOldFont);
            }

            SelectObject(hdcMem, hOldBitmap);
        }

        ICONINFO ii = {};
        ii.fIcon = TRUE;
        ii.hbmMask = (HBITMAP)hMonoMask;
        ii.hbmColor = (HBITMAP)hBitmap;

        HICON newIcon = CreateIconIndirect(&ii);

        DeleteDC(hdcMem);

        if (m_hCurrentIcon) DestroyIcon(m_hCurrentIcon);
        m_hCurrentIcon = newIcon;

        m_nid.uFlags = NIF_ICON | NIF_TIP;
        m_nid.hIcon = m_hCurrentIcon;
        std::wstring tooltip = L"ATK Battery: " + ((batteryLevel >= 0) ? std::to_wstring(batteryLevel) + L"%" : L"Disconnected");
        wcscpy_s(m_nid.szTip, tooltip.c_str());
        Shell_NotifyIcon(NIM_MODIFY, &m_nid);
    }

    // Registry Helpers
    void SaveSelectedDevice(const std::wstring& name) {
        HKEY hKey;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_PATH_APP, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
            RegSetValueExW(hKey, REG_VAL_DEVICE, 0, REG_SZ, (BYTE*)name.c_str(), (DWORD)((name.length() + 1) * sizeof(wchar_t)));
            RegCloseKey(hKey);
        }
    }

    std::wstring LoadSelectedDevice() {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH_APP, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t buf[256]; DWORD sz = sizeof(buf);
            if (RegQueryValueExW(hKey, REG_VAL_DEVICE, NULL, NULL, (LPBYTE)buf, &sz) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return buf;
            }
            RegCloseKey(hKey);
        }
        return L"";
    }

    bool IsStartupEnabled() {
        HKEY hKey; bool res = false;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH_RUN, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            res = (RegQueryValueExW(hKey, STARTUP_VALUE_NAME, NULL, NULL, NULL, NULL) == ERROR_SUCCESS);
            RegCloseKey(hKey);
        }
        return res;
    }

    void SetStartup(bool enable) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH_RUN, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            if (enable) {
                WCHAR path[MAX_PATH]; GetModuleFileNameW(NULL, path, MAX_PATH);
                std::wstring cmd = L"\"" + std::wstring(path) + L"\"";
                RegSetValueExW(hKey, STARTUP_VALUE_NAME, 0, REG_SZ, (BYTE*)cmd.c_str(), (DWORD)((cmd.length() + 1) * sizeof(wchar_t)));
            }
            else {
                RegDeleteValueW(hKey, STARTUP_VALUE_NAME);
            }
            RegCloseKey(hKey);
        }
    }

    void SetTimerInterval(HWND hWnd, UINT cid) {
        m_currentIntervalId = cid; // Save to Registry
        SaveInterval(cid);

        int mins = (cid == IDM_INTERVAL_1) ? 1 : (cid == IDM_INTERVAL_15) ? 15 : (cid == IDM_INTERVAL_30) ? 30 : 5;

        m_stats.pollingIntervalMinutes = mins; // SAVE TO STATS
        m_hWnd = hWnd; //unnecessary but imma keep it for now
        SetTimer(hWnd, 1, 1000 * 60 * mins, NULL);
    }

    void ShowMenu() {
        POINT pt; GetCursorPos(&pt);
        HMENU hMenu = CreatePopupMenu();
        HMENU hDev = CreatePopupMenu();
        HMENU hInt = CreatePopupMenu();

        for (const auto& d : g_deviceDatabase)
            AppendMenuW(hDev, (g_selectedDeviceId == d.commandId ? MF_CHECKED : 0) | MF_STRING, d.commandId, d.displayName.c_str());

        AppendMenuW(hInt, (m_currentIntervalId == IDM_INTERVAL_1 ? MF_CHECKED : 0), IDM_INTERVAL_1, L"1 Minute");
        AppendMenuW(hInt, (m_currentIntervalId == IDM_INTERVAL_5 ? MF_CHECKED : 0), IDM_INTERVAL_5, L"5 Minutes");
        AppendMenuW(hInt, (m_currentIntervalId == IDM_INTERVAL_15 ? MF_CHECKED : 0), IDM_INTERVAL_15, L"15 Minutes");
        AppendMenuW(hInt, (m_currentIntervalId == IDM_INTERVAL_30 ? MF_CHECKED : 0), IDM_INTERVAL_30, L"30 Minutes");

        AppendMenuW(hMenu, MF_STRING, IDM_AUTO_DETECT, L"Auto-Detect Device");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hDev, L"Select Device Manually");

        AppendMenuW(hMenu, MF_SEPARATOR, 0, 0);
        AppendMenuW(hMenu, MF_STRING, IDM_REFRESH, L"Refresh Now");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hInt, L"Interval");
        AppendMenuW(hMenu, (IsStartupEnabled() ? MF_CHECKED : 0) | MF_STRING, IDM_STARTUP, L"Start with Windows");
        AppendMenuW(hMenu, MF_STRING, IDM_DIAGNOSTICS, L"Log Diagnostics");
        AppendMenuW(hMenu, MF_SEPARATOR, 0, 0);
        AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"Exit");

        SetForegroundWindow(m_hWnd);
        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hWnd, NULL);
        DestroyMenu(hMenu);
    }

    // Message Processing
    static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
        BatteryApp* pApp = nullptr;
        if (msg == WM_NCCREATE) {
            pApp = (BatteryApp*)((LPCREATESTRUCT)lp)->lpCreateParams;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pApp);
        }
        else {
            pApp = (BatteryApp*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        }
        return pApp ? pApp->HandleMessage(hWnd, msg, wp, lp) : DefWindowProc(hWnd, msg, wp, lp);
    }

    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
            case WM_CREATE:
                m_hWnd = hWnd;
                LoadDeviceDatabase();
                m_nid.hWnd = hWnd; m_nid.uID = 1; m_nid.uCallbackMessage = WM_TRAYICON;
                m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
                m_nid.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
                Shell_NotifyIcon(NIM_ADD, &m_nid);

                // Persistence
                {
                    std::wstring saved = LoadSelectedDevice();
                    for (const auto& d : g_deviceDatabase) {
                        if (d.displayName == saved || (saved.empty() && d.displayName == L"ATK A9 Plus Nearlink")) {
                            g_selectedDeviceId = d.commandId;
                            break;
                        }
                    }
                }
                m_currentIntervalId = LoadInterval();
                SetTimerInterval(hWnd, m_currentIntervalId);
                RequestBatteryUpdate();
                return 0;

            case WM_BATTERY_UPDATE_READY:
            {
                int level = (int)wp;
                m_isUpdating = false;

                // Update Diagnostics Stats
                m_stats.lastBatteryLevel = level;
                if (level >= 0) {
                    m_stats.successfulRefreshes++;
                    m_stats.lastStatus = L"Success";
                }
                else {
                    m_stats.lastStatus = L"Failed (Device Not Found or Timed Out)";
                }

                UpdateTrayIcon(level);
                return 0;
            }

            case WM_TIMER: RequestBatteryUpdate(); return 0;
            case WM_TRAYICON: if (lp == WM_RBUTTONUP) ShowMenu(); return 0;
            case WM_COMMAND:
            {
                UINT id = LOWORD(wp); // stackoverflow: LOWORD for safety x64 x86

                if (id == IDM_AUTO_DETECT) {
                    AutoDetectDevice();
                }
                else if (id >= IDM_DEVICE_START && id < IDM_DEVICE_START + g_deviceDatabase.size()) {
                    g_selectedDeviceId = id;
                    for (const auto& d : g_deviceDatabase) {
                        if (d.commandId == id) {
                            SaveSelectedDevice(d.displayName);
                            break;
                        }
                    }
                    RequestBatteryUpdate();
                }
                else {
                    switch (id) {
                    case IDM_EXIT: DestroyWindow(hWnd); break;
                    case IDM_REFRESH: RequestBatteryUpdate(); break;
                    case IDM_INTERVAL_1: case IDM_INTERVAL_5: case IDM_INTERVAL_15: case IDM_INTERVAL_30:
                        SetTimerInterval(hWnd, id); break;
                    case IDM_STARTUP: SetStartup(!IsStartupEnabled()); break;
                    case IDM_DIAGNOSTICS: {
                        std::lock_guard<std::mutex> lock(m_hidMutex);
                        m_hidController.runDiagnostics(m_stats);
                        break;
                    }
                    }
                }
                return 0;
            }

            case WM_DESTROY:
                Shell_NotifyIcon(NIM_DELETE, &m_nid);
                if (m_hCurrentIcon) DestroyIcon(m_hCurrentIcon);
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProc(hWnd, msg, wp, lp);
    }
};

int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
    BatteryApp app(hInst);
    if (!app.Init()) return FALSE;
    app.Run();
    return 0;
}