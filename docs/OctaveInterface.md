---
layout: docs
title: Octave Interface
permalink: /docs/octaveinterface/
published: true
---

ResInsight provides a flexible interface to [Octave](http://www.gnu.org/software/octave/ "Octave").
This includes a set of Octave functions that communicates with a running ResInsight session, features in ResInsight that makes it easy to manage and edit Octave scripts, and their execution using Octave.  

The Octave functions are documented in [Octave Interface Reference]({{ site.baseurl }}/docs/octaveinterfacereference).

## Script management 
Octave scripts are available in the **Scripts** folder in the **Project Tree**. 

![]({{ site.baseurl }}/images/OctaveScriptTree.png)

This folder contains an entry for each of the directories you have added as a **Script Folder**. Each of the folder lists available `*.m` files and sub directories.

### Adding Script Folders
You can add directories by right clicking the **Scripts** item to access the context menu.

Multiple standard script folder locations can also be defined in the field **Shared Script Folder(s)** in the **Preferences Dialog** (**Edit -> Preferences**). 

### Editing scripts 
To enable script editing from ResInsight you need to set up the path to a text editor in the **Script Editor** field in the **Preferences Dialog** (**Edit -> Preferences**) 

When done, scripts can be edited using the context menu command **Edit** on the script item in the tree.

If you add a script file directly by creating a new file, the new script can be made visible in the user interface by activating **Refresh** in the context menu of a script folder. 

## Script execution
Octave scripts can be executed with or without a selection of cases as context. The [Octave Interface Reference]({{ site.baseurl }}/docs/octaveinterfacereference) highlights in more depth how to design your Octave scripts to utilize these features.

### Without a case selection 
A script can be started by navigating to the script in the **Project Tree**, and selecting **Execute** from the context menu. The currently active case (The one with the active 3D View) will then be set as ResInsight's *Current Case*. 

### With a case selection
One script can be executed on many cases by first selecting a set of cases, and then activating **Execute script** from the context menu for the case selection. The script is then executed once pr selected case. Each time ResInsight's *Current Case* is updated, making it accessible from the Octave script. 

![]({{ site.baseurl }}/images/ExecuteOctaveScriptOnSelectedCases.png)

## Script Examples

Here are some example-scripts that illustrates the use of the octave interface. 

<div class="note info"> 
<b>Disclaimer:</b> The scripts are provided as illustration only. They are probably not the best way of using the octave script language, and could potentially be incorrect. 
</div>

#### Example 1

```matlab 
# Calculate change of oil saturation from timestep to timestep
SOIL = riGetActiveCellProperty("SOIL");
SOILDIFF = SOIL;

i = 0;
for timestep = SOIL
	if (i > 0) 
		SOILDIFF(:,i) = timestep - SOIL(:,i);
	endif
	i++;
endfor
SOILDIFF(:,i) = 0;

riSetActiveCellProperty(SOILDIFF, "SOILDIFF");
```

#### Example 2

```matlab    
# Set all values to "Undefined" exept k-layers from 17 to 20
CInfo = riGetActiveCellInfo();
SOIL = riGetActiveCellProperty("SOIL");

Mask = (CInfo(:,4) < 17) | (CInfo(:,4) > 20)
LGRSOIL = SOIL;
i = 0;
for i = (1:columns(LGRSOIL))
    LGRSOIL(Mask,i) = nan;
endfor

riSetActiveCellProperty(LGRSOIL, "KSlice");
```

#### Example 3

```matlab    
# Keep the values in the first LGR only

CInfo = riGetActiveCellInfo();
SOIL = riGetActiveCellProperty("SOIL");

Mask = (CInfo(:,1) != 1);

LGRSOIL = SOIL;
i = 0;
for i = (1:columns(LGRSOIL))
    LGRSOIL(Mask,i) = nan;
endfor

riSetActiveCellProperty(LGRSOIL, "LGRSOIL");
```  

#### Example 4 ( Might be slow on big models)

```matlab 
# Calculate the average SOIL value across K layers

CInfo = riGetActiveCellInfo();
SOIL = riGetActiveCellProperty("SOIL");

SOIL_KAverage = SOIL;
SOIL_KAverage(:) = nan;

mini = min(CInfo(:,2))
maxi = max(CInfo(:,2))

minj = min(CInfo(:,3))
maxj = max(CInfo(:,3))

for i = mini:maxi
  for j = minj:maxj 
    Mask = (CInfo(:,1) == 0) & (CInfo(:,2) == i) & (CInfo(:,3) == j) ;

    for ts = (1:columns(SOIL)) 
      SOIL_KAverage(Mask, ts) = mean(SOIL(Mask, ts));
    endfor
  endfor
endfor

riSetActiveCellProperty(SOIL_KAverage, "SOIL_KAverage");
```