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

- If we want to have multiple words in a  layer, we can`t use kebab-case because some things can`t be named like that 
-Solution
  * Use abbreviations
  * Create custom words for example medical center = medcenter
  * Just write the words together 

# Things that can be named in Wwise

## Work Units
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

# Additional Rules 
- Eventsuffix can be _lp, _start, _stop
- Versions 0 to 99
