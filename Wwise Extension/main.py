import datetime
import os
import waapi.wamp.interface
from ExtensionModules.QueryModule import Query as q
from waapi import WaapiClient
from GUI import GUI
import json
import uuid
import win32gui
from datetime import datetime
from Settings import Settings

# TODO
# 1. Hardcoded Queries readable from json
# 2. open nuendo, edit file
# 3. logical gates for combining queries
# 4. update on Wwise changes
# 5. edit custom query input
# 6. also block stuff, when nothing is selected
# 7. Settings to select directory etc.
# 8. failed to connect screen

# Global Variables
appSettings = Settings.cSettings()
client = None

queryDictionary = {}
waqlQueries = {}
waapiQueries = []

active_query_dictionary = {}
marked_files_dictionary = {}

hardcoded_query_uuid = str(uuid.uuid4())
custom_query_uuid = str(uuid.uuid4())
wwise_query_uuid = str(uuid.uuid4())

# Load Settings for the application
def LoadSettings():
    global appSettings
    file = open('Data/Settings.json')
    settings = json.load(file)
    appSettings.SetSettings(settings['Settings']['WwiseProjectPath'], settings['Settings']['WwiseAuthoringPath'], settings['Settings']['CustomQueryPath'])
    file.close()


# Connect or open a Wwise Instance
def ConnectToWwise():
    global client
    global appSettings

    try:
        client = WaapiClient()
    except waapi.wamp.interface.CannotConnectToWaapiException:
        try:
            commandline = '%s waapi-server %s --allow-migration --wamp-port 8080' % ('"' + appSettings.wwiseAuthoringPath + '"', '"' + appSettings.wwiseProjectPath + '"')
            os.popen(commandline)
            client = WaapiClient()

        except waapi.wamp.interface.CannotConnectToWaapiException:
            print("Somethings wrong with the Settings. Console could not be opened")


# Load Queries and other stats from Wwise and Json files
def QueryLoadingHandler():
    #myGUI.all_tv.insert(parent='', index='end', iid=wwise_query_uuid, values=("Wwise Queries").replace(" ", "\ "))
    GetWwiseQueries('\\Queries', wwise_query_uuid)

    #myGUI.all_tv.insert(parent='', index='end', iid=custom_query_uuid, values=("Custom Queries").replace(" ", "\ "))
    GetCustomQueries(custom_query_uuid)


def GetWwiseQueries(path, list_parent):
    wwiseQueries = q.cQuery(0, {"from": {"path": [path]}, "transform": [{"select": ['children']}]}, "wwise_query_collector", str(uuid.uuid4()))

    for output in wwiseQueries.run_query(client)['return']:
        if str(output['type']) == 'WorkUnit':
            unique_id = str(uuid.uuid4())
            #myGUI.all_tv.insert(parent=list_parent, index='end', iid=unique_id, values=output['name'].replace(" ","\ "))
            GetWwiseQueries(path + '\\' + output['name'], unique_id)

        elif str(output['type']) == 'Folder':
            unique_id = str(uuid.uuid4())
            #myGUI.all_tv.insert(parent=list_parent, index='end', iid=unique_id, values=output['name'].replace(" ","\ "))
            GetWwiseQueries(path + '\\' + output['name'], unique_id)

        elif str(output['type'] == 'QueryModule'):
            unique_id = str(uuid.uuid4())
            #myGUI.all_tv.insert(parent=list_parent, index='end', iid=unique_id, values=(output['name'].replace(" ", "\ "), unique_id))
            queryDictionary[unique_id] = q.cQuery(1, output['path'], output['name'], unique_id)


def GetCustomQueries(list_parent):
    with open(appSettings.customQueryPath, 'r') as json_file:
        data = json.load(json_file)
        for queryType in data:
            # WAQL Queries
            for waqlQuery in data['waqlQueries']:
                query_uuid = str(uuid.uuid4())
                newWaqlQuery = q.cQuery(2, waqlQuery['query_info'], waqlQuery['query_name'], query_uuid)
                #myGUI.all_tv.insert(parent=list_parent, index='end', iid=query_uuid, values=((newWaqlQuery.query_name).replace(" ", "\ "), query_uuid))
                queryDictionary[query_uuid] = newWaqlQuery
                waqlQueries[query_uuid] = newWaqlQuery
            # WAAPI Queries
            for waapiQuery in data['waapiQueries']:
                query_uuid = str(uuid.uuid4())
                newWaapiQuery = q.cQuery(0, waapiQuery['query_info'], waapiQuery['query_name'], query_uuid)
                queryDictionary[query_uuid] = newWaapiQuery
                waapiQueries[query_uuid] = newWaapiQuery
        json_file.close()


