$("#deduct-form").on("submit",function(e){
    // 以下のコードは引き出しタブにある引き出しボタンが押されたときに呼ばれる。
    
    let formData = $(this).serialize();    
    let isSuspended = isIDsuspended("#deduct-id"); //アカウントが停止されているかチェック
    
    if(isSuspended == false){ //アカウントが停止"されていない"場合
	sendFormDataToFlask(e,"deduct_coin",formData);
    }else{
	e.preventDefault(); // フォームの送信を阻止。
    }    
});

