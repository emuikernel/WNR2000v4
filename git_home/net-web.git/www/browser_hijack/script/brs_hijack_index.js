function initIndex()
{
	var page_body = document.getElementsByTagName("body");
	
	page_body.onload = loadValue();

	changeUrl();
}

function loadValue()
{	
	var content_frame = document.getElementById("content_frame");

	if( from_restore == "1" )
		content_frame.setAttribute("src", "BRS_03B_haveBackupFile_ping.html");
	else if( from_nowan == "1" )
		content_frame.setAttribute("src", "BRS_03A_A_noWan_check_net.html");
	else
		content_frame.setAttribute("src", "BRS_01_checkNet_ping.html");
	
	showFirmVersion("none");
}

function changeUrl()
{
	if((dns_hijack == "1") && (this.location.hostname != lanip && this.location.hostname.indexOf("routerlogin.net") == -1 && this.location.hostname.indexOf("routerlogin.com") == -1))
		this.location.hostname = "www.routerlogin.net";
}

addLoadEvent(initIndex);
