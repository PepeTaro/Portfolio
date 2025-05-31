/* 
 以下の変数が必要な理由は、例えばデモから説明ページに切り替えて、その後にデモページに戻ったときに、
 毎回概要タブページ、アカウントページがロードされ多少使いにくいため。
*/

let defaultMainTab  = "about"
let defaultUpperTab = "overview" // upper-tab(概要、送信などが含まれるタブのこと)のデフォルトのタブ
let defaultLowerTab = "accounts" // lower-tab(アカウント、ウォレットなどが含まれるタブのこと)のデフォルトのタブ

var cheaters = [];// チーター(ダブルスペンドしたアカウント)を記録。

$(function(){
    // ここのコードは、index.htmlがロードされたときに呼ばれる
    
    // デフォルトページを表示
    $("#main-wrapper").load(defaultMainTab,function(){	
	// デフォルトタブのページをロード
	$("#upper-tab-wrapper").load(defaultUpperTab);	
	$("#lower-tab-wrapper").load(defaultLowerTab,function(){
	    refreshAccountsTable(); // ロード後にテーブルを更新。
	});	
    });
});

function isUpperTabPage(page){
    return (page == "overview" || page == "register" || page == "send" || page == "deduct" || page == "deposit" || page == "cheat");
}

function isLowerTabPage(page){
    return (page == "accounts" || page == "wallets" || page == "used");
}

function refreshTable(page){
    // 引数pageに応じてテーブルを更新。
    
    if(page == "accounts"){
	refreshAccountsTable();
    }else if(page == "wallets"){
	refreshWalletsTable();
    }else if(page == "used"){
	refreshUsedTable();
    }else{
	console.log("Error(refreshTable): そのようなページは存在しません.");
    }
}

function pageHandler(page){
    // 引数pageに応じて、どのページをDOMにロードするかを選択する。
    
    if(page == "about"){ // 説明ページが押されたとき
	$("#main-wrapper").load(page); // ページ(page)を読み出し
	
    }else if(page == "demo"){ // デモページが押されたとき
	$("#main-wrapper").load(page,function(){
	    // 上下のタブをロード
	    loadPage(defaultUpperTab);	
	    loadPage(defaultLowerTab); 
	});	
	
    }else if(isUpperTabPage(page)){ // upper-tabに含まれるページの場合
	defaultUpperTab = page // 直前に押したタブページをデフォルトに設定。
	$("#upper-tab-wrapper").load(page);
	
    }else if(isLowerTabPage(page)){ // lower-tabに含まれるページの場合
	defaultLowerTab = page // 直前に押したタブページをデフォルトに設定。
	$("#lower-tab-wrapper").load(page,function(){
	    refreshTable(page); // ロードしたページのテーブルを更新。	
	});	
    }    
}

function loadPage(page){
    // タブボタンが押されたときに呼ばれる関数。
    // 引数pageにどのタブボタンが押されたかを示す文字列が含まれる。

    $.post({
	url: "update_tab",
	success: function(){
	    pageHandler(page);
	},
	error: function(XMLHttpRequest,textStatus,errorThrown) { 
            alert("Status: " + textStatus); alert("Error: " + errorThrown);
        }
    });
    
}
