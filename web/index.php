<html>
	<head>
		<title>RAICHU</title>
		<?php
			$db_handle = mysqli_connect("localhost", "root", "admin", "raichu") or die(mysql_error("Error connecting to MySQL server"));
			$query = "SELECT * FROM song"; 
			$result = mysqli_query($db_handle, $query) or die(mysql_error());
		?>
		
		<link rel="stylesheet" href="bootstrap/css/bootstrap.css" type="text/css">
	</head>
	<body>
		<form name='song list' action='process.php' method='post'>
			<select multiple='multiple' name='song[]'>
		<?php
				while($item = mysqli_fetch_array($result))
					echo "<option value=\"". $item['title']. "\">" . $item['title']. "</option>";  		
		?>
			</select>
			</br>
			<hr>
			<input class='btn' name='create' value='create playlist' type='submit'>
			<input class='btn' name='play' value='play' type='submit'>
			<input class='btn' name='stop' value='stop' type='submit'>
			<input class='btn' name='forward' value='forward' type='submit'>
			<input class='btn' name='back' value='back' type='submit'>
			<input class='btn' name='pause' value='pause/resume' type='submit'>
			<input class='btn' name='volup' value='volume up' type='submit'>
			<input class='btn' name='voldown' value='volume down' type='submit'>
			<hr>
		</form>
	</body>
</html>
