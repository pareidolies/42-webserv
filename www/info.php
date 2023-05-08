<?php
	echo "test\n";

	// Parse the request body as URL-encoded data and populate the $_POST variable
	// parse_str(file_get_contents('php://input'), $_POST);
	print_r($_POST);

	// echo $_POST['age'];
?>
