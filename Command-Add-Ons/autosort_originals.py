import shutil
import os
from pathlib import Path
import xml.etree.ElementTree as ET
from waapi import WaapiClient
from helpers import remove_dir

#Paths that are needed
w_proj_path = ""
originalsPath = ""
actorMixerWwuPath = ""
interactiveMusicWwuPath = ""


wavFileDic = {}
SFXUseDic = {}

actorMixerWwu = {}
interactiveMusicWwu = {}

def set_paths(client):
    global w_proj_path
    global originalsPath
    global actorMixerWwuPath
    global interactiveMusicWwuPath

    query = {"from": {"path": ['\\Actor-Mixer Hierarchy']}}
    queryResult = client.call("ak.wwise.core.object.get", query, options={'return': ['filePath']})

    w_proj_path = os.path.dirname(queryResult['return'][0]['filePath'])
    originalsPath = w_proj_path + "\\Originals\\SFX"
    actorMixerWwuPath = w_proj_path + "\\Actor-Mixer Hierarchy"
    interactiveMusicWwuPath = w_proj_path + "\\Interactive Music Hierarchy"

def get_files_in_dir(result, dir, suffix):
    for path in Path(dir).iterdir():
        if path.is_dir():
            get_files_in_dir(result, path, suffix)
        elif path.suffix == suffix:
            result.append(str(path))

def get_wwu_in_dir(result, dir, suffix):
    for path in Path(dir).iterdir():
        if path.is_dir():
            get_wwu_in_dir(result, path, suffix)
        elif path.suffix == suffix:
            result[os.path.basename(path)] = path

def create_sfxDic(dir):
    global audio_file_use_dic

    for path in Path(dir).iterdir():
        if path.is_dir():
            create_sfxDic(path)
        elif path.suffix == ".wav":
            relative_path = str(path).replace(w_proj_path + "\\Originals\\SFX\\", "")
            wavFileDic[relative_path] = 0

def create_workspace_folder(folder_path):
    if not os.path.isdir(folder_path):
        #print("created folder " + folder_path)
        os.mkdir(folder_path)

def create_folders_from_wwu_hierarchy(directory, attachment):
    for path in Path(directory).iterdir():
        if path.is_dir():
            new_attachment = "\\" + attachment + "\\" + str(os.path.basename(path))
            create_workspace_folder(originalsPath + "\\" + new_attachment)
            create_folders_from_wwu_hierarchy(path, new_attachment)
        elif path.suffix == ".wwu":
                new_attachment = "\\" + attachment + "\\" + str(os.path.basename(path)).replace(".wwu", "")
                create_workspace_folder(originalsPath + new_attachment)

def createFolderStructure(path, wwuXmlRoot, isinit, wwuPhysicalPath):
    for child in wwuXmlRoot:
        if child.tag == 'WorkUnit':
            if child.attrib['PersistMode'] == 'Standalone':
                isinit = False
                newPath = path + "\\" + child.attrib['Name']
                create_workspace_folder(newPath)
                createFolderStructure(newPath, child, isinit, wwuPhysicalPath)

            elif child.attrib['PersistMode'] == 'Reference' and not isinit:
                referenceWwuTree = ET.parse(wwuPhysicalPath + '\\' + child.attrib['Name'] + '.wwu')
                referenceWwuXmlRoot = referenceWwuTree.getroot()
                createFolderStructure(path, referenceWwuXmlRoot, isinit, wwuPhysicalPath)

            elif child.attrib['PersistMode'] == 'Nested' and not isinit:
                newPath = path + "\\" + child.attrib['Name']
                create_workspace_folder(newPath)
                createFolderStructure(newPath, child, isinit, wwuPhysicalPath)

        elif child.tag == 'Folder' or child.tag == 'ActorMixer':
            newPath = path + "\\" + child.attrib['Name']
            create_workspace_folder(newPath)
            createFolderStructure(newPath, child, isinit, wwuPhysicalPath)

        else:
            if child.tag == "Sound" or child.tag == "MusicTrack":
                SFXUseDic[child.attrib['Name']] = {"AudioFileSource":[], "ActiveSpurce": ""}
                get_wwise_xml_sound_info(child, path, child.attrib['Name'])
            if child.tag == "AudioFile":
                if child.text in wavFileDic.keys():
                    wavFileDic[child.text] = wavFileDic[child.text] + 1

            createFolderStructure(path, child, isinit, wwuPhysicalPath)

def get_wwise_xml_sound_info(root, folder_path, sound_name):
    global SFXUseDic
    for child in root:
        if child.tag == "AudioFileSource":
            for file in child:
                if file.tag == "AudioFile":
                    SFXUseDic[sound_name]["AudioFileSource"].append({"AudioFileSource_Name": child.attrib['Name'],
                                                                "AudioFileSource_ID": child.attrib['ID'],
                                                                "AudioFile": file.text,
                                                                "New_Path": folder_path
                                                                     })
        elif child.tag == "ActiveSource":
            SFXUseDic[sound_name]["ActiveSource"] = child.attrib['ID']
        else:
            get_wwise_xml_sound_info(child, folder_path, sound_name)

