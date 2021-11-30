echo "arg:$1"
    find $1  -name "Makefile" -print0| while IFS= read -r -d '' file
    do 
        dir=$(dirname "${file}")
        cd $dir
        if [ ! -f DONOTRUN ]; then 
            echo "dir  is $dir "
        fi
    done

