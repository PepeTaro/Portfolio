from PyQt5.QtWidgets import QApplication
from PyQt5.QtGui import QVector2D

from crypto.params import GENESIS_BLOCKHASH
from gui.qt import MainWindow,BlockItem,LineItem

""" グローバル変数(ブロックを描写するための情報) """
BLOCK_WIDTH = 50 # ブロックの横幅
BLOCK_HEIGHT= 50 # ブロックの縦幅
GAP_WIDTH = 150  # ブロック間の空間横幅
GAP_HEIGHT = 80  # ブロック間の空間縦幅
REF_POINT = QVector2D(10,300) # ブロックチェーンの根(Genesisブロック)の位置

def get_text_of_block(block):
    s = ""
    s += "ブロックの高さ: %d\n"%(block.height)
    s += "このブロックのハッシュ: %s\n"%(block.get_hash())
    s += "親ブロックのハッシュ: %s\n"%(block.get_prevhash())
    s += "Merkle Root: %s\n"%(block.get_merkleroot())
    s += "タイムスタンプ: %s\n"%(block.get_timestamp())
    s += "マイニング難易度: %d\n"%(block.get_difficulty())
    s += "Nonce: %d\n"%(block.get_nonce())

    for i,tx in enumerate(block.txs):
        s += "\nトランザクション %d (txid: %s)%s\n"%(i,tx.get_hash(),tx.to_json())
        
    return s

def add_diagram_block(tree,blocks,ref_point,pos_idx,idx):    
    block_pos = QVector2D(ref_point.x() + GAP_WIDTH,ref_point.y() + pos_idx*GAP_HEIGHT)    
    blockhash = blocks[idx].get_hash()
    text  = get_text_of_block(blocks[idx])
    tree[blockhash] = BlockItem(block_pos,BLOCK_WIDTH,BLOCK_HEIGHT,text) # BlockItem追加

def add_blocks_on_layer(tree,blocks,ref_point):
    """ treeにブロックを追加 """
    
    assert BLOCK_WIDTH <= GAP_WIDTH and BLOCK_HEIGHT <= GAP_HEIGHT # この条件がないと重なる
    
    n = len(blocks)
    half_n = n//2
    idx = 0

    # レイヤー上のブロック(同じブロック高さをもつブロック)数が、奇数か偶数かで描写配置を変える
    if n%2 == 0:
        for i in range(-half_n, half_n + 1):
            if i == 0: continue                        
            add_diagram_block(tree,blocks,ref_point,i,idx)
            idx += 1
            
    else:
        for i in range(-half_n, half_n + 1):                        
            add_diagram_block(tree,blocks,ref_point,i,idx)
            idx += 1

def add_blockchain_to_diagram(diagram,nodes,links):
    """ nodes,linksから、diagramにブロックチェーンを追加 """
    
    offset_width = QVector2D(BLOCK_WIDTH + GAP_WIDTH,0) # ブロック間に存在する空間の横幅
    tree = {} # {"hash1":block1,"hash2":block2,...}

    # treeにブロックを格納
    for height in nodes.keys():
        blocks = nodes[height]
        ref_point_on_layer = REF_POINT + height*offset_width        
        add_blocks_on_layer(tree,blocks,ref_point_on_layer)
    
    # ブロックを図に追加
    for block in tree.values():
        diagram.add_item(block)

    # ブロック間の線分を表示
    for start_id,link in links.items():
        for end_id in link:
            block1 = tree[start_id]
            block2 = tree[end_id]

            # 線分の始点をブロックの右端の中央部とする
            start_x = block1.p.x() + block1.width
            start_y = block1.p.y() + block1.height//2

            # 線分の終点をブロックの左端の中央部とする
            end_x = block2.p.x()
            end_y = block2.p.y() + block2.height//2

            start = QVector2D(start_x,start_y)
            end   = QVector2D(end_x,end_y)

            # 線分を図に追加
            diagram.add_item(LineItem(start,end)) # LineItemを追加
            
def create_tree(miner,nodes,links,prevhash):
    """ ブロックチェーン描写に必要な情報をnodes,linksに保存 """
    blocks = miner._blockchain.get_blocks_by_prevhash(prevhash)
    
    for block in blocks:
        blockhash = block.get_hash()
        
        if prevhash not in links.keys():
            links[prevhash] = []        
        links[prevhash].append(blockhash)
        
        height = int(block.get_height())
        
        if height not in nodes.keys():
            nodes[height] = []        
        nodes[height].append(block)
        
        create_tree(miner,nodes,links,blockhash)
        
def show_qt(title,nodes,links):
    """ Qtアプリを起動 """
    
    app = QApplication([])
    window = MainWindow(1280,720,title)
    add_blockchain_to_diagram(window.diagram,nodes,links) # ブロックチェーンをダイアグラムに追加       
    window.show()    
    app.exec()

def show(miner,title=""):
    """ minerのブロックチェーンを描写 """
    
    nodes = {}
    links = {}

    # 必要な情報をnodes,linksに保存
    create_tree(miner,nodes,links,GENESIS_BLOCKHASH)
    
    # nodesの先頭にGenesisブロックを保存
    nodes[0] = []
    nodes[0].append(miner.get_genesis_block())
    
    show_qt(title,nodes,links)
