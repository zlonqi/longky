$(function() {
	$('#userName').change(function() {
		if ($('#userName').val().length == 0) {
			$('#mess').attr({"src":"images/pic/error.png"});
			$("#nmlabel").html("<span style='color:red;'>用户名不能为空！</span>")
			$('#mess').show();
		} else {
			$("#nmlabel").html("")
			$.ajax({
				//type : "get", //使用get方法访问后台  
				type:"post",//使用post方法访问后台  
				dataType : "text", //返回数据格式
				url : "/cgi/register_check.cgi", //要访问的后台地址
				//contentType : "application/json;charset=utf-8",
				contentType :"application/x-www-form-urlencoded",//如果是post方式，必须是这个,get可以任意方式。
				data : {
					userName : $('#userName').val()
				}, //要发送的数据，采用josn格式
				beforeSend : function() {
				},//AJAX请求之前
				complete : function() {
				}, //AJAX请求完成时  
				success : function(data) { //data为返回的数据
					if (data == "no") {
						$("#nmlabel").html("")
						$('#mess').attr({"src":"images/pic/correct.png"});
						$('#mess').show();
					} else {
						$('#mess').attr({"src":"images/pic/error.png"});
						$('#mess').show();
						$('#userName').val("");
						$("#nmlabel").html("<span style='color:red;'>此用户已存在</span>")
						$('#userName').focus();
					};
				},
				error : function(XMLResponse) {
					alert(XMLResponse.responseText);
				}
			});
		}
	})
})