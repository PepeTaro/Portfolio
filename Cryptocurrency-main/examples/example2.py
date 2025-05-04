import sys
sys.path.append("../")

from crypto.node import Node
from crypto.params import COINBASE_REWARD # デフォルトで50

def main():
    node1 = Node()
    node2 = Node()
    
    b1  = node1.mine()
    b2  = node1.mine()
    b3  = node2.mine()
    
    assert node1.get_balance() == 2*COINBASE_REWARD # 100
    assert node2.get_balance() == COINBASE_REWARD # 50
    
    node2.receive_block(b2)
    assert node2.get_balance() == COINBASE_REWARD # 50
    
    node2.receive_block(b1)
    assert node2.get_balance() == 0 # 0
    assert node1.get_balance() == 2*COINBASE_REWARD # 100
    
if __name__ == '__main__':
    main()
