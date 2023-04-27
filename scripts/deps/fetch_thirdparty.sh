#!/usr/bin/env bash
# this script downloads the archive code from repo instead
# of cloning it.

ROOT=$(dirname -- "$0")/../..
TP_DIR=$ROOT/third-party

UPSTREAM=$1
PROJECT=$2
COMMIT=$3
ZIP=$COMMIT.zip
PROJDIR=$TP_DIR/$PROJECT

wget -q --show-progress $UPSTREAM/archive/$ZIP -O $TP_DIR/$ZIP
unzip -q $TP_DIR/$ZIP -d $PROJDIR
mv $PROJDIR/* $PROJDIR/rename
mv $PROJDIR/rename/*  $PROJDIR/
rm -rf $PROJDIR/rename

