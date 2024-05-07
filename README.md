# USC_HUB

----

## Installation
1. When cloning be sure to also pull all submodules. `git submodule update --init --recursive`
2. To build the sketch I recommend ditching Arduino IDE and using some other editor. Im personally using Visual Studio Code with the [Arduino](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) extension.
  - If using the VS Code extension above, be sure to press F1 => Search for "Arduino" => Select relevent options to configure your board config, board type, serial port, ect.
  - This will create a new file at `.vscode/arduino.json` you can easily edit as needed later
3. This Sketch utilizes the [KerbalSimpitRevamped-Arduino](https://github.com/Simpit-team/KerbalSimpitRevamped-Arduino) library. You can install it using either  or 
  - The Arduino Library Manager to get the official release
  - Running [scripts/setup.bat](scripts/setup.bat) to create an Arduino library symlink with the source code directly. This will allow you to edit, debug, and commit the Simpit code within your IDE easily.

