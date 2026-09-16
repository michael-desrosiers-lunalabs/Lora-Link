This serves as a demonstration for telemetry capability for Acuity LS devices.

Development log is here: [Sharepoint](https://lunalabsusa.sharepoint.us/:w:/s/CorrosionTechProducts/EUCmXF1qEjxLudqR_ERlS5EBuUnKulAu6vkKNQjhv869cQ?e=sKWjwZ)


The goals are:
* Establish communications with a radio module over AT commands
* Establish connection with cellular network
* Demonstrate transmission of data over HTTP interfaces
* Demonstrate data collection from Acuity LS nodes
* Demonstrate sending that data to a C-DAT endpoint
* Demonstrate control / sleeping of radio node for power saving
* Demonstrate auto-wakeup on a controller interval

Primary directories are:
* `.vscode` - helpers for interacting with Visual Studio code. You may need to rework some the paths in here, but it will let you activate "Tasks" to build & flash directly in VSCode with CTRL + SHIFT + B
* `boards` - this is a demonstration of how to define an overlay for a board. I'm building off of an already-existing blackpill board, so a real overlay would be more complicated. But it's a good start.
* `dts\bindings` - this shows how to set up a new binding type for a device tree. This is a little goofy, but it lets you clearly define how you're using an interface. I really only have two in here: GPIOs for the modem, and a GPIO to interace with the `DE` pin on an RS485 chip.
* `scripts` - these are helper scripts to work with `west`, the main application for Zephyr. These let you flash, build (both pristine and auto).
* `src` - the actual code.

@TODO - to be described:
* what is `CMakeLists.txt` and how do we use it?
* what is `prj.conf` and how do we use it?
* how do I install Zephyr on my system?
* how do I configure my system to work with Zephyr (e.g., environment variables)