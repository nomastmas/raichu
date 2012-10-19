function getDismissibleAlert(title, msg, alertType)
{
	var alert = '<div class="alert alert-block alert-' + alertType + ' fade in">';
		alert += '<a class="close" data-dismiss="alert" href="#">&times;</a>';
		alert += '<strong>' + title + '</strong></BR>' + msg;
		alert += '</div>';
		
	return alert;
}