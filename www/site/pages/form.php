 <?php
	if ($_SERVER['REQUEST_METHOD'] === 'POST')
	{
		
		$requestBody = file_get_contents('php://input');
		parse_str($requestBody, $_POST);
		$name = $_POST['name'];
		$email = $_POST['email'];
		$message = $_POST['message'];
		
		echo "Name:" . $name . "\n";
		echo "Email:" . $email . "\n";
		echo "Message:" . $message . "\n";

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
	}
?>