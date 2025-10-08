# ATK Battery Monitor

A lightweight, standalone Windows System Tray utility to display the real-time battery level of various ATK, VXE, and Dragonfly wireless mice.

The tray icon is dynamically generated, showing a readable battery percentage with a color-coded background for at-a-glance status.

<p align="center">
  <img src="https://i.imgur.com/mJ6coCv.png" alt="ATK Battery Monitor Tray" width="300">
  <br>
  <em>The tray icon showing % battery.</em>
</p>

<p align="center">
  <img src="https://i.imgur.com/3QgvdQF.png" alt="ATK Battery Monitor Selector" width="350">
  <br>
  <em>The right-click context menu with device and interval selectors.</em>
</p>

---

## Features

-   **Real-time Battery Monitoring:** Periodically queries your device for its current battery level.
-   **Tray Icon:** dynamically drawn number showing the exact percentage, making it easy to read.
-   **Color-Coded Status:** The icon background changes color for instant status recognition:
    -   **Green:** Normal
    -   **Blue:** Fully Charged (>95%)
    -   **Red:** Low Battery (<=20%)
    -   **Grey:** Disconnected
-   **Extensive Device Support:** Includes a database of dozens of ATK, VXE, and related mice.
-   **Device Selector:** Right-click the tray icon to easily switch between different supported devices.
-   **Persistent Selection:** The application remembers your chosen device between sessions using the Windows Registry.
-   **Customizable Polling Interval:** Choose how often the battery is checked (1, 5, 15, or 30 minutes).
-   **Lightweight:** Extremely low CPU and memory usage. It's a native C++ application with no heavy dependencies.
-   **Diagnostics Tool:** Includes a simple utility to log all connected HID devices to a file on your desktop for troubleshooting.

## Supported Devices

The following devices are included in the application's database. While the communication protocol should be similar, only devices marked with ✅ have been confirmed to work by me.

If you are able to test a device marked with ❌, please open an issue to let me know!

| Device Name                        | Tested? |
| ---------------------------------- | :-----: |
| **ATK A9 Plus Nearlink**           |    ✅    |
| ATK A9                             |    ❌    |
| ATK A9 Nearlink                    |    ❌    |
| ATK A9 Pro Max                     |    ❌    |
| ATK A9 SE Nearlink                 |    ❌    |
| ATK A9 Ultra                       |    ❌    |
| ATK A9 Ultimate                    |    ❌    |
| ATK A9 Ultra Max 2.0               |    ❌    |
| ATK Duckbill                       |    ❌    |
| ATK Duckbill Ultra                 |    ❌    |
| ATK F1 EXTREME                     |    ❌    |
| ATK F1 LEVIATAN                    |    ❌    |
| ATK F1 PRO                         |    ❌    |
| ATK F1 PRO MAX                     |    ❌    |
| ATK F1 Ultra                       |    ❌    |
| ATK F1 Ultimate                    |    ❌    |
| ATK F1 Ultimate 2.0                |    ❌    |
| ATK FIERCE X                       |    ❌    |
| ATK U2                             |    ❌    |
| ATK U2 MINI                        |    ❌    |
| ATK U2 Nearlink                    |    ❌    |
| ATK U2 Plus Nearlink               |    ❌    |
| ATK U2 Pro Max                     |    ❌    |
| ATK U2 SE Nearlink                 |    ❌    |
| ATK U2 Ultra                       |    ❌    |
| ATK U2 Ultimate                    |    ❌    |
| ATK V1 PRO MAX                     |    ❌    |
| ATK V1 Ultimate                    |    ❌    |
| ATK V3 Nearlink                    |    ❌    |
| ATK V3 Nearlink Pro                |    ❌    |
| ATK V3 Nearlink Pro Max            |    ❌    |
| ATK X1 EXTREME                     |    ❌    |
| ATK X1 Lite                        |    ❌    |
| ATK X1 PRO                         |    ❌    |
| ATK X1 PRO MAX                     |    ❌    |
| ATK X1 S Nearlink                  |    ❌    |
| ATK X1 SE Nearlink                 |    ❌    |
| ATK X1E                            |    ❌    |
| ATK X1 Ultra                       |    ❌    |
| ATK X1 Ultimate                    |    ❌    |
| ATK X1 Ultimate 2.0                |    ❌    |
| ATK Y9 Nearlink                    |    ❌    |
| ATK Y9 Plus Nearlink               |    ❌    |
| ATK Y9 Pro Max                     |    ❌    |
| ATK Y9 PRO MAX (4349)              |    ❌    |
| ATK Y9 PRO MAX (4350)              |    ❌    |
| ATK Y9 SE Nearlink                 |    ❌    |
| ATK Y9 Ultimate                    |    ❌    |
| ATK Z1 PRO                         |    ❌    |
| ATK Z1 PRO MAX                     |    ❌    |
| ATK Z1 Ultra                       |    ❌    |
| ATK ZERO EX                        |    ❌    |
| DRAGONFLY F1 MOBA                  |    ❌    |
| DRAGONFLY F1 PRO                   |    ❌    |
| DRAGONFLY F1 PRO MAX               |    ❌    |
| DRAGONFLY F1 PRO MAX Elden         |    ❌    |
| DRAGONFLY F1 PRO MAX JOJO          |    ❌    |
| DRAGONFLY F1 V2 Nearlink           |    ❌    |
| DRAGONFLY F1 V2 Plus Nearlink      |    ❌    |
| DRAGONFLY F1 V2 PRO MAX            |    ❌    |
| DRAGONFLY F1 V2 SE Nearlink        |    ❌    |
| Dragonfly F1S                      |    ❌    |
| MAD R                              |    ❌    |
| MAD R +                            |    ❌    |
| MAD R MAJOR                        |    ❌    |
| MAD R MAJOR+                       |    ❌    |
| VXE R1                             |    ❌    |
| VXE R1 NearLink                    |    ❌    |
| VXE R1 PRO                         |    ❌    |
| VXE R1 PRO MAX                     |    ❌    |
| VXE R1 UlTRA                       |    ❌    |
| VXE R1S+                           |    ❌    |
| VXE R1SE                           |    ❌    |
| VXE R1SE+                          |    ❌    |

## How to Use

1.  Go to the [Releases page](https://github.com/ReformedDoge/0x44oge-ATK/releases) on GitHub.
2.  Download the latest `0x44oge ATK.exe` from the Assets section.
3.  Run the executable. No installation is required.
4.  The battery icon will appear in your system tray. Right-click it to select your device and configure settings.

## For Developers (How to Build)

### Prerequisites
-   Visual Studio 2022 (or later) with the "Desktop development with C++" workload.
-   The `hidapi` library.

### Build Steps
1.  Clone the repository: `git clone https://github.com/ReformedDoge/0x44oge-ATK.git`
2.  Place the `hidapi` library files in the appropriate folders:
    -   `hidapi.h` should be placed in the `/include/` directory.
    -   `hidapi.lib` should be placed in the `/lib/` directory.
3.  Open the `0x44oge ATK.sln` file in Visual Studio.
4.  Select the `Release` and `x64` build configuration.
5.  Build the solution (F7 or Build > Build Solution). The executable will be in the `/x64/Release/` folder.

## License

TBD

## Contributing

Contributions, bug reports, and feedback are welcome! Please feel free to open an issue or submit a pull request.

Especially valuable would be confirmation of untested devices. If you can confirm a device on the list works, please let us know by creating an issue!