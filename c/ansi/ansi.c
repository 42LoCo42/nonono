#include "ansi.h"
#include <stdlib.h>

void _ansi(
	enum ansi_seq s,
	const char* a1,
	const char* a2,
	const char* a3
) {
	switch(s) {
	case RESET: printf("\33[[m"); break;
	case FG: printf("\33[38;5;%sm", a1); break;
	case BG: printf("\33[48;5;%sm", a1); break;
	case FGRGB: printf("\33[38;2;%s;%s;%sm", a1, a2, a3); break;
	case BGRGB: printf("\33[48;2;%s;%s;%sm", a1, a2, a3); break;
	case BOLD: printf("\33[1m"); break;
	case FAINT: printf("\33[2m"); break;
	case ITALIC: printf("\33[3m"); break;
	case UNDERLINE: printf("\33[4m"); break;
	case BLINKING: printf("\33[5m"); break;
	case HIGHLIGHTED: printf("\33[7m"); break;
	case HIDDEN: printf("\33[8m"); break;
	case STRIKETHROUGH: printf("\33[9m"); break;
	case MOVE: printf("\33[%s;%sH", a1, a2); break;
	case HOME: printf("\33[H"); break;
	case UP: printf("\33[%sA", a1); break;
	case DOWN: printf("\33[%sB", a1); break;
	case RIGHT: printf("\33[%sC", a1); break;
	case LEFT: printf("\33[%sD", a1); break;
	case SAVE: printf("\33[s"); break;
	case RESTORE: printf("\33[u"); break;
	case ERASEEND: printf("\33[K"); break;
	case ERASESTART: printf("\33[1K"); break;
	case ERASE: printf("\33[2K"); break;
	case ERASEBOTTOM: printf("\33[J"); break;
	case ERASETOP: printf("\33[1J"); break;
	case CLEAR: printf("\33[2J"); break;
	case LI_ON: printf("\33(0"); break;
	case LI_OFF: printf("\33(B"); break;
	case LI_HORI: printf("q"); break;
	case LI_VERT: printf("x"); break;
	case LI_UL: printf("j"); break;
	case LI_DL: printf("k"); break;
	case LI_DR: printf("l"); break;
	case LI_UR: printf("m"); break;
	case LI_UDR: printf("t"); break;
	case LI_UDL: printf("u"); break;
	case LI_ULR: printf("v"); break;
	case LI_DLR: printf("w"); break;
	case LI_CROSS: printf("n"); break;
	case LI_SHADE: printf("a"); break;
	}
}

#define ansi(s) _ansi(s, "", "", "")
#define ansi1(s, a1) _ansi(s, a1, "", "")
#define ansi2(s, a1, a2) _ansi(s, a1, a2, "")
#define ansi3(s, a1, a2, a3) _ansi(s, a1, a2, a3)

int main() {
	printf("it works\n");
	ansi(UP);
	printf("¡\n");
}
