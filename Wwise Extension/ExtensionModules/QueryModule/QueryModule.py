import json
import uuid
from waapi import WaapiClient
from ExtensionModules.QueryModule import Query as q

class cQueryModule():
    def __init__(self, core):
        self.core = core
        self.client = core.client
        self.queryDictionary = {}
        self.waqlQueries = {}
        self.waapiQueries = {}

    def LoadAllQueries(self):
        self.QueryLoadingHandler()
        return self.queryDictionary

    def QueryLoadingHandler(self):
        self.GetWwiseQueries('\\Queries', [(str(uuid.uuid4()), "", "Wwise Queries")])
        self.GetCustomQueries([(str(uuid.uuid4()), "", "Custom Queries")])

    def GetWwiseQueries(self, path, parentList):

        wwiseQueries = q.cQuery(0, {"from": {"path": [path]}, "transform": [{"select": ['children']}]},
                                "wwise_query_collector", str(uuid.uuid4()), parentList)

        for output in wwiseQueries.run_query(self.client)['return']:
            if str(output['type']) == 'WorkUnit':
                unique_id = str(uuid.uuid4())
                prnt = list(parentList)
                prnt.append((unique_id, output['type'], output['name']))
                self.GetWwiseQueries(path + '\\' + output['name'], prnt)

            elif str(output['type']) == 'Folder':
                unique_id = str(uuid.uuid4())
                prnt = list(parentList)
                prnt.append((unique_id, output['type'], output['name']))
                self.GetWwiseQueries(path + '\\' + output['name'], prnt)

            elif str(output['type'] == 'QueryModule'):
                unique_id = str(uuid.uuid4())
                prnt = list(parentList)
                self.queryDictionary[unique_id] = q.cQuery(1, output['path'], output['name'], unique_id, prnt)


    def GetCustomQueries(self, parentList):
        with open(self.core.coreSettings.customQueryPath, 'r') as json_file:
            # WAQL Queries
            data = json.load(json_file)
            prnt = list(parentList)
            prnt.append((str(uuid.uuid4()), 'Folder', 'WAQL Queries'))

            for waqlQuery in data['waqlQueries']:
                query_uuid = str(uuid.uuid4())
                newWaqlQuery = q.cQuery(2, waqlQuery['query_info'], waqlQuery['query_name'], query_uuid, prnt)
                self.queryDictionary[query_uuid] = newWaqlQuery
                self.waqlQueries[query_uuid] = newWaqlQuery

            # WAAPI Queries
            prnt = list(parentList)
            prnt.append((str(uuid.uuid4()), 'Folder', 'WAAPI Queries'))
            for waapiQuery in data['waapiQueries']:
                query_uuid = str(uuid.uuid4())
                newWaapiQuery = q.cQuery(0, waapiQuery['query_info'], waapiQuery['query_name'], query_uuid, prnt)
                self.queryDictionary[query_uuid] = newWaapiQuery
                self.waapiQueries[query_uuid] = newWaapiQuery
            json_file.close()