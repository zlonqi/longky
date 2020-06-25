/**
 * 
 */

$(function(){
	  
	$("#mm").blur(function(){
		$("#pw").html("<span style='color:red;'>含(@#$%&!),数字,字母！</span>");
		if($("#mm").val()=="")
	{
		$("#pw").html("<span style='color:red;'>密码不能为空！</span>");
	}else{
		var a=/^.*\d+.*$/;
		var b=/^.*[a-zA-Z]+.*$/;
		var c=/^.*[@#$%&!]+.*$/;
		if(!a.test($("#mm").val()) || !(b.test($("#mm").val())) ||!(c.test($("#mm").val())))
		{
			$("#pw").html("<span style='color:red;'>含(@#$%&!),数字,字母！</span>");
			$("#mm").focus();
		}
		else
		{
			if($("#mm").val().length<6 || $("#mm").val().length>14)
			{
				$("#pw").html("<span style='color:red;'>密码6~14位</span>");
				$("#mm").focus();
			}else{
				$("#mess1").show();
				$("#pw").html("");
				$("#pw1").html("<span style='color:red;'>确认你的密码</span>");
				$("#mm1").blur(function(){
					if($("#mm1").val()=="")
				{
					$("#pw1").html("<span style='color:red;'>密码不能为空！</span>");
					$("#mm1").focus();
				}
				else
				{
					if(document.getElementById('mm').value!=document.getElementById('mm1').value)
					{
						$("#pw1").html("<span style='color:red;'>密码不相等，请重录！</span>");
						$("#mm1").val("");
						$("#mm").val("");
						$("#mm").focus();
					}
					else
					{
						$("#mess2").show();
						$("#pw1").html("");
						$("#yx").blur(function()
					{
						var a=/^[a-z]+\w*@\w+\.\w+$/;
						if(!a.test($("#yx").val()))
						{
						   $("#mail").html("<span style='color:red;'>格式:123@foxmail.com</span>"); 
						   $("#yx").focus();	
						}
						else
						{
							$("#mess3").show();
							$("#mail").html("")
						}
						})
					}
					
					
				}
					})
			}
		}
	}
		})
		 $("#butt1").click(function()
							{
								if($("#userName").val()=="")
								{
									$("#nmlabel").html("<span style='color:red;'>用户名不能为空！</span>");
									$("#userName").focus();//光标进入
								}else{
									if($("#mm").val()=="")
									{
										$("#pw").html("<span style='color:red;'>密码不能为空！</span>");
										$("#mm").focus();//光标进入
									}
									else{
										 if($("#mm1").val()=="")
											{
												$("#pw1").html("<span style='color:red;'>密码不能为空！</span>");
												$("#mm1").focus();//光标进入
											}
											else{
												 if($("#yx").val()=="")
													{
														$("#mail").html("<span style='color:red;'>邮箱不能为空！</span>");
														$("#yx").focus();//光标进入
													}else{
														if($("#userName").val()!=""&& $("#mm1").val()!=""&&$("#yx").val()!="")
														{
														    $("#form1").submit();
															
														}
													}
											}
									}
								}
							})
})