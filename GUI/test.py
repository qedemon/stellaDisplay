import tkinter as tk
from tkinter import messagebox
from tkinter.ttk import Combobox
import serial
import serial.tools.list_ports


# 시리얼 연결 클래스
class SerialConnection:
    def __init__(self):
        self.serial = None

    def connect(self, port, baudrate=9600):
        try:
            self.serial = serial.Serial(port, baudrate)
            return True
        except serial.SerialException as e:
            print(f"Connection failed: {e}")
            return False

    def disconnect(self):
        if self.serial and self.serial.is_open:
            self.serial.close()
            self.serial = None

    def send(self, data):
        if self.serial and self.serial.is_open:
            self.serial.write((data + "\n").encode())


# 전역 시리얼 객체 생성
serial_conn = SerialConnection()

# 함수: Connect 버튼 클릭 시
def connect():
    """시리얼 연결 및 버튼 상태 변경"""
    port = port_combobox.get()  # 사용자가 선택한 포트
    if serial_conn.connect(port):
        connect_button.config(state=tk.DISABLED)
        disconnect_button.config(state=tk.NORMAL)
        msg_text.insert(tk.END, f"Connected to {port}\n")
        msg_text.see(tk.END)
    else:
        msg_text.insert(tk.END, "Failed to connect!\n")
        msg_text.see(tk.END)

# 함수: Disconnect 버튼 클릭 시
def disconnect():
    """시리얼 연결 해제 및 버튼 상태 변경"""
    serial_conn.disconnect()
    connect_button.config(state=tk.NORMAL)
    disconnect_button.config(state=tk.DISABLED)
    msg_text.insert(tk.END, "Disconnected\n")
    msg_text.see(tk.END)

# 함수: 서보 위치와 속도 전송
def send_position_and_speed():
    """서보 위치 및 속도를 아두이노로 전송"""
    if serial_conn.serial is None:
        messagebox.showerror("Error", "Serial connection not established!")
        return

    try:
        set_number = int(set_combobox.get())
        angle1 = int(angle1_entry.get())
        angle2 = int(angle2_entry.get())
        speed1 = int(speed1_entry.get())
        speed2 = int(speed2_entry.get())

        data = f"SET:{set_number}, S1:{angle1}, S2:{angle2}"
        speed_data = f"SET_SPEED:{set_number}, V1:{speed1}, V2:{speed2}"

        serial_conn.send(data)
        serial_conn.send(speed_data)

        msg_text.insert(tk.END, f"Sent: {data}\n")
        msg_text.insert(tk.END, f"Speed Update: {speed_data}\n")
        msg_text.see(tk.END)

    except ValueError:
        messagebox.showerror("Error", "Invalid input! Please enter numbers.")

# 함수: 키보드로 서보 1(상하)와 서보 2(좌우) 값 미세 조정
def adjust_value(event, entry, min_val=120, max_val=600, step=1):
    """키보드로 서보 값을 미세하게 조정"""
    try:
        current_value = int(entry.get())
        if event.keysym == "Up":  # 위쪽 화살표 키 (서보 1)
            new_value = min(current_value + step, max_val)
        elif event.keysym == "Down":  # 아래쪽 화살표 키 (서보 1)
            new_value = max(current_value - step, min_val)
        elif event.keysym == "Left":  # 왼쪽 화살표 키 (서보 2)
            new_value = max(current_value - step, min_val)
        elif event.keysym == "Right":  # 오른쪽 화살표 키 (서보 2)
            new_value = min(current_value + step, max_val)
        else:
            return

        entry.delete(0, tk.END)
        entry.insert(0, str(new_value))

        send_position_and_speed()  # 값이 변경되면 자동 전송
    except ValueError:
        messagebox.showerror("Error", "Invalid input! Please enter numbers.")

# Tkinter GUI 설정
root = tk.Tk()
root.title("Servo Control")

# 포트 선택 콤보박스 (자동으로 연결 가능한 포트 목록 표시)
ports = [port.device for port in serial.tools.list_ports.comports()]
port_combobox = Combobox(root, values=ports, state="readonly")
port_combobox.grid(row=0, column=1)

# Connect / Disconnect 버튼
connect_button = tk.Button(root, text="Connect", command=connect)
connect_button.grid(row=1, column=1)

disconnect_button = tk.Button(root, text="Disconnect", command=disconnect, state=tk.DISABLED)
disconnect_button.grid(row=2, column=1)

# 서보 세트 번호 선택
set_combobox = Combobox(root, values=[str(i) for i in range(1, 101)], state="readonly")
set_combobox.grid(row=3, column=1)
set_combobox.set("1")  # 기본값

# 서보 위치 입력 (기본값 150)
tk.Label(root, text="Servo 1 Angle (120-600):").grid(row=4, column=0)
angle1_entry = tk.Entry(root)
angle1_entry.grid(row=4, column=1)
angle1_entry.insert(0, "150")  # 기본값 150
angle1_entry.bind("<Up>", lambda event, entry=angle1_entry: adjust_value(event, entry))  # 위쪽 화살표 (서보 1)
angle1_entry.bind("<Down>", lambda event, entry=angle1_entry: adjust_value(event, entry))  # 아래쪽 화살표 (서보 1)

tk.Label(root, text="Servo 2 Angle (120-600):").grid(row=5, column=0)
angle2_entry = tk.Entry(root)
angle2_entry.grid(row=5, column=1)
angle2_entry.insert(0, "150")  # 기본값 150
angle2_entry.bind("<Left>", lambda event, entry=angle2_entry: adjust_value(event, entry))  # 왼쪽 화살표 (서보 2)
angle2_entry.bind("<Right>", lambda event, entry=angle2_entry: adjust_value(event, entry))  # 오른쪽 화살표 (서보 2)

# 서보 속도 입력 (기본값 10)
tk.Label(root, text="Servo 1 Speed (1-100):").grid(row=6, column=0)
speed1_entry = tk.Entry(root)
speed1_entry.grid(row=6, column=1)
speed1_entry.insert(0, "10")  # 기본값 10

tk.Label(root, text="Servo 2 Speed (1-100):").grid(row=7, column=0)
speed2_entry = tk.Entry(root)
speed2_entry.grid(row=7, column=1)
speed2_entry.insert(0, "10")  # 기본값 10

# Send 버튼
send_button = tk.Button(root, text="Send Data", command=send_position_and_speed)
send_button.grid(row=8, column=1)

# Speed Update 버튼
speed_button = tk.Button(root, text="Update Speed", command=send_position_and_speed)
speed_button.grid(row=9, column=1)

# 메시지 출력
msg_text = tk.Text(root, height=10, width=40)
msg_text.grid(row=10, column=0, columnspan=2)

# Tkinter 실행
root.mainloop()
