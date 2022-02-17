import json

class cSettings():
    def __init__(self):
        self.wwiseAuthoringPath = ""
        self.wwiseProjectPath = ""
        self.customQueryPath = ""
        self.initialModule = ""

    def SetSettings(self, wwiseProjectPath, wwiseAuthoringPath, customQueryPath, initialModule):
        self.wwiseProjectPath = wwiseProjectPath
        self.wwiseAuthoringPath = wwiseAuthoringPath
        self.customQueryPath = customQueryPath
        self.initialModule = initialModule

    def SaveSettings(self, newProjectPath, newWwiseAuthingPath, newCustomPath):
        file = open('Data/Settings.json', 'r')
        settings = json.load(file)

        data = {
                    "WwiseAuthoringPath": newProjectPath,
                    "WwiseProjectPath": newWwiseAuthingPath,
                    "CustomQueryPath": newCustomPath,
                    "InitialModule": "QUERYMODULE"
                }
        file.close()
        file = open('Data/Settings.json', 'w')
        json.dump(data, file)
        file.close()
