$("#deposit-form").on("submit",function(e){
    // 以下のコードは預金タブにある預けるボタンが押されたときに呼ばれる。

    let formData = $(this).serialize();
    let isSuspended = isIDsuspended("#deposit-id"); //アカウントが停止されているかチェック
    
    if(isSuspended == false){ //アカウントが停止"されていない"場合
	sendFormDataToFlask(e,"deposit_coin",formData,function(status,data){	    
	    // ダブルスペンドを検出した場合。
	    if(status == "double_spent" && data.length == 1){//(data.length != 1)はチーターIDを特定できなかった場合。
		let cheaterID = data[0].toString();
		cheaters.push(cheaterID); // チーターIDを記録。
	    }
	});
    }else{
	e.preventDefault(); // フォームの送信を阻止。
    }    
});
