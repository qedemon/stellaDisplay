import numpy as np
import tkinter as tk
import json
from tkinter import messagebox

# File to store data
data_file = "fitting_data.json"

# Load saved data
try:
    with open(data_file, "r") as f:
        saved_data = json.load(f)
        X_values = saved_data.get("X_values", [])
        Y_values = saved_data.get("Y_values", [])
        Theta1_values = saved_data.get("Theta1_values", [])
        Theta2_values = saved_data.get("Theta2_values", [])
except (FileNotFoundError, json.JSONDecodeError):
    X_values, Y_values, Theta1_values, Theta2_values = [], [], [], []

def save_data():
    with open(data_file, "w") as f:
        json.dump({
            "X_values": X_values,
            "Y_values": Y_values,
            "Theta1_values": Theta1_values,
            "Theta2_values": Theta2_values
        }, f)

def add_data():
    try:
        X = float(entry_X.get())
        Y = float(entry_Y.get())
        theta1 = float(entry_theta1.get())
        theta2 = float(entry_theta2.get())
        
        X_values.append(X)
        Y_values.append(Y)
        Theta1_values.append(theta1)
        Theta2_values.append(theta2)
        
        listbox.insert(tk.END, f"X={X}, Y={Y}, θ1={theta1}, θ2={theta2}")
        save_data()
        
        entry_X.delete(0, tk.END)
        entry_Y.delete(0, tk.END)
        entry_theta1.delete(0, tk.END)
        entry_theta2.delete(0, tk.END)
    except ValueError:
        messagebox.showerror("Input Error", "Please enter valid numbers!")

def remove_selected_data():
    try:
        selected_index = listbox.curselection()[0]
        listbox.delete(selected_index)
        
        del X_values[selected_index]
        del Y_values[selected_index]
        del Theta1_values[selected_index]
        del Theta2_values[selected_index]
        
        save_data()
    except IndexError:
        messagebox.showerror("Selection Error", "Please select an item to remove!")

def fit_data():
    if len(X_values) < 6:
        messagebox.showerror("Insufficient Data", "At least 6 data points are required.")
        return
    
    A = np.vstack([
        np.array(X_values)**2,
        np.array(X_values) * np.array(Y_values),
        np.array(Y_values)**2,
        X_values,
        Y_values,
        np.ones(len(X_values))
    ]).T
    
    Theta = np.vstack([Theta1_values, Theta2_values]).T
    
    W = np.linalg.lstsq(A, Theta, rcond=None)[0]
    
    a1, b1, c1, d1, e1, f1 = W[:, 0]
    a2, b2, c2, d2, e2, f2 = W[:, 1]
    
    result_text.set(f"θ1 = {a1:.4f}X² + {b1:.4f}XY + {c1:.4f}Y² + {d1:.4f}X + {e1:.4f}Y + {f1:.4f}\n"
                    f"θ2 = {a2:.4f}X² + {b2:.4f}XY + {c2:.4f}Y² + {d2:.4f}X + {e2:.4f}Y + {f2:.4f}")

def load_saved_data():
    for X, Y, theta1, theta2 in zip(X_values, Y_values, Theta1_values, Theta2_values):
        listbox.insert(tk.END, f"X={X}, Y={Y}, θ1={theta1}, θ2={theta2}")

# Create UI
root = tk.Tk()
root.title("Quadratic Least Squares Fitting")

# Input fields
tk.Label(root, text="X:").grid(row=0, column=0)
tk.Label(root, text="Y:").grid(row=1, column=0)
tk.Label(root, text="θ1:").grid(row=2, column=0)
tk.Label(root, text="θ2:").grid(row=3, column=0)

entry_X = tk.Entry(root)
entry_Y = tk.Entry(root)
entry_theta1 = tk.Entry(root)
entry_theta2 = tk.Entry(root)

entry_X.grid(row=0, column=1)
entry_Y.grid(row=1, column=1)
entry_theta1.grid(row=2, column=1)
entry_theta2.grid(row=3, column=1)

# Add buttons
tk.Button(root, text="Add Data", command=add_data).grid(row=4, column=0, columnspan=2)
tk.Button(root, text="Remove Selected", command=remove_selected_data).grid(row=5, column=0, columnspan=2)
tk.Button(root, text="Run Fitting", command=fit_data).grid(row=6, column=0, columnspan=2)

# Listbox to display data
listbox = tk.Listbox(root, width=40, height=10)
listbox.grid(row=7, column=0, columnspan=2)

# Label to display results
result_text = tk.StringVar()
tk.Label(root, textvariable=result_text, fg="blue").grid(row=8, column=0, columnspan=2)

# Load saved data into listbox
load_saved_data()

# Run application
root.mainloop()
