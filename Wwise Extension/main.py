import datetime
import os

import MyQuery as q
from waapi import WaapiClient
import GUI
import json
import keyboard
import uuid
import win32gui
from datetime import datetime
import subprocess

# TODO
# 1. Hardcoded Queries readable from json
# 2. open nuendo, edit file
# 3. logical gates for combining queries
# 4. update on Wwise changes
# 5. edit custom query input
# 6. also block stuff, when nothing is selected
# 7. Settings to select directory etc.
# 8. failed to connect screen

is_connected = False
# Connect (default URL)
try:
    client = WaapiClient()
    myGUI = GUI.GUI_Connected()
    is_connected = True
except:
    try:
        os.popen('"YourPath\\Wwise\\Authoring\\x64\Release\\bin\\WwiseConsole.exe" waapi-server "PathToYourWwiseProject\\YourWwiseProject.wproj" --allow-migration --wamp-port 8080')
        client = WaapiClient()
        myGUI = GUI.GUI_Connected()
        is_connected = True
    except:
        myGUI = GUI.GUI_NotFound()



w = win32gui

all_query_dictionary = {}
custom_query_dictionary = {}
hardcoded_queries = []

active_query_dictionary = {}
marked_files_dictionary = {}

hardcoded_query_uuid = str(uuid.uuid4())
custom_query_uuid = str(uuid.uuid4())
wwise_query_uuid = str(uuid.uuid4())


# create all treeview structure
def set_all_query_tree():
    myGUI.all_tv.insert(parent='', index='end', iid=wwise_query_uuid, values=("Wwise Queries").replace(" ","\ "))
    get_all_wwise_queries('\\Queries', wwise_query_uuid)

    myGUI.all_tv.insert(parent='', index='end', iid=custom_query_uuid, values=("Custom Queries").replace(" ","\ "))
    get_all_custom_queries(custom_query_uuid)

    myGUI.all_tv.insert(parent='', index='end', iid=hardcoded_query_uuid, values=("Hardcoded Queries").replace(" ", "\ "))
    get_all_hardcoded_queries(hardcoded_query_uuid)

def get_all_wwise_queries(path, list_parent):
    wwise_queries = q.myQuery(0, {"from": {"path": [path]}, "transform": [{"select": ['children']}]}, "wwise_query_collector", str(uuid.uuid4()))

    for output in wwise_queries.run_query(client)['return']:
        if str(output['type']) == 'WorkUnit':
            unique_id = str(uuid.uuid4())
            myGUI.all_tv.insert(parent=list_parent, index='end', iid=unique_id, values=output['name'].replace(" ","\ "))
            get_all_wwise_queries(path + '\\' + output['name'], unique_id)

        elif str(output['type']) == 'Folder':
            unique_id = str(uuid.uuid4())
            myGUI.all_tv.insert(parent=list_parent, index='end', iid=unique_id, values=output['name'].replace(" ","\ "))
            get_all_wwise_queries(path + '\\' + output['name'], unique_id)

        elif str(output['type'] == 'Query'):
            unique_id = str(uuid.uuid4())
            myGUI.all_tv.insert(parent=list_parent, index='end', iid=unique_id, values=(output['name'].replace(" ", "\ "), unique_id))
            #all incoming wwise queries
            all_query_dictionary[unique_id] = q.myQuery(1, output['path'], output['name'], unique_id)

def get_all_custom_queries(list_parent):
    with open('custom_query_data.json', 'r') as json_file:
        data = json.load(json_file)
        for query in data['queries']:
            query_uuid = str(uuid.uuid4())
            new_query = q.myQuery(2, query['query_info'], query['query_name'], query_uuid)
            myGUI.all_tv.insert(parent=list_parent, index='end', iid=query_uuid, values=((new_query.query_name).replace(" ", "\ "), query_uuid))
            all_query_dictionary[query_uuid] = new_query
            custom_query_dictionary[query_uuid] = new_query

def get_all_hardcoded_queries(list_parent):
    hardcoded_queries.append(q.myQuery(0, {"from": {"path": ['\\Queries\\Default Work Unit']},"transform": [{"select": ['descendants']},{"where": ['type:isIn', ['Query']]}]}, "example", str(uuid.uuid4())))
    for hardcoded_query in hardcoded_queries:
        all_query_dictionary[hardcoded_query.query_uuid] = hardcoded_query
        myGUI.all_tv.insert(parent=list_parent, index='end', iid=hardcoded_query.query_uuid, values=((hardcoded_query.query_name).replace(" ", "\ "), hardcoded_query.query_uuid))


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

        query_type = get_query_type_name(all_query_dictionary[key].query_type)

        myGUI.set_option("name: " + str(all_query_dictionary[key].query_name), "type: " + query_type, "info: " + str(all_query_dictionary[key].query_info))
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
            active_query_dictionary[all_key] = all_query_dictionary[all_key]
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

# Custom Query Handling
def delete_custom_query():
    try:
        all_key = str(myGUI.all_tv.item(myGUI.all_tv.focus())['values'][1])

        if all_query_dictionary[all_key].query_type == 2:
            del all_query_dictionary[all_key]
            del custom_query_dictionary[all_key]

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

    for key in all_query_dictionary.keys():
        if all_key == key:
            is_key_in_dic = True

    if is_key_in_dic:
        unique_id = str(uuid.uuid4())
        updated_query = q.myQuery(2, all_query_dictionary[all_key].query_info, str(myGUI.rename_entry.get()), unique_id)
        del all_query_dictionary[all_key]
        del custom_query_dictionary[all_key]
        custom_query_dictionary[unique_id] = updated_query
        all_query_dictionary[unique_id] = updated_query
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
        for key in all_query_dictionary.keys():
            if key == myGUI.query_name_input.get():
                is_key_in_dic = True

        if not is_key_in_dic:
            unique_id = str(uuid.uuid4())
            create_query = q.myQuery(2, myGUI.query_input.get(), myGUI.query_name_input.get(), unique_id)
            custom_query_dictionary[unique_id] = create_query
            all_query_dictionary[unique_id] = create_query
            save_to_json()
            myGUI.all_tv.insert(parent=custom_query_uuid, index='end', iid=unique_id, values=((create_query.query_name).replace(" ", "\ "), create_query.query_uuid))

        myGUI.query_input.delete(0,'end')
        myGUI.query_name_input.delete(0,'end')


# Utility
def open_multiditor():

    if "entry" in str(myGUI.root.focus_get()):
        print("Is Typing")
    else:
        if str(w.GetWindowText(w.GetForegroundWindow())) == "Query Extension":
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
        with open('custom_query_data.json') as data_file:
            delete_data = json.load(data_file)
            delete_data.clear()
    except:
        pass

    saving_data = {}
    saving_data['queries'] = []
    for query in custom_query_dictionary.keys():
        saving_data['queries'].append({
            'query_info': custom_query_dictionary[query].query_info,
            'query_name': custom_query_dictionary[query].query_name,
            'query_type': int(custom_query_dictionary[query].query_type)
        })

    with open('custom_query_data.json', 'w') as out_file:
        json.dump(saving_data, out_file)

def get_query_type_name(query_type_int):
    query_type = ""
    if query_type_int == 0:
        query_type = "Hardcoded Query"
    elif query_type_int == 1:
        query_type = "Wwise Query"
    elif query_type_int == 2:
        query_type = "Modified Query"
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

if is_connected:
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