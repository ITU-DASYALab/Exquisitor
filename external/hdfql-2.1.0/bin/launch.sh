#!/bin/bash

# Copyright (C) 2016-2019
# This file is part of the Hierarchical Data Format query language (HDFql)
# For more information about HDFql, please visit the website http://www.hdfql.com

# $Rev: 186 $



# set library path environment variable with the directory where the HDFql shared library is located
if [[ "$OSTYPE" == "darwin"* ]]; then
	export DYLD_LIBRARY_PATH=../lib:$DYLD_LIBRARY_PATH
else
	export LD_LIBRARY_PATH=../lib:$LD_LIBRARY_PATH
fi

# launch HDFqlCLI (command-line interface) and pass all user-defined parameters to it
./HDFqlCLI "$@"

