class cQuery():
    def __init__(self, query_type, query_info, name, uuid, parent):
        self.query_type = query_type
        self.query_info = query_info
        self.query_name = name
        self.query_uuid = uuid
        self.query_parents = parent

    def run_query(self, client):
        query_results = None

        if self.query_type == 0:
            query_results = client.call("ak.wwise.core.object.get", self.query_info,
                                        options={'return': ['name', 'id', 'path', 'color', 'type', 'workunit', 'filePath', 'originalWavFilePath']})
        elif self.query_type == 1:
            arg = '$ from query "{}"'.format(self.query_info)
            args = {'waql': arg}
            query_results = client.call("ak.wwise.core.object.get", args, options={'return': ['name', 'id', 'path', 'color', 'type', 'workunit', 'filePath', 'originalWavFilePath']})

        elif self.query_type == 2:
            args = {'waql': self.query_info}
            query_results = client.call("ak.wwise.core.object.get", args, options={'return': ['name', 'id', 'path', 'color', 'type', 'workunit', 'filePath', 'originalWavFilePath']})

        return query_results