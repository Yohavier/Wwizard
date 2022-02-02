from waapi import WaapiClient

resetList = []
legal_types = ['Bus', 'AuxBus', 'ActorMixer', 'RandomSequenceContainer', 'BlendContainer', 'SwitchContainer', 'Sound', 'MusicSegment', 'MusicPlaylistContainer', 'MusicSwitchContainer', 'MusicTrack']

def get_current_guid(client):
    options = {"return": ["id", "name", "type", "notes"]}
    ret = client.call('ak.wwise.ui.getSelectedObjects', options= options)
    return ret

def get_children(client, guid):
    query = {"from": {"id": [guid]}, 'transform': [{'select': ['children']}]}
    options = {"return": ["id", "name", "type", "notes"]}

    ret = client.call('ak.wwise.core.object.get', query, options=options)

    for child in ret['return']:
        set_property_value(client, child['id'], 0, child['type'], child['notes'])

def set_property_value(client, guid, value, type, notes):
    if type in legal_types:
        if '@ignore' not in notes:
            if type == 'Bus' or type == 'AuxBus':
                client.call('ak.wwise.core.object.setProperty', {'object': guid, 'property': 'BusVolume', 'value': value})
            else:
                client.call('ak.wwise.core.object.setProperty', {'object': guid, 'property': 'Volume', 'value': value})
            get_children(client, guid)

with WaapiClient() as client:
    current_selection = get_current_guid(client)['objects'][0]
    set_property_value(client, current_selection['id'], 0, current_selection['type'], current_selection['notes'])
    get_children(client, current_selection['id'])

