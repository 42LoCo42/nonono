# hdb - Usage
1. Create a database
```bash
$ mkdir users
```
2. Enter some data
```bash
$ hdb users set nomad d # make nomad a directory
$ hdb users set nomad/name f <<< "Elaine Roberts" # let nomad/name be a file containing the string "Elaine Roberts"
$ base64 /dev/urandom | head -c 32 | hdb users set nomad/salt f # create and set salt
$ printf '%s%s' "$(hdb users get nomad/salt)" "correcthorsebatterystaple" \
  | sha512sum | awk '{print $1}' | hdb users set nomad/hash f # combine salt and password, create and set hash
```
3. Additional things
```bash
$ hdb users update nomad/name sed "s|$|'); DROP TABLE users;--|" # pipe the contents of a file through a command, save stdout
$ hdb users del nomad # delete data, works on directories

$ hdb users set example d
$ hdb users set example/foo f <<< "bar"
$ hdb users get example
# The only case where hdb returns 1 but does not print error messages is when getting a directory
# Instead, this is printed to stdout:

# example is a directory with entries:                                                                                                    │
# example/foo

# Works on toplevel too:
$ hdb users get .
# . is a directory with entries:
# ./example
```
