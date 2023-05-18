<?php
	if ($_SERVER['REQUEST_METHOD'] === 'POST')
	{
	$name = $_POST['name'];
	$email = $_POST['email'];
	$message = $_POST['message'];
	
	// Send email with submitted information
	$to = 'you@example.com';
	$subject = 'New message from website form';
	$body = "Name: $name\nEmail: $email\nMessage: $message";
	$headers = 'From: webmaster@example.com' . "\r\n" .
				'Reply-To: ' . $email . "\r\n" .
				'X-Mailer: PHP/' . phpversion();

	if (mail($to, $subject, $body, $headers)) {
		echo 'Message sent successfully.';
	} else {
		echo 'An error occurred while sending the message.';
	}
	}
?>