<a name="top-anchor"/>

| [Contents](../README.md#table-of-contents) | [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |
|---|---|---|

# Transitions (Connections)
[Transitions](https://alexzhornyak.github.io/SCXML-tutorial/Doc/transition.html) define how a [state](https://alexzhornyak.github.io/SCXML-tutorial/Doc/state.html) reacts to events that are generated either by the state machine or external entities. When events occur, the state machine checks for a matching transition defined in the active state and moves to its target state.

## Transitions between states
### Creation
#### 1. Press 'Connect button'
![tr_connect_btn](../Images/Transition_Connect.gif)

#### 2. Press 'Ctrl+Right Mouse Button'
![tr_connect_right](../Images/Transition_ConnectCtrlRight.gif)

### Visual types
* **Lines**

![tr_lines](../Images/Transition_Lines.gif)

* **Square (IN: Top|Bottom)**

![tr_top](../Images/Transition_SquareTop.gif)

* **Square (IN: Left|Right)**

![tr_left](../Images/Transition_SquareLeft.gif)

* **Curve**

![tr_curve](../Images/Transition_Curve.gif)

* **Connection over Left | Top | Right | Bottom**

![tr_over](../Images/Transitions_ConnectionOver.gif)

## Self Transitions (Connections)
A self transition is a transition that starts and ends in the same state.
### How to create:
* select state and press **'Make Self-connection'** button in **'Tools (Bookmarks)'** panel
* select state, right-click to call context menu and hit menu item **'Make Self-connection'**

![Transitions_Self](../Images/Transitions_Self.gif)

### Arrange Self Transitions (Connections)
Multiple self transitions may be quickly arranged by pressing **'Arrange Self-connections'** menu item. Primary left and right coordinates for transitions are defined by **the first transition**

![Transitions_Self_Arrange](../Images/Transitions_Self_Arrange.gif)

### Self Transitions Inside States
It is available since ScxmlEditor 2.4 to arrange self transitions inside of state.

![tr_inside](../Images/Transition_Self_Inside.png)

1. Set option **`SelfConnectionInside`** to **`true`** in the property inspector
2. Execute **`Arrange Self-Connections`** command from IDE Insight or right popup menu
3. (Optionally) Align Self-Transition to left, top, right or bottom side of the state

![tr_align](../Images/Transition_Self_Align.gif)

## Delayed transition
A delayed transition is a transition that happens after a period of time, specifically being in a specific state for a certain amount of time.

Delayed transitions are not part of [SCXML]((https://alexzhornyak.github.io/SCXML-tutorial/)).  Instead, you have to specify to send a delayed event, and a normal transition that handles that event. The delayed event must be given a name, and also a unique identifier (in order to cancel the delayed event).

![Transitions_Delayed](../Images/Transitions_Delayed.gif)

## Copy-Paste Transitions

1. Select a transition that you want to be copied

![tr_copy](../Images/Transitions_Copy_Paste_1.png)

2. Press **'Ctrl+C'**

3. Select two states that would be a source and a target of the transition

![tr_sel](../Images/Transition_Copy_Src_Trg.png)

4. Press **'Ctrl+V'**

![tr_paste](../Images/Transitions_Paste.png)

5. Adjust transition layout if necessary


| [TOP](#top-anchor) | [Contents](../README.md#table-of-contents) | [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |
|---|---|---|---|
