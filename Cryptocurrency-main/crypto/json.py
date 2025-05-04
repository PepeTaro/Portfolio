import json

def json_dumps(cls):
    return json.dumps(cls,default=lambda o: o.__dict__,indent=1)
