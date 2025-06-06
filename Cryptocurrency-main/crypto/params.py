
# Blockheaderに設定できる最大のマイニング難易度
MAX_DIFFICULTY = 115792089237316195423570985008687907853269984665640564039457584007913129639936 # = 2**256

# マイニング難易度リスト
DIFFICULTY0 = MAX_DIFFICULTY                                                               # 2**256 0s
DIFFICULTY1 = 1809251394333065553493296640760748560207343510400633813116524750123642650624 # 2**250 0s
DIFFICULTY2 = 1766847064778384329583297500742918515827483896875618958121606201292619776    # 2**240 1s
DIFFICULTY3 = 441711766194596082395824375185729628956870974218904739530401550323154944     # 2**238 3s
DIFFICULTY4 = 55213970774324510299478046898216203619608871777363092441300193790394368      # 2**235 11s
DIFFICULTY5 = 1725436586697640946858688965569256363112777243042596638790631055949824       # 2**230 28s 

# デフォルトマイニング難易度
DEFAULT_DIFFICULTY = DIFFICULTY1

# コインベースTx情報
COINBASE_REWARD           = 50
COINBASE_TXID             = "COINBASE_TXID"
COINBASE_IDX              = 0
COINBASE_UNLOCKING_SCRIPT = "COINBASE_UNLOCKING_SCRIPT"

# Genesisブロックの情報
GENESIS_BLOCKHASH  = "0000002b0c240b41d956f9b8fab9f9ea9ec3f28aa6599ce89655121449b6ea4c"
GENESIS_DIFFICULTY = DIFFICULTY5 
GENESIS_NONCE      = 5008578

# LOCKING SCRIPTフォーマット
LOCKING_SCRIPT   = "DUP HASH %s EQUALVERIFY CHECKSIG"

# ひとつのブロック内に詰め込むことのできる最大Tx数
NUM_TX_IN_BLOCK  = 10
