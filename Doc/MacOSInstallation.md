<a name="top-anchor"/>

| [Contents](../README.md#table-of-contents) | [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |
|---|---|---|

# [ScxmlEditor](../README.md) Mac OS Installation
The only way that was found to use ScxmlEditor on Mac OS is via [Wine](https://www.winehq.org/) and [Wineskin](https://github.com/Gcenx/WineskinServer)

1. Download **ScxmlEditor.zip** from latest Release page and unpack it in any target location

2. **Install [Wine](https://www.winehq.org/):**
```batch
brew tap homebrew/cask-versions
brew install --cask --no-quarantine wine-stable
```

3. **Install [Wineskin](https://github.com/Gcenx/WineskinServer)**
- Install Wineskin Server
```batch
brew install --no-quarantine gcenx/wine/unofficial-wineskin
```
- Start Wineskin
![wineskin_start](../Images/MacOs_wineskin.png)

- Install Wineskin Engine, Wrapper Version
![wineskin_start](../Images/MacOs_wineskin_2.png)

- Create new blank Wineskin wrapper
![wineskin_wrapper](../Images/MacOs_wineskin_wrapper.png)

- Press **Install Software** button after creation was completed
![wineskin_advanced](../Images/MacOs_wineskin_adv.png)

- **Copy** or **Move** ScxmlEditor folder inside
![wineskin_copy](../Images/MacOs_wineskin_copy.png)

- Select ScxmlEditor executable
![wineskin_exe](../Images/MacOs_wineskin_exe.png)

4. Run program
![macos_run](../Images/MacOs_wineskin_run.png)

## Preview
![macos_app_preview](../Images/MacOs_preview.png)

| [TOP](#top-anchor) | [Contents](../README.md#table-of-contents) | [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |
|---|---|---|---|