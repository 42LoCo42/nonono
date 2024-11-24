#include <crypt.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct resource {
	void* data;
	struct resource* next;
} resource;

resource* resources;
resource* current_resource;

void newResource(void* data) {
	current_resource->data = data;
	current_resource->next = malloc(sizeof(resource));
	current_resource = current_resource->next;
	current_resource->next = NULL;
}

void cleanup() {
	current_resource = resources;
	while(current_resource->next != NULL) {
		free(current_resource->data);
		resource* next = current_resource->next;
		free(current_resource);
		current_resource = next;
	}
	free(current_resource);
}

void syntax_error() {
	error(1, 0, \
"Needs an option, either:\n\
	-m	Hashes the password with the specified method\n\
	-s	Hashes the password with the specified salt");
}

typedef enum {
	GEN_SALT,
	USE_SALT,
} opmode;

int main(int argc, char** argv) {
	if(argc < 2) {
		syntax_error();
	}

	// init resource storage and cleanup
	if(atexit(cleanup) != 0) {
		error(1, 0, "Could not init resource cleanup!");
	}
	resources = malloc(sizeof(resource));
	current_resource = resources;

	// is stdin a tty?
	int is_tty = isatty(STDIN_FILENO);

	// select operation mode
	opmode mode;
	char* first_input_name;
	if(strcmp(argv[1] , "-m") == 0) {
		mode = GEN_SALT;
		first_input_name = "Method";
	} else if(strcmp(argv[1], "-s") == 0) {
		mode = USE_SALT;
		first_input_name = "Salt";
	} else {
		syntax_error();
	}

	// get first input (method or salt)
	size_t prefix_length = 0;
	char* prefix = NULL;
	if(is_tty) {
		printf("%s: ", first_input_name);
	}
	ssize_t ret = getline(&prefix, &prefix_length, stdin);
	newResource(prefix);
	if(ret < 0) {
		error(1, 0, "Could not read %s!", first_input_name);
	}
	prefix[strlen(prefix) - 1] = 0;

	// get password
	size_t password_length = 0;
	char* password = NULL;
	if(is_tty) {
		printf("Password: ");
	}
	ret = getline(&password, &password_length, stdin);
	newResource(password);
	if(ret < 0) {
		error(1, 0, "Could not read password!");
	}
	password[strlen(password) - 1] = 0;

	// select salt
	char* salt = NULL;
	if(mode == GEN_SALT) {
		salt = crypt_gensalt(prefix, 0, NULL, 0);
		if(salt == NULL) {
			error(1, errno, "Invalid method %s", prefix);
		}
	} else {
		salt = prefix;
	}

	// do the crypto thing
	char* hash = crypt(password, salt);
	if(hash == NULL) {
		error(1, errno, "Invalid salt");
	}
	printf("%s\n%s\n", salt, hash);
}