def InitializeGUI():
    pass

#Start Up Chain
LoadSettings()
ConnectToWwise()
QueryLoadingHandler()
InitializeGUI()

myGUI = GUI.GUI_Connected()
w = win32gui

# Update GUI
def update_triggered_by_all(event):
    update_options("all")

def update_triggered_by_active(event):
    update_options("active")

def update_triggered_by_file(event):
    update_options("file")

def update_options(event):
    if event == "file":
        myGUI.set_notes(True)
    else:
        myGUI.set_notes(False)

    try:
        key = ""
        if event == "all":
            key = str(myGUI.all_tv.item(myGUI.all_tv.focus())['values'][1])
        elif event == "active":
            key = str(myGUI.active_tv.item(myGUI.active_tv.focus())['values'][1])
        elif event == "file":
            key = "file"

        query_type = get_query_type_name(queryDictionary[key].query_type)

        myGUI.set_option("name: " + str(queryDictionary[key].query_name), "type: " + query_type, "info: " + str(queryDictionary[key].query_info))
    except:
        myGUI.set_option("name: ", "type: ", "info: ")

def update_file_treeview():
    marked_files_dictionary.clear()
    counter = 0

    for i in myGUI.file_tv.get_children():
        myGUI.file_tv.delete(i)

    for active_query in myGUI.active_tv.get_children():
        query_ref = myGUI.active_tv.item(active_query)['values'][2]
        query_key = myGUI.active_tv.item(active_query)['values'][1]
        new_query_files = active_query_dictionary[query_key].run_query(client)['return']

        for file in new_query_files:
            try:
                valid_color = file['color']
            except:
                valid_color = 0

            try:
                new_ref = str(myGUI.file_tv.item(file['id'])['values'][2]) + ", " + str(query_ref)
                myGUI.file_tv.item(file['id'], values=(file['name'].replace(" ", "\ "), file['id'], new_ref))
            except:
                myGUI.file_tv.insert(parent='', iid=file['id'], index=counter, values=(file['name'].replace(" ", "\ "), file['id'], query_ref), tags=(valid_color,))
                marked_files_dictionary[file['id']] = file
                counter += 1

    myGUI.file_tv.heading(1, text="Name (" + str(counter) + ")")
    myGUI.root.update()

def update_active_treeview():
    counter = 0
    for i in myGUI.active_tv.get_children():
        myGUI.active_tv.delete(i)

    for key in active_query_dictionary.keys():
        myGUI.active_tv.insert(parent='', index='end', values=(active_query_dictionary[key].query_name.replace(" ","\ "), key, counter))
        counter += 1
    myGUI.root.update()


# Handle treeview selection
def handle_file_selection(event):
    file_key = str(myGUI.file_tv.item(myGUI.file_tv.focus())['values'][1])
    if file_key in marked_files_dictionary.keys():
        openHierarchy = {"command": "FindInProjectExplorerSyncGroup1", "objects":
            [
                str(file_key)
            ]
                         }
        openProperty = {"command": "Inspect", "objects":
            [
                str(file_key)
            ]
                        }
        client.call("ak.wwise.ui.commands.execute", openProperty)
        client.call("ak.wwise.ui.commands.execute", openHierarchy)

def handle_all_selection(event):
    try:
        all_key = str(myGUI.all_tv.item(myGUI.all_tv.focus())['values'][1])
        if all_key not in active_query_dictionary.keys():
            active_query_dictionary[all_key] = queryDictionary[all_key]
            update_active_treeview()
            update_file_treeview()
    except:
        pass

def handle_active_selection(event):
    try:
        active_key = str(myGUI.active_tv.item(myGUI.active_tv.focus())['values'][1])
        if active_key in active_query_dictionary.keys():
            del active_query_dictionary[active_key]
            update_active_treeview()
            update_file_treeview()
    except:
        pass

