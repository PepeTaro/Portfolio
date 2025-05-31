import sys
sys.path.append("..")

from cash.bank import Bank
from cash.spender import Spender
from cash.init import init_primes,init_primitive_root
from cash.error import Error
from cash.status import Status

from flask import Flask,render_template,request,jsonify
app = Flask(__name__)

bank = None
users = {}    # {ID(str):account(Spender)}
"""
 メモ: usersのIDがstrなのは、javascriptからIDが送られてきたときIDはstrであるため
       javascriptに合わせる。
"""

def init():
    # 初期化
    global bank,users

    SECUIRTY = 20
    users = {}
    p,q = init_primes(SECUIRTY)
    g,g1,g2 = init_primitive_root(p)    
    bank = Bank(g,g1,g2,p,q)    

def does_id_exist(ID):
    # IDが既存のアカウント内に存在するか
    assert isinstance(ID,str)
    return ID in users.keys()

def status_data(status,data):
    return jsonify({"status":status,"data":data})

def error(status):
    return status_data(status,None)

def info(status):
    error = status.error
    data  = status.data
    message = ""
    
    if(error == Error.SUCCESS):
        message = "success"
    elif(error == Error.REGISTER):
        message = "register"
    elif(error == Error.NO_COIN):
        message = "no_coin"
    elif(error == Error.USED_COIN):
        message = "used_coin"
    elif(error == Error.INVALID_COIN):
        message = "invalid_coin"
    elif(error == Error.DOUBLE_SPENT):
        message = "double_spent"
    elif(error == Error.INDEX):
        message = "index"
    elif(error == Error.OTHERS):
        message = "others"
    else:
        print("[!] Error(info) エラー.")
        message = "error"

    return status_data(message,data)


@app.route('/double_spend_coin',methods=['POST'])
def double_spend_coin():
    # ダブルスペンドボタンを押したときに呼ばれる関数。
    global bank,users

    # IDをフォームから取り出す。
    sender_id = request.form['senderID']
    receiver1_id = request.form['receiver1ID']
    receiver2_id = request.form['receiver2ID']

    # 上記3つのIDが存在するかチェック
    if not (does_id_exist(sender_id) and does_id_exist(receiver1_id) and does_id_exist(receiver2_id)):        
        return error("id_error")
    
    status = users[sender_id].double_spend_to(users[receiver1_id],users[receiver2_id])
    return info(status)        

@app.route('/deposit_coin',methods=['POST'])
def deposit_coin():
    # 預金ボタンを押したときに呼ばれる関数。
    global bank,users
    
    ID = request.form['ID']

    # IDが存在するかチェック
    if not does_id_exist(ID):        
        return error("id_error")
        
    status = users[ID].deposit(bank)                    
    return info(status)

@app.route('/deduct_coin',methods=['POST'])
def deduct_coin():
    # 引き出しボタンを押したときに呼ばれる関数。
    global bank,users
    
    ID = request.form['ID']
    
    # IDが存在するかチェック
    if not does_id_exist(ID):            
        return error("id_error")

    # アカウント内にコインがあるかチェック
    if not bank.do_i_have_coin(request.form['ID']):
        return error("no_coin")
    
    status = users[ID].deduct_coin(bank)
    return info(status)

@app.route('/send_coin',methods=['POST'])
def send_coin():
    # 送信ボタンを押したときに呼ばれる関数。
    global bank,users
    
    # IDをフォームから取り出す
    sender_id = request.form['senderID']
    receiver_id = request.form['receiverID']

    # 送り手と受け手のIDが存在するかチェック
    if not (does_id_exist(sender_id) and does_id_exist(receiver_id)):
        return error("id_error")
        
    status = users[sender_id].send_coin_to(users[receiver_id])
    return info(status)
    
@app.route('/register_account',methods=['POST'])
def register_account():
    # 登録ボタンを押したときに呼ばれる関数。
    global bank,users

    # Spenderクラス生成
    name = request.form['userName']        
    account = Spender(name)

    # 銀行に登録
    status = account.register(bank,10)

    # IDとアカウントをdictであるusersに保存。
    ID = str(account.ID) # strに変換していることに注意。
    users[ID] = account

    # javascriptにIDを渡すために、dataにIDを追加
    status.push_data(ID)
    
    return info(status)
    
@app.route('/load_overview',methods=['POST'])
def load_overview():
    global bank
    params = {}
    params["p"] = bank.p()
    params["q"] = bank.q()
    params["g"] = bank.g()
    params["g1"] = bank.g1()
    params["g2"] = bank.g2()
    return jsonify(params)

@app.route('/load_used',methods=['POST'])
def load_used():
    global bank
    used = bank.fetch_used_coins()
    return jsonify(used)

@app.route('/load_wallets',methods=['POST'])
def load_wallets():
    global bank,users
    wallets = {ID:"%s"%(users[ID].wallet) for ID in users.keys()}
    return jsonify(wallets)
    
@app.route('/load_accounts',methods=['POST'])
def load_accounts():
    global bank
    accounts_table = bank.fetch_accounts()
    return jsonify(accounts_table)

@app.route('/update_tab',methods=['POST'])
def update_tab():
    return {}

@app.route('/used')
def used():
    return render_template('used.html')

@app.route('/wallets')
def wallets():
    return render_template('wallets.html')

@app.route('/accounts')
def accounts():
    return render_template('accounts.html')

@app.route('/cheat')
def cheat():
    return render_template('cheat.html')

@app.route('/deposit')
def deposit():
    return render_template('deposit.html')

@app.route('/deduct')
def deduct():
    return render_template('deduct.html')

@app.route('/send')
def send():
    return render_template('send.html')

@app.route('/register')
def register():
    return render_template('register.html')

@app.route('/overview')
def overview():
    return render_template('overview.html')

@app.route('/about')
def about():
    return render_template('about.html')

@app.route('/demo')
def demo():
    return render_template('demo.html')

@app.route('/',methods=["GET", "POST"])
def index():
    init()
    iframe = "index.html"
    return render_template("index.html")

if __name__ == '__main__':
    app.run(debug = True)
