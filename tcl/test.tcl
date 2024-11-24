#!/usr/bin/env tclsh
set foo 0

proc handle {client addr port} {
	upvar foo foo
	puts "connection from $addr:$port, foo is $foo"
	set cmd [gets $client]
	puts $cmd
}

socket -server handle 37812
vwait forever
