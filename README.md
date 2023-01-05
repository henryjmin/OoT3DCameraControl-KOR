Free camera control hack for The Legend of Zelda: Ocarina of Time 3D.

This tool scans the memory of Citra to find and modify the addresses corresponding to the LocalPlayer and LocalCamera.

The project can be compiled with Visual Studio.

It supports any controller supported by SDL:
- The right stick moves the free camera, toggling it on if it's off.
- The L button resets the angle of the camera, and toggles off the free camera.

Open citra-qt.exe and start the game, run the CameraOoT.exe once you can control Link.

TODO: Find the memory addresses for Epona's coordinates (?), and allow free camera control when riding Epona.
TODO: Add a config file to change the inverted cameras, sensitivity and reset camera button.
TODO: Check whether the jitter of the free camera can be further mitigated.
