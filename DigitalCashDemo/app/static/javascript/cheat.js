$("#cheat-form").on("submit",function(e){
    // 以下のコードはダブルスペンドタブにあるダブルスペンドボタンが押されたときに呼ばれる。
    let formData = $(this).serialize();
    sendFormDataToFlask(e,"double_spend_coin",formData);
});
