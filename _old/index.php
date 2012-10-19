<html>
<head>
<title>Tricky</title>
</head>
<body>


<?php



mysql_connect("localhost", "root","") or die(mysql_error());
mysql_select_db("raichu") or die(mysql_error());
$querytext = "SELECT * FROM song"; 
$cup = mysql_query($querytext) or die(mysql_error());

/*echo "<form name=\"myWebForm\" action=\"testee.php\" method=\"post\">
	stuffone: <input type=\"checkbox\" name=\"crap\" value=$name  /><br />

	stuff2: <input type=\"checkbox\" name=\"crap\" value=\"too\"  /><br />
	</select>
	<input type=\"submit\" />
</form>";*/

echo "<form name=\"myWebForm\" action=\"process.php\" method=\"post\">
<select multiple=\"multiple\" name = \"crap[]\">";
while($item = mysql_fetch_array($cup))
{
	echo "<option value=\"". $item['title']. "\">" . $item['title']. "</option>";  //<option value=\"volvo\">Volvo</option>
}
echo "</select>
</select>
<input type=\"submit\" />
</form>";

?>



</body>
</html>