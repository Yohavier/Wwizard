# Wwise Naming Convention

A naming convention is a set of rules for choosing the character sequence to be used for identifiers. Inconsistencies lead to a lot of edge cases and context-specific convetions only turn into temporary solutions instead of a non-context-dependent design management tool.

Solid naming convention promotes:
- Easier implementations
- Easier understanding
- Easier collaboration with others

## Goals
- Readable
- Parsable
- Explicit
- As long as necessary, as short as possible

# Universal Rules
- All lower case
- snake_case is always a new layer of description
- Acronyms can be 2/3 letters
- Children inherit parents name

- If we want to have multiple words in a  layer, we can't use kebab-case because some things cant be named like that 
-Solution
  * Use abbreviations
  * Create custom words for example medical center = medcenter
  * Just write the words together 

# Additional Rules 
- Eventsuffix can be _lp, _start, _stop
- Versions 0 to 99


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


# Things that can be named in Wwise

## Special Containers
- Physical Folder

### Actor-Mixer Hierarchy Containers
- Virtual Folder
- Actor-Mixer
- Random Container
- Sequence Container
- Switch Container 
- Blend Container
  * Blend Track
- Sound SFX
  * Source
- Sound Voice
  * Source
- Work Unit

### Master-Mixer Hierarchy
- Virtual Folder
- Work Unit
- Audio Bus
- Auxillary Bus

### Interactive Music Hierarchy
- Virtual Folder
- Work Unit
- Music Switch Container 
- Music Playlist Container
- Music Segment
- Music Track

### Audio Devices
- Physical Folder
- Virtual Folder
- Work Unit
- Audio Device

### Events
- Physical Folder
- Virtual Folder
- Work Unit
- Event

### Dynamic Dialogue
- Physical Folder
- Virtual Folder
- Work Unit
- Dialogue Event

### SoundBanks
- Physical Folder
- Virtual Folder
- Work Unit
- SoundBank

### Switches
- Physical Folder
- Virtual Folder
- Work Unit
- Switch Group
- Switch

### States
- Physical Folder
- Virtual Folder
- Work Unit
- State Group
- State

### Game Parameters
- Physical Folder
- Virtual Folder
- Work Unit
- Game Parameter

### Triggers
- Physical Folder
- Virtual Folder
- Work Unit
- Trigger

### Queries 
- Physical Folder
- Virtual Folder
- Work Unit
- Query

### Soundcaster Session
- Physical Folder
- Virtual Folder
- Work Unit
- Soundcaster Session 

### Mixing Session
- Physical Folder
- Virtual Folder
- Work Unit
- Mixing Session

### Control Surface Session
- Physical Folder
- Virtual Folder
- Work Unit
- Control Surface Session 
  * group
  * 

### Effects 
- Physical Folder
- Virtual Folder
- Work Unit
- Effect

### Attenuations
- Physical Folder
- Virtual Folder
- Work Unit
- Attenuation 

### Conversion Settings
- Physical Folder
- Virtual Folder
- Work Unit
- Conversion Setting 

### Modulators 
- Physical Folder
- Virtual Folder
- Work Unit
- Modulator LFO
- Modulator Envelope
- Modulator Time

### Virtual Acoustics
- Physical Folder
- Virtual Folder
- Work Unit
- Acoustic Texture

### Metadata
- Physical Folder
- Virtual Folder
- Work Unit
- Wwise System Output Settings 

### Position Editor (3D Automation)
- Path

### Music 
- Transitions
- Cues
