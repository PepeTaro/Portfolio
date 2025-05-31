$("#register-form").on("submit",function(e){
    // 以下のコードは登録タブにある登録ボタンが押されたときに呼ばれる。
    
    let formData = $(this).serialize();
    sendFormDataToFlask(e,"register_account",formData,function(status,data){
	// 生成されたIDを表示。
	if(data != null){
	    let id = data;
	    $("#registerdName").val('')
	    $("#registerdID").val(id)	    
	}
    });
});
