#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct _Tree {
	char* text;
	struct _Tree* firstChild;
	struct _Tree* lastChild;
	struct _Tree* nextSibling;
};

typedef struct _Tree Tree;

Tree* newChild(Tree* root, char* text) {
	Tree* ret = calloc(1, sizeof(Tree));
	ret->text = text;

	if(root->lastChild == NULL) {
		root->firstChild = ret;
		root->lastChild = ret;
	} else {
		root->lastChild->nextSibling = ret;
		root->lastChild = ret;
	}

	return ret;
}

void freeTree(Tree* t) {
	Tree* tmp = NULL;
	for(
		Tree* child = t->firstChild;
		child != NULL;
		child = tmp
	) {
		tmp = child->nextSibling;
		freeTree(child);
		free(child);
	}
}

void printTreeHelper(Tree* t, int depth) {
	puts(t->text);
	for(
		Tree* child = t->firstChild;
		child != NULL;
		child = child->nextSibling
	) {
		for(int i = 0; i < depth; i++) {
			printf("│   ");
		}

		if(child == t->lastChild) {
			printf("└── ");
		} else {
			printf("├── ");
		}
		printTreeHelper(child, depth + 1);
	}
}

void printTree(Tree* t) {
	printTreeHelper(t, 0);
}

int main() {
	Tree root = {
		.text = "root",
		.firstChild = NULL,
		.lastChild = NULL,
		.nextSibling = NULL,
	};

	Tree* foo = newChild(&root, "foo");
	newChild(foo, "1");
	newChild(newChild(foo, "2"), "3");

	Tree* bar = newChild(&root, "bar");
	newChild(bar, "a");
	newChild(bar, "b");
	newChild(bar, "c");

	Tree* baz = newChild(&root, "baz");
	newChild(baz, "quux");

	printTree(&root);
	freeTree(&root);
}
