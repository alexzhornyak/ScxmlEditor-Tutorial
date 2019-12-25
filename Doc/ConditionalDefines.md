# Conditional Project Defines
State Chart elements are able to be excluded from saving by keyword which is defined in project options.

![program_options](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ConditionalDefines_ProgramOptions.png)

## 'ExcludeConditions' attribute
It is a semicolon-separated list of project definitions.
If the **'ExcludeConditions'** value matches the **'ProjectDefines'** value then the element is excluded from save. This attribute supports inversion also.
You may use **'~'** sign for **inversion**.

![ExcludeConditions](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ConditionalDefines_ExcludeConditions.png)

## Example:

**ProjectDefines == DEBUG**

     ExcludeConditions == DEBUG -> Element is excluded
     
     ExcludeConditions == ~DEBUG -> Element is not excluded for 'DEBUG' definition,
         but is excluded for any other project value


**Project definitions will be displayed at the right side of the application's caption.**

![debug_defined](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ConditionalDefines_DebugDefined.png)

**Excluded chart elements will be marked with the corresponding message**

![excluded_shapes](https://github.com/alexzhornyak/ScxmlEditor-Tutorial/blob/master/Images/ConditionalDefines_Excluded.png)
