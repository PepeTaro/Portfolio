from time import time
from datetime import datetime

def timestamp_int():
    """
    現在時刻からタイムスタンプを返す。
    @return: int
    """
    a = datetime.now()
    return int(a.strftime("%Y%m%d%H%M%S%f"))
