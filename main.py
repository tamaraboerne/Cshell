from subprocess import call
import tkinter as tk

def run_command():
    call(["./main", "args", "to", "main"])

master = tk.Tk()
master.title("Shell")
master.configure(background="black")
master.geometry("800x600")


master.mainloop()
