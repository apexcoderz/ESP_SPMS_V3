========================================================================================
                                      CHANGELOG
========================================================================================
12/28/2025 12:20 > apexCoderz

refactor: improve device control logic and UI layout
- Reposition manual control panel to the bottom of the dashboard.
- Assign unique IDs to manual control elements for better DOM manipulation.
- Implement JavaScript logic to toggle visibility between Auto and Manual modes.
-> file modified: 'spms.web_content.cpp' 
- Synchronize relay state: force disable manual relay when Auto mode is enabled.
- Refactor conditional statements using shorthand logic for better performance.
-> file modified: 'spms.system_manager.cpp'
- Add a standalone HTML test page for UI validation.
-> file added: 'test_page.html'

========================================================================================
12/28/2025 17:05 > apexCoderz

Fix: Auto/Manual toggle unstable behavior
- Persist autoMode in non-volatile preferences so device restarts preserve user choice. 
-> files modified: 'examples/ESP_SPMS_V3/ESP_SPMS_V3.ino', 'src/spms.network_manager.cpp'
- Add server-side logging and JSON response for '/api/automode' for easier debugging and authoritative confirmation. 
->file modified: 'src/spms.network_manager.cpp'
- Include 'autoMode' in SSE 'sensor-data' payload and send immediate SSE update when mode changes so UI receives real-time authoritative state. 
-> files modified: 'src/spms.sensor_module.cpp', 'src/spms.network_manager.cpp'
- Client-side: implement optimistic UI when toggling Auto, add a short debounce window to avoid fetch polling overwrites, and handle failures gracefully. 
-> file modified: 'src/spms.web_content.cpp'
- Add Serial debug print on startup showing loaded Auto mode. 
-> file modified: 'examples/ESP_SPMS_V3/ESP_SPMS_V3.ino'

///////////////////////////////////////////////////////////////////////////////////////