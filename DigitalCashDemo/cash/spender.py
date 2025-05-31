from .ntheory import sq_and_mul,rand_between,inv
from .init import H,H0
from .bank import Bank
from .coin import Coin,SpendingParams,CoinSecret,CoinInWallet
from .timestamp import timestamp_int
from .error import Error
from .status import Status

def _choose_coin_secret(p,q):
    """
    コイン生成時に必要なパラメータを生成。
    
    @param p:int
    @param q:int
    @return: tuple (s,x1,x2,a1,a2)
    """
    
    # s%q != 0を満たす乱数sを生成、s%q==0の場合コインが正しく生成されない。
    while True:
        s  = rand_between(1,p - 1)
        if s%q != 0:
            break
        
    x1 = rand_between(1,p - 1)
    x2 = rand_between(1,p - 1)
    a1 = rand_between(1,p - 1)
    a2 = rand_between(1,p - 1)

    return s,x1,x2,a1,a2

class Spender:
    """
    コインの送り手と受け手を表すクラス。
    """
    def __init__(self,name):
        assert isinstance(name,str)
        self.name = name # 名前
        self.wallet = [] # 後にwallet(財布)内にコインを格納する。
        
    def register(self,bank,num_of_coins=10):
        """
        銀行bankにアカウントを登録。
        
        @param bank: Bank
        @param num_of_coins:int,初期コイン数。
        @return: Status
        """
        assert isinstance(bank,Bank)

        # 銀行からの公開情報を保存
        self.p = bank.p()
        self.q = bank.q()
        self.g = bank.g()
        self.g1 = bank.g1()
        self.g2 = bank.g2()
        self.h = bank.h()
        self.h1 = bank.h1()
        self.h2 = bank.h2()
        
        self.u = rand_between(1,self.p - 1) # 秘密鍵のようなもの
        self.ID = sq_and_mul(self.g1,self.u,self.p) # 公開鍵のようなもの
        
        if (self.ID*self.g2) % self.p == 1:
            # 上記の等式が成り立つと、コインが正しく生成ができない。
            print("[!] Error(register): アカウント登録エラーが発生しました。再度登録を試してください。")
            return Status(Error.REGISTER)

        # 銀行にアカウントを登録したあとに銀行から、後に使用する値が渡される。
        z_prime = bank.add_user(self.ID,self.name,num_of_coins)
        
        if z_prime is None:
            print("[!] Error(register): すでにアカウントが登録されています。")
            return Status(Error.REGISTER)
        else:
            self.z_prime = z_prime

        return Status(Error.SUCCESS)
    
    def deduct_coin(self,bank):
        """
        コインを銀行から引き出す。
        
        @param bank: Bank
        @return: Status
        """

        # 銀行のアカウント内にコインがあるか確認。
        if not bank.do_i_have_coin(self.ID):
            print("[!] Error(deduct_coin): アカウントにコインをありません。")
            return Status(Error.NO_COIN)

        # コイン生成に必要な計算をする。
        gw,beta = bank.deduct_coin(self.ID)        
        s,x1,x2,a1,a2 = _choose_coin_secret(self.p,self.q)

        A = sq_and_mul(self.ID*self.g2,s,self.p)
        B = (sq_and_mul(self.g1,x1,self.p)*sq_and_mul(self.g2,x2,self.p)) % self.p
        z = sq_and_mul(self.z_prime,s,self.p)
        a = (sq_and_mul(gw,a1,self.p)*sq_and_mul(self.g,a2,self.p)) % self.p
        b = (sq_and_mul(beta,s*a1,self.p)*sq_and_mul(A,a2,self.p)) % self.p
        
        a1_inv = inv(a1,self.q)
        c = (a1_inv*H(A,B,z,a,b,self.q)) % self.q
        c1 = bank.blind_coin(c)
        r = (a1*c1 + a2) % self.q
        
        # A == 1であるコインを使用しない。(sとself.IDの選び方から、このassertには引っかからないはず)
        assert A != 1

        # コインとコイン生成に使用したパラメータを保存。
        coin = Coin(A,B,z,a,b,r)
        coin_secret = CoinSecret(s,x1,x2,a1,a2)

        # コインをウォレットに保存。
        self.store_coin(coin,coin_secret,None)

        return Status(Error.SUCCESS)
    
    def store_coin(self,coin,coin_secret,spending_params):
        """
        コインと後に使用するパラメータをCoinInWalletにまとめて、ウォレットに保存。
        
        @param coin:Coin
        @param coin_secret:CoinSecret
        @param spending_params:SpendingParams
        """
        
        coin_in_wallet = CoinInWallet(coin,coin_secret,spending_params)
        self.wallet.append(coin_in_wallet)

    def is_coin_valid(self,coin):
        """
        コインが正当(コインであるための条件を満たす)であるかチェック。
        正当である場合True、そうでない場合Falseを返す。

        @param coin:Coin
        @return: bool
        """

        h_exp = H(coin.A,coin.B,coin.z,coin.a,coin.b,self.q)
        
        g_r = sq_and_mul(self.g,coin.r,self.p)
        h_H = sq_and_mul(self.h,h_exp,self.p)
        test1 = ((g_r - coin.a*h_H) % self.p == 0)
        
        A_r = sq_and_mul(coin.A,coin.r,self.p)
        z_H = sq_and_mul(coin.z,h_exp,self.p)
        test2 = ((A_r - z_H*coin.b) % self.p == 0)
        
        return test1 & test2

    def try2receive_coin(self,coin):
        """
        コインを受け取るのに必要なパラメータを計算。
        @param coin:Coin
        @return: int
        """
        
        if not self.is_coin_valid(coin):
            print("[!] Error(try2receive_coin): コインが正当でない。")
            return None

        t = timestamp_int()
        d = H0(coin.A,coin.B,self.ID,t,self.q)
        return d

    def accept_coin(self,coin,coin_secret,spending_params):
        """
        コインと必要なパラメータを受け取る。
        
        @param coin:Coin
        @param coin_secret:CoinSecret
        @param spending_params:SpendingParams
        @return: bool
        """

        # コインを受け取る前に、コインが正当であるかチェック。
        r1 = spending_params.r1
        r2 = spending_params.r2
        d  = spending_params.d
        
        g1_r1 = sq_and_mul(self.g1,r1,self.p)
        g2_r2 = sq_and_mul(self.g2,r2,self.p)
        AdB = (sq_and_mul(coin.A,d,self.p)*coin.B) % self.p
        test = ((g1_r1*g2_r2 - AdB) % self.p) == 0

        if not test:
            # コインを受け取らない。
            print("[!] Error(accept_coin): コインが正当でない。")
            return False
        else:
            # コインを受け取る。
            self.store_coin(coin,coin_secret,spending_params)
            return True

    def double_spend_to(self,merchant1,merchant2,idx=0):
        """
        merchant1とmerchant2にself.wallets[idx]にあるコインをダブルスペンドする。
        
        @param merchant1:Spender
        @param merchant2:Spender
        @param idx:int
        @return: Status
        """

        # まずmerchant1にコインを送り、エラーが発生した場合はその時点で中止。
        status = self.send_coin_to(merchant1,False)
        if status.error != Error.SUCCESS:
            return status
        
        return self.send_coin_to(merchant2,True)
    
    def send_coin_to(self,merchant,delete_coin=True):
        """
        コインをmerchantに送る。
        
        @param merchant:Spender
        @param delete_coin:bool、この値がTrueの場合コインを支払った後にそのコインをself.walletsから削除する。Falseの場合削除しない。
        @return: Status
        """

        # コインがウォレットにない場合、中止
        if len(self.wallet) == 0:
            return Status(Error.NO_COIN)

        # 未使用のコインをウォレットから探し、そのコインを送る。
        for idx in range(len(self.wallet)):
            if self.wallet[idx].spending_params is None: # 未使用のコインを発見。
                return self.send_coin_to_with_idx(merchant,delete_coin,idx)
            
        return Status(Error.NO_COIN)
    
    def send_coin_to_with_idx(self,merchant,delete_coin=True,idx=0):
        """
        self.wallet[idx]にあるコインをmerchantに送る。
        
        @param merchant:Spender
        @param delete_coin:bool、この値がTrueの場合コインを支払った後にそのコインをself.walletsから削除する。Falseの場合削除しない。
        @param idx:int
        @return: Status
        """

        # idxがself.walletの範囲内に含まれるかチェック。
        if idx < 0 or idx >= len(self.wallet):
            print("[!] Error(send_coin_to_with_idx): 与えられたインデックスが正しくありません。")
            return Status(Error.NO_COIN)
        elif self.wallet[idx].spending_params is not None: # 使用済みコイン
            print("[!] Error(send_coin_to_with_idx): 使用済みコインを使用しています。")
            return Status(Error.USED_COIN)

        # 必要なパラメータを変数に保存(HACK:可読性)
        coin = self.wallet[idx].coin
        coin_secret = self.wallet[idx].coin_secret
        spending_params = self.wallet[idx].spending_params

        if spending_params is not None:
            print("[!] Error(send_coin_to_with_idx): 使用済みコインを使用しています。")
            return Status(Error.USED_COIN)
        
        if not self.is_coin_valid(coin):
            print("[!] Error(send_coin_to_with_idx): コインが正当でありません。")
            return Status(Error.INVALID_COIN)

        # 受け手から必要なパラメータdを受け取る
        d = merchant.try2receive_coin(coin)
        if d is None: # コインが正当でない場合。
            return Status(Error.INVALID_COIN)

        r1 = (d*self.u*coin_secret.s + coin_secret.x1) % self.q
        r2 = (d*coin_secret.s + coin_secret.x2) % self.q

        spending_params = SpendingParams(r1,r2,d)
        accept = merchant.accept_coin(coin,coin_secret,spending_params)

        if delete_coin:
            del self.wallet[idx] # コインをウォレットから削除。

        # acceptに応じてステータスを返す。
        if accept:
            return Status(Error.SUCCESS)
        else:
            return Status(Error.OTHERS)

    def deposit(self,bank):
        """
        ウォレット内にあるコインを銀行に預金。
        
        @param bank:Bank
        @return: Status
        """

        # コインがウォレットにあるかチェック。
        if len(self.wallet) == 0:
            print("[!] Error(deposit): 未使用コインがウォレットにありません。")
            return Status(Error.NO_COIN)

        # 使用済みコインをウォレットから探す。
        for idx in range(len(self.wallet)):
            if self.wallet[idx].spending_params is not None: # 使用済みコインを見つけた場合
                return self.deposit_with_idx(bank,idx)
            
        return Status(Error.NO_COIN)

    def deposit_with_idx(self,bank,idx=0):
        """
        self.wallet[idx]にあるコインを銀行に預金。
        
        @param bank:Bank
        @param idx:int
        @return: Status
        """

        # idxがself.walletの範囲内かチェック。
        if idx < 0 or idx >= len(self.wallet):
            print("[!] Error(deposit_with_idx): 与えられたインデックスが正しくありません。")
            return Status(Error.NO_COIN)

        # 必要なパラメータを変数に保存(HACK:可読性)
        coin = self.wallet[idx].coin
        params = self.wallet[idx].spending_params
        
        # コインがダブルスペンドされた記録があるかチェック。
        test,dup_coin,dup_params = bank.is_double_spent(coin,params)
        
        if test == True: # コインがダブルスペンドされた場合
            ID = bank.fraud_control(coin,params,dup_coin,dup_params) # ダブルスペンドした者のIDを特定。
            del self.wallet[idx]
            return Status(Error.DOUBLE_SPENT,ID)
        else: # コインがダブルスペンドされていない場合
            pass

        # 以下の一連のコードでコインの正当性をチェック。
        
        r1 = params.r1
        r2 = params.r2
        d  = params.d

        h_exp = H(coin.A,coin.B,coin.z,coin.a,coin.b,self.q)
        
        g_r = sq_and_mul(self.g,coin.r,self.p)
        h_H = sq_and_mul(self.h,h_exp,self.p)
        test1 = ((g_r - coin.a*h_H) % self.p == 0)
        
        A_r = sq_and_mul(coin.A,coin.r,self.p)
        z_H = sq_and_mul(coin.z,h_exp,self.p)
        test2 = ((A_r - z_H*coin.b) % self.p == 0)

        g1_r1 = sq_and_mul(self.g1,r1,self.p)
        g2_r2 = sq_and_mul(self.g2,r2,self.p)
        AdB = (sq_and_mul(coin.A,d,self.p)*coin.B) % self.p
        test3 = ((g1_r1*g2_r2 - AdB) % self.p) == 0
        
        if test1 & test2 & test3: # コインが正当である場合
            bank.deposite_coin(self.ID) # コインを預金
            bank.store_used_coin(coin,params) # 預金したコインを使用済みコインに登録
            del self.wallet[idx]              # コインをウォレットから削除
            return Status(Error.SUCCESS)
        else: # コインが正当でない。
            print("[!] Error(deposit_with_idx): コインが正当でない。")
            return Status(Error.INVALID_COIN)
