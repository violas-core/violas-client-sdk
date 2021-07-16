#stdlib = 'ls ~/Projects/work/Violas/language/move-stdlib/modules'
#echo $stdlib
# stdlib = 
(ls ~/Projects/work/Violas/language/diem-framework/modules/*.move && ls ~/Projects/work/Violas/language/move-stdlib/modules/*.move && ls ~/Projects/work/Violas/language/move-stdlib/nursery/*.move) | xargs ~/Projects/work/Violas/target/release/move-build $1 -d $2 $3 $4 $5
# ls ~/Projects/work/Violas/language/move-stdlib/modules/*.move | xargs ~/Projects/work/Violas/target/release/move-build $1 -s $2 -d $3 $4 $5