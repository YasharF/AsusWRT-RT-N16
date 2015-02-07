<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title>ASUS Wireless Router <#Web_Title#> - <#menu5_6_2#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="usp_style.css">
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/jquery.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" language="JavaScript" src="/help.js"></script>
<script type="text/javascript" language="JavaScript" src="/detect.js"></script>
<script>
wan_route_x = '<% nvram_get_x("IPConnection", "wan_route_x"); %>';
wan_nat_x = '<% nvram_get_x("IPConnection", "wan_nat_x"); %>';
wan_proto = '<% nvram_get_x("Layer3Forwarding",  "wan_proto"); %>';

<% login_state_hook(); %>
var wireless = [<% wl_auth_list(); %>];	// [[MAC, associated, authorized], ...]

function initial(){
	show_banner(1);
	
	show_menu(5,6,1);
	
	show_footer();

	setScenerion(sw_mode);
}

function saveMode(){
	//document.form.next_page.value = '/QIS_wizard.htm';
	
	if(document.form.sw_mode[0].checked == true){
		document.form.flag.value = 'router_mode';
	}
	else if(document.form.sw_mode[1].checked == true){
		document.form.action="/survey.asp"
	}
	else if(document.form.sw_mode[2].checked == true){
		document.form.flag.value = 'adv_repeater_mode';
	}
	
	document.form.submit();
}

function applyRule(){

		document.form.action_mode.value = " Apply ";
		document.form.next_page.value = "";
		
		document.form.submit();
}

function done_validating(action){
	refreshpage();
}

var $j = jQuery.noConflict();
var id_WANunplungHint;

function setScenerion(mode){
	document.form.sw_mode[mode-1].checked = true;
	if(mode == '1'){
		$j("#Senario").css("background","url(/images/rt.gif) no-repeat");
		$j("#radio2").hide();
		$j("#Internet_span").hide();
		$j("#ap-line").show();
		//$j("#ap-line").animate({width:"133px"}, 2000);
		$j("#AP").html("<#Internet#>");
		$j("#mode_desc").html("<#OP_RT_desc1#>");
		$j("#nextButton").attr("value","<#CTL_next#>");
	}	
	else if(mode == '2'){
		$j("#Senario").css("background", "url(/images/re.gif) no-repeat");
		$j("#radio2").css("display", "block");
		$j("#Internet_span").css("display", "block");
		$j("#AP").html("<#Device_type_03_AP#>");
		$j("#mode_desc").html("<#OP_RE_desc1#>");
		$j("#nextButton").attr("value","<#CTL_next#>");
		clearTimeout(id_WANunplungHint);
		$j("#Unplug-hint").css("display", "none");
		$j("#ap-line").css("display", "none");
	}
	else if(mode == '3'){
		$j("#Senario").css("background", "url(/images/re.gif) no-repeat");
		$j("#radio2").css("display", "none");
		$j("#Internet_span").css("display", "block");
		$j("#ap-line").css("display", "block");
		$j("#AP").html("<#Device_type_02_RT#>");
		$j("#mode_desc").html("<#OP_AP_desc1#>");
		$j("#nextButton").attr("value","<#CTL_next#>");
		clearTimeout(id_WANunplungHint);
		$j("#Unplug-hint").css("display", "none");
	}
}

</script>
</head>

<body onload="initial();" onunLoad="disable_auto_hint(11, 3);return unload_body();">
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" id="ruleForm" action="/QIS_wizard.htm">
<input type="hidden" name="productid" value="<% nvram_get_f("general.log","productid"); %>">

<input type="hidden" name="productid" value="<% nvram_get_f("general.log","productid"); %>">

<input type="hidden" name="prev_page" value="/Advanced_OperationMode_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get_x("LANGUAGE", "preferred_lang"); %>">
<input type="hidden" name="wl_ssid2" value="<% nvram_get_x("WLANConfig11b",  "wl_ssid2"); %>">
<input type="hidden" name="firmver" value="<% nvram_get_x("",  "firmver"); %>">
<input type="hidden" name="flag" value="">

<table class="content" align="center" cellpadding="0" cellspacing="0">
  <tr>
	<td width="23">&nbsp;</td>
	<td valign="top" width="202">
	  <div id="mainMenu"></div>
	  <div id="subMenu"></div>
	</td>
	
    <td valign="top">
	<div id="tabMenu" class="submenuBlock"></div><br />
		<!--===================================Beginning of Main Content===========================================-->
<table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td valign="top" >
		
