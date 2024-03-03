# LinDirViz
## Prerequisites
The shell scripts in this project assume that you have jgraph installed and added to your path. If you do not then you will need to install jgraph from [Jgraph Home Page](https://web.eecs.utk.edu/~jplank/plank/jgraph/jgraph.html) and you can temporarily put jgraph into your PATH with the command:\
`export PATH="/your/file/path/to/jgraph:$PATH"`

## Usage
To get started just run make which compiles the main file and runs the example shell script. This should make some ex\*.pdf files that show off the different visualizations you can generate. ex1.pdf is just showing that given no mode, the program will put all of the different graphs into one pdf file.

Aside from just running the example scripts you can run the command `sh viz.sh <filepath> [mode]` which will create a junk.jgr and a junk.pdf with your output.
Mode is a number 0-15 where the bits determine which pages are generated.
### Mode Examples:
&nbsp; 0  [0000] will generate nothing\
&nbsp; 15 [1111] will generate all 4 pages\
&nbsp; 5  [0101] will generate pages 1 and 2\
&nbsp; etc.

# Here are what the different example PDFs should look like

## ex2.pdf
![example pdf #2](/imgs/simpleTreeEx.png)

## ex3.pdf
![example pdf #3](/imgs/largest.png)

## ex4.pdf
![example pdf #4](/imgs/fdCount.png)

## ex5.pdf
![example pdf #5](/imgs/fdSize.png)

## ex6.pdf
This one will look vastly different depending on where you run it from, it calls LinDirViz on the directory above the current one and just generates the tree. Here is what it looks like on my Mac when I run it. Obviously, in many cases, lines will run into each other however I find the graph is still typically plenty clear.
![example pdf #6](/imgs/bigTreeEx.png)
