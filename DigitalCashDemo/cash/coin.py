import json

class CoinInWallet:
    """
    ウォレットにあるコインを表すクラス。
    コインの送信、預金時に必要なパラメータも共に格納。    
    """
    def __init__(self,coin,coin_secret,spending_params):        
        assert isinstance(coin,Coin)
        assert isinstance(coin_secret,CoinSecret)
        assert isinstance(spending_params,(SpendingParams,type(None)))
        
        self.coin = coin # (A,B,z,a,b,r)
        self.coin_secret = coin_secret # (s,x1,x2,a1,a2)
        self.spending_params = spending_params # (r1,r2,d)

    def __str__(self):
        return "{%s|%s|%s}"%(str(self.coin),str(self.coin_secret),str(self.spending_params))

    def __repr__(self):
        return str(self)

class SpendingParams:
    """
    コインの預金時に必要なパラメータを格納
    """
    def __init__(self,r1,r2,d):
        self.r1 = r1
        self.r2 = r2
        self.d = d
        
    def __str__(self):
        return "(%d,%d,%d)"%(self.r1,self.r2,self.d)

    def __repr__(self):
        return str(self)
    
    def dumps(self):
        # 内容をリストにして返す。
        return [self.r1,self.r2,self.d]

    @staticmethod
    def loads(l):
        # リストからSpendingParamsを作る
        assert isinstance(l,list)
        return SpendingParams(l[0],l[1],l[2])
    
class CoinSecret:
    """
    コインの送信時に必要なパラメータを格納
    """
    def __init__(self,s,x1,x2,a1,a2):
        self.s = s
        self.x1 = x1
        self.x2 = x2
        self.a1 = a1
        self.a2 = a2
        
    def __str__(self):
        return "(%d,%d,%d,%d,%d)"%(self.s,self.x1,self.x2,self.a1,self.a2)

    def __repr__(self):
        return str(self)

    def dumps(self):
        # 内容をリストにして返す。
        return [self.s,self.x1,self.x2,self.a1,self.a2]

    @staticmethod
    def loads(l):
        # リストからCoinSecretを作る
        assert isinstance(l,list)
        return CoinSecret(l[0],l[1],l[2],l[3],l[4])

class Coin:
    """
    コインを表すクラス。
    """
    def __init__(self,A,B,z,a,b,r):
        self.A = A
        self.B = B
        self.z = z
        self.a = a
        self.b = b
        self.r = r

    def __str__(self):
        return "(%d,%d,%d,%d,%d,%d)"%(self.A,self.B,self.z,self.a,self.b,self.r)

    def __repr__(self):
        return str(self)

    def dumps(self):
        # 内容をリストにして返す。
        return [self.A,self.B,self.z,self.a,self.b,self.r]

    @staticmethod
    def loads(l):
        # リストからCoinを作る
        assert isinstance(l,list)
        return Coin(l[0],l[1],l[2],l[3],l[4],l[5])
