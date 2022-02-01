# Wwise Naming Convention

## Summary 
- [Intention](#intention)
- [Rules](#rules)
- [Naming](#naming)
- [Prefix](#prefix)



## Intention <a name="intention"></a>
A naming convention is a set of rules for choosing the character sequence to be used for identifiers. Inconsistencies lead to a lot of edge cases and context-specific convetions only turn into temporary solutions instead of a non-context-dependent design management tool.

Solid naming convention promotes:
- Easier implementations
- Easier understanding
- Easier collaboration with others

#### Goals
- Readable
- Parsable
- Explicit
- As long as necessary, as short as possible



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



## Naming <a name="naming"></a>
We can pretty much split up the naming convention into two parts. 
- Hierarchical Naming
- Non Hierarchical Naming

There is a lot that we can name according to the Hierarchy and the Rules set in [Rules](#rules), those are handled in this [Chapter](#hierarchicalNaming).
The rest is being discussed in [Non Hierarchical Naming](#nonHierarchicalNaming).

### Hierarchical Naming <a name="hierarchicalNaming"></a>

> Tipp: For things up high in the hierarchy use something short names or better an abbreviations, since the Hierarchy below will inherit that name. 

|Physical Folder|Virtual Folder|Actor-Mixer|Random Container|Sequence Container|Switch Container|Blend Container|Sound SFX|Sound Voice|
|--------------|-----------|----------------|------------------|----------------|---------------|---------|-----------|-|
|Music Switch Container|Music Playlist Container|Music Segment Container|Music Track|Event|Dialogue Event|Sound SFX Source|Sound Voice Source|


|SoundBank|Game Parameter|Trigger|State Group|State|Switch Group|Switch|Attenuation|
|---------|--------------|-------|-----------|-----|------------|------|-----------|

### Non Hierarchical Naming
All of these cases need individual names, so people can make sense of whats in there. A naming convention for these things is tedious, but makes it easier to find and understand certain things. 

|Effect|Conversion Setting|Modulator LFO|Modulator Envelope|Modulator Time|Acoustic Texture| Wwise System Output Settings|
|------|------------------|-------------|------------------|--------------|----------------|-----------------------------|
|Query |Soundcaster Session|Mixing Session|Control Surface Session|

|Control Surface Session Group|Blend Container Blend Track |Path (Position Editor)|Music Track Cue|Transitions|
|-----------------------------|----------------------------|----------------------|---------------|-----------|






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

