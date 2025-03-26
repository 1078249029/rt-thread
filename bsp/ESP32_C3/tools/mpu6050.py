import socket
import threading
import queue
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque
from datetime import datetime

# 配置
HOST = '0.0.0.0'
PORT = 8080
RECENT_DATA_ENABLE = 0  # 只画出最近若干个点
MAX_DATA_POINTS = 500  # 如果使能RECENT_DATA_ENABLE，那么需要设置最近多少个点参与画图
RECV_TIMEOUT = 5
PLOT_INTERVAL_MS = 50
SAMPLE_COUNT_START = 0
SAMPLE_COUNT_END = 2000
GYRO_RANGE = 10
ACCEL_RANGE = 10

# 数据存储
data_queue = queue.Queue()
if RECENT_DATA_ENABLE:
    time_history = [deque(maxlen=MAX_DATA_POINTS)]
    data_history = [deque(maxlen=MAX_DATA_POINTS) for _ in range(6)]
else:
    time_history = []
    data_history = [ [] for _ in range(6)]

sensor_labels = ['Accel-X (g)', 'Accel-Y (g)', 'Accel-Z (g)', 'Gyro-X (deg/s)', 'Gyro-Y (deg/s)', 'Gyro-Z (deg/s)']
colors = ['b', 'g', 'r', 'c', 'm', 'y']

# 接收数据的线程
def receive_data():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print(f"Waiting for connection on {HOST}:{PORT}...")
        conn, addr = s.accept()
        with conn:
            print(f"Connected to: {addr}")
            conn.settimeout(RECV_TIMEOUT)
            buffer = bytearray()
            while True:
                try:
                    chunk = conn.recv(1024)
                    if not chunk:
                        print("Connection closed")
                        break
                    buffer.extend(chunk)
                    while b'\n' in buffer:
                        line, buffer = buffer.split(b'\n', 1)
                        decoded = line.decode('utf-8').strip()
                        if not decoded:
                            continue
                        try:
                            values = list(map(float, decoded.split(',')))
                            if len(values) == 6:
                                data_queue.put((datetime.now(), values))
                        except ValueError:
                            print(f"Failed to parse data: {decoded}")
                except socket.timeout:
                    continue
                except ConnectionResetError:
                    print("Client disconnected")
                    break

# 处理数据并更新图表
def update_plot(frame):
    while not data_queue.empty():
        timestamp, new_data = data_queue.get()
        time_history.append(timestamp.timestamp())  # 记录时间戳
        for i in range(6):
            data_history[i].append(new_data[i])

    for i, (ax, line) in enumerate(zip(axes, lines)):
        if len(time_history) > 1:
            line.set_data(time_history, data_history[i])
            ax.set_xlim(time_history[0], time_history[-1])  # X 轴范围

    return lines

# 创建图表
try:
    plt.style.use('seaborn-v0_8')
except:
    plt.style.use('ggplot')
fig, axes = plt.subplots(6, 1, figsize=(12, 12), sharex=True)
fig.canvas.manager.set_window_title('MPU6050 Real-time Data - 6 Axes')

# Initialize lines list
lines = []
colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b']
# 在创建子图时，手动设置 y 轴范围
y_limits = [(-GYRO_RANGE, GYRO_RANGE), (-GYRO_RANGE, GYRO_RANGE), (-ACCEL_RANGE, ACCEL_RANGE), (-ACCEL_RANGE, ACCEL_RANGE), (-ACCEL_RANGE, ACCEL_RANGE), (-ACCEL_RANGE, ACCEL_RANGE)]
x_limits = [(SAMPLE_COUNT_START, SAMPLE_COUNT_END)]
for i, ax in enumerate(axes):
    line, = ax.plot([], [], label=sensor_labels[i], color=colors[i])
    lines.append(line)
    ax.set_ylabel(sensor_labels[i])
    ax.legend(loc='upper right')
    ax.grid(True)
    ax.set_ylim(y_limits[i])  # 设定y轴范围
    ax.set_xlim(x_limits[0])  # 设定x轴范围

# axes[-1].set_xlabel("Time (seconds)")

# 启动数据接收线程
threading.Thread(target=receive_data, daemon=True).start()

# 动画更新
ani = animation.FuncAnimation(fig, update_plot, interval=PLOT_INTERVAL_MS, blit=True, cache_frame_data=False)
plt.show()
