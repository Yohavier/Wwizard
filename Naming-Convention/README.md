# Wwise Naming Convention

## Summary 
- [Goals](#goals)
- [Rules](#rules)
- [Prefix](#prefix)
- [Naming](#naming)
- [To-Do](#todo)

<br/>
<br/>

## Goals <a name="goals"></a>
A naming convention is a set of rules for choosing the character sequence to be used for identifiers. Inconsistencies lead to a lot of edge cases and context-specific convetions only turn into temporary solutions instead of a non-context-dependent design management tool.

Solid naming convention promotes:
- Easier implementations
- Easier understanding
- Easier collaboration with others

The goal of this naming convention is that every name matches with the following 
- Readable
- Parsable
- Explicit
- As long as necessary, as short as possible

<br/>
<br/>

## Rules <a name="rules"></a>
- All lower case
- snake_case is always a new layer of description
- Acronyms can be 2/3 letters
- Children inherit parents name

- If we want to have multiple words in a  layer, we can't use kebab-case because some things cant be named like that 
-Solution
  * Use abbreviations
  * Create custom words for example medical center = medcenter
  * Just write the words together 


#### Additional Rules 
- Eventsuffix can be _lp, _start, _stop
- Versions 0 to 99

<br/>
<br/>

## Prefix <a name="prefix"></a>
Sometimes you want to copy the wwu structure from the Actor-Mixer Hierarchty for the Event Hierarchy. In that case it might be hard to differentiate which Work Unit belongs to which in your version control tool. Therefor I like to use a prefix for the Work Units. 

>The prefix always should be set in front of the name that results out of the hierarchy depending on their workspace.

| Workspace | Prefix |
| --------- | ------ | 
| Master-Mixer Hierarchy | mxh_ |
| Actor-Mixer Hierarchry | amh_ |
| Interactive Music Hierarchy | imh_ |
| Events | evt_ |
| Dynamic Dialogue | dde_ |
| Soundbanks | bnk_ | 
| Switches | sw_ |
| States | st_ |
| Game Parameters | gp_ |
| Triggers | tr_ | 
| Audio Devices | ad_ | 
| Virtual Acoustics | va_ | 
| Metadata | mtd_ |
| Soundcaster Session | scs_ |
| Mixing Session | mxs_ |
| Control Surface Session | css_ |
| Queries | que_ | 
| Conversion Settings | con_ |
| Attenuations | att_ | 
| Effects | fx_ |

<br/>
<br/>

## Naming <a name="naming"></a>
We can pretty much split up the naming convention into two parts. One that can be named after their hierarchy and the other one cant.
1. [Hierarchical Naming](#hierarchicalNaming)
2. [Non Hierarchical Naming](#nonHierarchicalNaming)

<br/>

### Hierarchical Naming <a name="hierarchicalNaming"></a>
For this kind of naming the chapter [Rules](#rules) comes into play. If you apply all of these guidelines to the containers you will get a nice consistent naming just as in the example below.

> Tipp: For things up high in the hierarchy use something short names or better an abbreviations, since the Hierarchy below will inherit that name. 

|Physical Folder|Virtual Folder|Actor-Mixer|Random Container|Sequence Container|Switch Container|Blend Container|Sound SFX|Sound Voice|
|--------------|-----------|----------------|------------------|----------------|---------------|---------|-----------|-|
|Music Switch Container|Music Playlist Container|Music Segment Container|Music Track|Event|Dialogue Event|Sound SFX Source|Sound Voice Source|


|SoundBank|Game Parameter|Trigger|State Group|State|Switch Group|Switch|Attenuation|
|---------|--------------|-------|-----------|-----|------------|------|-----------|

#### Example for the Actor-Mixer Hierarchy
![wwiseNaming](https://user-images.githubusercontent.com/40822700/152829297-81d9f58c-a3ae-4ab4-a6e2-e1d19d9d4afb.PNG)

<br/>

### Non Hierarchical Naming <a name="nonHierarchicalNaming"></a>
All of these cases need individual names, so people can make sense of whats in there. A naming convention for these things is tedious, but makes it easier to find and understand certain things. 

|Effect|Conversion Setting|Modulator LFO|Modulator Envelope|Modulator Time|Acoustic Texture| Wwise System Output Settings|
|------|------------------|-------------|------------------|--------------|----------------|-----------------------------|
|Query |Soundcaster Session|Mixing Session|Control Surface Session|

|Control Surface Session Group|Blend Container Blend Track |Path (Position Editor)|Music Track Cue|Transitions|
|-----------------------------|----------------------------|----------------------|---------------|-----------|

#### Effect
wip

#### Conversion Settings
First there should be a short descriptive layer, to indicate what it should be used for. For example music, oneshot, loop, etc.
After that follows the channel configuration, then the sample rate, then the format and at the end we add "seek", if its seeking. If not we just leave it empty. 
In the end it would look like that.
```
con_music_mono_48000_pcm_seek
```
#### Modulator LFO
wip
#### Modulator Envelope
wip
#### Modulator Time
wip
#### Acoustic Texture
wip
#### Wwise System Output Settings
wip
#### Query
wip
#### Soundcaster Session
wip
#### Mixing Session
wip
#### Control Surface Session
wip
#### Control Surface Session Group
wip
#### Blend Container Track
wip
#### Path(Position Editor)
wip
#### Music Track Cue
wip
#### Transistion
wip

<br/>
<br/>

## To-Do <a name="todo"></a>
- [ ] find naming for all Non-Hierarchical things
