// DeviceInfo.cpp
#include "framework.h"
#include "DeviceInfo.h"

//  Global Variable Definitions
std::vector<DeviceInfo> g_deviceDatabase;
UINT g_selectedDeviceId = 0; // Initialized properly on startup

void LoadDeviceDatabase() {
    UINT currentId = IDM_DEVICE_START;

    //  Default Device ( My A9 Plus )
    g_deviceDatabase.push_back({
        L"ATK A9 Plus Nearlink", currentId++, 14139, 4373,
        L"PAW3395", L"a9",
        { { L"1k", 14139, 4297 } }
        });

    // All Other Devices            vendorId 14139, wiredPid 4439, ReceiverInfo {{ L"1k", 14139, 4297 }}
    g_deviceDatabase.push_back({ L"ATK A9", currentId++, 14139, 4439, L"PAW3395SE", L"a9", {} });
    g_deviceDatabase.push_back({ L"ATK A9 Nearlink", currentId++, 14139, 4374, L"PAW3395SE", L"a9", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK A9 Pro Max", currentId++, 14139, 4375, L"PAW3395Ultra", L"a9", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK A9 SE Nearlink", currentId++, 14139, 4405, L"PAW3311", L"a9", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK A9 Ultra", currentId++, 14139, 4377, L"PAW3950Ultra", L"a9", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK A9 Ultimate", currentId++, 14139, 4534, L"PAW3950Ultra", L"a9", {{ L"8k", 14139, 4519 }} });
    g_deviceDatabase.push_back({ L"ATK A9 Ultra Max 2.0", currentId++, 14139, 4573, L"PAW3395Ultra", L"a9", {{ L"8k", 14139, 4519 }} });
    g_deviceDatabase.push_back({ L"ATK Duckbill", currentId++, 14139, 4517, L"PAW3950", L"duckbill", {{ L"8k", 14139, 4519 }} });
    g_deviceDatabase.push_back({ L"ATK Duckbill Ultra", currentId++, 14139, 4518, L"PAW3950Ultra", L"duckbill", {{ L"8k", 14139, 4519 }} });
    g_deviceDatabase.push_back({ L"ATK F1 EXTREME", currentId++, 14139, 4165, L"PAW3950Ultra", L"f1", {{ L"8k", 14139, 4145 }} });
    g_deviceDatabase.push_back({ L"ATK F1 LEVIATAN", currentId++, 14139, 4187, L"PAW3950Ultra", L"f1", {{ L"8k", 14139, 4145 }} });
    g_deviceDatabase.push_back({ L"ATK F1 PRO", currentId++, 14139, 4116, L"PAW3950", L"f1", {{ L"1k", 14139, 4122 }, { L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK F1 PRO MAX", currentId++, 14139, 4117, L"PAW3950", L"f1", {{ L"1k", 14139, 4122 }, { L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK F1 Ultra", currentId++, 14139, 4156, L"PAW3950Ultra", L"f1", {{ L"1k", 14139, 4122 }, { L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK F1 Ultimate", currentId++, 14139, 4142, L"PAW3950Ultra", L"f1", {{ L"1k", 14139, 4145 }} });
    g_deviceDatabase.push_back({ L"ATK F1 Ultimate 2.0", currentId++, 14139, 4580, L"PAW3950Ultra", L"f1", {} });
    g_deviceDatabase.push_back({ L"ATK FIERCE X", currentId++, 14139, 4144, L"PAW3950Ultra", L"fierce-x", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK U2", currentId++, 14139, 4522, L"PAW3395SE", L"u2", {} });
    g_deviceDatabase.push_back({ L"ATK U2 MINI", currentId++, 14139, 4230, L"PAW3950", L"u2", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK U2 Nearlink", currentId++, 14139, 4366, L"PAW3395SE", L"u2", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK U2 Plus Nearlink", currentId++, 14139, 4365, L"PAW3395", L"u2", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK U2 Pro Max", currentId++, 14139, 4440, L"PAW3395Ultra", L"u2", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK U2 SE Nearlink", currentId++, 14139, 4425, L"PAW3311", L"u2", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK U2 Ultra", currentId++, 14139, 4268, L"PAW3950", L"u2", {} });
    g_deviceDatabase.push_back({ L"ATK U2 Ultimate", currentId++, 14139, 4231, L"PAW3950Ultra", L"u2", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK V1 PRO MAX", currentId++, 14139, 4227, L"PAW3950", L"v1", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK V1 Ultimate", currentId++, 14139, 4225, L"PAW3950Ultra", L"v1", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK V3 Nearlink", currentId++, 14139, 4326, L"PAW3320", L"v3", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK V3 Nearlink Pro", currentId++, 14139, 4327, L"PAW3311", L"v3", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK V3 Nearlink Pro Max", currentId++, 14139, 4328, L"PAW3395SE", L"v3", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK X1 EXTREME", currentId++, 14139, 4329, L"PAW3950Ultra", L"x1", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK X1 Lite", currentId++, 14139, 4414, L"PAW3395", L"x1", {{ L"1k", 14139, 4229 }} });
    g_deviceDatabase.push_back({ L"ATK X1 PRO", currentId++, 14139, 4118, L"PAW3950", L"x1", {{ L"1k", 14139, 4122 }, { L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK X1 PRO MAX", currentId++, 14139, 4119, L"PAW3950", L"x1", {{ L"1k", 14139, 4122 }, { L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK X1 S Nearlink", currentId++, 14139, 4358, L"PAW3395", L"x1", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK X1 SE Nearlink", currentId++, 14139, 4359, L"PAW3395SE", L"x1", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK X1E", currentId++, 14139, 4415, L"PAW3395SE", L"x1", {{ L"1k", 14139, 4229 }} });
    g_deviceDatabase.push_back({ L"ATK X1 Ultra", currentId++, 14139, 4143, L"PAW3950Ultra", L"x1", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK X1 Ultimate", currentId++, 14139, 4157, L"PAW3950Ultra", L"x1", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK X1 Ultimate 2.0", currentId++, 14139, 4583, L"PAW3950Ultra", L"x1", {} });
    g_deviceDatabase.push_back({ L"ATK Y9 Nearlink", currentId++, 14139, 4444, L"PAW3395SE", L"y9", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK Y9 Plus Nearlink", currentId++, 14139, 4443, L"PAW3395", L"y9", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK Y9 Pro Max", currentId++, 14139, 4516, L"PAW3395Ultra", L"y9", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK Y9 PRO MAX (4349)", currentId++, 14139, 4349, L"PAW3950", L"y9", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK Y9 PRO MAX (4350)", currentId++, 14139, 4350, L"PAW3950Ultra", L"y9", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK Y9 SE Nearlink", currentId++, 14139, 4445, L"PAW3311", L"y9", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"ATK Y9 Ultimate", currentId++, 14139, 4559, L"PAW3950Ultra", L"y9", {{ L"8k", 14139, 4519 }} });
    g_deviceDatabase.push_back({ L"ATK Z1 PRO", currentId++, 14139, 4120, L"PAW3950", L"z1", {{ L"1k", 14139, 4122 }, { L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK Z1 PRO MAX", currentId++, 14139, 4121, L"PAW3950", L"z1", {{ L"1k", 14139, 4122 }, { L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK Z1 Ultra", currentId++, 14139, 4170, L"PAW3950Ultra", L"z1", {{ L"1k", 14139, 4122 }, { L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"ATK ZERO EX", currentId++, 14139, 4556, L"PAW3950Ultra", L"duckbill", {{ L"8k", 14139, 4519 }} });
    g_deviceDatabase.push_back({ L"DRAGONFLY F1 MOBA", currentId++, 13652, 62726, L"PAW3395", L"f1-moba", {} });
    g_deviceDatabase.push_back({ L"DRAGONFLY F1 PRO", currentId++, 13652, 62722, L"PAW3395", L"f1-pro", {} });
    g_deviceDatabase.push_back({ L"DRAGONFLY F1 PRO MAX", currentId++, 13652, 62726, L"PAW3395", L"f1-pro-max", {} });
    g_deviceDatabase.push_back({ L"DRAGONFLY F1 PRO MAX Elden", currentId++, 13652, 62726, L"PAW3395", L"f1-pro-max-elden", {} });
    g_deviceDatabase.push_back({ L"DRAGONFLY F1 PRO MAX JOJO", currentId++, 13652, 62726, L"PAW3395", L"f1-pro-max-jojo", {} });
    g_deviceDatabase.push_back({ L"DRAGONFLY F1 V2 Nearlink", currentId++, 14139, 4427, L"PAW3395SE", L"f1", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"DRAGONFLY F1 V2 Plus Nearlink", currentId++, 14139, 4426, L"PAW3395", L"f1", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"DRAGONFLY F1 V2 PRO MAX", currentId++, 14139, 4523, L"PAW3395Ultra", L"f1", {{ L"8k", 14139, 4123 }} });
    g_deviceDatabase.push_back({ L"DRAGONFLY F1 V2 SE Nearlink", currentId++, 14139, 4428, L"PAW3311", L"f1", {{ L"1k", 14139, 4297 }} });
    g_deviceDatabase.push_back({ L"Dragonfly F1S", currentId++, 13652, 62730, L"PAW3395", L"f1-s", {} });
    g_deviceDatabase.push_back({ L"MAD R", currentId++, 14139, 4159, L"PAW3395", L"mad-r", {{ L"8k", 14139, 4160 }} });
    g_deviceDatabase.push_back({ L"MAD R +", currentId++, 14139, 4171, L"PAW3395", L"mad-r", {{ L"8k", 14139, 4160 }} });
    g_deviceDatabase.push_back({ L"MAD R MAJOR", currentId++, 14139, 4158, L"PAW3950", L"mad-r", {{ L"8k", 14139, 4160 }} });
    g_deviceDatabase.push_back({ L"MAD R MAJOR+", currentId++, 14139, 4172, L"PAW3950", L"mad-r", {{ L"8k", 14139, 4160 }} });
    g_deviceDatabase.push_back({ L"VXE R1", currentId++, 13652, 62863, L"PAW3395", L"r1", {{ L"1k", 14139, 4229 }} });
    g_deviceDatabase.push_back({ L"VXE R1 NearLink", currentId++, 14139, 4197, L"PAW3395SE", L"r1", {} });
    g_deviceDatabase.push_back({ L"VXE R1 PRO", currentId++, 13652, 62860, L"PAW3395", L"r1-pro", {{ L"1k", 13652, 62858 }} });
    g_deviceDatabase.push_back({ L"VXE R1 PRO MAX", currentId++, 13652, 62860, L"PAW3395", L"r1-pro-max", {{ L"1k", 13652, 62858 }, { L"4k", 13652, 62861 }} });
    g_deviceDatabase.push_back({ L"VXE R1 UlTRA", currentId++, 14139, 4562, L"PAW3395", L"r1", {} });
    g_deviceDatabase.push_back({ L"VXE R1S+", currentId++, 14139, 4228, L"PAW3311", L"r1", {{ L"1k", 14139, 4229 }} });
    g_deviceDatabase.push_back({ L"VXE R1SE", currentId++, 13652, 62863, L"PAW3395SE", L"r1-se", {{ L"1k", 14139, 4229 }} });
    g_deviceDatabase.push_back({ L"VXE R1SE+", currentId++, 13652, 62863, L"PAW3395SE", L"r1-se+", {{ L"1k", 14139, 4229 }} });
}