from datetime import datetime

_DATE_FORMAT = "%Y/%m/%d, %H:%M:%S"

def get_timestamp() -> str:
    """ タイムスタンプを_DATE_FORMATで指定されたフォーマットで返す """
    now = datetime.now()
    date_time = now.strftime(_DATE_FORMAT)
    return date_time
