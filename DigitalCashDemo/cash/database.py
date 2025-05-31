import sys
import os
import sqlite3
import json

INIT_USERS = """CREATE TABLE users(
id INTEGER PRIMARY KEY NOT NULL,
name TEXT NOT NULL
);"""

INIT_RECORDS = """CREATE TABLE records(
coin INTEGER NOT NULL DEFAULT 0,
id INTERGER REFERENCES users (id)
);"""

INIT_USED = """CREATE TABLE used(
coin TEXT NOT NULL,
params TEXT NOT NULL
);"""

def connect(db_file="./data.db"):
    # データベースに接続し、sqlite3.Connectionを返す。
    conn = None
    does_db_exist = os.path.isfile(db_file)
    
    try:
        # check_same_thread=False はFlaskのために必要。
        conn = sqlite3.connect(db_file,check_same_thread=False)
    except sqlite3.Error as e:
        print("[!] Error(connect): データベース接続エラー (%s)"%(e))
        sys.exit(-1)

    if not does_db_exist: #データベースが存在しない場合初期化
        init_db(conn)
        
    return conn

def init_db(conn):
    # データベースを初期化。
    try:
        conn.execute(INIT_USERS)
        conn.execute(INIT_RECORDS)
        conn.execute(INIT_USED)
    except sqlite3.Error as e:
        print("[!] Error(init): データベース初期化エラー (%s)"%(e))
        sys.exit(-1)
    
def add_user_in_db(conn,ID,name,num_of_coins=10):
    # ユーザーを追加。
    conn.execute("INSERT INTO users (id,name) VALUES (?,?)",(ID,name))
    conn.execute("INSERT INTO records (coin,id) VALUES (?,?)",(num_of_coins,ID))
    
def store_used_coin_in_db(conn,coin,params):
    # 使用済みコインを保存。
    _coin = json.dumps(coin.dumps())
    _params = json.dumps(params.dumps())
    conn.execute("INSERT INTO used (coin,params) VALUES (?,?)",(_coin,_params))
    
def deposit_coin_into_db(conn,ID):
    # コインひとつを預金
    conn.execute("UPDATE records SET coin = coin + 1 WHERE id = ?",(ID,))
    
def deduct_coin_from_db(conn,ID):
    # コインひとつを引き出す
    conn.execute("UPDATE records SET coin = coin - 1 WHERE id = ?",(ID,))

def is_double_spent_in_db(conn,coin):
    # ダブルスペンドされたコインがある場合は、そのコインとパラメータを返す。
    _coin = json.dumps(coin.dumps())
    cursor = conn.execute("SELECT coin,params from used WHERE coin = ?",(_coin,))
    items = cursor.fetchone()

    list_coin = None
    list_params = None
    if items is not None:
        list_coin = json.loads(items[0])
        list_params = json.loads(items[1])
        
    return list_coin,list_params # list,list
    
def does_have_coin_in_db(conn,ID):
    # コインがデータベースにあるかチェック
    cursor = conn.execute("SELECT coin FROM records WHERE id = ?",(ID,))
    coins = int(cursor.fetchone()[0])
    return coins > 0

def does_id_exist_in_db(conn,ID):
    # IDがデータベースに存在するかチェック
    cursor = conn.execute("SELECT id FROM users WHERE id = ?",(ID,))
    return cursor.fetchone() is not None

def fetch_users_from_db(conn):
    cursor = conn.execute("SELECT id,name FROM users")
    return cursor

def fetch_records_from_db(conn):
    cursor = conn.execute("SELECT coin,id FROM records")
    return cursor

def fetch_used_from_db(conn):
    cursor = conn.execute("SELECT coin,params FROM used")
    return cursor

def fetch_accounts_from_db(conn):
    cursor = conn.execute("SELECT users.name,users.id,records.coin FROM users INNER JOIN records ON users.id = records.id ORDER BY users.id")
    return cursor
