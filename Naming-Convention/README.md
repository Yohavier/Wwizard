# Wwise Naming Convention

## Summary 
- [Intention](#intention)
- [Rules](#rules)
- [Containers](#containers)

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


## Containers <a name="containers"></a>
First we want to get an overview of what containers there are and what can be named.

#### Audio Tab
| Master-Mixer Hierarchy | Actor-Mixer Hierarchy | Interactive Music Hierarchy | Audio Devices |
| ---------------------- | --------------------- | --------------------------- | ------------- |
| Physical Folder        | Physical Folder       | Physical Folder             | Physical Folder|
| Virtual Folder         | Virtual Folder        | Virtual Folder              | Virtual Folder|
| Audio Bus              | Work Unit             | Work Unit                   | Work Unit    |
| Auxillary Bus          | Actor-Mixer           | Music Switch Container      | Audio Device |
|                        | Random Container      | Music Playlist Container    |              |
|                        | Sequence Container    | Music Segment               |              |
|                        | Switch Container      | Music Track<ul><li>Cue</li></ul>|          |
|                        | Blend Container<ul><li>Blend Track</li></ul>| Transitions|         |
|                        | Sound SFX<ul><li>Source</li></ul>|                  |              |
|                        | Sound Voice<ul><li>Source</li></ul>|                |              |
|                        | Path (Position Editor)|                             |              |

#### Events                                              
| Events | Dynamic Dialogue |
| ---------------------- | --------------------- | 
| Physical Folder        | Physical Folder       |
| Virtual Folder         | Virtual Fodler        |
| Work Unit              | Work Unit             |
| Event                  | Dialogue Event        |

#### SoundBanks
| SoundBanks | 
| ---------------------- |
| Physical Folder        |
| Virtual Folder         | 
| Work Unit              | 
| SoundBank              | 

#### Game Syncs
| Switches | States | Game Parameters | Triggers |
| -------- | ------ | --------------- | -------- |
| Physical Folder| Physical Folder | Physical Folder | Physical Folder |
| Virtual Folder| Virtual Folder | Virtual Fodler | Virtual Folder |
| Work Unit| Work Unit | Work Unit | Work Unit |
| Switch Group| State Group| Game Parameter | Trigger |
| Switch   | State |                  |          |

#### ShareSets
| Effects | Attenuations | Conversion Settings | Modulators | Virtual Acoustics | Metadata |
| ------- | ------------ | ------------------- | ---------- | ----------------- | -------- |
| Physical Fodler | Physical Folder | Physical Folder | Physical Folder | Physical Folder | Physical Folder |
| Virtual Folder | Virtual Folder | Virtual Folder | Virtual Folder | Virtual Folder | Virtual Folder |
| Work Unit | Work Unit  | Work Unit           | Work Unit  | Work Unit         | Work Unit | 
| Effect  | Attenuation  | Conversion Setting  | Modulator LFO | Acoustic Texture | Wwise System Output Settings |
|         |              |                     | Modulator Envelope |           |           |
|         |              |                     | Modulator Time |               |           |

#### Sessions
| Soundcaster Session | Mixing Session | Control Surface Session |
| ------------------- | -------------- | ----------------------- |
| Physical Folder     | Physical Folder | Physical Folder        | 
| Virtual Folder      | Virtual Folder | Virtual Fodler          |
| Work Unit           | Work Unit      | Work Unit               |
| Soundcaster Session | Mixing Session | Control Surface Session<ul><li>Group</li></ul>|

#### Queries
| Queries |
| ------- | 
| Physical Folder |
| Virtual Folder | 
| Work Unit |
| Query |

### Work Units
- Master-Mixer Hierarchy = mxh_
- Actor-Mixer Hierarchy = amh_
- Interactive Music Hierarchy = imh_
- Events = evt_
- Dynamic Dialogue = dde_
- SoundBanks = bnk_
- Switches = sw_
- States = st_
- Game Parameters = gp_
- Triggers = tr_
- Audio Devices = ad_
- Virtual Acoustics = va_
- Metadata = mtd_
- Soundcaster Session scs_
- Mixing Session = mxs_
- Control Surface Session = css_
- Queries = que_
- Modulators = mod_
- Conversion Settings = con_
- Attenuations = att_
- Effects = fx_
