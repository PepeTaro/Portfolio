import sys
sys.path.append("../")

import crypto.logging
crypto.logging.turn_off_log()

from crypto.node import Node
from gui.diagram import show

def demo1():
    node = Node()
    show(node,"Genesisブロックのみデモ 1/3")

def demo2():
    node = Node()
    node.mine()

    for i in range(20):
        node.create_tx(node.get_address(),1)

    for i in range(3):
        node.mine()

    show(node,"Txデモ 2/3")

def demo3():
    """
                   --|31|<-|41|<-|51|<-|61|       node1
                   |
    |g|<-|1|<-|2|<---|32|<-|42|<-|52|             node2
                            ^
                            |
                            -----|53|<-|63|<-|73| node3    
    """
    
    node1 = Node()
    node2 = Node()
    node3 = Node()

    block1  = node1.mine()
    block2  = node1.mine()

    node2.receive_block(block1)
    node2.receive_block(block2)

    node3.receive_block(block1)
    node3.receive_block(block2)
    
    # ここからフォーク
    block31 = node1.mine()
    block41 = node1.mine()
    block51 = node1.mine()
    block61 = node1.mine()

    block32 = node2.mine()
    block42 = node2.mine()
    block52 = node2.mine()

    node3.receive_block(block32)
    node3.receive_block(block42)

    block53 = node3.mine()
    block63 = node3.mine()
    block73 = node3.mine()

    # node3がnode1とnode2のブロックを受け取る、しかしメインチェーンは変わらない(長いから)
    node3.receive_block(block31)
    node3.receive_block(block41)
    node3.receive_block(block51)
    node3.receive_block(block61)

    node3.receive_block(block32)
    node3.receive_block(block42)
    node3.receive_block(block52)

    # node2がnode1とnode2のブロックを受け取る、
    # まずメインチェーンがnode1のそれに変わり、次にnode3のそれに変わる
    node2.receive_block(block31)
    node2.receive_block(block41)
    node2.receive_block(block51)
    node2.receive_block(block61)
        
    node2.receive_block(block53)
    node2.receive_block(block63)
    node2.receive_block(block73)
    
    # node1がnode3とnode2のブロックを受け取る、メインチェーンがnode3のそれに変わる    
    node1.receive_block(block53)
    node1.receive_block(block63)
    node1.receive_block(block73)

    node1.receive_block(block32)
    node1.receive_block(block42)
    node1.receive_block(block52)        

    show(node1,"フォークデモ 3/3")

def main():
    demo1()
    demo2()
    demo3()
    
if __name__ == '__main__':
    main()
