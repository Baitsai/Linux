### Linux

mkdir -p build

cmake .
cmake --build .
./hello_opencv ./Day1Day2_memory/cat.png


git init
git add .
git commit -m "feat: Day2"
git branch -M main
ssh -T git@github.com
git remote -v
git remote add origin git@github.com:Baitsai/Linux.git
git push -u origin main