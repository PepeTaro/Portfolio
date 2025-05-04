from crypto.miner import Miner
from crypto.block import Block

from crypto.logging import log_error,log_warning,log_info

class Node(Miner):
    def __init__(self):
        super().__init__()
        
    def request_block(self,node: "Node",blockhash: str) -> Block:
        """ ノードからブロックを受け取る """
        return node.get_block(blockhash)

    def sync_with(self,node: "Node") -> bool:
        success  = self.sync_mainchain_with(node)
        success &= node.sync_mainchain_with(self)
        return success
        
    def sync_mainchain_with(self,node: "Node") -> bool:
        """ 
        他のノードとメインチェーンを同期。
        長いメインチェーンのブロックを取得する。
        メインチェーンの長さが同じ場合は何もしない。
        """
        
        blockhash = node.get_best_blockhash()
        best_height = node.get_best_height()
        
        # メインチェーンの最後尾にあるブロックハッシュを比較
        if self.get_best_blockhash() == blockhash:
            log_info("Node.sync_with(): メインチェーンが同じです")
            return True

        # メインチェーンの長さを比較
        if self.get_best_height() >= best_height:
            log_info("Node.sync_with(): 現在のメインチェーンのほうが長い")
            return True
        
        while True:
            # ブロックを送信するようリクエスト
            block = self.request_block(node,blockhash)
            if block is None:
                log_error("Node.sync_with(): ブロックリクエストに失敗")
                return False
            
            # Genesisブロックまで受け取る
            if block.is_genesis():
                log_info("Node.sync_with(): メインチェーンの同期を完了しました")
                return True

            # ブロックを受け取る
            self.receive_block(block)

            # 親ブロックのハッシュを保存
            blockhash = block.get_prevhash()

        
