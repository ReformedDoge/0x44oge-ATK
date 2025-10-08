#include "framework.h"
#include "0x44oge ATK.h"
#include "HIDController.h"
#include <resource.h>
#include "DeviceInfo.h"
#include <winreg.h> 

#define MAX_LOADSTRING 100
#define WM_TRAYICON (WM_APP + 1)

// Global Variables
HINSTANCE               hInst;
WCHAR                   szTitle[MAX_LOADSTRING];
WCHAR                   szWindowClass[MAX_LOADSTRING];
HIDController           g_hidController;
UINT_PTR                g_timerId = 1;
HICON                   g_hIcon = nullptr;
UINT                    g_currentMenuId = IDM_INTERVAL_5; // For the interval submenu
NOTIFYICONDATAW         g_nid = {};

const WCHAR* REGISTRY_KEY_PATH = L"Software\\0x44oge\\ATK Battery Monitor";
const WCHAR* REGISTRY_VALUE_NAME = L"SelectedDevice";


// Forward Declarations
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void                AddToTray(HWND);
void                UpdateTrayIcon(HWND, int batteryLevel);
void                ShowContextMenu(HWND);
void                SetPollingInterval(HWND, UINT commandId);
void                QueryBatteryAndUpdateIcon(HWND hWnd);
void                SaveSelectedDevice(const std::wstring& displayName);
std::wstring        LoadSelectedDevice();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY0X44OGEATK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow)) { return FALSE; }
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance;
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	return hWnd != NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CREATE:
		LoadDeviceDatabase(); // Load the device list from DeviceInfo.cpp

		// Persistence Logic
		{
			std::wstring savedDeviceName = LoadSelectedDevice();
			bool foundSavedDevice = false;
			if (!savedDeviceName.empty()) {
				for (const auto& device : g_deviceDatabase) {
					if (device.displayName == savedDeviceName) {
						g_selectedDeviceId = device.commandId;
						foundSavedDevice = true;
						break;
					}
				}
			}
			if (!foundSavedDevice) {
				// Find the default (My Mouse) "ATK A9 Plus Nearlink" and save it
				for (const auto& device : g_deviceDatabase) {
					if (device.displayName == L"ATK A9 Plus Nearlink") {
						g_selectedDeviceId = device.commandId;
						SaveSelectedDevice(device.displayName);
						break;
					}
				}
			}
		}

		AddToTray(hWnd);
		SetPollingInterval(hWnd, g_currentMenuId);
		QueryBatteryAndUpdateIcon(hWnd);
		break;

	case WM_TIMER:
		if (wParam == g_timerId) { QueryBatteryAndUpdateIcon(hWnd); }
		break;

	case WM_TRAYICON:
		if (lParam == WM_RBUTTONUP) { ShowContextMenu(hWnd); }
		break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		//  Check if it's a device selection COM
		if (wmId >= IDM_DEVICE_START && wmId < (IDM_DEVICE_START + g_deviceDatabase.size())) {
			if (g_selectedDeviceId != wmId) {
				g_selectedDeviceId = wmId;
				for (const auto& device : g_deviceDatabase) {
					if (device.commandId == g_selectedDeviceId) {
						SaveSelectedDevice(device.displayName);
						break;
					}
				}
				QueryBatteryAndUpdateIcon(hWnd);
			}
			break;
		}

		switch (wmId)
		{
		case IDM_EXIT: DestroyWindow(hWnd); break;
		case IDM_REFRESH: QueryBatteryAndUpdateIcon(hWnd); break;
		case IDM_INTERVAL_1: case IDM_INTERVAL_5: case IDM_INTERVAL_15: case IDM_INTERVAL_30:
			SetPollingInterval(hWnd, wmId);
			break;
		case IDM_DIAGNOSTICS: g_hidController.runDiagnostics(); break;
		default: return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &g_nid);
		KillTimer(hWnd, g_timerId);
		if (g_hIcon) DestroyIcon(g_hIcon);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void QueryBatteryAndUpdateIcon(HWND hWnd) {
	DeviceInfo selectedDevice;
	bool deviceFoundInDb = false;
	for (const auto& device : g_deviceDatabase) {
		if (device.commandId == g_selectedDeviceId) {
			selectedDevice = device;
			deviceFoundInDb = true;
			break;
		}
	}

	if (!deviceFoundInDb) {
		UpdateTrayIcon(hWnd, -1);
		return;
	}

	std::vector<unsigned short> pidsToSearch;
	if (selectedDevice.wiredPid != 0) {
		pidsToSearch.push_back(selectedDevice.wiredPid);
	}
	for (const auto& receiver : selectedDevice.receivers) {
		pidsToSearch.push_back(receiver.productId);
	}

	int batteryLevel = -1;
	if (g_hidController.findAndOpenDevice(selectedDevice.vendorId, pidsToSearch)) {
		batteryLevel = g_hidController.getBatteryLevel();
	}

	UpdateTrayIcon(hWnd, batteryLevel);
}


