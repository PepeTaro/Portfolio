from crypto.tx import WalletTx,Tx,TxIn,TxOut
from crypto.map import Map
from crypto.logging import log_error,log_warning,log_info
from crypto.address import gen_address
from crypto.signature import sign_tx

class Wallet(Map[WalletTx]):
    """ Walletは財布の様なもの(UTXOプール) """
    
    def __init__(self,address) -> None:
        super().__init__()
        self._address    = address # 自身のアドレス
        
    def add_wtx(self,new_wtx) -> bool:
        """ new_wtxを保存、もしすでに存在するなら情報を更新 """
        
        assert isinstance(new_wtx,WalletTx)
        
        txid = new_wtx.get_hash()
        
        if self.contains(txid):# new_wtxがWalletにすでに存在
            # すでに存在するold_wtxを更新
            log_warning("WalletMap.add_wtx(): すでにwtxが存在します")

            # すでに存在するwtxを取り出す
            old_wtx = self.get(txid)
            
            # spentフラグを更新(new_wtxが使用済みの場合上書き)
            if new_wtx.spent and not old_wtx.spent:
                old_wtx.spent = new_wtx.spent
                            
            return True
        
        else: # new_wtxがWalletに存在しない場合
            log_info("WalletMap.add_wtx(): wtxを追加しました")
            
            return self.add(new_wtx)
            
    def add_tx_if_mine(self,tx) -> bool:
        """ Txが自分宛のTxOutを含む場合、Walletに追加 """
        
        if tx.is_mine(self._address):
            wtx = WalletTx(tx)
            return self.add_wtx(wtx)
        else:
            log_warning("WalletMap.add_tx_if_mine(): 自分宛のTxOutを含まないため、TxをWalletに追加しませんでした")
            return False
        
    def get_balance(self) -> int:
        """ マップの中からすでに使用していないTxの合計金額を返す """
        
        total = 0
        for wtx in self.values():
            if not wtx.spent:
                total += wtx.get_amount(self._address)
        return total

    def select_coins(self,target_amount) -> tuple[int,list[WalletTx]]:
        """ マップの中から、合計金額がtarget_amount以上となるようにコインを取り出す """
        
        if target_amount <= 0:
            return None

        coins = []
        total_amount = 0
        
        for wtx in self.values():
            
            if wtx.spent: # wtxがすでに使用さている
                continue
                
            amount = wtx.get_amount(self._address) # wtxから所持金を取得
            total_amount += amount
            coins.append(wtx)
            
            if total_amount >= target_amount:
                # (合計金額,コイン)を返す
                # 合計金は送りたい金額を超えることあり
                return total_amount,coins # コインの合計金額、コインのリスト

        log_error("WalletMap.select_coins(): 所持金が足りない")
        return None

    def create_tx(self,pubkey,privkey,raddr,target_amount) -> Tx:
        """ raddr宛にtarget_amout分のお金を送るためのTxを生成(raddr := Recipient's address) """

        wtx = WalletTx()
        
        saddr = gen_address(pubkey)
        slocking_script = "DUP HASH %s EQUALVERIFY CHECKSIG"%(saddr)
        rlocking_script = "DUP HASH %s EQUALVERIFY CHECKSIG"%(raddr)
        
        # お金は正整数
        if target_amount <= 0:
            log_error("WalletMap.create_tx(): 金額を正整数で渡してください")
            return None

        # 使用するコインを選択
        coin_info = self.select_coins(target_amount)
        
        if coin_info is None: # 十分なコインがない場合
            log_error("WalletMap.create_tx(): お金が足りません")
            return None
        else:
            coins_amount,coins = coin_info # コインの合計金額、コインのリスト

        # 受け取り側用のTxOutをセット
        wtx.vout.append(TxOut(target_amount,raddr,rlocking_script))
        
        # お釣りがある場合、お釣り用TxOutを自分向けにセット
        if coins_amount > target_amount:
            change_amount = coins_amount - target_amount
            wtx.vout.append(TxOut(change_amount,saddr,slocking_script))

        # TxInを生成して、Vinを埋める
        for coin in coins:
            for idx in range(len(coin.vout)):
                if coin.vout[idx].is_mine(self._address):
                    txid = coin.get_hash()
                    wtx.vin.append(TxIn(txid,idx))
        
        # デジタル署名
        sign_tx(wtx,pubkey,privkey)
        
        # wtxを保存
        self.add_wtx(wtx)
        
        # txinが参照しているwtxに使用済みフラグをつける
        for txin in wtx.vin:
            used_wtx = self.get(txin.txid)
            used_wtx.spent = True
        
        # wtxからtxに変換
        return Tx(wtx.vin,wtx.vout)
