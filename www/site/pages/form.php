 <!doctype html>

<html lang="en">
<head>
	<meta charset="utf-8">
	<title>Form submit</title>
</head>
<body>
 <?php
	echo "<h1> CGI for ". $_SERVER['REQUEST_METHOD']."  request</h1>";
	echo "<hr>";
	if ($_SERVER['REQUEST_METHOD'] === 'POST')
	{
		echo "<h1> CGI for POST request</h1>";
		$requestBody = file_get_contents('php://input');
		parse_str($requestBody, $_POST);
		$name = $_POST['name'];
		$email = $_POST['email'];
		$message = $_POST['message'];
		
		echo "<h2>Name:" . $name . "\r\n</h2>";
		echo "<h2>Email:" . $email . "\r\n</h2>";
		echo "<h2>Message:" . $message . "\r\n</h2>";
	}

	if ($_SERVER['REQUEST_METHOD'] === 'GET')
	{
		echo "<h1> CGI for GET request</h1>";
		// $requestBody = file_get_contents('php://input');
		// parse_str($requestBody, $_GET);
		$name = $_GET['name'];
		$email = $_GET['email'];
		$message = $_GET['message'];

		echo "<h2>Name: " . $name . "</h2>";
		echo "<h2>Email: " . $email . "</h2>";
		echo "<h2>Message: " . $message . "</h2>";

	}
	// // Send email with submitted information
	// $to = 'you@example.com';
	// $subject = 'New message from website form';
	// $body = "Name: $name\nEmail: $email\nMessage: $message";
	// $headers = 'From: webmaster@example.com' . "\r\n" .
	// 			'Reply-To: ' . $email . "\r\n" .
	// 			'X-Mailer: PHP/' . phpversion();

	// if (mail($to, $subject, $body, $headers)) {
	// 	echo 'Message sent successfully.';
	// } else {
	// 	echo 'An error occurred while sending the message.';
	// }
	?>
</body>