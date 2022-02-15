class cSettings():
    def __init__(self):
        self.wwiseAuthoringPath = ""
        self.wwiseProjectPath = ""
        self.customQueryPath = ""
        self.initialModule = "QUERYMODULE"

    def SetSettings(self, wwiseProjectPath, wwiseAuthoringPath, customQueryPath, initialModule):
        self.wwiseProjectPath = wwiseProjectPath
        self.wwiseAuthoringPath = wwiseAuthoringPath
        self.customQueryPath = customQueryPath
        self.initialModule = initialModule