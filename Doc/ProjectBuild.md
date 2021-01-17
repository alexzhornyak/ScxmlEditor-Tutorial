<a name="top-anchor"/>

| [Contents](../README.md#table-of-contents) | [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |

# Building a project

Complex state machines may consists of many invoked state machines inside and nested data or script sources as separate files.
Such projects may be inconvinient during the deployment because you should collect all files and check that all links to external files are valid.
So we offer the system of building all external files to one source.

## Setting build configuration
Select menu **Tools->Project Options**

![project_options](../Images/ProjectBuild_ProjectOptions.png)

### Build types

![build_params](../Images/ProjectBuild_BuildTypes.png)

**pbtSaveAll** - saves all modifications before build

**pbtSaveFlat** - saves all nested state machines and data sources into single file

**pbtPostBuild** - executes post-build script

### Post-build script
You are able to define CMD script wich will be executed after every build operation if the flag **pbtPostBuild** is set.

![post_build](../Images/ProjectBuild_PostBuild.png)

Double click on **ProjectPostBuild** item and script editor will be displayed.

![script_editor](../Images/ProjectBuild_PostBuildScript.png)

### Execution of project building
Press **Ctrl+B** or select menu **File->Build**

![file_build](../Images/ProjectBuild_FileBuild.png)

### CMD output
You may review script execution summary in **CMD Output** tab

![cmd_output](../Images/ProjectBuild_PostBuildCMDOutput.png)

## Example

### Source files for build:

**state_machine_project_build.scxml**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<scxml datamodel="lua" name="ScxmlShape1" version="1.0" xmlns="http://www.w3.org/2005/07/scxml">
	<datamodel>
		<data id="VarTable" src="dataVarTable.lua"/>
	</datamodel>
	<state id="StateShape1">
		<onentry>
			<foreach array="VarTable" index="index" item="item">
				<log expr="string.format('[%s] %s', index, item)" label="INFO"/>
			</foreach>
		</onentry>
		<transition event="Step" target="StateShape2"/>
	</state>
	<state id="StateShape2">
		<invoke autoforward="true" id="ID_CHILD" src="include_state_machine.scxml"/>
		<transition event="done.invoke.ID_CHILD" target="FinalShape1"/>
	</state>
	<final id="FinalShape1"/>
</scxml>
```
**include_state_machine.scxml**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<scxml name="ScxmlChild" version="1.0" xmlns="http://www.w3.org/2005/07/scxml">
	<state id="StateChild">
		<transition event="Step" target="FinalChild"/>
	</state>
	<final id="FinalChild"/>
</scxml>
```
**dataVarTable.lua**
```
{
    "Item 1",
    "Item 2",
    "Item 3"    
}
```

### Build Output
**state_machine_project_build.flat.scxml**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<scxml datamodel="lua" name="ScxmlShape1" version="1.0" xmlns="http://www.w3.org/2005/07/scxml">
	<datamodel>
		<data id="VarTable">{
    'Item 1',
    'Item 2',
    'Item 3'    
}
		</data>
	</datamodel>
	<state id="StateShape1">
		<onentry>
			<foreach array="VarTable" index="index" item="item">
				<log expr="string.format('[%s] %s', index, item)" label="INFO"/>
			</foreach>
		</onentry>
		<transition event="Step" target="StateShape2"/>
	</state>
	<state id="StateShape2">
		<invoke autoforward="true" id="ID_CHILD">
			<content>
				<scxml name="ScxmlChild" version="1.0" xmlns="http://www.w3.org/2005/07/scxml">
					<state id="StateChild">
						<transition event="Step" target="FinalChild"/>
					</state>
					<final id="FinalChild"/>
				</scxml>
			</content>
		</invoke>
		<transition event="done.invoke.ID_CHILD" target="FinalShape1"/>
	</state>
	<final id="FinalShape1"/>
</scxml>
```

| [TOP](#top-anchor) | [Contents](../README.md#table-of-contents) | [SCXML Wiki](https://alexzhornyak.github.io/SCXML-tutorial/) | [Forum](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/discussions) |