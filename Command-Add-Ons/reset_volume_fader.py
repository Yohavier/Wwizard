from waapi import WaapiClient
import os

def get_selected_guid():
    pass

def _walk_depth_first(client, start, props, ret_props, types):
    query = {
        'from': {'path': [start]} if start.startswith('\\') else {'id': [start]},
        'transform': [{'select': ['children']}]
    }

    ret = client.call(_c.core_object_get, query, options={'return': props})
    if not _check_get_ret(ret):
        return

    for obj in ret['return']:
        if types == 'any' or obj['type'] in types:
            yield tuple(obj.get(p, None) for p in ret_props)
        yield from _walk_depth_first(client, obj['id'], props, ret_props, types)


def walk_wproj(client: _w.WaapiClient,
               start_guids_or_paths: _StrOrSeqOfStr,
               properties: _StrOrSeqOfStr = None,
               types: _StrOrSeqOfStr = 'any') -> _t.Iterator[_t.Tuple[_WaapiValue]]:
    """
    Walk through descendants of an object and yield their properties.
    :param client: WAAPI client, it should be connected.
    :param start_guids_or_paths: Either an ID or a path where iteration starts. If a list is passed,
                                 the iterator will walk descendants of each object in the list.
                                 Can be either a str or a list of str.
    :param properties: A list of property names. Their values will be yielded as tuples. Default is ['id'].
                       Can be either a str or a list of str.
    :param types: A list of object types which properties will be yielded during walk. Default is 'any'.
                  Can be either a str or a list of str.
    :return: A tuple of property values in the order specified by the 'properties' argument.
             If a property doesn't exist, its corresponding value will be 'None'.
    Example:
    .. code-block:: python
       from waapi import WaapiClient
       from waapi_helpers import *
       with WaapiClient() as client:
           for guid, name in walk_wproj(client, '\\Actor-Mixer Hierarchy',
                                        properties=['id', 'name'], types='any'):
              print('Object', name, 'has ID of', guid)
    """
    if properties is None:
        properties = ['id']

    if not _check_client(client):
        raise ValueError('Waapi client is none or disconnected')
    if _is_any_val_none(start_guids_or_paths, types):
        return ValueError('start_guids_or_path and types cannot be None')

    start_list = _ensure_str_list(start_guids_or_paths)
    props = _ensure_str_list(properties)
    req_types = _ensure_str_list(types, 'any')

    ret_props = copy.copy(props)
    for p in 'id', 'type':
        if p not in props:
            props.append(p)

    for start in start_list:
        yield from _walk_depth_first(client, start, props, ret_props, req_types)

with WaapiClient() as client:
    num_reset_faders = 0
    selected_guid = get_selected_guid()

    for obj_id, obj_type, obj_notes in walk_wproj(client, selected_guid,
                                                  properties=['id', 'type', 'notes']):
        if '@ignore' in obj_notes:
            continue

        # note, we want to change different properties based on whether
        # an object belongs to the Actor-Mixer or the Master Mixer hierarchy
        prop_name = 'Volume'
        if obj_type == 'Bus' or obj_type == 'AuxBus':
            prop_name = 'BusVolume'

        cur_volume = get_property_value(client, obj_id, prop_name)
        if cur_volume is not None:
            # by convention, if a property doesn't exist,
            # `get_property_value` will return None, which
            # allows to skip calling `set_property_value` when
            # there's no volume property on the object
            set_property_value(client, obj_id, prop_name, 0)
            num_reset_faders += 1