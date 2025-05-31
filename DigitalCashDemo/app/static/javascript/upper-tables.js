function isIDsuspended(formID){
    // formIDに含まれるIDが停止されている場合True,そうでない場合Falseを返す
    
    let id = $(formID).val() // フォームに入力された値からIDを読み出す
    if(isCheater(id)){
	alert("このアカウントは停止されています。");
	return true;
    }else{
	return false;
    }    
}

function refreshAllTables(){
    // すべてのテーブルを更新。    
    refreshAccountsTable();
    refreshWalletsTable();
    refreshUsedTable();
}

function checkResultStatus(status){
    if(status == "success"){
	refreshAllTables();
    }else if(status == "register"){
	alert("[!] 登録エラー。");
    }else if(status == "no_coin"){
	alert("[!] ウォレットにコインが存在しません。");
    }else if(status == "used_coin"){
	alert("[!] 使用コインエラー。");
    }else if(status == "invalid_coin"){
	alert("[!] コインが正当でない。");
    }else if(status == "double_spent"){
	alert("[!] ダブルスペンドが検出されました。");
    }else if(status == "index"){
	alert("[!] インデックスエラー。");
    }else if(status == "others"){
	alert("[!] エラー");
    }else if(status == "id_error"){
	alert("[!] 入力されたIDが存在しません。");
    }else{
	console.log("不明なステータス");
    }
    return status
}

function sendFormDataToFlask(e,url,form_data,callback){
    // Flaskにform_dataを送信。

    $.post({
	url: url,
	data: form_data,
	success:function(result){
	    let status = checkResultStatus(result["status"]);// Flaskからきたデータのステータスをチェック。
	    if(callback){ // コールバックが設定されている場合、データを引数として呼び出す。
		callback(status,result["data"]); 
	    }
	    refreshAllTables(); // 必要?
	},error: function(XMLHttpRequest,textStatus,errorThrown) { 
            alert("Status: " + textStatus); alert("Error: " + errorThrown);
	}
    });
    
    e.preventDefault(); // フォームの送信を阻止
}