def reorder(root):
    for child in root:
        if child.tag == "Sound":
            reorder_entry(child, child.attrib['Name'])
        else:
            reorder(child)

def reorder_entry(path, sound):
    global SFXUseDic
    global wavFileDic

    for child in path:
        if child.tag == 'AudioFileSource':
            for audioFileSource in child:
                if audioFileSource.tag == 'AudioFile':
                    #wav file is used in multiple containers
                    if audioFileSource.text in wavFileDic.keys():
                        if wavFileDic[audioFileSource.text] > 1:
                            try:
                                shutil.move(originalsPath + '\\' + audioFileSource.text, originalsPath + '\\' + 'MultiUse' + '\\' + os.path.basename(audioFileSource.text))
                            except:
                                print('was already moved')

                            audioFileSource.text = 'MultiUse\\' + os.path.basename(audioFileSource.text)

                        else:
                            new_path = SFXUseDic[sound]["AudioFileSource"][0]['New_Path'] + "\\" + os.path.basename(audioFileSource.text)
                            shutil.move(originalsPath + '\\' + audioFileSource.text, new_path)
                            audioFileSource.text = new_path.replace(originalsPath + '\\', '')

        else:
            reorder_entry(child, sound)

def createFoldersFromPath(path):
    path = path[1:]
    layers = path.split("\\")
    currentPath = ''
    for layer in layers:
        currentPath += '\\'+layer
        create_workspace_folder(originalsPath+currentPath)

def checkStandalone(wwuToCheck):
    wwuTree = ET.parse(wwuToCheck)
    wwuXmlRoot = wwuTree.getroot()
    for child in wwuXmlRoot:
        for first in child:
            if first.tag == "WorkUnit":
                if first.attrib['PersistMode'] == "Standalone":
                    return True
    return False

def deleteObsoleteWav():
    for key in wavFileDic.keys():
        if wavFileDic[key] == 0:
            os.remove(originalsPath + '\\' + key)


with WaapiClient() as client:
    #Set all needed paths
    set_paths(client)

    #Get all .wav files that are currently in the Originals folder
    create_sfxDic(originalsPath)

    #find all wwu`s
    get_wwu_in_dir(actorMixerWwu, actorMixerWwuPath, ".wwu")
    get_wwu_in_dir(interactiveMusicWwu, interactiveMusicWwuPath, ".wwu")

    #Create the three folders where we want to sort the files
    create_workspace_folder(originalsPath + "\\Actor-Mixer Hierarchy")
    create_workspace_folder(originalsPath + "\\Interactive Music Hierarchy")
    create_workspace_folder(originalsPath + "\\MultiUse")

    #create wwu folder structure
    #3 types create new folders "wwu", "virtual folders", "Actor-Mixers"
    for wwu in actorMixerWwu:
        wwuTree = ET.parse(actorMixerWwu[wwu])
        wwuXmlRoot = wwuTree.getroot()

        path = str(actorMixerWwu[wwu]).replace(w_proj_path, "")

        if checkStandalone(actorMixerWwu[wwu]):
            createFoldersFromPath(path.replace('.wwu', ''))
            path = str(originalsPath) + path.replace('\\' + wwu, '')

            physicalWwuPath = str(actorMixerWwu[wwu]).replace('\\' + os.path.basename(str(actorMixerWwu[wwu])), "")
            createFolderStructure(path, wwuXmlRoot, True, physicalWwuPath)

    for wwu in interactiveMusicWwu:
        wwuTree = ET.parse(interactiveMusicWwu[wwu])
        wwuXmlRoot = wwuTree.getroot()

        path = str(interactiveMusicWwu[wwu]).replace(w_proj_path, "")

        if checkStandalone(actorMixerWwu[wwu]):
            createFoldersFromPath(path.replace('.wwu', ''))
            path = str(originalsPath) + path.replace('\\' + wwu, '')

            physicalWwuPath = str(interactiveMusicWwu[wwu]).replace('\\' + os.path.basename(str(interactiveMusicWwu[wwu])), "")
            createFolderStructure(path, wwuXmlRoot, True, physicalWwuPath)

    #iterate through wwu xml and
    for wwu in actorMixerWwu:
        wwu_tree = ET.parse(actorMixerWwu[wwu])
        wwu_xml_root = wwu_tree.getroot()
        reorder(wwu_xml_root)
        wwu_tree.write(actorMixerWwu[wwu])

    for wwu in interactiveMusicWwu:
        wwu_tree = ET.parse(interactiveMusicWwu[wwu])
        wwu_xml_root = wwu_tree.getroot()
        reorder(wwu_xml_root)
        wwu_tree.write(interactiveMusicWwu[wwu])

    deleteObsoleteWav()
    remove_dir.removeEmptyFolders(originalsPath)
