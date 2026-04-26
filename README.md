# Pico USB Bulk Echo (Minimal & High Stability)

這是一個針對 Raspberry Pi Pico 系列優化的 **USB Bulk 傳輸模板**。它經過了高頻壓力測試（連續 10 分鐘傳輸 140MB+ 資料），具備工業級的穩定性，並支援一鍵編譯環境配置。

## 🌟 特色
- **極致穩定**：解決了常見的 Data Toggle 不同步、大封包（64B）卡死以及 Linux 端讀取超時問題。
- **支援全系列**：一套代碼支援 Pico, Pico W, Pico 2, 以及 Pico 2 W。
- **零配置編譯**：內建 `build.sh` 腳本，若系統缺少編譯器會自動下載 ARM GNU Toolchain，並自動處理 SDK 路徑。
- **魯棒測試**：隨附 `test_usb.py` 腳本，具備自動重試與 ZLP (Zero Length Packet) 處理邏輯。

## 🛠️ 環境準備
在開始之前，請確保你的電腦已安裝以下工具：
- `git`
- `cmake`
- `python3` (以及 `pip install pyusb`)
- `libusb` (Linux 用戶通常已內建)

## 🚀 快速開始

### 1. 複製專案 (含子模組)
```bash
git clone --recursive https://github.com/MagnusJhang/pico-usb-bulk-echo.git
cd pico-usb-bulk-echo
```

### 2. 一鍵編譯
執行 `build.sh` 腳本。第一次執行時，若系統沒有 `arm-none-eabi-gcc`，腳本會詢問是否自動下載（建議選擇 Y）。

```bash
# 預設編譯為 pico
./build.sh

# 或是指定開發板型號
./build.sh pico_w
./build.sh pico2
./build.sh pico2_w
```
編譯完成後，`.uf2` 檔案會產生在 `build_<板號>/src/bulk_device.uf2`。

### 3. 燒錄韌體
1. 按住 Pico 上的 **BOOTSEL** 按鈕並插上 USB 線。
2. 將產生的 `.uf2` 檔案複製到電腦識別出的 `RPI-RP2` 磁碟中。

## 🧪 執行測試
燒錄完成後，使用 Python 腳本驗證傳輸穩定性：
```bash
python3 test_usb.py
```
該腳本會連續發送 100 個 63 位元組的封包，並即時回報進度與校驗結果。

## 📌 技術細節
- **VID/PID**: `0xCafe / 0x4888`
- **Interface**: Interface 0 (Vendor Class)
- **Endpoints**: `0x01` (OUT), `0x81` (IN)
- **核心邏輯**: 採用同步輪詢 (Synchronous Polling) 與強對齊讀寫，確保資料在硬體 FIFO 中不積壓。

## 📄 授權
MIT License
