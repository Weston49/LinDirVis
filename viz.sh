if [ $# -lt 1 ]; then
  echo 'usage: sh viz.sh filepath [mode]' >& 2
  exit 1
fi
if [ $# -gt 2 ]; then
  echo 'usage: sh viz.sh filepath [mode]' >& 2
  exit 1
fi

if [ $# -eq 1 ]; then
  mode=15
fi
if [ $# -eq 2 ]; then
  mode=$2
fi

filepath=$1
ls -lR $filepath | ./bin $filepath $mode > junk.jgr
cat junk.jgr | jgraph -P | ps2pdf - > junk.pdf
