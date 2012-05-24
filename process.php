
<html>
<head>
<title>Magic</title>
</head>
<body>


<?php

	

// Set the ip and port we will listen on
$address = '130.65.178.125';
$port = 2222;

// Create a TCP Stream socket
$sock = socket_create(AF_INET, SOCK_STREAM, 0);
if($socket){
	throw new Exception("Error: socket create failed");
}

socket_connect($sock, $address, $port) or die(socket_strerror(socket_last_error($socket)));

$packet = "";
foreach($_POST['crap'] as $names)
{
	$packet .= $names . '#';
}
echo $packet;

socket_write($sock, $packet, strlen($packet)) or die("Error: could not send data to server");
socket_close($socket);

?>
</body>
</html>