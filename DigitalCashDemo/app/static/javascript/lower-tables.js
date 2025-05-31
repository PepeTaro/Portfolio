function isCheater(id){
    // 引数idがチーターである場合True、そうでない場合Falseを返す。
    return cheaters.includes(id);
}

function colorCheatersInTable(idName){
    // idNameで指定されたテーブル(accounts_id,wallets_id)
    //　内において、チーターの行を赤く塗る(警告色?)
    
    for (const id of cheaters) {
	let selector = "#" + idName + "_"+id
	$(selector).css("background-color","red");	
    }
}

function refreshAccountsTable(){
    // accounts-tableを更新
    refreshLowerTable("load_accounts",appendToAccountsTable);       
}

function refreshWalletsTable(){
    // wallets-tableを更新
    refreshLowerTable("load_wallets",appendToWalletsTable);
}

function refreshUsedTable(){
    // used-tableを更新
    refreshLowerTable("load_used",appendToUsedTable);
}

function appendToAccountsTable(data){
    // accounts-tableにデータを追加

    // まずテーブルを初期化
    $("#accounts-table tbody").empty();

    // ヘッダーセルを追加
    let row = "<tr><th>名前</th><th>ID</th><th>所持コイン数</th></tr>"
    $("#accounts-table tbody").append(row);

    // データセルを追加
    for(const id in data){
	let accounts_id = "accounts_id_" + id;
	row = "<tr id="+accounts_id+"><td>"+data[id]["name"]+"</td><td>"+id+"</td><td>" + data[id]["coin"] + "</td></tr>"
	$("#accounts-table tbody").append(row);
    }

    // チーターの行を赤くする
    colorCheatersInTable("accounts_id");
}

function appendToWalletsTable(data){
    // wallets-tableにデータを追加

    // まずテーブルを初期化
    $("#wallets-table tbody").empty();

    // ヘッダーセルを追加
    let row = "<tr><th>ID</th><th>ウォレットの中身(コイン)</th></tr>"
    $("#wallets-table tbody").append(row);

    // データセルを追加
    for(const id in data){
	let wallets_id = "wallets_id_" + id;
	row = "<tr id="+wallets_id+"><td>"+id+"</td><td>"+data[id]+"</td></tr>"
	$("#wallets-table tbody").append(row);		
    }

    // チーターの行を赤くする
    colorCheatersInTable("wallets_id"); 
}

function appendToUsedTable(data){
    // used-tableにデータを追加

    // まずテーブルを初期化
    $("#used-table tbody").empty();

    // ヘッダーセルを追加
    let row = "<tr><th>コイン</th><th>パラメータ</th></tr>"
    $("#used-table tbody").append(row);

    // データセルを追加
    for(const i in data){
	let coin = data[i][0]
	let params = data[i][1]
	row = "<tr><td>"+coin+"</td><td>"+params+"</td></tr>"
	$("#used-table tbody").append(row);	
    }    
}

function refreshLowerTable(url,callback){
    // 3つのテーブル:アカウント、ウォレット、使用したコイン、を更新。
    
    $.post({
	url: url,
	success: function(data){
	    callback(data);
	},error: function(XMLHttpRequest,textStatus,errorThrown) { 
            alert("Status: " + textStatus); alert("Error: " + errorThrown);
        }
    });    
}
