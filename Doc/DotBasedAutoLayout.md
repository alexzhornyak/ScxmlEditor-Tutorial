# DOT-based Auto-Layout

Since version **2.1.0.1347** auto-layout option is based on Graphviz DOT render.

## Requirements
1. [Visual C++ Redistributable for Visual Studio 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145) must be installed
2. **ScxmlEditor\Graphviz** core must be present

## Auto-layout is applied when:
**1.** **The SCXML document without shapes and connections metainformation** is opened 
**2.** User calls **'Auto Arrange Layout'** option.

## How to automatically arrange layout
**1.** Press right mouse button somewhere on the chart and select menu 'Auto Arrange Layout'

![AutoLayout_MenuArrange](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/AutoLayout_MenuArrange.png)

**2.** Or press "**Ctrl + .**" shorcut and type "Auto" in **IDE Insight** focus search dialog

![AutoLayout_IDEInsightArrange](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/AutoLayout_IDEInsightArrange.png)

**3.** Select **Orthographic splines** option to have square-like connections (default)

![AutoLayout_OrthoSplines](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/AutoLayout_OrthoSplines.png)

**4.** Select **Polyline splines** option to draw connections as polylines

![AutoLayout_PolyLineSplines](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/AutoLayout_PolyLineSplines.png)

