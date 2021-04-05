set indir input
set outdir output
set binary ./fm

function execute
    time $binary $indir/input_"$argv".dat $outdir/output_"$argv".txt
end

function evaluate
    python evaluate.py $indir/input_"$argv".dat $outdir/output_"$argv".txt
end

function checkDirs
    function exitIfMiss
        if not test -d $argv[1]
            echo Missing folder: $argv[1]
            exit 255
        end
    end
    
    exitIfMiss $indir
    exitIfMiss $outdir
end

argparse 'C/clean' 'c/comp' 'e/eval' 'r/run' -- $argv

if set -q _flag_clean
    make clean
    rm -r $outdir
    mkdir -p $outdir
end

if set -q _flag_comp
    make FLAGS=$argv[2..-1]
end

if set -q _flag_run
    checkDirs

    for i in (seq 0 5)
        execute $i
    end
end

if set -q _flag_eval
    checkDirs

    for i in (seq 0 5)
        evaluate $i
    end
end
