"""
楕円曲線用に簡易化したDER エンコーディング

https://crypto.stackexchange.com/questions/1795/how-can-i-convert-a-der-ecdsa-signature-to-asn-1

[エンコードの形]
0x30 | b1 | 0x02 | b2 | <r> | 0x02 | b3 | <s>

"""

def _int_byte_length(i: int) -> int:
    """ 
    正整数iのバイト長を返す
    e.g.) b10 => 1,b1011 => 1,b11110000 => 2,b100000001 => 3
    """    
    assert i >= 0
    return (i.bit_length() + 7) // 8

def _hex_byte_length(h: str) -> int:
    """ 16進法文字列のバイト長を返す """
    assert isinstance(h,str)
    i = int(h,16)
    return _int_byte_length(i)

def _one_byte_to_hex(i: int) -> str:
    """
    1バイト正整数を16進法文字列に変換
    
    e.g.) 1 => "01",10 => "0a" etc.
    """
    assert 0 <= i < 256
    return "%0.2x"%(i)

def _encode(r: str,s: str) -> str:
    """
    rとsを実際にエンコードする関数

    r,sは共に16進数文字列と仮定。
    e.g.)    
    r = '1ea1fdff81b3a271659df4aad19bc4ef83def389131a36358fe64b245632e777'
    s = '29e164658be9ce810921bf81d6b86694785a79ea1e52dbfa5105148d1f0bc1'
    """
    
    assert isinstance(r,str)
    assert isinstance(s,str)
    
    assert len(r)%2 == 0
    assert len(s)%2 == 0

    assert len(r) < 512
    assert len(s) < 512
    
    b2 = _hex_byte_length(r)
    b3 = _hex_byte_length(s)

    b2_hex = _one_byte_to_hex(b2)
    b3_hex = _one_byte_to_hex(b3)
    
    s = "02" + b2_hex + r + "02" + b3_hex + s

    b1 = len(s)
    b1_hex = _one_byte_to_hex(b1)
    
    s = "30" + b1_hex + s    
    return s

def _decode(e: str) -> tuple[str,str]:
    """ _encodeの逆操作 """    
    assert isinstance(e,str)
    
    # 1) 0x30 check
    magic0 = e[0:2]
    if magic0 != "30":
        return

    size = int(e[2:4],16)

    # 2) 0x02 check
    magic1 = e[4:6]
    if magic1 != "02":
        return

    # 3) rを取り出す。
    r_size = int(e[6:8],16)
    r_start = 8
    r_end   = r_start + r_size*2    
    r = e[r_start:r_end]
    
    # 4) 0x02 check
    magic2 = e[r_end:r_end + 2]
    if magic2 != "02":
        return

    # 5) sを取り出す。
    s_size  = int(e[r_end + 2:r_end + 4],16)
    s_start = r_end + 4
    s_end   = s_start + s_size*2
    s = e[s_start:s_end]

    return r,s


def der_encode(r: int,s: int) -> str:
    """ rとsをエンコード """    
    assert isinstance(r,int)
    assert isinstance(s,int)
    
    r_hex = hex(r)[2:]
    s_hex = hex(s)[2:]

    # r_hex,s_hexの文字列の長さが偶数出ない場合、偶数に調整。
    if len(r_hex)%2 != 0:
        r_hex = "0" + r_hex

    if len(s_hex)%2 != 0:
        s_hex = "0" + s_hex

    return _encode(r_hex,s_hex)

def der_decode(e: str) -> tuple[int,int]:
    """ der_encodeの逆操作 """    
    assert isinstance(e,str)
    
    s,r = _decode(e)
    return int(s,16),int(r,16)
