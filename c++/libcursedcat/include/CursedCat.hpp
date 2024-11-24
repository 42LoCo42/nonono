#pragma once
#include <vector>
#include <string>

namespace CursedCat {
	struct LineData { // holds information about a single line
		int x, y, length;
		LineData(int _x, int _y, int _length): x(_x), y(_y), length(_length) {}
	};
	struct LineDataSet { // created through reading - a block of LineData with an offset (the absolute y for y=0)
		std::vector<LineData> lineData;
	};

	struct operation { // a block of lines that will be written and the LineDataSet that shall be constructed
		std::vector<std::string> lines;
		LineDataSet* lds = nullptr;
		std::vector<int> usedLDSindices;
	};

	extern int totalLineCount; /**  number of lines read in all calls */
	extern std::vector<operation> pending; /** list of operations to be performed by write */

	void read(const std::string& text, LineDataSet* lds = nullptr); /** read some text and update lds on write */
	void readFile(const char* fileName, LineDataSet* lds =  nullptr); /** read a file and update lds on write */
	void write(); /** perform all pending options and update all hooked LineDataSets */

}
