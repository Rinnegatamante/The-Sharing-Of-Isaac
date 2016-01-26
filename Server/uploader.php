<?

	/* Configuration*/
	$host_db='DATABASE';
	$Login_db='LOGIN';
	$pwd_db='PASS';
	$bdd_name='TABLE';
	/* end of Configuration */
	
	mysql_connect($host_db,$Login_db,$pwd_db);
	
	/* Data submit*/
	$Login=$_POST['l'];
	$password=$_POST['p'];
	$Login2=addslashes($_POST['l']);
	$password2=addslashes($_POST['p']);
	/* end of Data submit*/
	if ((strcmp($Login, $Login2) == 0) and (strcmp($password, $password2) == 0)){
	if (strlen($password)>1 and strlen($password)<11 and strlen($Login)>1 and strlen($Login)<13){
		
		$result=mysql_query("select password from `$Login_db`.`$bdd_name` where id='$Login' and password=password('$password')");
		$rs=mysql_fetch_object($result);
		if($rs){

			$uploaddir = 'PATH' . $Login . '_';
			$uploadfile = $uploaddir . basename($_FILES['save']['name']);
			if ($_FILES['save']['size'] > 1024 and $_FILES['save']['size'] < 2048){
				if (strcmp($_FILES['save']['name'],"gamedata1.dat") == 0){
					if (move_uploaded_file($_FILES['save']['tmp_name'], $uploadfile)) {
						echo "1";
					} else {
						echo "0";
					}
				}else{
					echo "0";
				}
			}else{
				echo "0";
			}

			exit;
		}else{
			echo "0";exit;
		}
	}else{
		echo "0";exit;
	}
	}else{
		echo "0";exit;
	}
?>