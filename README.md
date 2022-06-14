[![CMake](https://github.com/Yohavier/Wwizard/actions/workflows/cmake.yml/badge.svg?branch=main)](https://github.com/Yohavier/Wwizard/actions/workflows/cmake.yml)
# Wwizard

**Table of Content**
 - [Intro](#intro)
    - [Good to know](#good-to-know)
 - [Dependencies](#dependencies)
 - [Getting Started](#getting-started)
 - [Color Coding](#color-coding)
 - [Naming Convention](#naming-convention)
 - [Sort Originals](#sort-originals)
 - [Query Editor](#query-editor)
    - [Node Editor](#node-editor)
 - [Toolbox](#toolbox)
    - [Remove invalid events](#remove-invalid-events)
    - [Reset faders](#reset-faders)
 - [Issues](#issues)

## Intro
This is a tool, that extends Wwise a bit. Its made to improve your project workflows and keep it tidy. 
In the other two folders I explain a bit about my best practices and ideas about how to improve wwise. 
A more detailed Readme for the application is in the Wwizard folder.

Main features are:
  - assign color coding 
  - Checking naming convention
  - sort originals
  - advanced query editor
  - toolbox with smaller features

### Good to know
- if Wwise is open and you double click, in Wwizard on a query result for example, Wwise will focus on that file.

## Dependencies
The GUI is done with [Dear ImGui](https://github.com/ocornut/imgui) and an extension for a [Node Graph](https://github.com/rokups/ImNodes)

## Getting-Started
First head to the settings Tab you should see something like that. 

<img src="https://user-images.githubusercontent.com/40822700/172848545-e164232e-37fe-4719-9c51-57c30f0d926b.PNG" width="1000">

In most cases you only have to set the project path and the sdk path. Once everything is right and found the "Connected" sign should turn green. If its red check if the paths and IP/Port are right.

If you you have an open Wwise instance you the App will automatically connect to that one. If there is no open Wwise instance it will open the stated project within the WwiseConsole.

If you have an open Wwise instance and close it just hit the reconnect button and Wwizard should open the project in the console.

## Color Coding
![colorcoding](https://user-images.githubusercontent.com/40822700/172857013-7f4e1378-d32a-4b8e-b418-380ebb164bd0.PNG)

Defining keywords: you can use multiple keywords by using a "," between them. All the elements in Wwise that have that word in their name will receive that color. 

There are 4 different modes:
  - Single - soft
  - Single - hard
  - Hierarchy - soft
  - Hierarchy - hard 

The rule is single is always stronger than hierarchy and hard is stronger than soft. So the the weakest setting is "Hierarchy - soft" then "Single - Soft" then "Hierarchy - Hard" then "Single - Hard". If there are elements, that are affected by multiple color settings, they will be override or ignored depending on those settings.

you can also define colors, that can never change, for example red to mark errors and not change them.

This is the colorpicker that opens everytime you click on one of the colored boxes.</br>
<img src="https://user-images.githubusercontent.com/40822700/173699501-bbc1b23f-0ad5-409b-8d46-8e5763a61437.PNG" width="300">


## Naming Convention
Define a naming convention in the following setting tabs and check for it. This module doesnt correct anything, but outputs a little error message on what is wrong. It only displays the first wrong name in a hierarchy.
You also have the option to define multiple naming conventions and save them. You can also switch through them with the drop down menu, save new ones and clear existing ones if you need a fresh start.
![NamingConvention](https://user-images.githubusercontent.com/40822700/173699218-ea8aa88b-84f4-444d-b764-bb6664fdf64f.PNG)

Work Unit Setttings let you define rules for each section with its own kind of work units. For example Axtor-Mixer Hierarchy or Events. There you can define a prefix for all containers, if uppercase should be allowed and if spaces in the name are ok. 
![wwuSettings](https://user-images.githubusercontent.com/40822700/173699277-bbf60849-af12-4ed7-badf-83136728899e.PNG)

Container Settings are a bit more granular and only count for one type of container. Like that you can define a suffix for any type of container there is. 
At max you can have one string suffix and one number suffix. String always comes before number.
You can also set a variation of string suffixes, where the system checks if one of them is matching. Like that you can have, for example, events with the suffix _lp and _os.
![containerSettings](https://user-images.githubusercontent.com/40822700/173699284-0fc8ead1-e8dc-41bb-aa1d-d1e9bee1a3bf.PNG)


## Sort Originals
![sort Originals](https://user-images.githubusercontent.com/40822700/172855032-ceb77347-b72a-4304-a48f-02e7c08278c0.PNG)

You can control how the folder hierarchy will look like, by checking all the wwise elements that should create a folder. 

Scan Button: lets you scan the project and displays how many originals, music sfx, sfx there are. 

Delete unused Originals Button: deletes all the originals .wav files, that are not used in Wwise.

Sort Originals Button: Also deletes unused .wav and sorts the remaining one depending on the selected settings.

## Query Editor
Here is an overview of the whole Query Editor. In the details you can click on inspect query to find it in the Wwise editor, if it is open and the query is a Wwise query of course.
![queryeditor](https://user-images.githubusercontent.com/40822700/172952216-a9400311-5909-4407-803b-97ece9d57ca7.PNG)

The config window lets you choose between just activating queries, every file found with one of the queries will be displayed in the results. But there is also the option to configure everything within the node editor.</br>
![config](https://user-images.githubusercontent.com/40822700/172952205-30db5a4e-0569-4ab0-b74e-ac76de3c914e.PNG)

### Node Editor
Via drag and drop you can drop any query into the nodegraph and connect things. For further configuration you can use a "And" and "Or" node. 

Or: Every file found will be in the results. </br>
And: Only files that are found in every query will be displayed in the results.

![nodegraph](https://user-images.githubusercontent.com/40822700/172952233-9732aee3-c718-4973-bd97-a9037d74cac8.PNG)

### Create a new query
You can also create new queries with Waapi or Waql and save them. Currently it is a bit hard to create new ones, because there is no help/IntelliSense and one needs to know about Waapi or Waql. 

![create query](https://user-images.githubusercontent.com/40822700/172952210-41513759-1a39-4afe-8b6b-11171f68d725.PNG)

## Toolbox
### Remove invalid events
![toolboxEvents](https://user-images.githubusercontent.com/40822700/173699312-ce327571-d875-42e4-b8b5-8e6345734915.PNG)
This one has 2 modes either you use it on all events by ticking the checkbox or if you have an open Wwise instance and the checkbox is unchecked, you can also remove the empty events in the selected hierarchy.

### Reset faders
![toolbox fader](https://user-images.githubusercontent.com/40822700/172851047-c6e23df9-214a-4195-8bb4-05298748394c.PNG)
Resets the fader in a hierarchy except for the ones, that have the text in their comments.
This can be used for Master-Mixer Hierarchy, Interactive Music Hierachy and Actor-Mixer Hierarchy.


## Issues
-  Too long file names in Wwise might break the sort originals module, because it wants to create folders.