def handle_cross_selection(event):
    for item in myGUI.all_tv.selection():
        myGUI.all_tv.selection_remove(myGUI.all_tv.selection()[0])

    for item in myGUI.active_tv.selection():
        myGUI.active_tv.selection_remove(myGUI.active_tv.selection()[0])

    for item in myGUI.file_tv.selection():
        myGUI.file_tv.selection_remove(myGUI.file_tv.selection()[0])

# Custom QueryModule Handling
def delete_custom_query():
    try:
        all_key = str(myGUI.all_tv.item(myGUI.all_tv.focus())['values'][1])

        if queryDictionary[all_key].query_type == 2:
            del queryDictionary[all_key]
            del waqlQueries[all_key]

            if all_key in active_query_dictionary.keys():
                del active_query_dictionary[all_key]

            save_to_json()

            myGUI.all_tv.delete(all_key)

            update_active_treeview()
            update_file_treeview()
            myGUI.set_option("", "", "")

    except:
        pass

def rename_custom_query():
    all_key = str(myGUI.all_tv.item(myGUI.all_tv.focus())['values'][1])
    is_key_in_dic = False

    for key in queryDictionary.keys():
        if all_key == key:
            is_key_in_dic = True

    if is_key_in_dic:
        unique_id = str(uuid.uuid4())
        updated_query = q.cQuery(2, queryDictionary[all_key].query_info, str(myGUI.rename_entry.get()), unique_id)
        del queryDictionary[all_key]
        del waqlQueries[all_key]
        waqlQueries[unique_id] = updated_query
        queryDictionary[unique_id] = updated_query
        save_to_json()

        myGUI.all_tv.delete(all_key)
        myGUI.all_tv.insert(parent=custom_query_uuid, index='end', iid=unique_id, values=((updated_query.query_name).replace(" ", "\ "), updated_query.query_uuid))

        try:
            del active_query_dictionary[all_key]
            active_query_dictionary[unique_id] = updated_query
            update_active_treeview()
        except:
            pass

    myGUI.rename_entry.delete(0, 'end')

def create_new_custom_query():
    is_key_in_dic = False

    if myGUI.query_name_input.get()!="":
        for key in queryDictionary.keys():
            if key == myGUI.query_name_input.get():
                is_key_in_dic = True

        if not is_key_in_dic:
            unique_id = str(uuid.uuid4())
            create_query = q.cQuery(2, myGUI.query_input.get(), myGUI.query_name_input.get(), unique_id)
            waqlQueries[unique_id] = create_query
            queryDictionary[unique_id] = create_query
            save_to_json()
            myGUI.all_tv.insert(parent=custom_query_uuid, index='end', iid=unique_id, values=((create_query.query_name).replace(" ", "\ "), create_query.query_uuid))

        myGUI.query_input.delete(0,'end')
        myGUI.query_name_input.delete(0,'end')


# Utility
def open_multiditor():

    if "entry" in str(myGUI.root.focus_get()):
        print("Is Typing")
    else:
        if str(w.GetWindowText(w.GetForegroundWindow())) == "QueryModule Extension":
            multieditorARGS= {"command": "ShowMultiEditor", "objects":
                              [

                              ]}
            for file in myGUI.file_tv.selection():
                single_file = myGUI.file_tv.item(file)
                multieditorARGS["objects"].append(single_file['values'][1])

            client.call("ak.wwise.ui.commands.execute", multieditorARGS)

def multi_add_note():
    if "text" in str(myGUI.root.focus_get()):
        print("Is Typing")
    else:
        for file in myGUI.file_tv.selection():
            single_file = myGUI.file_tv.item(file)

            query_results = client.call("ak.wwise.core.object.get", {"from": {"id": [single_file['values'][1]]}}, options={'return': ['notes']})['return']
            note_args = "error"
            if query_results[0]['notes'] == "":
                note_args = {"object": single_file['values'][1], "value": myGUI.note_text.get(1.0, 'end')}
            else:
                note_args ={"object": single_file['values'][1], "value": str(query_results[0]['notes'] +"\n"+ myGUI.note_text.get(1.0, 'end'))}

            client.call("ak.wwise.core.object.setNotes", note_args)
        myGUI.note_text.delete(1.0, 'end')

