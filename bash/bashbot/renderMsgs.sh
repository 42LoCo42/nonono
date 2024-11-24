#!/usr/bin/env bash
< msgs jq -r '
	reverse[]
	| "[31;1m\(.author.username)[m:\u0001\(.content)"
' \
| sed -E 's|^([^])|\x01\1|' \
| column -ts $'\1'
