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
		<div class='span6'>
			<div class='hero-unit'>
				<h2>raichu online</h2>
				<hr>
				<form name='song list' action='process.php' method='post'>
					<!-- 
					<select multiple='multiple' name='song[]'>
				<?php
						//while($item = mysqli_fetch_array($result))
						//	echo "<option value=\"". $item['title']. "\">" . $item['title']. "</option>";  		
				?>
					</select>
					</br>
					
					<button class='btn' name='create' value='create playlist' type='submit'>create playlist</button>
					</br>
					</br> ❙❙ ▶
					-->
					<div class='row' id='controlBtn'>
						<div class='span3'>
						<button class='btn' name='back' value='back' type='submit'><back>&#9668;&#9668;</button>
						<button class='btn btn-info btn-large' id='play' name='play' value='play' type='submit'><strong>&#9654;</strong></button>
						<button class='btn' name='forward' value='forward' type='submit'>&#9658;&#9658;
						</button>
						</div>
						<div class='span1'>
						<button class='btn volume' name='volup' value='volume up' type='submit'>&#9650;</button>
						<button class='btn volume' name='voldown' value='volume down' type='submit'>&#9660;</button>
						</div>
						</br></br>
						<div class='span2'>
						<button class='btn btn-danger' name='stop' value='stop' type='submit'>&#9724;</button>
						<button class='btn' name='pause' value='pause/resume' type='submit'>&#10073;&#10073;</button>
						</div>
					</div>
					<hr>
				</form>
			</div>
		</div>
	</body>
</html>
