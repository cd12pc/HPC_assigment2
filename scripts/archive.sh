#!/bin/bash

pushd . > /dev/null
SCRIPT_PATH="${BASH_SOURCE[0]}";
if ([ -h "${SCRIPT_PATH}"  ]) then
      while([ -h "${SCRIPT_PATH}"  ]) do cd `dirname "$SCRIPT_PATH"`; SCRIPT_PATH=`readlink "${SCRIPT_PATH}"`; done
fi
cd `dirname ${SCRIPT_PATH}` > /dev/null
SCRIPT_PATH=`pwd`;
popd  > /dev/null


BASE_DIR_ABS="$(dirname ${SCRIPT_PATH})"
BASE_DIR="$(realpath --relative-to="${PWD}"  $BASE_DIR_ABS)"



FILES=("makefile" "README.md" ".gitignore" "run.sh")
FOLDERS=("src" "saved" "include" "scripts")

echo ${FOLDERS}

OUT_FILE=$(pwd)/$1
pushd $BASE_DIR > /dev/null
if [ $# -eq 0 ]
then
    tar czf results/files.tar.gz ${FILES[*]} ${FOLDERS[*]}
else
    tar czf $OUT_FILE ${FILES[*]} ${FOLDERS[*]}
fi
popd > /dev/null
