<a name="top-anchor"/>

| [Contents](../README.md#table-of-contents) | [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |

# DOT-based Auto-Layout

Since version **2.1.0.1347** auto-layout option is based on Graphviz DOT render.

### Requirements
1. [Visual C++ Redistributable for Visual Studio 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145) must be installed

2. **ScxmlEditor\Graphviz** core must be present

## Auto-layout is applied when:
**1.** **The SCXML document without shapes and connections metainformation** is opened

**2.** User calls **'Auto Arrange Layout'** option.

## How to automatically arrange layout
**1.** Press right mouse button somewhere on the chart and select menu 'Auto Arrange Layout'

![AutoLayout_MenuArrange](../Images/AutoLayout_MenuArrange.png)

**2.** Or press "**Ctrl + .**" shorcut and type "Auto" in **IDE Insight** focus search dialog

![AutoLayout_IDEInsightArrange](../Images/AutoLayout_IDEInsightArrange.png)

**3.** Select **Orthographic splines** option to have square-like connections (default)

![AutoLayout_OrthoSplines](../Images/AutoLayout_OrthoSplines.png)

**4.** Select **Polyline splines** option to draw connections as polylines

![AutoLayout_PolyLineSplines](../Images/AutoLayout_PolyLineSplines.png)

## How to convert SCXML to DOT
#### 1. Search in IDE Insight (Ctrl+.) "DOT" or select menu "File->Export->Export as graph DOT"
![AutoLayout_ExportAsDot](../Images/AutoLayout_ExportAsDot.png)

#### 2. Save as Graphviz (\*.gl) file
![ExportAsDot_TimeGenerator](../Images/ExportAsDot_TimeGenerator.png)

```dot
digraph {
	compound=true;
	graph [splines=ortho, fontname="Arial"];
	node [ fontname="Arial" ];
	edge [ fontname="Arial" ];
	subgraph cluster_ScxmlShape1__93A9A677 {
		label=<ScxmlTimeGenerator>;
		StateShape1__9E02659C [shape=rectangle label=<Off>];
		subgraph cluster_StateShape2__9F1CE7D7 {
			label=<Generator>;
			subgraph cluster_StateShape3__AB6D10DC {
				label=<<FONT color="#800000"><b>&lt;---- Do.Timer ----&gt;</b></FONT><br/>
						<FONT color="#800000"> Every ~1000ms </FONT><br/><br/>
						StateShape1>;
				StateShape3__AB6D10DC_children [shape=rectangle fontcolor=blue label=<onentry<br/>
						log {INFO:string.format(&quot;Elapsed:%.2fs&quot;, os.clock() - tm_ELAPSED)}<br/>
						send {Do.Timer[1000ms][ID_TIMER]}<br/>>];
			}

		StateShape2__9F1CE7D7_children [shape=rectangle fontcolor=blue label=<onentry<br/>
			assign {tm_ELAPSED=os.clock()}<br/>
			onexit<br/>
			cancel {ID_TIMER}<br/>>];
		}

		ScxmlShape1__93A9A677_children [shape=rectangle fontcolor=blue label=<datamodel<br/>data {tm_ELAPSED=0}<br/>>];
	}

	StateShape1__9E02659C->StateShape2__9F1CE7D7_children[tooltip="StateMachineConnection1__A3834CB0",color="teal",lhead="cluster_StateShape2__9F1CE7D7",label=<<FONT color="#008080"><b>Start</b></FONT><br/>>]
	StateShape2__9F1CE7D7_children->StateShape1__9E02659C[tooltip="StateMachineConnection2__A61AC440",color="teal",ltail="cluster_StateShape2__9F1CE7D7",label=<<FONT color="#008080"><b>Stop</b></FONT><br/>>]
}
```

## Special SCXML to DOT convertation features

### 1. Graphviz cluster can not be connected to itself so self-transitions are encoded in cluster label
![ExportAsDot_TimeGenerator_SelfTransitions](../Images/ExportAsDot_TimeGenerator_SelfTransitions.png)

### 2. Executable content is represented in single node without edges
![ExportAsDot_TimeGenerator_ExecContent](../Images/ExportAsDot_TimeGenerator_ExecContent.png)

# Examples

## Example 1. Autolayouting [Morse Code Trainer](https://github.com/alexzhornyak/SCXML-tutorial/tree/master/Examples/Qt/Morse) source code SCXML
![morse_code_trainer](https://raw.githubusercontent.com/alexzhornyak/SCXML-tutorial/master/Images/morseNull.png)

## Example 2. Autolayoting LCCA tests from [USCXML Benchmarks Tests](https://github.com/tklab-tud/uscxml/blob/master/docs/BENCHMARKS.md)
![LCCA4](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/LCCA.4.png)

![LCCA16](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/LCCA.16.png)

| [TOP](#top-anchor) | [Contents](../README.md#table-of-contents) | [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |
