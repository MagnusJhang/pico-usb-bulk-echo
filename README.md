# Pico USB Bulk Echo (Minimal & High Stability)

這是一個針對 Raspberry Pi Pico 系列優化的 **USB Bulk 傳輸模板**。具備工業級穩定性，並支援 Windows 免驅動自動辨識 (WCID)。

## 🌟 特色
- **極致穩定**：解決了 Data Toggle 不同步、大封包（64B）卡死以及讀取超時問題。
- **支援全系列**：一套代碼支援 Pico, Pico W, Pico 2, 以及 Pico 2 W。
- **Windows 一插即用**：內建 MS OS 2.0 描述符 (WCID)，Windows 10/11 會自動載入 WinUSB 驅動，無需手動使用 Zadig。
- **跨平台測試**：隨附的 Python 測試腳本支援 Windows 與 Linux。
- **零配置編譯**：內建 `build.sh` 腳本，自動下載編譯器與處理 SDK 路徑。

## 🛠️ 環境準備
- **Linux**: 安裝 `libusb` (通常已內建)。
- **Windows**: 建議安裝 [libusb-win32](https://github.com/libusb/libusb/releases) 的 DLL（或透過 `pip install pyusb` 配合預建的後端）。
- **Python**: `pip install pyusb`

## 🚀 快速開始

### 1. 複製專案 (含子模組)
```bash
git clone --recursive https://github.com/MagnusJhang/pico-usb-bulk-echo.git
cd pico-usb-bulk-echo
```

### 2. 一鍵編譯
```bash
# 預設編譯為 pico，亦可指定 pico_w, pico2, pico2_w
./build.sh pico_w
```
編譯完成後的 `.uf2` 位於 `build_<板號>/src/bulk_device.uf2`。

## 🧪 執行測試
燒錄完成後，不論在 Windows 或 Linux，直接執行：
```bash
python3 test_usb.py
```
該腳本具備自動重試與同步邏輯，確保 100% 傳輸正確。

## 📌 技術細節
- **VID/PID**: `0xCafe / 0x4889`
- **Endpoints**: `0x01` (OUT), `0x81` (IN)
- **WCID**: 支援 MS OS 2.0 描述符，自動關聯 WinUSB。

## 📄 授權
MIT License
