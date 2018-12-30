#!/bin/bash
if [ "$#" - ne 1 ]; then
	echo "Usage : git_new_project project_name"
fi

echo $1

git checkout --orphan $1
rm .git/index
mkdir $1
