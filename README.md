# ThemeSwitcher Sysmodule

**This is experimental and hasn't been throughly tested, use it at your own risk. There's a sd corruption risk for exfat users if you shut down your console while the theme is being switched (10 seconds after boot).**

This sysmodule automatically changes the theme of the console at every boot.
Memory usage shouldn't be an issue as it closes as soon as the copy is over, but it may crash if you have too many modules at the same time.

## Usage
### Manual setup:
Add the kip sysmodule to your cfw, then create a folder called `shuffle` in the `themes` folder in your sd. \
Inside of it create a folder for each set of themes you want to be applied at boot. **For now only SZS files work with this** \
With "set" i mean multiple files that togheter make a full theme like `ResidentMenu.szs`, `Entrance.szs` and so on, they will all be applied at once if they're in the same folder. **Theme compatibility is not checked** \
Sets are cycled alphabetically from the first to the last one, you can reset the order by deleting the index.db file that will automatically be created. You can also set random shuffle with the NXThemes Installer app.
### NXThemes Installer:
Keep R pressed when installing themes so they will be installed in a shuffle set.

Memory usage is kept small so it works with LayeredFS but having other sysmodules at the same time may crash.
