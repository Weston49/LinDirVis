ls -lR ./testDir | ./bin ./testDir | jgraph -P | ps2pdf - > ex1.pdf
ls -lR . | ./bin . 1 | jgraph -P | ps2pdf - > ex2.pdf
ls -lR . | ./bin . 2 | jgraph -P | ps2pdf - > ex3.pdf
ls -lR . | ./bin . 4 | jgraph -P | ps2pdf - > ex4.pdf
ls -lR . | ./bin . 8 | jgraph -P | ps2pdf - > ex5.pdf
ls -lR ../ | ./bin ../ 1 | jgraph -P | ps2pdf - > ex6.pdf