<table width="95%" border="0" align="center" cellpadding="5" cellspacing="0" class="FormTitle">
	<thead>
	<tr>
		<td><#t1SYS#> - <#t2OP#></td>
	</tr>
	</thead>
	<tbody>
	<tr>
	  <td bgcolor="#FFFFFF"><#OP_desc1#></td>
	  </tr>
	</tbody>
	<tr>
	  <td bgcolor="#C0DAE4">
	<fieldset style="width:95%; margin:0 auto; padding-bottom:3px;">
	<legend>
		<span style="font-size:13px; font-weight:bold;">
			<input type="radio" name="sw_mode" class="input" value="1" onclick="setScenerion(1);"><#OP_RT_item#>
			<input type="radio" name="sw_mode" class="input" value="2" onclick="setScenerion(2);"><#OP_RE_item#>
			<input type="radio" name="sw_mode" class="input" value="3" onclick="setScenerion(3);"><#OP_AP_item#>
		</span>
	</legend>
	<div id="mode_desc" style="height:60px;">
		<#OP_RT_desc1#>		
	</div>
	<div id="Senario" style="z-index:99; position:relative; top:0px; left:0px;">
		<span style="margin:140px 0px 0px 200px;"><#Web_Title#></span>
		<span id="AP" style="margin:120px 0px 0px 355px;"><#Device_type_03_AP#></span>
		<span id="Internet_span" style="margin:70px 0px 0px 405px;"><#Internet#></span>
		<span style="margin:220px 0px 0px 40px;"><#Wireless_Clients#></span>
		<span style="margin:220px 0px 0px 360px;"><#Wired_Clients#></span>
		<!--EMBED id="radio1" style="position:absolute; margin:150px 0px 0px 100px;" src="/images/radio1.gif" width=85 height=85 type=application/x-shockwave-flash allowscriptaccess="never" wmode="transparent"></EMBED-->
			<object id="radio1" style="position:absolute; margin:130px 0px 0px 100px;" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" width="85" height="85">
			<param name="movie" value="/images/radio1.gif" />
			<param name="wmode" value="transparent">
			<param name="quality" value="high" />
			<embed src="/images/radio1.gif" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash" width="85" height="85" wmode="transparent"></embed>
			</object>
			<object id="radio2" style="position:absolute; margin:70px 0px 0px 240px; background:transparent;" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" width="85" height="85">
			<param name="movie" value="/images/radio2.gif" />
			<param name="wmode" value="transparent">
			<param name="quality" value="high" />
			<embed src="/images/radio2.gif" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash" width="85" height="85" wmode="transparent"></embed>
			</object>
		<!--EMBED id="radio2" style="position:absolute; margin:90px 0px 0px 240px;" src="/images/radio2.gif" width=85 height=85 type=application/x-shockwave-flash allowscriptaccess="never" wmode="transparent"></EMBED-->
		<div id="ap-line" style="border:0px solid #333;margin:77px 0px 0px 245px;width:133px; height:41px; position:absolute; background:url(/images/wanlink.gif) no-repeat;"></div>
		<div id="Unplug-hint" style="border:2px solid red; background-color:#FFF; padding:3px;margin:0px 0px 0px 150px;width:250px; position:absolute; display:block; display:none;"><#web_redirect_suggestion1#></div>
		<div id="switch_hint"></div>
	</div>	
	</fieldset>
	  </td>
	</tr>
	<tr>
		<td align="right" bgColor="#C0DAE4">
			<input name="button" type="button" class="button" onClick="saveMode();" value="<#CTL_onlysave#>">
		</td>
	</tr>
</table>

</td>
		<td id="help_td" style="width:15px;" valign="top">
			 <div id="helpicon" onClick="openHint(0,0);" title="<#Help_button_default_hint#>"><img src="images/help.gif" /></div>
	  	<div id="hintofPM" style="display:none;">
	    <table width="100%" cellpadding="0" cellspacing="1" class="Help" bgcolor="#999999">
		  <thead>
		  <tr>
			<td>
			  <div id="helpname" class="AiHintTitle"></div>
			  <a href="javascript:;" onclick="closeHint()" ><img src="images/button-close.gif" class="closebutton" /></a>
			</td>
		  </tr>
		  </thead>
		  
		  <tr>				
			<td valign="top" >
			  <div class="hint_body2" id="hint_body"></div>
			  <iframe id="statusframe" name="statusframe" class="statusframe" src="" frameborder="0"></iframe>
			</td>
		  </tr>
			</table>
	  	</div><!--End of hintofPM--> 
		 </td>
		</tr>
      </table>
		<!--===================================Ending of Main Content===========================================-->
	</td>
    <td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>

</form>
<div id="footer"></div>
</body>
</html>