def save_to_json():
    try:
        with open('Data/CustomQueries.json') as data_file:
            delete_data = json.load(data_file)
            delete_data.clear()
    except:
        pass

    saving_data = {}
    saving_data['waqlQueries'] = []
    for waqlQuery in waqlQueries.keys():
        saving_data['waqlQueries'].append({
            'query_info': waqlQueries[waqlQuery].query_info,
            'query_name': waqlQueries[waqlQuery].query_name,
            'query_type': int(waqlQueries[waqlQuery].query_type)
        })
    for waapiQuery in waapiQueries.keys():
        saving_data['waapiQueries'].append({
            'query_info': waapiQueries[waapiQuery].query_info,
            'query_name': waapiQueries[waapiQuery].query_name,
            'query_type': int(waapiQueries[waapiQuery].query_type)
        })

    with open('Data/CustomQueries.json', 'w') as out_file:
        json.dump(saving_data, out_file)

def get_query_type_name(query_type_int):
    query_type = ""
    if query_type_int == 0:
        query_type = "Hardcoded QueryModule"
    elif query_type_int == 1:
        query_type = "Wwise QueryModule"
    elif query_type_int == 2:
        query_type = "Modified QueryModule"
    return query_type

def get_right_p4_file(file):
    #print(file['type'], file['filePath'])
    if file['type'] == "Sound":
        return file['originalWavFilePath']
    else:
        if file['type'] == "WorkUnit":
            if ".wwu" not in file['filePath']:
                return ""
        return file['filePath']

def save_query_result(event):
    data = {}
    data['info'] = {"timestamp": str(datetime.now()), "queries": [], "results": []}

    os.popen('p4 set P4CLIENT="pknohl_11330_re-main"')
    os.popen('p4 set P4PORT="p4-renaissance-bbd-proxy:3980"')
    os.popen('p4 set P4USER="pknohl"')
    os.popen('p4 sync "S:\\RE\\audio\\..."')

    for query in active_query_dictionary:
        myquery = active_query_dictionary[query]
        data['info']['queries'].append({"name": myquery.query_name, "id": query, "query_type": get_query_type_name(myquery.query_type)})

    for file in marked_files_dictionary:
        p = get_right_p4_file(marked_files_dictionary[file])
        command = p.replace("S:\\RE\\audio\\sound", "").replace("\\", "/") + "\""
        if p != "":
            submit_list = os.popen('p4 filelog "//renaissance-audio/dev/main/source/sound' + command)
            lines = submit_list.readlines()
            submit_name = ""
            for char in lines[1]:
                if char == " ":
                    submit_name = ""
                elif char == "@":
                    break
                else:
                    submit_name += char
            data['info']['results'].append({"name": marked_files_dictionary[file]['name'], 'id': marked_files_dictionary[file]['id'], 'wwu': marked_files_dictionary[file]['workunit']['name'], 'path': marked_files_dictionary[file]['filePath'], 'last_submit': submit_name})

    with open("querylog/Query_" + str(datetime.now()).replace(":", ",").replace(" ", "_") + ".json", "w", encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=4)

if False:
    print('rf')
    set_all_query_tree()

    myGUI.active_tv.bind('<Button-1>', handle_cross_selection)
    myGUI.all_tv.bind('<Button-1>', handle_cross_selection)
    myGUI.file_tv.bind('<Button-1>', handle_cross_selection)

    myGUI.result_Button.bind('<Button-1>', save_query_result)

    myGUI.all_tv.bind('<<TreeviewSelect>>', update_triggered_by_all)
    myGUI.active_tv.bind('<<TreeviewSelect>>', update_triggered_by_active)
    myGUI.file_tv.bind('<<TreeviewSelect>>', update_triggered_by_file)

    myGUI.file_tv.bind('<Double-1>', handle_file_selection)
    myGUI.active_tv.bind('<Double-1>', handle_active_selection)
    myGUI.all_tv.bind('<Double-1>', handle_all_selection)
    myGUI.query_Button['command'] = create_new_custom_query
    myGUI.delete_query_button['command'] = delete_custom_query
    myGUI.rename_query_button['command'] = rename_custom_query
    keyboard.on_press_key('m', lambda _:open_multiditor())
    keyboard.on_press_key('n', lambda _:multi_add_note())

myGUI.root.mainloop()
save_to_json()