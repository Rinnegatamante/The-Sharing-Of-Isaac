<html>
<title>The Sharing of Isaac - Registration</title>
<head>
</head>
<body background="wall.jpg">
<style>
table {
	border-radius: 5px;
	margin: 8px;
}
</style>
<?php

/* Configuration*/
$host_db='HOST';
$Login_db='LOGIN';
$pwd_db='PASSWORD';
$bdd_name='TABLE';
/* end of Configuration */

if(isset($_POST['Login'])){

	/* Data submit*/
	$Login=$_POST['Login'];
	$password=$_POST['password'];
	$password2=$_POST['password2'];
	$email=$_POST['email'];
	/* end of Data submit*/

	mysql_connect($host_db,$Login_db,$pwd_db);
	
	echo "<center><img src='logo.png'><br><br><table cellpadding='5' cellspacing='1' border='1' bordercolor=#2F4E50 bordercolorlight=#2F4E50 bordercolordark=#2F4E50 bgcolor=#FFFFFF align='center'><tr><td>";
	
	if ($password==$password2){
		if (strlen($password)>1 and strlen($password)<11 and strlen($Login)>1 and strlen($Login)<13 and strlen($email)<30 ){
			$result=mysql_query("select id from `$Login_db`.`$bdd_name` where id='$Login'");
			$rs=mysql_fetch_object($result);
			if($rs){
				echo("<font face='Verdana' color='black'>This ID already exists! </font><a href=\"javascript:history.back();\"><font face='Verdana' color='black'>Go Back!</font></a>");
			}else{
				if(mysql_query("INSERT INTO `$Login_db`.`$bdd_name` (`id` , `password` ,`email`)
									VALUES ('$Login', PASSWORD('$password'), '$email');")){
					echo("<font face='Verdana' color='black'>$Login user created successfully!");
				}else{
					echo "<font face='Verdana' color='black'>$Login already exists! </font><a href=\"javascript:history.back();\"><font face='Verdana' color='black'>Go Back!</font></a>";
				}
			}
		}else{
			echo("<font face='Verdana' color='black'>Wrong data! </font><a href=\"javascript:history.back();\"><font face='Verdana' color='black'>Go Back!</font></a>");
		}
	}else{
		echo "<font face='Verdana' color='black'>Wrong password!</font> <a href=\"javascript:history.back();\"><font face='Verdana' color='black'>Go Back!</font></a>";
	}
	
	echo "</td></tr></table>";
	
}else{

 
	echo "	<FORM action='register.php' method='post'>
	
			<center><img src='logo.png'><br><br><table cellpadding='5' cellspacing='1' border='1' bordercolor=#2F4E50 bordercolorlight=#2F4E50 bordercolordark=#2F4E50 bgcolor=#FFFFFF align='center'>
            <tr><td colspan='2'><font color=red><center><b>Registration Form</b></center></font></td></tr>
			<tr>


            <td><font face='Verdana' color='black'>ID : </font><font face='Verdana' color='red' size='1'>(Max 12 characters)</font></td><td> 
            <input tabindex='1' name='Login' class='application' size='30' maxlength='12' /></td>
            </tr>

            <tr>
            <td><font face='Verdana' color='black'>Password :</font><font face='Verdana' color='red' size='1'> (Max 10 characters)</font></td><td> 
            <input tabindex='2' name='password' type='password' class='application' size='30' maxlength='10' /></td>

            </tr>

            <tr>
            <td><font face='Verdana' color='black'>Confirm Password :</font></td><td> 
            <input tabindex='3' name='password2' type='password' class='application' size='30' maxlength='10' /></td>
			</tr>
            </tr>
			
            <tr>
            <td><font face='Verdana' color='black'>Email :</font></td><td> 
            <input tabindex='6' name='email' class='application' size='30' maxlength='30' /></td>

            </tr>
            <tr>
            
            </tr>

            </table>
			<br>
			<center><button tabindex='8' name='submit' value='submit' class='eingabe-button' type='submit'>Submit</button></td></center>
			</FORM>";
}
?>
<style>
#footer{
	position:fixed;
	bottom:0px;
	width:100%;
}
</style>
<div id="footer" align="center">
<center><table cellpadding='5' cellspacing='1' border='2' bordercolor=#2F4E50 bordercolorlight=#2F4E50 bordercolordark=#2F4E50 bgcolor=#FFFFFF align='center'>
<tr><td><font color=red><center><b>Server Data</b></center></font></td></tr>
<tr><td>Server Address: <? echo gethostbyname('www.rinnegatamante.it'); ?></font></td></tr>
<tr><td>Host Address: rinnegatamante.it</font></td></tr>
</table><br>
<embed src="bgm.mp3" autostart="true" loop="true"
width="200" height="16"></center><br>
</div>
</embed> 
</html>