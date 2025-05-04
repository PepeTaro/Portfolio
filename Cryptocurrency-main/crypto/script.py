from crypto.logging import log_error
from crypto.address import gen_address
from crypto.signature import verify_tx

class Stack:
    def __init__(self) -> None:
        self._stack = []
    
    def push(self,data) -> None:
        self._stack.append(data)    

    def pop(self) -> None:
        return self._stack.pop()
    
    def empty(self) -> bool:
        if len(self._stack) == 0:
            return True
        else:
            return False
        
    def dup(self) -> bool:
        """ 1つデータをポップして、同じデータを２回プッシュ """
        if self.empty():
            return False
        else:
            dup = self.pop()
            self.push(dup)
            self.push(dup)
            return True
        
    def hash(self) -> bool:
        """ 1つデータをポップして、そのデータをハッシュした値をプッシュ """
        if self.empty():
            return False
        else:
            pubkey = self.pop()
            addr = gen_address(pubkey)        
            self.push(addr)
            return True
        
    def equalverify(self) -> bool:
        """ ２つデータをポップして、比較結果を返す """
        data1 = self.pop()
        data2 = self.pop()
        
        if data1 != data2:
            return False
        else:
            return True
        
    def checksig(self,tx) -> bool:
        """ ２つデータをポップして、デジタル署名チェック """
        pubkey = self.pop() # 公開鍵をポップ
        sig    = self.pop() # デジタル署名をポップ

        # デジタル署名チェック        
        if verify_tx(tx,sig,pubkey):
            return True
        else:
            return False

def check_signature(tx,txin,txout) -> bool:
    """ 
    Txのデジタル署名が正しい場合True、そうでない場合Falseを返す 

    [注意] txinはtxoutを参照している、つまりtxinはtxoutをUTXOとして使用している
           (txin.txidとtxin.idxからtxoutが一意的に特定できる)    
    """
    
    # script := <sig> <pubkey> DUP HASH <address> EQUALVERIFY CHECKSIG、の形をしている
    
    script = "%s %s"%(txin.unlocking_script,txout.locking_script)
    ops = script.split()
    stack = Stack()
    
    for op in ops:
        if op == "DUP":
            if not stack.dup():
                log_error("check_signature(): Stack.dup()に失敗")
                return False
        elif op == "HASH":
            if not stack.hash():
                log_error("check_signature(): Stack.hash()に失敗")
                return False
        elif op == "EQUALVERIFY":
            if not stack.equalverify():
                log_error("check_signature(): Stack.equalverify()に失敗")
                return False
        elif op == "CHECKSIG":
            if not stack.checksig(tx):
                log_error("check_signature(): Stack.checksig()に失敗")
                return False
        else: # Value
            stack.push(op)

    return True
    
