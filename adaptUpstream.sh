#!/bin/bash -e

if [ -z "$1" -o -z "$2" ]; then
  echo "Usage: ./adaptUpstream.sh path/to/Upstream/WorkingCopy path/to/Community/WorkingCopy"
  exit 1
fi

UPSTREAM="`pwd`/$1"
COMMUNITY="`pwd`/$2"

echo "Updating community working copy..."
cd "${COMMUNITY}"
git pull
git checkout upstream

echo "Updating upstream working copy..."
cd "${UPSTREAM}"
git pull
git checkout development
git reset --hard
git clean -f -d
COMMITID=`git log | head -n1`
LOG=`git cat-file -p fcfd9b8c11d85a5099f675351aae3502d67d1738`
rm -rf RF2000
mv RF1000/Repetier .
rmdir RF1000

echo "Correcting file format..."
find -type f -not -path '*/.git/*' -exec bash -c 'F="{}" ; tab2space "$F" > "$F.new" ; mv "$F.new" "$F" ; dos2unix "$F"' \;

echo "Copying changes to community repository..."
rsync -av --exclude=.git --delete ${UPSTREAM}/ ${COMMUNITY}/

echo "Commiting changes to community repository..."
cd "${COMMUNITY}"
git add .
git commit -a -m "Imported changes from upstream repository, now on ${COMMITID}\n${LOG}"
