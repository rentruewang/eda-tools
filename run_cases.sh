INDIR=input
OUTDIR=output

BINARY=./fm

rm -r $OUTDIR
mkdir -p $OUTDIR

time $BINARY $INDIR/input_0.dat $OUTDIR/output_0.txt
time $BINARY $INDIR/input_1.dat $OUTDIR/output_1.txt
time $BINARY $INDIR/input_2.dat $OUTDIR/output_2.txt
time $BINARY $INDIR/input_3.dat $OUTDIR/output_3.txt
time $BINARY $INDIR/input_4.dat $OUTDIR/output_4.txt
time $BINARY $INDIR/input_5.dat $OUTDIR/output_5.txt
