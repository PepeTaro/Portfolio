from .ntheory import sq_and_mul,rand_between,inv
from .coin import Coin,SpendingParams
from .database import connect,add_user_in_db,does_id_exist_in_db,does_have_coin_in_db,deposit_coin_into_db,deduct_coin_from_db,is_double_spent_in_db,store_used_coin_in_db,fetch_used_from_db,fetch_accounts_from_db

class Bank:
    def __init__(self,g,g1,g2,p,q):
        self._g = g
        self._g1 = g1
        self._g2 = g2
        self._p = p
        self._q = q
        self.w = None  # コイン生成に使用する一時変数。
        self.db = None # sqlite3.Connectionクラス
        
        self.init_params()
        self.init_db()

    def init_db(self):
        # データベースに接続
        self.db = connect()
        
    def init_params(self):
        # 必要なパラメータを初期化
        self._x = rand_between(1,self._p - 1)        
        self._h  = sq_and_mul(self._g,self._x,self._p)
        self._h1 = sq_and_mul(self._g1,self._x,self._p)
        self._h2 = sq_and_mul(self._g2,self._x,self._p)
        
    def add_user(self,ID,name,num_of_coins=10):
        """
        ユーザーを追加(登録)
        
        @param ID:id
        @apram name:str
        @return:int or None => ユーザーの追加に成功した場合、戻り値として送り手が後に使用するパラメータを返す。
        """
        assert isinstance(ID,int)
        assert isinstance(name,str)
        
        if does_id_exist_in_db(self.db,ID): # 与えられたIDがすでに存在する場合
            return None
        else:
            add_user_in_db(self.db,ID,name,num_of_coins) # データベースにユーザーを追加
            z = sq_and_mul(ID*self._g2,self._x,self._p)
            return z

    def deposite_coin(self,ID):
        """
        IDをもつアカウントにコインを追加(預金)
        @param ID:int
        """
        assert does_id_exist_in_db(self.db,ID)
        deposit_coin_into_db(self.db,ID) # コインをデータベースに預金
        
    def deduct_coin(self,ID):
        """
        IDをもつアカウントからコインをひとつ引き出す。
        @param ID:int
        """
        
        assert does_id_exist_in_db(self.db,ID)
        assert self.w is None # 一度に複数のコインを引き出そうとした場合ここで引っかかる。
        
        deduct_coin_from_db(self.db,ID) # コインをデータベースから引き出す。

        # コイン引き出しに必要なパラメータを計算
        self.w = rand_between(1,self._p - 1)
        gw = sq_and_mul(self._g,self.w,self._p)
        beta = sq_and_mul(ID*self._g2,self.w,self._p)

        return gw,beta

    def blind_coin(self,c):
        c1 = (c*self._x + self.w) % self._q
        self.w = None
        return c1

    def is_double_spent(self,coin,params):
        """
        コインがダブルスペンドされているかチェック。
        @param coin:Coin
        @param params:SpendingParams:
        @return:bool,Coin or None,SpendingParams or None => Coinはダブルスペンドされたコイン、SpendingParamsは後の計算に使用。
        """
        
        list_coin,list_params = is_double_spent_in_db(self.db,coin) # coinと同じデータをもつコインを探す(ダブルスペンドされたコイン)
        
        if list_coin is None and list_params is None: # coinがダブルスペンドされていない場合
            return False,None,None
        else:                                        # coinがダブルスペンドされている場合
            # リストからクラスに変換。
            dup_coin = Coin.loads(list_coin)
            dup_params = SpendingParams.loads(list_params)
            return True,dup_coin,dup_params

    def fraud_control(self,coin,params,dup_coin,dup_params):
        """
        ダブルスペンドしたIDを探す。

        @param coin:Coin
        @param params:SpendingParams
        @param dup_coin:Coin
        @param dup_params:SpendingParams
        @return:int or None => ダブルスペンドしたID
        """
        
        delta_r1 = params.r1 - dup_params.r1
        delta_r2 = params.r2 - dup_params.r2
        
        if delta_r2%self._q == 0: # IDを特定できなかった場合
            # self._qの値が小さいと、IDを見つけられない可能性が高くなる。
            print("[!] Error(fraud_control): IDを特定できませんでした。")
            return None
        else:                     # IDを特定できた場合。
            inv_delta_r2 = inv(delta_r2,self._q)
            u = (delta_r1*inv_delta_r2) % self._q
            ID = sq_and_mul(self._g1,u,self._p) # 特定したID
            return ID

    def do_i_have_coin(self,ID):
        """
        引数IDで指定されたアカウントがコインを持っているかチェック。
        @return:bool
        """
        assert does_id_exist_in_db(self.db,ID)
        return does_have_coin_in_db(self.db,ID)
        
    def fetch_accounts(self):
        """
        アカウントテーブルを取得。
        @return: dict => {ID:{"name":name,"coin":coin}}
        """
        cursor = fetch_accounts_from_db(self.db)
        records = {}
        for row in cursor:
            name = row[0]
            ID = row[1]
            coin = row[2]
            records[ID] = {}
            records[ID]["name"] = name
            records[ID]["coin"] = coin
        return records
    
    def fetch_used_coins(self):
        """
        使用済みコインテーブルを取得。
        @return: list => [coin,params]
        """
        cursor = fetch_used_from_db(self.db)
        used = [[row[0],row[1]] for row in cursor]
        return used
    
    def store_used_coin(self,coin,params):
        """
        使用済みコインをデータベースに保存。
        @param coin:Coin
        @param params:SpendingParams        
        """
        store_used_coin_in_db(self.db,coin,params)
        
    def g(self):
        return self._g

    def g1(self):
        return self._g1

    def g2(self):
        return self._g2

    def p(self):
        return self._p

    def q(self):
        return self._q

    def h(self):
        return self._h

    def h1(self):
        return self._h1

    def h2(self):
        return self._h2
    
    
