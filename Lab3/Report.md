# HW#3

## 作業環境 :

* Visual studio 2019
* Window 10
* glew - 2.1.0
* assimp - 5.0.1
* glfw - 3.3.6
* glm

## 方法說明 : 

1. 使用 Assimp 獲取 model 的資訊
2. 根據 bones 之間轉移矩陣做線性內插，獲取 local 轉移矩陣
3. 將 local 的轉移矩陣 乘上 parant 的轉移矩陣，獲取 global 轉移矩陣
4. 若有 child，則將 child 重新執行 2至3步驟
5. 將結果秀出

## 程式如何執行 : 

### 兩種方法 :
1. 直接執行 lab3.exe 
2. 打開專案執行

## 注意事項 : 

* GLSL : shader 檔案
* include : 第三方開源庫
* model : 模型檔案
* resource : texture 圖片
* src : 程式碼