void AddToTray(HWND hWnd)
{
	g_nid.cbSize = sizeof(g_nid);
	g_nid.hWnd = hWnd;
	g_nid.uID = 1;
	g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	g_nid.uCallbackMessage = WM_TRAYICON;
	g_nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wcscpy_s(g_nid.szTip, L"ATK Battery Status: Initializing...");
	Shell_NotifyIcon(NIM_ADD, &g_nid);
}

void UpdateTrayIcon(HWND hWnd, int batteryLevel)
{
	// batteryLevel = 100;
	std::wstring text = (batteryLevel >= 0) ? std::to_wstring(batteryLevel) : L"X";

	// CREATE BLANK CANVAS
	const int iconSizeX = GetSystemMetrics(SM_CXSMICON);
	const int iconSizeY = GetSystemMetrics(SM_CYSMICON);

	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, iconSizeX, iconSizeY);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
	RECT rect = { 0, 0, iconSizeX, iconSizeY };

	// DRAW THE BACKGROUND
	HBRUSH hBrush;
	if (batteryLevel < 0) { hBrush = CreateSolidBrush(RGB(128, 128, 128)); } // Grey
	else if (batteryLevel <= 20) { hBrush = CreateSolidBrush(RGB(210, 43, 43)); } // Red
	else if (batteryLevel > 95) { hBrush = CreateSolidBrush(RGB(0, 150, 255)); } // Blue
	else { hBrush = CreateSolidBrush(RGB(46, 139, 87)); } // Green

	FillRect(hdcMem, &rect, hBrush);
	DeleteObject(hBrush);

	// DYNAMICALLY SIZE AND DRAW THE TEXT
	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, RGB(255, 255, 255));

	HFONT hFont = NULL;

	// Start with the largest desired font size and shrink until the text fits.
	for (int pointSize = 15; pointSize >= 7; pointSize--) {
		// Create a font of the current test size.
		int fontSize = -MulDiv(pointSize, GetDeviceCaps(hdcMem, LOGPIXELSY), 72);
		hFont = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			NONANTIALIASED_QUALITY, DEFAULT_PITCH, L"Arial"); // Segoe UI or Arial? also CLEARTYPE_QUALITY or NONANTIALIASED_QUALITY

		HGDIOBJ hOldFont = SelectObject(hdcMem, hFont);

		// Calculate the text dimensions without drawing it.
		RECT textRect = { 0, 0, 0, 0 };
		DrawTextW(hdcMem, text.c_str(), -1, &textRect, DT_CALCRECT | DT_SINGLELINE);

		SelectObject(hdcMem, hOldFont);

		// Check if it fits within the icon
		if ((textRect.right - textRect.left) < (iconSizeX)) {
			break;
		}

		// It didn't fit. Clean up this font and the loop will try the next smaller size.
		DeleteObject(hFont);
		hFont = NULL;
	}

	// Draw the text for real W drop shadow
	if (hFont) {
		HGDIOBJ hOldFont = SelectObject(hdcMem, hFont);

		RECT finalDrawRect = rect;
		OffsetRect(&finalDrawRect, 0, 0);

		RECT shadowRect = finalDrawRect;
		OffsetRect(&shadowRect, 1, 1);
		SetTextColor(hdcMem, RGB(0, 0, 0));
		DrawTextW(hdcMem, text.c_str(), -1, &shadowRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		SetTextColor(hdcMem, RGB(255, 255, 255));
		DrawTextW(hdcMem, text.c_str(), -1, &finalDrawRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		SelectObject(hdcMem, hOldFont);
	}

	// CONVERT TO ICON & CLEAN UP GDI OBJECTS
	ICONINFO ii = {};
	ii.fIcon = TRUE;
	ii.hbmColor = hBitmap;
	ii.hbmMask = hBitmap;
	if (g_hIcon) DestroyIcon(g_hIcon);
	g_hIcon = CreateIconIndirect(&ii);

	SelectObject(hdcMem, hOldBitmap);
	DeleteObject(hBitmap);
	if (hFont) {
		DeleteObject(hFont);
	}
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	// UPDATE THE SYSTEM TRAY ICON
	g_nid.uFlags = NIF_ICON | NIF_TIP;
	g_nid.hIcon = g_hIcon;
	std::wstring tooltip = L"ATK Battery: " + ((batteryLevel >= 0) ? std::to_wstring(batteryLevel) + L"%" : L"Disconnected");
	wcscpy_s(g_nid.szTip, tooltip.c_str());

	Shell_NotifyIcon(NIM_MODIFY, &g_nid);
}

void ShowContextMenu(HWND hWnd) {
	POINT pt; GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();

	HMENU hDeviceSubMenu = CreatePopupMenu();
	for (const auto& device : g_deviceDatabase) {
		AppendMenuW(hDeviceSubMenu,
			(g_selectedDeviceId == device.commandId) ? (MF_STRING | MF_CHECKED) : MF_STRING,
			device.commandId,
			device.displayName.c_str());
	}

	HMENU hIntervalSubMenu = CreatePopupMenu();
	AppendMenuW(hIntervalSubMenu, (g_currentMenuId == IDM_INTERVAL_1) ? (MF_STRING | MF_CHECKED) : MF_STRING, IDM_INTERVAL_1, L"1 Minute");
	AppendMenuW(hIntervalSubMenu, (g_currentMenuId == IDM_INTERVAL_5) ? (MF_STRING | MF_CHECKED) : MF_STRING, IDM_INTERVAL_5, L"5 Minutes");
	AppendMenuW(hIntervalSubMenu, (g_currentMenuId == IDM_INTERVAL_15) ? (MF_STRING | MF_CHECKED) : MF_STRING, IDM_INTERVAL_15, L"15 Minutes");
	AppendMenuW(hIntervalSubMenu, (g_currentMenuId == IDM_INTERVAL_30) ? (MF_STRING | MF_CHECKED) : MF_STRING, IDM_INTERVAL_30, L"30 Minutes");

	HMENU hDiagMenu = CreatePopupMenu();
	AppendMenuW(hDiagMenu, MF_STRING, IDM_DIAGNOSTICS, L"Log All HID Devices to Desktop");

	AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hDeviceSubMenu, L"Select Device");
	AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenu, MF_STRING, IDM_REFRESH, L"Refresh Now");
	AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hIntervalSubMenu, L"Interval");
	AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hDiagMenu, L"Diagnostics");
	AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"Exit");

	SetForegroundWindow(hWnd);
	TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
	PostMessage(hWnd, WM_NULL, 0, 0);
	DestroyMenu(hMenu);
}

