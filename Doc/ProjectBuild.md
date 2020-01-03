# Building a project

Complex state machines may consists of many invoked state machines inside and nested data or script sources as separate files.
Such projects may be inconvinient during the deployment because you should collect all files and check that all links to external files are valid.
So we offer the system of building all external files to one source.

## Setting build configuration
Select menu **Tools->Project Options**

![project_options](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ProjectBuild_ProjectOptions.png)

### Build types

![build_params](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ProjectBuild_BuildTypes.png)

**pbtSaveAll** - saves all modifications before build

**pbtSaveFlat** - saves all nested state machines and data sources into single file

**pbtPostBuild** - executes post-build script

### Post-build script
You are able to define CMD script wich will be executed after every build operation if the flag **pbtPostBuild** is set.

![post_build](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ProjectBuild_PostBuild.png)

Double click on **ProjectPostBuild** item and script editor will be displayed.

![script_editor](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ProjectBuild_PostBuildScript.png)

### Execution of project building
Press **Ctrl+B** or select menu **File->Build**

![file_build](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ProjectBuild_FileBuild.png)

### CMD output
You may review script execution summary in **CMD Output** tab

![cmd_output](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ProjectBuild_PostBuildCMDOutput.png)
