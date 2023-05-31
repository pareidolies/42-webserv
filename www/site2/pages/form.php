<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Exemple de requête CGI</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
        }
        h1 {
            text-align: center;
            margin-top: 50px;
            color: #444;
        }
        .container {
            display: flex;
            justify-content: center;
            margin-top: 50px;
        }
        .form {
            width: 400px;
            padding: 20px;
            background-color: #fff;
            border-radius: 5px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
            margin-right: 20px;
        }
        .form label {
            display: block;
            margin-bottom: 10px;
            color: #555;
        }
        .form input[type="text"] {
            width: 100%;
            padding: 10px;
            border: 1px solid #ccc;
            border-radius: 3px;
            box-sizing: border-box;
            margin-bottom: 20px;
        }
        .form input[type="submit"] {
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            background-color: #008CBA;
            color: white;
            font-weight: bold;
            text-decoration: none;
            text-align: center;
            cursor: pointer;
        }
        .form input[type="submit"]:hover {
            background-color: #004555;
        }
		button {
			padding: 10px 20px;
            border: none;
            border-radius: 5px;
            background-color: #008CBA;
            color: white;
            font-weight: bold;
            text-decoration: none;
            text-align: center;
            cursor: pointer;
		}
    </style>
</head>
<body>
    <h1>Formulaire de requête CGI</h1>

    <div class="container">
		<div class="form">
		<?php
			echo "<h1> CGI for ". $_SERVER['REQUEST_METHOD']."  request</h1>";
			echo "<hr>";
			if ($_SERVER['REQUEST_METHOD'] === 'POST')
			{
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
			// }`
		?>
		</div>
		
    </div>
	<div class="container">
		<button onclick="window.location.href = '/';">Home</button>
	</div>
</body>
</html>