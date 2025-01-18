# Drawing-graphs-C
A program for drawing graphs of mathematical functions, developed in the C language.

Libraries used: [TinyExpr](https://github.com/codeplea/tinyexpr) and WinBGIm (graphics library).

I left a couple of files from the TinyExpr library in the repository.

## Program interface:
![first](https://github.com/Kostya-Vologin/Drawing-graphs-C/blob/main/pictures/first.png)

The program will warn you in red text if you have entered something incorrectly:

![second](https://github.com/Kostya-Vologin/Drawing-graphs-C/blob/main/pictures/second.png)

## Program functions:
* entering formulas (the maximum number is 10, after entering the graph automatically appears in a rectangular coordinate system);

* selection of ranges for X and Y, initially the program has the values: for X &mdash; -30, 30 (min and max, respectively) and for Y &mdash; -30, 30 (min and max, respectively), after entering the values, the chart ranges change;

* drawing graphs using the entered formulas (each graph has a separate color);

* editing and deleting formulas;

## License
The [TinyExpr](https://github.com/codeplea/tinyexpr) library files are distributed under the [zlib License](https://choosealicense.com/licenses/zlib/). Please read the library before using it.
