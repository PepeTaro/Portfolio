$(document).ready(function(){
    // 以下のコードは概要タブが押されたときに呼ばれる。
    displayParamsInOverview();
})

function displayParamsInOverview(){
    // 使用するパラメータを概要タブに表示。
    $.post({
	url: "load_overview",
	success: function(data){
	    p = "<var>p</var>: "   + data["p"];
	    q = "<var>q</var>: "   + data["q"];
	    g = "<var>g</var>: "   + data["g"];
	    g1 = "<var style='font-size:1.0em;'>g<small style='font-size:0.7em;bottom:0px;right:0px;'>1</small></var>: " + data["g1"];
	    g2 = "<var style='font-size:1.0em;'>g<small style='font-size:0.7em;bottom:0px;right:0px;'>2</small></var>: " + data["g2"];
	    $("#param_p").append(p);
	    $("#param_q").append(q);
	    $("#param_g").append(g);
	    $("#param_g1").append(g1);
	    $("#param_g2").append(g2);
	},error: function(XMLHttpRequest,textStatus,errorThrown) { 
            alert("Status: " + textStatus); alert("Error: " + errorThrown);
        }	
    });    
}
