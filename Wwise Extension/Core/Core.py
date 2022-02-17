import waapi.wamp.interface
from waapi import WaapiClient
import os
import json
from Settings import Settings
from GUI import GUI
from Data import ModuleEnum as m
from ExtensionModules.QueryModule import QueryModule

class cCore():
    def __init__(self):
        self.coreSettings = self.GetSettings()
        self.client = self.GetWwiseClient()
        self.coreQueryModule = QueryModule.cQueryModule(self)
        self.coreGUI = self.InitGUI()


    def GetSettings(self):
        tempSettings = Settings.cSettings()
        file = open('Data/Settings.json')
        settings = json.load(file)
        print(settings['Settings']['InitialModule'])
        tempSettings.SetSettings(settings['Settings']['WwiseProjectPath'], settings['Settings']['WwiseAuthoringPath'], settings['Settings']['CustomQueryPath'], settings['Settings']['InitialModule'])
        file.close()
        return tempSettings

    def GetWwiseClient(self):
        try:
            return WaapiClient()
        except waapi.wamp.interface.CannotConnectToWaapiException:
            try:
                commandline = '%s waapi-server %s --allow-migration --wamp-port 8080' % (
                '"' + self.coreSettings.wwiseAuthoringPath + '"', '"' + self.coreSettings.wwiseProjectPath + '"')
                os.popen(commandline)
                return WaapiClient()

            except waapi.wamp.interface.CannotConnectToWaapiException:
                print("Somethings wrong with the Settings. Console could not be opened")

    def InitGUI(self):
        tempGUI = GUI.cCoreGui(self)
        tempGUI.ChangeLayout(m.Modules[self.coreSettings.initialModule])
        return tempGUI

    def CloseCore(self):
        print("Disconnected from Wwise.")
        self.coreGUI.root.quit()
        self.client.disconnect()

    def CoreLoop(self):
        self.coreGUI.root.protocol("WM_DELETE_WINDOW", self.CloseCore)
        self.coreGUI.root.mainloop()
