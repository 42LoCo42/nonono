#include <fstream>
#include <vector>
#include <string>
#include <ncurses.h>

#include "CursedCat.hpp"
using namespace std;
using namespace CursedCat;

void split(const string& s, const string& delim, vector<string>& strings) {
	int last = 0, next = 0;

	while((next = s.find(delim, last)) != string::npos) {
		strings.push_back(s.substr(last, next-last));
		last = next + delim.length();
	}
	strings.push_back(s.substr(last));
}

void CursedCat::read(const string& text, LineDataSet* lds) {
	operation currentOP;

	// loop through text and find line breaks
	string delim = "\n";
	vector<string> strings;
	split(text, delim, strings);

	for(int i=0; i<strings.size(); ++i) {
		if(lds) {
			// add an element to our lds and take note of its index
			lds->lineData.push_back(LineData(0, 0, 0));
			currentOP.usedLDSindices.push_back(lds->lineData.size()-1);
		}
		currentOP.lines.push_back(strings[i]);
	}

	totalLineCount += strings.size();

	currentOP.lds = lds;
	pending.push_back(currentOP);
}

void CursedCat::readFile(const char* filename, LineDataSet* lds) {
	operation currentOP;

	ifstream file;
	file.open(filename);
	if(!file.good()) return;

	string line;
	while(getline(file, line)) {
		currentOP.lines.push_back(line);
		if(lds) {
			// add an element to our lds and take note of its index
			lds->lineData.push_back(LineData(0, 0, 0));
			currentOP.usedLDSindices.push_back(lds->lineData.size()-1);
		}
		++totalLineCount;
	}

	file.close();
	currentOP.lds = lds;
	pending.push_back(currentOP);
}

void CursedCat::write() {
	int height, width;
	getmaxyx(stdscr, height, width);

	int y = (height - totalLineCount) / 2, x, length;

	for(int i=0; i<pending.size(); ++i) {
		for(int j=0; j<pending[i].lines.size(); ++j) {
			length = pending[i].lines[j].length();
			x = (width - length) / 2;

			if(pending[i].lds) { // this operation has a LineDataSet attached
				int LDSindex = pending[i].usedLDSindices[j];
				pending[i].lds->lineData[LDSindex].x = x;
				pending[i].lds->lineData[LDSindex].y = y;
				pending[i].lds->lineData[LDSindex].length = length;
			}

			mvprintw(y, x, pending[i].lines[j].c_str());
			++y;
		}
	}

	pending = vector<operation>();
	totalLineCount = 0;
}
