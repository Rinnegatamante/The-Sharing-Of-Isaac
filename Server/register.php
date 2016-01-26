<html>
<title>The Sharing of Isaac - Registration</title>
<head>
</head>
<style>
table {
	border-radius: 5px;
	margin: 8px;
}
body {
    background-image:    url(wall.jpg);
    background-size:     cover;                      /* <------ */
    background-repeat:   no-repeat;
}
</style>
<body>
<?php

/* Configuration*/
$host_db='DATABASE';
$Login_db='LOGIN';
$pwd_db='PASS';
$bdd_name='TABLE';
/* end of Configuration */

if(isset($_POST['Login'])){

	mysql_connect($host_db,$Login_db,$pwd_db);
	
	/* Data submit*/
	$Login=$_POST['Login'];
	$Login2=addslashes($_POST['Login']);
	$password=$_POST['password'];
	$password2=$_POST['password2'];
	$password3=addslashes($_POST['password']);
	$email=$_POST['email'];
	$email2=addslashes($_POST['email']);
	/* end of Data submit*/
	
	echo "<center><img src='logo.png'><br><br><table cellpadding='5' cellspacing='1' border='1' bordercolor=#2F4E50 bordercolorlight=#2F4E50 bordercolordark=#2F4E50 bgcolor=#FFFFFF align='center'><tr><td>";
	if ((strcmp($Login, $Login2) == 0) and (strcmp($password, $password3) == 0) and (strcmp($email, $email2) == 0)){
	if ($password==$password2){
		if (strlen($password)>1 and strlen($password)<11 and strlen($Login)>1 and strlen($Login)<13 and strlen($email)<30 ){
			$result=mysql_query("select id from `$Login_db`.`$bdd_name` where id='$Login'");
			$rs=mysql_fetch_object($result);
			if($rs){
				echo("<font face='Verdana' color='black'>This ID already exists! </font><a href=\"javascript:history.back();\"><font face='Verdana' color='black'>Go Back!</font></a>");
			}else{
				if(mysql_query("INSERT INTO `$Login_db`.`$bdd_name` (`id` , `password` ,`email`)
									VALUES ('$Login', PASSWORD('$password'), '$email');")){
					echo("<font face='Verdana' color='black'>Welcome on <b>The Sharing of Isaac</b>, $Login!<br>");
?>
	<br><font size=4 color = red>Get Started (Windows Tutorial)</font><br><br>
	<font size=2>
	First of all download the Starter Kit from this link: <a href="http://adf.ly/1VsDp8">Starter Kit v.0.1 BETA</a>.<br><br>
	Then you must create a file named as <b>TSOI.cfg</b> (you'll need also a duplicate, one for 3DS and another for PC) with this text inside:<br><br>
	</font>
	<?
	$host = gethostbyname('www.rinnegatamante.it');
	echo "<textarea name='xml' cols='40' rows='3' >
<id>$Login</id>
<password>$password</password>
<host>$host</host>
	</textarea>";
	?><br><br><font size=4 color = red>PC Setup</font><br><br><font size=2>
	Place the <b>content of "PC/Windows 64 Bit" folder</b> and a <b>TSOI.cfg</b> file in the folder where TBOI: Rebirth saves your files on Steam (Usually it is something like: <b>C:\Program Files (x86)\Steam\userdata\YOUR_ID\250900\remote</b>) like in <a href="setup1.png">THIS SCREENSHOT</a>.<br><br>
	I suggest you to create two shortcuts and place them in a comfortable place (Desktop or similars), one for <b>uploader.exe</b> and one for <b>downloader.exe</b> to use TSOI rapidly.
	</font><br><br><font size=4 color = red>N3DS Setup</font><br><br><font size=2>
	Place the <b>content of N3DS folder</b> in <b>/3ds/TSOI</b> and place a <b>TSOI.cfg</b> in the root of your microSD card like in <a href="setup2.png">THIS SCREENSHOT</a>.<br>
	You'll need also to install <b>hblauncher_loader</b> by <b>Yellows8</b> through whatever CIA Installer you want. (<a href="https://github.com/yellows8/hblauncher_loader/releases">Download hblauncher_loader</a>)<br>
	Make also sure to possess a <b>crypto-fixed</b> CIA of the game installed.
	</font><br><br><font size=4 color = red>How to use (PC)</font><br><br><font size=2>
	When you want to <b>update</b> the savegame in the server, just start <b>uploader.exe shortcut</b>.<br>
	When you want to <b>play TBOI: RE with LATEST savegame in the server</b>, just start <b>downloader.exe shortcut</b>. It will also start the game automatically after updating local savegame.<br>
	</font><br><br><font size=4 color = red>How to use (N3DS)</font><br><br><font size=2>
	When you want to <b>update</b> the local or the server savegame, start <b>hbauncher_loader</b> from emuNand.<br>
	Then launch <b>The Sharing of Isaac</b> and follow the instructions on the screen according to what you want to do.<br>
	</font><?
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
	}else{
		echo "Nice try, little cracker.";
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
<embed src="bgm.mp3" autostart="true" loop="true"
width="2" height="0"><br>
</div>
</embed> 
</html>