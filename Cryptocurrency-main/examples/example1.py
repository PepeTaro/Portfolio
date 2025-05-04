import sys
sys.path.append("../")

from crypto.node import Node
from crypto.params import COINBASE_REWARD # デフォルトで50

def main():
    node1 = Node()
    node2 = Node()
    
    block = node1.mine()
    assert node1.get_balance() == COINBASE_REWARD # 50
    
    node2.receive_block(block)
    
    tx = node1.create_tx(node2.get_address(),10)
    
    block = node1.mine()
    assert node1.get_balance() == 2*COINBASE_REWARD - 10 # 90 == 50 - 10 + 50
    
    node2.receive_block(block)
    assert node2.get_balance() == 10 # 10
    
if __name__ == '__main__':
    main()
