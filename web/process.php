
<html>
<head>
<title>Magic</title>
</head>
<body>


<?php

//web	->	client
//web	<-	client	*HELLO*
//web	->	client	create:song1.mp3#song2.mp3#song3.mp3#
//web	<-	client	ok
//web	->	client	play
//web	<-	client	ok
//web	->	client	stop
//web	<-	client	ok

function connect_to_server($server_ip){
	$port = 2000;
	$sock = socket_create(AF_INET, SOCK_STREAM, 0);
	
	socket_connect($sock, $server_ip, $port) or die("Error: Can't connect to server");
	//sleep(1);
	socket_recv($sock, $recv_buffer, 7, 0);
	echo $recv_buffer + '<br/>';
	if(strstr($recv_buffer, "*HELLO*")){
		return $sock;
	}
	else{
		return -1;
	}
}
function send_to_server($sock, $buffer){
	socket_send($sock, $buffer, strlen($buffer), 0);
	socket_recv($sock, $recv_buffer, 20, 0);
	if(strcmp("ok", $recv_buffer) === 0){
		//socket_close($sock);
		return 0;
	}
	else{
		return -1;
	}
}


$buffer = "";
$server_ip = '130.65.178.68';
$delay = 2;

//Play
//Forward
//Stop
//Back
//VolUp
//VolDown

/*
if($_POST['play']){
	echo 'play';
}
else if($_POST['stop']){
	echo 'stop';
}
else if($_POST['next']){
	echo 'next';
}
else if($_POST['back']){
	echo 'back';
}
else if($_POST['create']){
	//old
	echo 'create';
	foreach($_POST['song'] as $names)
	{
		//echo "You selected ". $names. "<br/>";
		$buffer .= $names . '#';
	}
	$sock = connect_to_server($server_ip);
	if( $sock !== 0){
		die("Error: *HELLO* not received");
	}
	sleep(1);
	$status = send_to_server($sock, $buffer);
	if($status !== 0){
		die("Error: client response not received");
	}
	else{
		//header redirect
		socket_close($sock);
	}
	//end old
	$server_ip = '130.65.178.131';
	$port = 2000;
	$sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	
	socket_connect($sock, $server_ip, $port) or die("Error: Can't connect to server");
	//sleep(1);
	socket_recv($sock, $recv_buffer, 7, 0);
	echo $recv_buffer + '<br/>';
	
	socket_send($sock, $buffer, strlen($buffer), 0);
	socket_recv($sock, $recv_buffer, 20, 0);
}
*/

if($_POST['play']){
	$buffer = 'Play';
	$delay = 5;
}
else if($_POST['stop']){
	$buffer = 'Stop';
}
else if($_POST['forward']){
	$buffer = 'Forward';
}
else if($_POST['back']){
	$buffer = 'Back';
}
else if($_POST['create']){
	foreach($_POST['song'] as $names)
	{
		//echo "You selected ". $names. "<br/>";
		$buffer .= $names . '#';
	}
}
else if($_POST['volup']){
	$buffer = 'VolUp';
}
else if($_POST['voldown']){
	$buffer = 'VolDown';
}
else if($_POST['pause']){
	$buffer = 'PauseResume';
}


//$address = '130.65.178.68'; $port = 33333;
$address = '130.65.178.95'; $port = 2000;

if((($sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false))
{
	echo "sock create error </br>";
}
//echo "socket created </br>";
if (socket_connect($sock, $address, $port) === false) {
    echo "socket_connect() failed: reason: " . socket_strerror(socket_last_error($sock)) . "\n";
}
//echo "socket connected </br>";
socket_recv($sock, $receive_buffer, 20, 0);
sleep($delay);

//echo "buffer: " . $buffer . '</br>';
socket_send($sock, $buffer, strlen($buffer), 0);
//echo "data sent </br>";
//socket_recv($sock, $receive_buffer, 20, 0);
//echo $receive_buffer;
sleep(2);
if(socket_close($sock, 2) === FALSE){
	die("Error: socket could not close</br>");
}
else{
	//echo "socket closed</br>";
}

header('Location: http://130.65.157.219/~thomas/web/');

?>
</body>
</html>