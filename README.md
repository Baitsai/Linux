### Linux
Week 1	單執行緒影片與基礎影像處理	可編譯、可執行	完成基本README
Week 2	多執行緒Producer–Consumer	安全停止、丟幀策略	加入測試與架構圖
Week 3	Linux/V4L2 Camera Backend	mmap/ioctl概念	加入除錯紀錄
Week 4	Benchmark與完整作品集	效能比較、Demo GIF	可放履歷與面試展示


mkdir -p build
cmake .
cmake --build .
./hello_opencv 

完整刪掉舊 build：
    rm -rf build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build

git init
git add .
git commit -m "feat: Day2"
git branch -M main
ssh -T git@github.com
git remote -v
git remote add origin git@github.com:Baitsai/Linux.git
git push -u origin main