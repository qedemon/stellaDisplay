import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports

# -------------------------------
# 시리얼 포트 설정
# -------------------------------
class SerialConnection:
    def __init__(self):
        self.serial = None

    def connect(self, port, baudrate=9600):
        try:
            self.serial = serial.Serial(port, baudrate, timeout=1)
            return True
        except serial.SerialException as e:
            messagebox.showerror("Error", f"Failed to connect: {e}")
            self.serial = None
            return False

    def disconnect(self):
        if self.serial:
            self.serial.close()
            self.serial = None

    def send(self, data):
        if self.serial and self.serial.is_open:
            self.serial.write((data + "\n").encode())

    def receive(self):
        if self.serial and self.serial.is_open and self.serial.in_waiting:
            return self.serial.readline().decode().strip()
        return None

serial_conn = SerialConnection()

# -------------------------------
# GUI 설정
# -------------------------------
class ServoControlApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Servo Motor Control")
        
        # 시리얼 포트 선택
        self.port_label = tk.Label(root, text="Serial Port:")
        self.port_label.grid(row=0, column=0)
        self.port_combobox = ttk.Combobox(root, values=self.get_serial_ports())
        self.port_combobox.grid(row=0, column=1)
        self.connect_button = tk.Button(root, text="Connect", command=self.connect_serial)
        self.connect_button.grid(row=0, column=2)
        self.disconnect_button = tk.Button(root, text="Disconnect", command=self.disconnect_serial, state=tk.DISABLED)
        self.disconnect_button.grid(row=0, column=3)

        # 세트 선택 (1~100)
        self.set_label = tk.Label(root, text="Set Number:")
        self.set_label.grid(row=1, column=0)
        self.set_combobox = ttk.Combobox(root, values=list(range(1, 101)))
        self.set_combobox.grid(row=1, column=1)
        self.set_combobox.current(0)

        # 스피드 선택
        self.speed_label = tk.Label(root, text="Speed:")
        self.speed_label.grid(row=2, column=0)
        self.speed_entry = tk.Entry(root)
        self.speed_entry.grid(row=2, column=1)
        self.speed_entry.insert(0, "50")

        # 서보 1 제어
        self.servo1_label = tk.Label(root, text="Servo 1 Angle:")
        self.servo1_label.grid(row=3, column=0)
        self.servo1_entry = tk.Entry(root)
        self.servo1_entry.grid(row=3, column=1)
        self.servo1_entry.bind("<Up>", lambda event: self.adjust_value(event, self.servo1_entry))
        self.servo1_entry.bind("<Down>", lambda event: self.adjust_value(event, self.servo1_entry))
        self.servo1_entry.insert(0, "360")

        # 서보 2 제어
        self.servo2_label = tk.Label(root, text="Servo 2 Angle:")
        self.servo2_label.grid(row=4, column=0)
        self.servo2_entry = tk.Entry(root)
        self.servo2_entry.grid(row=4, column=1)
        self.servo2_entry.bind("<Up>", lambda event: self.adjust_value(event, self.servo2_entry))
        self.servo2_entry.bind("<Down>", lambda event: self.adjust_value(event, self.servo2_entry))
        self.servo2_entry.insert(0, "360")
        
    

        # 값 전송 버튼
        self.send_button = tk.Button(root, text="Send Data", command=self.send_data)
        self.send_button.grid(row=5, column=1)

        # 아두이노 메시지 출력 창
        self.msg_label = tk.Label(root, text="Arduino Messages:")
        self.msg_label.grid(row=6, column=0)
        self.msg_text = tk.Text(root, height=5, width=40)
        self.msg_text.grid(row=6, column=0, columnspan=3)
        
        # 시리얼 메시지 수신 주기적 업데이트
        self.update_serial()

    def get_serial_ports(self):
        """ 사용 가능한 시리얼 포트 목록 가져오기 """
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]

    def connect_serial(self):
        """ 시리얼 포트 연결 """
        port = self.port_combobox.get()
        if port:
            if serial_conn.connect(port):
                self.connect_button.config(state=tk.DISABLED)
                self.disconnect_button.config(state=tk.NORMAL)
                messagebox.showinfo("Info", f"Connected to {port}")
    
    def disconnect_serial(self):
        """ 시리얼 포트 연결 해제"""
        serial_conn.disconnect()
        self.connect_button.config(state=tk.NORMAL)
        self.disconnect_button.config(state=tk.DISABLED)

    def send_data(self):
        """ 서보 데이터를 아두이노로 전송 """
        if serial_conn.serial is None:
            messagebox.showerror("Error", "Serial connection not established!")
            return
        
        try:
            set_number = int(self.set_combobox.get())
            speed = int(self.speed_entry.get())
            servo1_angle = int(self.servo1_entry.get())
            servo2_angle = int(self.servo2_entry.get())
            
            # 값 범위 체크 (120 ~ 600)
            if not (120 <= servo1_angle <= 600 and 120 <= servo2_angle <= 600):
                messagebox.showerror("Error", "Angle must be between 120 and 600")
                return
            
            data = f"SET:{set_number}, S:{speed}, V1:{servo1_angle}, V2:{servo2_angle}"
            serial_conn.send(data)
            self.msg_text.insert(tk.END, f"Sent: {data}\n")
            self.msg_text.see(tk.END)
        except ValueError:
            messagebox.showerror("Error", "Invalid input! Please enter numbers.")

    def update_serial(self):
        """ 아두이노 메시지 수신 """
        message = serial_conn.receive()
        if message:
            self.msg_text.insert(tk.END, f"Arduino: {message}\n")
            self.msg_text.see(tk.END)
        
        self.root.after(100, self.update_serial)  # 100ms마다 실행
    
    def adjust_value(self, event, entry, min_val=120, max_val=600, step=1):
        """키보드로 서보 값을 미세하게 조정"""
        try:
            current_value = int(entry.get())
            if event.keysym == "Up":  # 위쪽 화살표 키 (서보 1)
                new_value = min(current_value + step, max_val)
            elif event.keysym == "Down":  # 아래쪽 화살표 키 (서보 1)
                new_value = max(current_value - step, min_val)
            else:
                return

            entry.delete(0, tk.END)
            entry.insert(0, str(new_value))

            self.send_data()  # 값이 변경되면 자동 전송
        except ValueError:
            messagebox.showerror("Error", "Invalid input! Please enter numbers.")

# -------------------------------
# GUI 실행
# -------------------------------
if __name__ == "__main__":
    root = tk.Tk()
    app = ServoControlApp(root)
    root.mainloop()
