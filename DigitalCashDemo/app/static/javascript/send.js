$("#send-form").on("submit",function(e){
    // 以下のコードは送信タブにある送信ボタンが押されたときに呼ばれる。

    let formData = $(this).serialize();    
    sendFormDataToFlask(e,"send_coin",formData);    
});
