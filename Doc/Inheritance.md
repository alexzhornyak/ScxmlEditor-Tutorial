# [Inheritance](https://github.com/alexzhornyak/SCXML-tutorial/blob/master/Doc/Inheritance_SCXML.md)

An option to inherit from base chart.

![example](../Images/Inheritance_TV_example.gif)

### 1. Create empty project
![empty_proj](../Images/Inheritance_empty_proj.png)

### 2. Add base chart
![add_base](../Images/Inheritance_add_base.png)

### 3. Add new chart which is planned to be derived
![add_derived](../Images/Inheritance_add_derived.png)

### 4. Select base chart for setting inheritance
![select_base_chart](../Images/Inheritance_select_base.png)

### 5. Confirm adding inheritance
![confirm](../Images/Inheritance_confirm.png)

If operation is successful you will see all states marked with **yellow circles**. That means **full matching** to base chart

![inheritance_look](../Images/Inheritance_inherited_look.png)

### 6. Give unique state machine name and save the unit with the corresponding name
![new_name](../Images/Inheritance_new_state_machine_name.png)

### 7. Set root for run and build if your new unit will be the main in the project
![root](../Images/Inheritance_root_for_run.png)

## Description
![desc](../Images/Inheritance_match.png)

#### Red circle - means that there is a difference with the base state chart. You can see the log files for details
![log](../Images/Inheritance_log.png)

#### Yellow circle - means that there is no difference with the base state chart

#### New states - will be marked without additional signs

## How to resolve mismatches
Since version 2.1 there is an option to mark mismatches as resolved
![resolve_menu](../Images/Inheritance_resolve.png)

You may review resolved values in property **InheritanceResolver**. 

![resolved](../Images/Inheritance_resolved_circle.png)

#### Elements with resolved properties will be marked with orange circle.
