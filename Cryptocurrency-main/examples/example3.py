import sys
sys.path.append("../")

from crypto.node import Node
from crypto.params import COINBASE_REWARD # デフォルトで50
from gui.diagram import show

def main():    
    node1 = Node()
    node2 = Node()
    
    block = node1.mine()
    node2.receive_block(block)

    # 新たなノードがネットワークに参加
    alice = Node()
    bob   = Node()

    # aliceとbobはそれぞれnode1、node2のブロックチェーンと同期(ブロックをダウンロード)
    alice.sync_with(node1)
    bob.sync_with(node2)

    # node1がaliceにお金を送信
    tx = node1.create_tx(alice.get_address(),20)

    # node2がトランザクションを受け取る
    node2.receive_tx(tx)

    # node2が今受け取ったトランザクションを含むブロックをマイニング
    block = node2.mine()

    # 各々のノードがブロックを受信
    node1.receive_block(block)
    alice.receive_block(block)
    bob.receive_block(block)

    assert alice.get_balance() == 20 # aliceの所持金

    # aliceが以下のトランザクションを生成
    tx1 = alice.create_tx(bob.get_address(),7) # bobに7トークン送る
    tx2 = alice.create_tx(bob.get_address(),3) # 再度bobに3トークン送る
    tx3 = alice.create_tx("some address",10)   # 適当なアドレスに10トークン送る(トークンを捨てる)
    
    assert alice.get_balance() == 0 # 所持金を使い切った

    # node1がトランザクションを受け取る
    node1.receive_tx(tx1)
    node1.receive_tx(tx2)
    node1.receive_tx(tx3)

    # node1が今受け取ったトランザクションを含むブロックをマイニング
    block = node1.mine()
    
    # bobがブロックを受け取る
    bob.receive_block(block)

    assert bob.get_balance() == 10 # ブロックを受け取ったことで所持金が増える
    
    # GUIを使用して、bobのブロックチェーンを表示
    show(bob)

if __name__ == '__main__':
    main()
