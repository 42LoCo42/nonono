#include <stdio.h>
#include <stdlib.h>

struct _bintree {
	size_t size;
	int key;
	void* val;
	struct _bintree* l;
	struct _bintree* r;
};

typedef struct _bintree bintree;

bintree* make(int key, void* val) {
	bintree* res = malloc(sizeof(bintree));
	res->size = 1;
	res->key = key;
	res->val = val;
	res->l = NULL;
	res->r = NULL;
	return res;
}

void add(bintree* tree, bintree* node) {
	if(tree->key > node->key) {
		if(tree->l == NULL) {
			tree->l = node;
		} else {
			add(tree->l, node);
		}
	} else {
		if(tree->r == NULL) {
			tree->r = node;
		} else {
			add(tree->r, node);
		}
	}
	tree->size++;
}

void print(bintree* tree) {
	if(tree == NULL) {
		printf("()");
	} else {
		printf("(%lu, %d: %p ", tree->size, tree->key, tree->val);
		print(tree->l);
		putchar(' ');
		print(tree->r);
		putchar(')');
	}
}

void cleanup(bintree* tree) {
	if(tree == NULL) return;
	cleanup(tree->l);
	cleanup(tree->r);
	free(tree);
}

int main() {
	bintree* root = make(8, NULL);
	add(root, make(3, NULL));
	add(root, make(6, NULL));
	add(root, make(7, NULL));
	add(root, make(4, NULL));
	add(root, make(1, NULL));
	add(root, make(10, NULL));
	add(root, make(14, NULL));
	add(root, make(13, NULL));
	print(root);
	cleanup(root);
}
