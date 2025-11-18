from subprocess import Popen, PIPE
import tkinter as tk
from tkinter import scrolledtext
import threading

class ShellGUI:
    def __init__(self, master):
        self.master = master
        self.master.title("C Shell")
        self.master.configure(background="black")
        self.master.geometry("800x600")
        
        self.process = None
        
        # Output display
        self.output_display = scrolledtext.ScrolledText(
            master,
            wrap=tk.WORD,
            bg="black",
            fg="#00ff00",
            font=("Courier", 11),
            state=tk.DISABLED
        )
        self.output_display.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Input frame
        input_frame = tk.Frame(master, bg="black")
        input_frame.pack(fill=tk.X, padx=5, pady=5)
        
        # Prompt label
        self.prompt_label = tk.Label(
            input_frame,
            text="lsh> ",
            bg="black",
            fg="#00ff00",
            font=("Courier", 11)
        )
        self.prompt_label.pack(side=tk.LEFT)
        
        # Input entry field
        self.input_field = tk.Entry(
            input_frame,
            bg="#333333",
            fg="#00ff00",
            font=("Courier", 11),
            insertbackground="#00ff00"
        )
        self.input_field.pack(side=tk.LEFT, fill=tk.X, expand=True)
        self.input_field.bind("<Return>", self.execute_command)
        
        self.input_field.focus()
        
        # Status label
        self.status_label = tk.Label(
            master,
            text="Starting shell...",
            bg="black",
            fg="#00ff00",
            font=("Courier", 9)
        )
        self.status_label.pack(fill=tk.X, padx=5)
        
        # Start the shell process
        self.start_shell()
    
    def start_shell(self):
        """Start the C shell process"""
        try:
            self.process = Popen(
                ["./main"],
                stdin=PIPE,
                stdout=PIPE,
                stderr=PIPE,
                text=True,
                bufsize=1
            )
            self.status_label.config(text="Shell running")
            self.read_output()
        except FileNotFoundError:
            self.append_output("Error: Could not find './main' executable\n")
            self.status_label.config(text="Error: executable not found")
    
    def execute_command(self, event):
        """Execute command when Enter is pressed"""
        command = self.input_field.get()
        
        if command.strip():
            # Display the command with prompt
            self.append_output(f"lsh> {command}\n")
            
            # Send command to shell process
            if self.process:
                try:
                    self.process.stdin.write(command + "\n")
                    self.process.stdin.flush()
                except:
                    self.append_output("Error: Shell process not responding\n")
            
            # Clear input field
            self.input_field.delete(0, tk.END)
    
    def append_output(self, text):
        """Append text to output display"""
        self.output_display.config(state=tk.NORMAL)
        self.output_display.insert(tk.END, text)
        self.output_display.see(tk.END)
        self.output_display.config(state=tk.DISABLED)
    
    def close_window(self):
        """Close GUI and ensure child process is terminated."""
        try:
            if self.process:
                # try graceful shutdown
                try:
                    if self.process.stdin:
                        self.process.stdin.close()
                except Exception:
                    pass
                try:
                    self.process.terminate()
                except Exception:
                    pass
        finally:
            # destroy the Tk window (must be called on main thread)
            try:
                self.master.destroy()
            except Exception:
                pass

    def read_output(self):
        """Read output from shell process in background thread"""
        def read_thread():
            if self.process and self.process.stdout:
                while True:
                    try:
                        line = self.process.stdout.readline()
                        if line:
                            self.append_output(line)
                            # detect sentinel from C exit function
                            if line.strip() == "###CSHELL_EXIT###":
                                # schedule GUI close on main thread
                                self.master.after(0, self.close_window)
                                break
                        else:
                            # EOF - child exited; close GUI
                            self.master.after(0, self.close_window)
                            break
                    except:
                        # on error, attempt to close GUI
                        self.master.after(0, self.close_window)
                        break
        
        thread = threading.Thread(target=read_thread, daemon=True)
        thread.start()

# Create GUI
master = tk.Tk()
gui = ShellGUI(master)
master.mainloop()
