from .error import Error

class Status:
    """
    エラーとデータを格納するラッパークラス。
    """
    
    def __init__(self,error,data=None):
        self.error = error

        if data is not None:
            self.data  = [data]
        else:
            self.data = []

    def has_data(self):
        return len(self.data) == 0

    def push_data(self,_data):
        self.data.append(_data)
