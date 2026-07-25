#include <iostream>
#include <opencv2/opencv.hpp>

// argc：argument count，參數數量
// argv：argument vector，參數內容的字串陣列
/* e.g ./hello_opencv ../assets/test.jpg
    程式收到：argc == 2
            argv[0] == "./hello_opencv"
            argv[1] == "../assets/test.jpg"
*/ 
int main(int argc, char* argv[]){
    if(argc<2){
        // 如果使用者沒有提供圖片路徑，則輸出使用說明並返回錯誤碼
        // std::cerr 是標準錯誤輸出流，通常用於輸出錯誤訊息
        // << 是串流插入運算子，代表把右邊的內容輸出到左邊的串流 也可以連續輸出：std::cerr << "Error code: " << 404 << '\n';
        std::cerr << "Usage: ./hello_opencv <image_path>\n";
        return 1;
    }

    // 使用 OpenCV 的 imread 函數讀取圖片，argv[1] 是使用者提供的圖片路徑
    cv::Mat image = cv::imread(argv[1]); 
    /*. 例如彩色圖片可以想成：高度 × 寬度 × 3個色彩通道
        OpenCV 預設彩色通道順序是：B、G、R，也就是藍、綠、紅，不是常見的 RGB 順序。
    */
    //  cv::Mat 是 OpenCV 用來儲存圖片或矩陣資料的類別，表示建立一個名為 image 的圖片變數
    /*  cv::imread(...) 會找到圖片檔案，解碼 JPEG、PNG 等格式，回傳一個 cv::Mat 物件，如果讀取失敗，則會回傳一個空的 cv::Mat 物件
        也可以指定讀取模式：
            cv::Mat color = cv::imread(argv[1], cv::IMREAD_COLOR);
            cv::Mat gray  = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    */ 

    if (image.empty()) {
        // 如果圖片讀取失敗，則輸出錯誤訊息並返回錯誤碼
        std::cerr << "Failed to load image: " << argv[1] << '\n';
        return 1;
    }

    // 輸出圖片的基本資訊：寬度、高度、通道數
    std::cout << "Image width: " << image.cols << '\n';
    std::cout << "Image height: " << image.rows << '\n';
    std::cout << "Channels: " << image.channels() << '\n';

    cv::imwrite("output.jpg", image);

    cv::imshow("Hello OpenCV", image);
    cv::waitKey(0);

    return 0;
}
