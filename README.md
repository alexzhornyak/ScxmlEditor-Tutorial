**[Video overview](https://youtu.be/tL-NICRwggY)**

# Scxml Editor 2.0
Powerful tool for creating, editing and debugging SCXML charts.

**New:** Inheritance, visual chart splitting, conditional defines, GIF maker etc.

## System requirments
Windows 7, 8, 10

! **vcredist_x86.exe** must be installed before starting application !

You can find portable version **[here](https://www.dropbox.com/sh/fjzm9ejdrtra1c0/AAB_ASgIPRFLX57x7rWPEv3Ta?dl=0)**

[Zip Link](https://www.dropbox.com/s/1sx8p8o1e4t55hj/ScxmlEditor.zip?dl=0)

Latest version: **2.0.0.1136**

![2018-09-24 16 23 50](https://user-images.githubusercontent.com/18611095/45954643-3e30d000-c016-11e8-8f79-205f770af667.png)

## Table of contents
[1. Inheritance](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Doc/Inheritance.md)

## Creating SCXML charts

### Creating a unit
**Unit** is a single scxml chart. It is mostly used for simple charts without invokable sessions.

Select **File**->**New Unit** or press **Shift+Ctrl+N**

### Creating a project
**Project** is the most preferred choice of designing scxml charts to get all benefits (custom triggers, nested scxml charts, etc.)

Select **File**->**New Project** or press **Ctrl+N**

## Editing SCXML charts
**ScxmlEditor** provides a project wizard for opening scxml. It supports either opening files with metainformation about the position of states and transitions or plain scxml files. You can use **ScxmlEditor** to add states and transitions to the chart. You can add executable content to a state chart to enable the state machine to modify its data model and to interact with external entities.

1. Select **File**->**Open Unit** or press **Shift+Ctrl+O** (for all types of .scxml files)
2. Select **File**->**Open Project** or press **Ctrl+O** (for ScxmlEditor .sproj files)

### Property inspector
Is used to edit and examine the properties of the currently selected states, executable contents and transitions.

![2018-09-26 09 50 33](https://user-images.githubusercontent.com/18611095/46062582-eeb3e680-c172-11e8-993b-cedbc270894d.png)

### Editor tabs
Are used to add scxml elements, zoom, align, change chart visual options, etc.

![2018-09-26 09 50 33](https://user-images.githubusercontent.com/18611095/46062784-8c0f1a80-c173-11e8-8fca-8937b9b0b721.png)

## Debugging
**ScxmlEditor** has an ability to listen UDP commands (AfterEnter, BeforeEnter, AfterExit, BeforeExit, Step, BeforeExecContent, AfterExecContent, BeforeInvoke, AfterInvoke, BeforeUnInvoke, AfterUnInvoke, BeforeTakingTransition, AfterTakingTransition, StableConfiguration, BeforeProcessingEvent). **Enter** and **Exit** graphically highlight the corresponding states. You can also trace the execution of the chart and use breakpoints.

### Local debugging

1. Based on [USCXML framework](https://github.com/tklab-tud/uscxml).
Supports **null, lua** datamodels

2. Based on [Qt SCXML framework](https://doc.qt.io/qt-5.9/qtscxml-index.html)
Supports **null, ecmascript** datamodels

### External debugging
#### Example of debugging Qt Calculator-QML project
Add the next [source code](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Include/scxmlexternmonitor.h) to **calculator-qml.cpp**
```
#include "scxmlexternmonitor.h"

g_ScxmlStateMachineName = "CalculatorStateMachine";
qInstallMessageHandler(myMessageOutput);
QLoggingCategory::setFilterRules("qt.scxml.statemachine=true");
```
![2018-09-26 10 47 00](https://user-images.githubusercontent.com/18611095/46064985-9207fa00-c179-11e8-99e6-e8a8d1035ce8.png)

1. Select **Run**->**Start listening from external application**
2. Select **Run**->**Run**
3. Launch **Calculator-QML.exe**
4. Use **Pause** for tracing (optionally)

![2018-09-26 10 53 10](https://user-images.githubusercontent.com/18611095/46065349-87019980-c17a-11e8-979b-1ec93351e84e.png)
