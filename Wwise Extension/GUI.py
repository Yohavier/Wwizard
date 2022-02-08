from tkinter import *
from tkinter import ttk


class GUI_Connected():
    def __init__(self):
        self.root = Tk()
        self.root.resizable(0, 0)

        self.root.title("Query Extension")
        self.modify_query_frame = ttk.Frame(self.root)
        self.modify_query_frame.grid(column=0, row=0, columnspan=3, rowspan=1, sticky="nesw")
        self.all_query_frame = ttk.Frame(self.root)
        self.all_query_frame.grid(column=0, row=1, columnspan=1, rowspan=1, sticky="nesw")
        self.active_query_frame = ttk.Frame(self.root)
        self.active_query_frame.grid(column=0, row=2, columnspan=1, rowspan=1, sticky="nesw")
        self.query_options_frame = ttk.Frame(self.root)
        self.query_options_frame.grid(column=1, row=1, columnspan=1, rowspan=2, sticky="nesw")
        self.file_display_frame = ttk.Frame(self.root)
        self.file_display_frame.grid(column=2, row=1, columnspan=1, rowspan=2, sticky="nesw")

        self.root.rowconfigure(0, weight=1)
        self.root.columnconfigure(0, weight=1)

        ######################
        #modified query input#
        ######################
        self.query_name_label = ttk.Label(self.modify_query_frame, text="Query Name:", width=12)
        self.query_name_label.grid(row=0, column=0, sticky="nesw")
        self.query_name_input = ttk.Entry(self.modify_query_frame, width=35)
        self.query_name_input.grid(row=0, column=1)
        self.query_input_label = ttk.Label(self.modify_query_frame, text="Query:", width=6)
        self.query_input_label.grid(row=0, column=2, sticky="nesw")
        self.query_input = ttk.Entry(self.modify_query_frame, width=110)
        self.query_input.grid(row=0, column=3)
        self.query_Button = ttk.Button(self.modify_query_frame, text='Create Query', takefocus=False)
        self.query_Button.grid(row=0, column=4)
        self.result_Button = ttk.Button(self.modify_query_frame, text='Results', takefocus=False)
        self.result_Button.grid(row=0, column=5)

        ##################
        #all queries view#
        ##################
        self.all_tv = ttk.Treeview(self.all_query_frame, columns=(1, 2), displaycolumns=1)
        self.all_tv.column('#0', width=60)
        self.all_tv.heading('#0', text="Name", anchor=W)
        self.all_tv.column(1, width=250)
        sb = ttk.Scrollbar(self.all_query_frame, orient=VERTICAL)
        sb.pack(side=RIGHT, fill=Y)
        self.all_tv.pack(fill=BOTH)
        self.all_tv.config(yscrollcommand=sb.set)
        sb.config(command=self.all_tv.yview)

        #####################
        #active queries view#
        #####################
        self.active_tv = ttk.Treeview(self.active_query_frame, columns=(1, 2, 3), show='headings', displaycolumns=(1,3))
        self.active_tv.column(1, width=250)
        self.active_tv.column(3, width=10)
        sb = ttk.Scrollbar(self.active_query_frame, orient=VERTICAL)
        sb.pack(side=RIGHT, fill=Y)
        self.active_tv.pack(fill=BOTH)
        self.active_tv.heading(3, text="Ref", anchor=W)
        self.active_tv.heading(1, text="Name", anchor=W)
        self.active_tv.config(yscrollcommand=sb.set)
        sb.config(command=self.active_tv.yview)

        ###########
        #file view#
        ###########
        self.file_tv = ttk.Treeview(self.file_display_frame, columns=(1, 2, 3), show='headings', selectmode='extended', height=22, displaycolumns=(1, 3))
        self.file_tv.column(1, width=450)
        sb = ttk.Scrollbar(self.file_display_frame, orient=VERTICAL)
        sb.pack(side=RIGHT, fill=Y)
        self.file_tv.pack(fill=BOTH)
        self.file_tv.heading(1, text="Name", anchor=W)
        self.file_tv.heading(2, text="guid")
        self.file_tv.heading(3, text="Ref", anchor=W)
        self.file_tv.column(3, width=50)
        self.file_tv.config(yscrollcommand=sb.set)
        sb.config(command=self.file_tv.yview)


        ##############
        #options view#
        ##############
        self.options_label = ttk.Label(self.query_options_frame, text="Options", anchor=W, font=13, padding=2)
        self.options_label.pack(fill=X)
        self.options_text = Frame(self.query_options_frame)
        self.options_text.pack(side=TOP, fill=BOTH)

        self.text_query_name = Text(self.options_text, height=5, width=45, background='#262626', foreground='#bdbdbd', borderwidth=0)
        self.text_query_name.config(state=DISABLED)
        self.text_query_name.pack(fill=X)
        self.label_query_type = Text(self.options_text, height=5, width=45, background='#262626', foreground='#bdbdbd', borderwidth=0)
        self.label_query_type.config(state=DISABLED)
        self.label_query_type.pack(fill=X)
        self.label_query_info = Text(self.options_text, height=5, width=45, background='#262626', foreground='#bdbdbd', borderwidth=0)
        self.label_query_info.config(state=DISABLED)
        self.label_query_info.pack(fill=X)

        self.delete_query_button = ttk.Button(self.options_text, text='Delete', takefocus=False)
        self.delete_query_button['state'] = DISABLED
        self.delete_query_button.pack(fill=X)

        self.rename_frame = ttk.Frame(self.options_text, padding=10)
        self.rename_frame.pack(fill=X)
        self.rename_label = ttk.Label(self.rename_frame, text="New Name: ")
        self.rename_label.pack(side=LEFT)
        self.rename_entry = ttk.Entry(self.rename_frame)
        self.rename_entry['state'] = DISABLED
        self.rename_entry.pack(side=LEFT)
        self.rename_query_button = ttk.Button(self.rename_frame, text='Rename', takefocus=False)
        self.rename_query_button['state'] = DISABLED
        self.rename_query_button.pack(side=LEFT)

        self.note_label = ttk.Label(self.options_text, padding=5, text="Notes Editor")
        self.note_label.pack(fill=X)
        self.note_text = Text(self.options_text, height=5, width=45, background='#262626', foreground='#bdbdbd', borderwidth=0)
        self.note_text.pack(side=LEFT)

        self.style = ttk.Style()
        self.style.theme_use("clam")

        #Button style
        self.style.configure("TButton",foreground='#bdbdbd', background="#5a5a5a", borderwidth=1, bordercolor='#838383', lightcolor='#838383', darkcolor='#838383')
        self.style.map('TButton', background=[('pressed', '#2b2b2b'), ('active', '#727272')], bordercolor=[('active', '#6b6b6b')])

        #Frame style
        self.style.configure('TFrame', background='#464646', bordercolor='#2f2f2f')

        #Label style
        self.style.configure('TLabel', background='#464646', foreground='#bdbdbd')

        #Entry style
        self.style.configure('TEntry', foreground='#bdbdbd', background='#464646', fieldbackground='#373737', bordercolor='#292929', lightcolor='#292929', darkcolor='#292929')
        #Treeview style
        self.style.configure('Treeview', fieldbackground='#262626', background='#262626', foreground='#bdbdbd', bordercolor='#464646', lightcolor='#464646', darktcolor='#464646')
        self.style.configure('Treeview.Heading', font=13, background='#464646', foreground='#bdbdbd', borderwidth=0)
        self.style.map('Treeview.Heading', background=[('pressed', '#464646'), ('active', '#464646')])

        self.style.configure("Vertical.TScrollbar", background='#747474', troughcolor='#424242', bordercolor='#464646', arrowcolor='black', gripcount=0, lightcolor='#464646', darkcolor='#464646')
        self.style.map("Vertical.TScrollbar", background=[('active', '#8d8d8d')])
        self.style.map("Treeview", background=[("selected", '#db9850')])
        self.style.layout("Treeview.Item", [('Treeitem.padding', {'sticky': 'nswe', 'children': [('Treeitem.indicator', {'side': 'left', 'sticky': ''}),
                          ('Treeitem.image', {'side': 'left', 'sticky': ''}), # ('Treeitem.focus', {'side': 'left', 'sticky': '', 'children': [
                          ('Treeitem.text', {'side': 'left', 'sticky': ''}), #]})
                          ], })])
        self.note_text.config(state=DISABLED)

        #treeview color tags
        self.file_tv.tag_configure('0', background="#646464")
        self.file_tv.tag_configure('1', background="#363ec8")
        self.file_tv.tag_configure('2', background="#1955cb")
        self.file_tv.tag_configure('3', background="#076868")
        self.file_tv.tag_configure('4', background="#186d18")
        self.file_tv.tag_configure('5', background="#56730c")
        self.file_tv.tag_configure('6', background="#787110")
        self.file_tv.tag_configure('7', background="#795715")
        self.file_tv.tag_configure('8', background="#78420c")
        self.file_tv.tag_configure('9', background="#72382b")
        self.file_tv.tag_configure('10', background="#892324")
        self.file_tv.tag_configure('11', background="#7c267d")
        self.file_tv.tag_configure('12', background="#732a97")
        self.file_tv.tag_configure('13', background="#5836ae")
        self.file_tv.tag_configure('14', background="#878787")
        self.file_tv.tag_configure('15', background="#6a6fc2")
        self.file_tv.tag_configure('16', background="#6383c5")
        self.file_tv.tag_configure('17', background="#438989")
        self.file_tv.tag_configure('18', background="#539353")
        self.file_tv.tag_configure('19', background="#80983d")
        self.file_tv.tag_configure('20', background="#a09726")
        self.file_tv.tag_configure('21', background="#ab873e")
        self.file_tv.tag_configure('22', background="#ae7941")
        self.file_tv.tag_configure('23', background="#ae6455")
        self.file_tv.tag_configure('24', background="#b95b5b")
        self.file_tv.tag_configure('25', background="#a950aa")
        self.file_tv.tag_configure('26', background="#9a55bd")
        self.file_tv.tag_configure('27', background="#8660e2")

    def set_option(self, query_name, query_type, query_info):
        self.text_query_name.config(state=NORMAL)
        self.text_query_name.delete(1.0, END)
        self.text_query_name.insert('end', query_name)
        self.text_query_name.config(state=DISABLED)

        self.label_query_type.config(state=NORMAL)
        self.label_query_type.delete(1.0, END)
        self.label_query_type.insert('end', query_type)
        self.label_query_type.config(state=DISABLED)

        self.label_query_info.config(state=NORMAL)
        self.label_query_info.delete(1.0, END)
        self.label_query_info.insert('end', query_info)
        self.label_query_info.config(state=DISABLED)

        if query_type == "type: Modified Query":
            self.delete_query_button['state'] = NORMAL
            self.rename_query_button['state'] = NORMAL
            self.rename_entry['state'] = NORMAL
        else:
            self.delete_query_button['state'] = DISABLED
            self.rename_query_button['state'] = DISABLED
            self.rename_entry['state'] = DISABLED

    def set_notes(self, active):
        if active == True:
            self.note_text.config(state=NORMAL)
        else:
            self.note_text.config(state=DISABLED)


class GUI_NotFound():
    def __init__(self):
        self.root = Tk()
        self.root.resizable(0, 0)