#! /bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd $DIR
git submodule init
git submodule update

svn checkout http://polypartition.googlecode.com/svn/trunk/ $DIR/external/polypartition -r 10
