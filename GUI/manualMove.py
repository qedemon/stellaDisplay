import serial
import time
import tkinter as tk
from tkinter import ttk
import serial.tools.list_ports

# Get list of serial ports
def get_serial_ports():
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]

# Setup Arduino connection via serial port
def connect_arduino(port):
    try:
        arduino = serial.Serial(port, 9600, timeout=1)
        time.sleep(2)  # Wait for the serial connection to stabilize
        return arduino
    except serial.SerialException as e:
        print(f"Connection failed: {e}")
        return None

# Create GUI window
root = tk.Tk()
root.title("Servo Motor Control")

# Serial port selection
tk.Label(root, text="Select Serial Port", font=("Arial", 12)).pack(pady=5)
port_list = get_serial_ports()
port_var = tk.StringVar(value=port_list[0] if port_list else "")
port_menu = ttk.Combobox(root, textvariable=port_var, values=port_list)
port_menu.pack(pady=5)

# Connect button
def connect_button_pressed():
    selected_port = port_var.get()
    global arduino
    arduino = connect_arduino(selected_port)
    if arduino:
        connection_label.config(text=f"Connected: {selected_port}")

connect_button = tk.Button(root, text="Connect", command=connect_button_pressed)
connect_button.pack(pady=5)

# Connection status label
connection_label = tk.Label(root, text="Not connected", font=("Arial", 12, "italic"))
connection_label.pack(pady=5)

# Set selection
tk.Label(root, text="Select Servo Set", font=("Arial", 12)).pack(pady=5)
servo_sets = [f"Set {i+1}" for i in range(100)]
set_var = tk.StringVar(value=servo_sets[0])
set_menu = ttk.Combobox(root, textvariable=set_var, values=servo_sets)
set_menu.pack(pady=5)

# Servo 1 inputs
tk.Label(root, text="Servo 1 Angle (120-600):", font=("Arial", 10)).pack()
servo1_var = tk.StringVar(value="300")
servo1_entry = tk.Entry(root, textvariable=servo1_var, width=5)
servo1_entry.pack()

tk.Label(root, text="Servo 1 Speed (1-100):", font=("Arial", 10)).pack()
servo1_speed_var = tk.StringVar(value="50")
servo1_speed_entry = tk.Entry(root, textvariable=servo1_speed_var, width=5)
servo1_speed_entry.pack()

# Servo 2 inputs
tk.Label(root, text="Servo 2 Angle (120-600):", font=("Arial", 10)).pack()
servo2_var = tk.StringVar(value="300")
servo2_entry = tk.Entry(root, textvariable=servo2_var, width=5)
servo2_entry.pack()

tk.Label(root, text="Servo 2 Speed (1-100):", font=("Arial", 10)).pack()
servo2_speed_var = tk.StringVar(value="50")
servo2_speed_entry = tk.Entry(root, textvariable=servo2_speed_var, width=5)
servo2_speed_entry.pack()

# Send servo angles button
def send_servo_angles():
    if arduino:
        try:
            servo1_value = int(servo1_var.get())
            servo2_value = int(servo2_var.get())

            if 120 <= servo1_value <= 600 and 120 <= servo2_value <= 600:
                selected_set = set_var.get()
                set_number = int(selected_set.split()[1])
                command = f"A,{set_number},{servo1_value},{servo2_value}\n"
                arduino.write(command.encode())
                print(f"Sent Angle: {command.strip()}")
            else:
                print("Error: Angles out of range")
        except ValueError:
            print("Error: Invalid input")

send_angles_button = tk.Button(root, text="Send Servo Angles", command=send_servo_angles)
send_angles_button.pack(pady=5)

# Send servo speeds button
def send_servo_speeds():
    if arduino:
        try:
            servo1_speed = int(servo1_speed_var.get())
            servo2_speed = int(servo2_speed_var.get())

            if 1 <= servo1_speed <= 100 and 1 <= servo2_speed <= 100:
                selected_set = set_var.get()
                set_number = int(selected_set.split()[1])
                command = f"S,{set_number},{servo1_speed},{servo2_speed}\n"
                arduino.write(command.encode())
                print(f"Sent Speed: {command.strip()}")
            else:
                print("Error: Speed out of range")
        except ValueError:
            print("Error: Invalid input")

send_speeds_button = tk.Button(root, text="Send Servo Speeds", command=send_servo_speeds)
send_speeds_button.pack(pady=5)

# Start GUI loop
root.mainloop()
