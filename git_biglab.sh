git pull

make clean

make host

make encoder

git add .
git commit -m "update executable $(date +"%Y-%m-%d %H:%M:%S")"
git push -u origin main