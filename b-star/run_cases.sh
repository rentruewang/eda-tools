BINARY=bin/fp
ALPHA=0.5
INPUT=./input
OUTPUT=./output
LOG=./log

time $BINARY $ALPHA $INPUT/ami33.block $INPUT/ami33.nets $OUTPUT/ami33.rpt 
time $BINARY $ALPHA $INPUT/ami49.block $INPUT/ami49.nets $OUTPUT/ami49.rpt 
time $BINARY $ALPHA $INPUT/apte.block  $INPUT/apte.nets  $OUTPUT/apte.rpt
time $BINARY $ALPHA $INPUT/hp.block    $INPUT/hp.nets    $OUTPUT/hp.rpt
time $BINARY $ALPHA $INPUT/xerox.block $INPUT/xerox.nets $OUTPUT/xerox.rpt
