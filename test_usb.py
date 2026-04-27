import usb.core
import usb.util
import time
import sys
import random

def main():
    # --- 配置參數 ---
    pid = 0x4905
    vid = 0xcafe
    test_duration = 310  # 10 分鐘
    chunk_size = 63      # 最穩定寬度
    log_interval = 5     # 5 秒紀錄一次
    
    print(f"Starting 10-Minute Stability Test (PID: {hex(pid)})")
    print(f"Logging every {log_interval} seconds to stdout and stability.log...")

    dev = usb.core.find(idVendor=vid, idProduct=pid)
    if dev is None:
        print("Error: Device not found.")
        return

    # 初始化連線
    try:
        if dev.is_kernel_driver_active(0):
            dev.detach_kernel_driver(0)
    except: pass
    
    dev.set_configuration()
    usb.util.claim_interface(dev, 0)
    dev.clear_halt(0x01)
    dev.clear_halt(0x81)

    # 徹底排空殘留資料
    while True:
        try:
            dev.read(0x81, 64, timeout=10)
        except:
            break

    # --- 測試循環 ---
    total_bytes = 0
    total_errors = 0
    start_time = time.time()
    last_log_time = start_time

    log_file = open("stability.log", "w")
    log_file.write("Time(s), Data(MB), Speed(KB/s), Errors\n")

    try:
        while (time.time() - start_time) < test_duration:
            # 準備隨機資料
            test_data = bytes([random.randint(ord('a'), ord('z')) for _ in range(chunk_size)])
            
            try:
                # 寫入
                dev.write(0x01, test_data, timeout=1000)
                
                # 魯棒讀取：處理可能的分段封包
                response = b""
                timeout_limit = time.time() + 1.0
                while len(response) < chunk_size and time.time() < timeout_limit:
                    try:
                        chunk = dev.read(0x81, chunk_size - len(response), timeout=100)
                        if chunk:
                            response += bytes(chunk)
                    except usb.core.USBError as e:
                        if e.errno == 110: # Timeout
                            continue
                        raise e
                
                # 校驗
                if response.upper() != test_data.upper():
                    total_errors += 1
                else:
                    total_bytes += chunk_size

            except Exception as e:
                total_errors += 1
                time.sleep(0.01)

            # --- 5 秒定時紀錄 ---
            now = time.time()
            if now - last_log_time >= log_interval:
                elapsed = now - start_time
                speed = (total_bytes / 1024) / elapsed if elapsed > 0 else 0
                log_msg = f"Time: {int(elapsed):>3}s | Data: {total_bytes/1024/1024:>6.2f}MB | Speed: {speed:>7.2f}KB/s | Errors: {total_errors}"
                
                print(log_msg)
                log_file.write(f"{int(elapsed)}, {total_bytes/1024/1024:.2f}, {speed:.2f}, {total_errors}\n")
                log_file.flush()
                last_log_time = now

    except KeyboardInterrupt:
        print("\nTest interrupted by user.")

    finally:
        total_elapsed = time.time() - start_time
        print(f"\n--- FINAL REPORT ---")
        print(f"Total Time  : {total_elapsed:.2f}s")
        print(f"Total Data  : {total_bytes/1024/1024:.2f} MB")
        print(f"Avg Speed   : {(total_bytes/1024)/total_elapsed:.2f} KB/s")
        print(f"Total Errors: {total_errors}")
        log_file.close()
        usb.util.dispose_resources(dev)

if __name__ == "__main__":
    main()
