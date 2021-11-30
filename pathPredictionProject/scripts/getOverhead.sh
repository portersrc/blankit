rundir=`pwd`
parentdir="$(dirname $rundir )"
folder="$(basename $rundir)"
preddir="$parentdir"/"$folder"_test
basedir="$parentdir"/"$folder"_base
echo "1. $rundir \n 2. $folder \n 3. $parentdir \n 4. $preddir"
python ~/project/pathPredictionProject/src/plot_unveil_tree_structure.py data1.pkl > decisionTree
sed  's/LLVMpathPredInstrument.so/LLVMembedDtree.so/' Makefile > Makefile.pred
sed 's/-Xclang -load -Xclang LLVMpathPredInstrument.so//' Makefile > Makefile.baseline
cd ../
rm -rf $preddir $basedir
cp -rf $rundir $preddir 
cp -rf $rundir $basedir 
cd $preddir 
export PATH=./:$PATH 
make clean; make  -f Makefile.pred &> compile_log 
if [ -e runme_large.sh ]
then
  exp_rundir=$(pwd)
else 
  exp_rundir=../
fi
cd $exp_rundir
ts=$(date +%s%N) ; runme_large.sh ; tt1=$((($(date +%s%N) - $ts)/1000000)) ; echo "Instrumented Time taken: $tt milliseconds"
ts=$(date +%s%N) ; runme_small.sh ; tt2=$((($(date +%s%N) - $ts)/1000000)) ; echo "Instrumented Time taken: $tt milliseconds"
execName=$(cat runme_large.sh | tail -n+2 | cut -f1 -d' ')
instrCount1=$(objdump -dw $execName | wc -l)
echo "instrumented instr count $instrCount"

cd $basedir
make clean; make -s -f Makefile.baseline 2>&1 &> compile_log
if [ -e runme_large.sh ]
then
  exp_rundir=$(pwd)
else 
  exp_rundir=../
fi
cd $exp_rundir
ts=$(date +%s%N) ; runme_large.sh ; tt3=$((($(date +%s%N) - $ts)/1000000)) ; echo "Baseline Time taken: $tt milliseconds"
ts=$(date +%s%N) ; runme_small.sh ; tt4=$((($(date +%s%N) - $ts)/1000000)) ; echo "Baseline Time taken: $tt milliseconds"
instrCount2=$(objdump -dw $execName | wc -l)
echo "Baseline instr count $instrCount"
echo "$rundir, $instrCount2, $instrCount1, $tt3, $tt1, $tt4, $tt2 "
