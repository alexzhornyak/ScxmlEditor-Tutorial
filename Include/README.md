<a name="top-anchor"/>

| [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |
|---|---|

# Qt [SCXML](https://alexzhornyak.github.io/SCXML-tutorial/) External Debugging Monitor
Native QtCreator's scxml designer does not allow user to debug [SCXML statecharts](https://alexzhornyak.github.io/SCXML-tutorial/) and observe statemachine work flow, so we offer to use [ScxmlEditor](../README.md) as an instrument for debugging complex SCXML state charts

## Description
We implemented Qt SCXML monitor in a single header [ScxmlExternMonitor2.h](scxmlexternmonitor2.h) as **UDPScxmlExternMonitor**. It has public a property **`QScxmlStateMachine *scxmlStateMachine`**. While **scxmlStateMachine** is not assigned it does nothing and you may leave it even in a release versions of your applications. When you need to observe statemachine work flow, just assign a valid **QScxmlStateMachine pointer** to the property, and monitor will start send UDP debug packages to the ScxmlEditor. And you will be able to observe when state is entered and when is exited, etc.

![example_debug](https://raw.githubusercontent.com/alexzhornyak/SCXML-tutorial/master/Images/StopWatchScxml.gif)

## 1. Usage in QML
### 1.1. Option 1. From inherited state machine object
![opt1](../Images/ExternMonitor_QML1.png)

### 1.2. Option 2. From QML object
#### Monitor registration
![opt2](../Images/ExternMonitor_QML2.png)
#### Usage in QML
![opt2_2](../Images/ExternMonitor_QML2_2.png)

## 2. Usage in C++ Qt Widgets
![cppOpt](../Images/ExternMonitor_CPP.png)

| [TOP](#top-anchor) | [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |
|---|---|---|
