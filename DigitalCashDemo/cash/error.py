from enum import Enum

class Error(Enum):
    """
    エラーを表すEnum
    """
    
    SUCCESS = 0      # 成功
    REGISTER = 1     # 登録エラー
    NO_COIN = 2      # コインがウォレットに存在しないエラー
    USED_COIN = 3    # 使用済みコインであるエラー
    INVALID_COIN = 4 # 正当でないコインを使用したエラー
    DOUBLE_SPENT = 5 # ダブルスペンドを検知したときのエラー
    INDEX = 6        # インデックスが正しくないエラー
    OTHERS = 7       # その他のエラー
