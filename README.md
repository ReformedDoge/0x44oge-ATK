# ATK Battery Monitor

A lightweight, native C++ Windows utility to monitor the real-time battery levels of ATK, VXE, and Dragonfly wireless mice directly from your system tray.

<table border="0">
  <tr>
    <td width="60%" valign="top">

### Features
* **Auto-Detect:** One-click hardware identification.
* **Dynamic Tray Icon:** High-contrast icon showing exact percentage.
* **Color-Coded Status:** Instant status recognition:
    * **Green:** Normal
    * **Blue:** Fully Charged (>95%)
    * **Red:** Low Battery (<=20%)
    * **Grey:** Disconnected / Sleeping
* **Full Persistence:** Remembers settings between sessions.
* **Start with Windows:** Add to Start-up toggle.
* **Diagnostics:** Detailed "Flight Recorder" logs.

<hr />

### How to Use
1. Download `0x44oge ATK.exe` from [Releases](https://github.com/ReformedDoge/0x44oge-ATK/releases).
2. Run the executable (no installation required).
3. Right-click the icon and select **Auto-Detect Device**.
4. Choose your refresh interval; settings save automatically.

    </td>
    <td width="40%" valign="top" align="center">
      <img src="https://i.imgur.com/GB03xKf.png" width="350">
      <br><br>
      <img src="https://i.imgur.com/mJ6coCv.png" width="300">
      <br>
      <i>Dynamic tray icon and context menu.</i>
    </td>
  </tr>
</table>

## Supported Devices

The database includes the following devices. While the protocol is standard across the lineup, only devices marked with ✅ have been confirmed to work by the developer.

| Device Name | Tested? |
| :--- | :---: |
| **ATK A9 Plus (with 1K dongle)** | ✅ |
| **VXE MAD R MAJOR +** (with 8K dongle) | ✅ |
| **VXE MAD R MAJOR** (with 8K dongle) | ✅ |
| **VGN R1 PRO MAX** (with 8K dongle) | 🟡 |
| VXE MAD R | ❌ |
| VXEMAD R + | ❌ |
| ATK A9 | ❌ |
| ATK A9 Nearlink | ❌ |
| ATK A9 Pro Max | ❌ |
| ATK A9 SE Nearlink | ❌ |
| ATK A9 Ultra | ❌ |
| ATK A9 Ultimate | ❌ |
| ATK A9 Ultra Max 2.0 | ❌ |
| ATK Duckbill | ❌ |
| ATK Duckbill Ultra | ❌ |
| ATK F1 EXTREME | ❌ |
| ATK F1 LEVIATAN | ❌ |
| ATK F1 PRO | ❌ |
| ATK F1 PRO MAX | ❌ |
| ATK F1 Ultra | ❌ |
| ATK F1 Ultimate | ❌ |
| ATK F1 Ultimate 2.0 | ❌ |
| ATK FIERCE X | ❌ |
| ATK U2 | ❌ |
| ATK U2 MINI | ❌ |
| ATK U2 Nearlink | ❌ |
| ATK U2 Plus Nearlink | ❌ |
| ATK U2 Pro Max | ❌ |
| ATK U2 SE Nearlink | ❌ |
| ATK U2 Ultra | ❌ |
| ATK U2 Ultimate | ❌ |
| ATK V1 PRO MAX | ❌ |
| ATK V1 Ultimate | ❌ |
| ATK V3 Nearlink | ❌ |
| ATK V3 Nearlink Pro | ❌ |
| ATK V3 Nearlink Pro Max | ❌ |
| ATK X1 EXTREME | ❌ |
| ATK X1 Lite | ❌ |
| ATK X1 PRO | ❌ |
| ATK X1 PRO MAX | ❌ |
| ATK X1 S Nearlink | ❌ |
| ATK X1 SE Nearlink | ❌ |
| ATK X1E | ❌ |
| ATK X1 Ultra | ❌ |
| ATK X1 Ultimate | ❌ |
| ATK X1 Ultimate 2.0 | ❌ |
| ATK Y9 Nearlink | ❌ |
| ATK Y9 Plus Nearlink | ❌ |
| ATK Y9 Pro Max | ❌ |
| ATK Y9 PRO MAX (4349) | ❌ |
| ATK Y9 PRO MAX (4350) | ❌ |
| ATK Y9 SE Nearlink | ❌ |
| ATK Y9 Ultimate | ❌ |
| ATK Z1 PRO | ❌ |
| ATK Z1 PRO MAX | ❌ |
| ATK Z1 Ultra | ❌ |
| ATK ZERO EX | ❌ |
| DRAGONFLY F1 MOBA | ❌ |
| DRAGONFLY F1 PRO | ❌ |
| DRAGONFLY F1 PRO MAX | ❌ |
| DRAGONFLY F1 PRO MAX Elden | ❌ |
| DRAGONFLY F1 PRO MAX JOJO | ❌ |
| DRAGONFLY F1 V2 Nearlink | ❌ |
| DRAGONFLY F1 V2 Plus Nearlink | ❌ |
| DRAGONFLY F1 V2 PRO MAX | ❌ |
| DRAGONFLY F1 V2 SE Nearlink | ❌ |
| Dragonfly F1S | ❌ |
| VXE R1 | ❌ |
| VXE R1 NearLink | ❌ |
| VXE R1 PRO | ❌ |
| VXE R1 PRO MAX | ❌ |
| VXE R1 UlTRA | ❌ |
| VXE R1S+ | ❌ |
| VXE R1SE | ❌ |
| VXE R1SE+ | ❌ |

---

## For Developers (How to Build)

- **Environment:** Visual Studio 2022 (v143).
- **Linking:** Use **Multi-threaded (/MT)** for a standalone, portable executable.
- **Dependencies:** Place the `hidapi` library files in the appropriate folders:
    -   `hidapi.h` should be placed in the `/include/` directory.
    -   `hidapi.lib` should be placed in the `/lib/` directory.
## Contributing

Hardware confirmations are highly appreciated. If your mouse is marked with ❌, please run **Log Diagnostics** and share the report in a GitHub issue to help improve the database.

## License
TBD