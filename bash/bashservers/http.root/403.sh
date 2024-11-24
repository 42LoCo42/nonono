#!/usr/bin/env bash
path="$(awk '{print $2}' <<< "$2")"
cat << EOF
<!DOCTYPE html>
<html>
	<head>
		<title>403 - Forbidden</title>
		<meta charset="utf-8"/>
	</head>
	<body>
		<h1>403 - Forbidden</h1>
		You may not access $(./urldecode "$path").
	</body>
</html>
EOF
echo "403" >&2
