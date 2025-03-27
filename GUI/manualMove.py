import serial
import time
import Tkinter as tk
import ttk
import serial.tools.list_ports


root = tk.Tk()
root.title("Servo Motor Control")

# Serial port selection
tk.Label(root, text="Select Serial Port", font=("Arial", 12)).pack(pady=5)
port_list = get_serial_ports()
port_var = tk.StringVar()
port_menu = ttk.Combobox(root, textvariable=port_var, values=port_list)
port_menu.pack(pady=5)

# Connect button
connect_button = tk.Button(root, text="Connect", command=connect_arduino)
connect_button.pack(pady=5)

# Connection status label
connection_label = tk.Label(root, text="Not connected", font=("Arial", 12, "italic"))
connection_label.pack(pady=5)

# Set selection
tk.Label(root, text="Select Servo Set", font=("Arial", 12)).pack(pady=5)
servo_sets = ["Set " + str(i+1) for i in range(100)]
set_var = tk.StringVar()
set_menu = ttk.Combobox(root, textvariable=set_var, values=servo_sets)
set_menu.pack(pady=5)

# Servo 1 inputs
tk.Label(root, text="Servo 1 Angle (120-600):", font=("Arial", 10)).pack()
servo1_var = tk.StringVar()
servo1_entry = tk.Entry(root, textvariable=servo1_var, width=5)
servo1_entry.pack()

tk.Label(root, text="Servo 1 Speed (1-100):", font=("Arial", 10)).pack()
servo1_speed_var = tk.StringVar()
servo1_speed_entry = tk.Entry(root, textvariable=servo1_speed_var, width=5)
servo1_speed_entry.pack()

# Servo 2 inputs
tk.Label(root, text="Servo 2 Angle (120-600):", font=("Arial", 10)).pack()
servo2_var = tk.StringVar()
servo2_entry = tk.Entry(root, textvariable=servo2_var, width=5)
servo2_entry.pack()

tk.Label(root, text="Servo 2 Speed (1-100):", font=("Arial", 10)).pack()
servo2_speed_var = tk.StringVar()
servo2_speed_entry = tk.Entry(root, textvariable=servo2_speed_var, width=5)
servo2_speed_entry.pack()

# Send servo angles button
def send_servo_angles():
    if arduino:
        try:
            servo1_value = int(servo1_var.get())
            servo2_value = int(servo2_var.get())

            if 120 <= servo1_value <= 600 and 120 <= servo2_value <= 600:
                set_number = int(set_var.get().split()[1])
                command = "A,{},{},{}\n".format(set_number, servo1_value, servo2_value)
                arduino.write(command.encode())
                print("Sent Angle:", command.strip())
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
                set_number = int(set_var.get().split()[1])
                command = "S,{},{},{}\n".format(set_number, servo1_speed, servo2_speed)
                arduino.write(command.encode())
                print("Sent Speed:", command.strip())
        except ValueError:
            print("Error: Invalid input")

send_speeds_button = tk.Button(root, text="Send Servo Speeds", command=send_servo_speeds)
send_speeds_button.pack(pady=5)

# Start GUI loop
root.mainloop()
