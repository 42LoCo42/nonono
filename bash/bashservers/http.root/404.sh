#!/usr/bin/env bash
path="$(awk '{print $2}' <<< "$2")"
cat << EOF
<!DOCTYPE html>
<html>
	<head>
		<title>404 - Not Found</title>
		<meta charset="utf-8"/>
	</head>
	<body>
		<h1>404 - Not Found</h1>
		$(./urldecode "$path") was not found.
	</body>
</html>
EOF
echo "404" >&2
