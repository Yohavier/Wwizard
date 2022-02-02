# Wwise Command DAdd Ons
This is a small collection of Add-Ons, Scripts etc that can help improve your Workflow with Wwise and add functionality and features.
Currently there are following projects that one could try out:

1. [Sort Originals](#sortOriginals)
2. [Delete Empty Events](#deleteEmptyEvents)
3. [Revers Faders](#reverseFaders)

## Sort Originals <a name="sortOriginals"></a>
There are 4 container inside Wwise, that can create a folder in the Originals Path.

![structure](https://user-images.githubusercontent.com/40822700/151835256-cf84f893-fb41-4655-8875-bd1207dddb2f.PNG)
- Work Units
- Physical Folders
- Virtual Folders
- Actor-Mixers

One could also use all Wwise Containers, but it would result in very long folder paths with often only one .wav file in it. Like that A folder could indicate a new feature and hence structure the originals accordingly.

To start the process just go to the WAAPI Rider and click "Sort Originals".

![waapi](https://user-images.githubusercontent.com/40822700/151866644-43589973-9cdc-4bcf-a0f0-68310789bd8a.PNG)



## Delete empty Events <a name="deleteEmptyEvents"></a>
With this command you delete every event that is empty. Go to the WAAPI Rider and just click "Delete empty events".

![delete](https://user-images.githubusercontent.com/40822700/151884604-a4b48198-1e82-4a72-80ae-d77499ccb9f6.PNG)


## Reverse Faders <a name="reverseFaders"></a>
This is a command which is helpful for mixing and revesing that without damaging previous mixes. 
You can revert the volume faders in the MasterMixer, Actor-Mixer or Interactive Music Hierarchy. Simply right click an object select "WAAPI->Reset volume faders".

![resetfader](https://user-images.githubusercontent.com/40822700/152076544-26c7efd0-cb65-4319-a496-72a2bf79676a.PNG)

>This will set all of the faders in the hierarchy to 0!

If you write "@ignore" into the notes of a container, thats where the process will be interrupted and that objects and its hierarchy is safe from the reverse process.
