#!/bin/bash
#
# MIT License
#
# Copyright (c) 2023 Andrei Rabusov

CWD=$(pwd)
if [[ ! -f Makefile ]]; then
    echo "Run in the repo root directory"
    exit 1
fi
if [[ ! -f ${CWD}/sudoku ]]; then
    make
fi
cd examples
PASS=0
TOT=0
for EX in ex*.txt; do
    TMP=$(mktemp)
    ${CWD}/sudoku < $EX > $TMP
    SOL=sol${EX#ex}
    DIFF=$(diff $SOL $TMP)
    rm ${TMP}
    if [[ "$DIFF" == "" ]]; then
        let PASS=$PASS+1
    fi
    let TOT=$TOT+1
done
echo Test result: [$PASS/$TOT]
if [[ $PASS -eq $TOT ]]; then
    echo Ok.
else
    echo Fail.
fi
cd $CWD
