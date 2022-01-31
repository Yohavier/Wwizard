from waapi import WaapiClient
import os
from pathlib import Path
import xml.etree.ElementTree as ET

eventWwuRootPath = ""

eventsToDelete = []
eventWwuList = []

def get_files_in_dir(result, dir, suffix):
    for path in Path(dir).iterdir():
        if path.is_dir():
            get_files_in_dir(result, path, suffix)
        elif path.suffix == suffix:
            result.append(str(path))

def delete_object(client, guid):
    client.call('ak.wwise.core.object.delete', {'object': guid})

def set_paths():
    global eventWwuRootPath
    eventWwuRootPath
    query = {"from": {"path": ['\\Actor-Mixer Hierarchy']}}
    queryResult = client.call("ak.wwise.core.object.get", query, options={'return': ['filePath']})
    eventWwuRootPath = os.path.dirname(queryResult['return'][0]['filePath']) + '\\' + 'Events'

def walk_through_wwu(wwu_xml_root):
    for child in wwu_xml_root:
        if child.tag == "Event":
            if is_event_empty(child):
                eventsToDelete.append(child.attrib['ID'])
                continue
            for eventAction in child:
                if is_event_empty(eventAction):
                    eventsToDelete.append(child.attrib['ID'])
                for actionReference in eventAction:
                    if is_event_empty(actionReference):
                        eventsToDelete.append(child.attrib['ID'])
                    else:
                        if child.attrib['ID'] in eventsToDelete:
                            eventsToDelete.remove(child.attrib['ID'])
        else:
            walk_through_wwu(child)

def is_event_empty(root):
    if len(root) > 0:
        return False
    else:
        return True


with WaapiClient() as client:
    set_paths()
    get_files_in_dir(eventWwuList, eventWwuRootPath, '.wwu')

    for wwu in eventWwuList:
        wwu_tree = ET.parse(wwu)
        wwu_xml_root = wwu_tree.getroot()
        walk_through_wwu(wwu_xml_root)

    for event in eventsToDelete:
        delete_object(client, event)