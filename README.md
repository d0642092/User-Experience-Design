# Visual Game

## 呈現影片

## 環境設定
1. 下載 opencv  
2. 環境變數 PATH: '{路徑}\opencv\build\x64\vc15\bin'
3. 使用 Visual Studio 2017，開新專案，平台x64
4. 專案屬性設定:   
	* c\c++, 其他include目錄: '{路徑}\opencv\build\include'  
	* 連結器
	 一般, 其他程式庫目錄: '{路徑}\opencv\build\x64\vc15\lib'  
	 輸入, 其他相依性: 'opencv_world347d.lib' (檔案在其他程式庫目錄中)

## 架構圖

## 流程圖

## 主程式(main)  
1. 遊戲介面
2. 當遊戲控制回傳完成遊戲時，移動到結束介面 

| 名稱 | 型態 | 意義 |
| ---- | ---- | ---------------  
| video | Mat | 鏡頭影像                   
| validVisual | Mat | 有效區域  
| gameVisual | Mat | 遊戲影像  
| gameControl | class | 遊戲控制
| finish_menu | func | 結束介面


## 遊戲控制(GameController)  
1. 遊戲初始化
2. 影像處理、偵測顏色位置並結合遊戲影像
3. 遊戲狀態更新: 顏色改變
4. 遊戲狀態判斷: 偵測結束或重新開始

| 名稱 | 型態 | 意義 |
| ---- | ---- | ---------------  
| parameter | class | 遊戲參數
| combine_video | func | 影像結合                 
| find_close | func | 偵測移動到的元件
| color_change | func | 改變顏色(相鄰點及本身)
| init_game | func | 初始化遊戲
| reset_detect_center | func | 重製偵測點
| isFinish | func | 是否完成遊戲
| isRestart | func | 是否重玩或結束
| get_now_status | func | 取得現在遊戲狀態


## 遊戲參數(GameParameter)  
1. 遊戲參數初始化
2. 生成遊戲圖片以及狀態保存


| 名稱 | 型態 | 意義 |
| ---- | ---- | ---------------  
| UnitCircle | Struct | 元件資訊                  
| circleList | vector | 元件陣列                
| validArea | vector | 有效區域
| xList, yList | vector | 用於判斷移動到的元件
| xFinishList, yFinishList | vector | 結束時判斷選擇
| lowHsv, highHsv | Scalar | 偵測顏色範圍(藍色)
| radius | Point | 元件半徑(大圓,小圓)
| detectCenter | Point | 顏色位置
| gameStatus | Mat | 遊戲狀態
| element | Mat | 做Opening 跟 dialate
| judge | int | 現有綠色數量
| bufferChange | int | 緩衝顏色變換