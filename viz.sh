if [ $# -ne 1 ]; then
  echo 'usage: sh viz.sh filepath' >& 2
  exit 1
fi
filepath=$1
make
ls -lR $filepath | ./bin $1 | ./jgraph/jgraph -P | ps2pdf - > junk.pdf
# ls -lR $filepath | ./bin $1 | ./jgraph/jgraph -P > junk.eps
# ls -lR $filepath | ./bin $1 > junk.jgr
open junk.pdf