void SaveSelectedDevice(const std::wstring& displayName) {
	HKEY hKey;
	if (RegCreateKeyExW(HKEY_CURRENT_USER, REGISTRY_KEY_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
		RegSetValueExW(hKey, REGISTRY_VALUE_NAME, 0, REG_SZ,
			(const BYTE*)displayName.c_str(),
			(DWORD)((displayName.length() + 1) * sizeof(wchar_t)));
		RegCloseKey(hKey);
	}
}

std::wstring LoadSelectedDevice() {
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_CURRENT_USER, REGISTRY_KEY_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		wchar_t buffer[256];
		DWORD bufferSize = sizeof(buffer);
		if (RegQueryValueExW(hKey, REGISTRY_VALUE_NAME, NULL, NULL, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return std::wstring(buffer);
		}
		RegCloseKey(hKey);
	}
	return L"";
}


void SetPollingInterval(HWND hWnd, UINT commandId)
{
	g_currentMenuId = commandId;

	// Translate the command ID to a time value
	int minutes;
	switch (commandId)
	{
	case IDM_INTERVAL_1:  minutes = 1; break;
	case IDM_INTERVAL_5:  minutes = 5; break;
	case IDM_INTERVAL_15: minutes = 15; break;
	case IDM_INTERVAL_30: minutes = 30; break;
	default: minutes = 5; break; // Future proof safety ig

	}

	KillTimer(hWnd, g_timerId);
	SetTimer(hWnd, g_timerId, 1000 * 60 * minutes, NULL);
}