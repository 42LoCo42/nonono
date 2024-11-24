#include <cupasy/Component.h>
#include <cupasy/RootComponent.h>
#include <cupasy/Label.h>
#include <cupasy/FileReader.h>
#include <cupasy/ASCIIMovie.h>

#include <cupasy/utils.h>
#include <clocale>
#include <unistd.h>
#include <cstdio>
using namespace std;

enum direction {
	NONE, UP, DOWN, LEFT, RIGHT
};

class MoveBox : public Component {
private:
	short curCol = 1;
	direction dir = direction::NONE;

	size_t index = 0;
	child_t* me = nullptr;

	void updComp() override;

public:
	[[nodiscard]] bool move(const direction& _dir) {
		// find index for movement
		if(!me) {
			parent->findChild(this, index);
			me = parent->getChild(index);
		}

		dir = _dir;

		// move & clear leftover space
		if(dir == direction::UP) {
			parent->moveChild(index, me->posX, me->posY-1);
			Component::draw(0, getHeight(), getWidth()-1, getHeight(), this);
		}
		else if(dir == direction::DOWN) {
			parent->moveChild(index, me->posX, me->posY+1);
			Component::draw(0, static_cast<ulong>(-1), getWidth()-1, static_cast<ulong>(-1), this);
		}
		else if(dir == direction::LEFT) {
			parent->moveChild(index, me->posX-1, me->posY);
			Component::draw(getWidth(), 0, getWidth(), getHeight()-1, this);
		}
		else if(dir == direction::RIGHT) {
			parent->moveChild(index, me->posX+1, me->posY);
			Component::draw(static_cast<ulong>(-1), 0, static_cast<ulong>(-1), getHeight()-1, this);
		}

		dir = direction::NONE;
		return Component::draw();
	}

	MoveBox(unsigned int _width, unsigned int _height): Component(_width, _height) {}

	void special() {
		init_pair(1, ++curCol, COLOR_BLACK);
	}
};

void MoveBox::updComp() {
	start_color();
	init_pair(1, curCol, COLOR_BLACK);

	cchar_t* tmp;
	for(ulong x=1; x<getWidth()-1; ++x) {
		for(ulong y=1; y<getHeight()-1; ++y) {
			cupasy::ctocc(static_cast<char>('0' + x + 2*y - 3), tmp, A_BLINK, 1);
			buffer[x][y] = *tmp;
		}
	}

	box();
}

int main() {
	int res = 0;

	RootComponent root;
	root.box();

	ASCIIMovie intro("assets/movies/intro");
	size_t introIndex = root.addChild(intro, 0, 0);
	res += intro.draw();
	root.removeChild(introIndex);
	usleep(2e6);
	clear();

	Component parent(root.getWidth()-2, root.getHeight()-2);
	root.addChild(parent, 1, 1);

	Label lblBG("bottom");
	parent.addChild(lblBG, 10, 5);

	FileReader testReader("assets/test.txt");
	testReader.update();
	Component testReaderBox(testReader.getWidth()+2, testReader.getHeight()+2);
	testReaderBox.addChild(testReader, 1, 1);
	testReaderBox.box();
	parent.addChild(testReaderBox, 16, 0);

	MoveBox moveBox(5, 5);
	parent.addChild(moveBox, 1, 1);

	Label lblFG("top");
	parent.addChild(lblFG, 10, 6);

	Component box1(5, 3);
	box1.box();
	Label lblBox("βøχ");
	box1.addChild(lblBox, 1, 1);
	parent.addChild(box1, 10, 8);

	root.update();
	root.draw();

	int input = '\0';
	while(true) {
		if(input == 'w') res += moveBox.move(direction::UP);
		else if(input == 's') res += moveBox.move(direction::DOWN);
		else if(input == 'a') res += moveBox.move(direction::LEFT);
		else if(input == 'd') res += moveBox.move(direction::RIGHT);
		else if(input == ' ') {moveBox.special(); res += moveBox.draw();}
		else if(input == 'q') break;

		input = getch();
	}

	endwin();

	printf("%d operations completed!\n", res);
	return !res; // if res is nonzero, return 0 as everything is OK
}
