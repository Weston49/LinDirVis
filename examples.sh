ls -lR . | ./bin . 1 | jgraph -P | ps2pdf - > curDirGraph.pdf
ls -lR ../ | ./bin ../ 1 | jgraph -P | ps2pdf - > upOneDirGraph.pdf
ls -lR ./testDir | ./bin ./testDir | jgraph -P | ps2pdf - > testDirAll.pdf
ls -lR . | ./bin . 2 | jgraph -P | ps2pdf - > curDirMaxSize.pdf
ls -lR . | ./bin . 4 | jgraph -P | ps2pdf - > curDirFileDist.pdf
