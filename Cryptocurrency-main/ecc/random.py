"""
[https://docs.python.org/3/library/secrets.html]
The secrets module is used for generating cryptographically strong random numbers suitable for managing data such as passwords, account authentication, security tokens, and related secrets.
つまりrandbelowはセキュアらしい。
"""

from secrets import randbelow

def secure_randbelow(n:int) -> int:
    """
    [0,n-1]の範囲にある乱数を返す。
    """
    return randbelow(n)
