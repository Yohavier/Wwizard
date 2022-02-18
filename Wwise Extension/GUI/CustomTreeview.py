from tkinter import *
from tkinter import ttk
from Data import CustomColours as cc
from Core import HelperFunctions as h

class cCustomTreeview():
    def __init__(self, masterGUI):
        self.tv = ttk.Treeview(masterGUI)
        self.tv['columns'] = ('Type', 'QueryRef')
        self.tv.column('#0')
        self.tv.column('Type')
        self.tv.column('QueryRef')

        self.tv.heading('#0', text='File Name')
        self.tv.heading('Type', text='Type')
        self.tv.heading('QueryRef', text='QueryRef')


        self.style = ttk.Style()
        self.style.theme_use("clam")

        """Overall Treeview container style"""
        self.style.configure('Treeview', fieldbackground='#262626', background='#262626', foreground='#bdbdbd',
                             bordercolor=h._from_rgb(cc.black), lightcolor=h._from_rgb(cc.black),
                             darkcolor=h._from_rgb(cc.black))

        """Heading doesnt change color on click"""
        self.style.configure('Treeview.Heading', padding=5, font=15, background=h._from_rgb(cc.grey),
                             foreground=h._from_rgb(cc.lightGrey), borderwidth=1, bordercolor=h._from_rgb(cc.greyedOut),
                             relief=SOLID)
        self.style.map('Treeview.Heading', background=[('pressed', h._from_rgb(cc.grey)), ('active', h._from_rgb(cc.grey))])

        self.style.configure('Treeview.Column', borderwidth=1, background=h._from_rgb(cc.yellow))
        """Treeview cell"""
        self.style.map("Treeview", background=[("selected", h._from_rgb(cc.yellow))])
        self.style.layout("Treeview.Item", [
            ('Treeitem.padding', {'sticky': 'nswe', 'children': [('Treeitem.indicator', {'side': 'left', 'sticky': ''}),
                                                                 ('Treeitem.image', {'side': 'left', 'sticky': ''}),
                                                                 ('Treeitem.text', {'side': 'left', 'sticky': ''}), ], })])


    def UpdateTreeview(self, queryList):
        i = 0
        for query in queryList:
            prevParent = ''
            for parent in queryList[query].query_parents:
                try:
                    self.tv.insert(parent=prevParent, index=i, iid=parent[0], text= parent[2], values=(parent[1], ''))
                    prevParent = parent[0]
                    i += 1
                except:
                    prevParent = parent[0]

            self.tv.insert(parent=prevParent, index=i, iid=query, text= queryList[query].query_name, values=('',''))
            i+=1