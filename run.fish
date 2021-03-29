set indir input
set outdir output
set binary ./fm

function execute
    time $binary $indir/input_"$argv".dat $outdir/output_"$argv".txt
end

function evaluate
    python evaluate.py $indir/input_"$argv".dat $outdir/output_"$argv".txt
end

if not test -d $indir; or not test -d $outdir
    printf "Missing either indir:$indir or outdir:$outdir"
    exit 255
end

switch $argv[1]

case "-*clean*"
    make clean
    rm -r $outdir
    mkdir -p $outdir
case "-*comp*"
    make FLAGS=$argv[2..-1]
case "-*run*"
    for i in (seq 0 5)
        execute $i
    end
case "-*eval*"
    for i in (seq 0 5)
        evaluate $i
    end
end
