import os
import shutil
import threading
import subprocess
import tkinter as tk
from tkinter import filedialog, scrolledtext, messagebox

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
if os.name == "nt":
    EXECUTABLE_PATH = os.path.join(BASE_DIR, "vm.exe")
else:
    EXECUTABLE_PATH = os.path.join(BASE_DIR, "vm")

class VMGui:
    def __init__(self, root):
        self.root = root
        self.root.title("Máquina Virtual")

        self.proc = None
        self.thread = None
        self.mode = 0  
        self.lock = threading.Lock()

        top = tk.Frame(root)
        top.pack(fill="x", padx=6, pady=6)

        tk.Label(top, text="Arquivo .obj:").pack(side="left")
        self.entry_file = tk.Entry(top, width=50)
        self.entry_file.pack(side="left", padx=6)
        tk.Button(top, text="Selecionar", command=self.select_file).pack(side="left")

        # === Botões de execução ===
        btn_frame = tk.Frame(root)
        btn_frame.pack(fill="x", padx=6, pady=4)

        tk.Button(btn_frame, text="Executar (Normal)", command=self.run_normal).pack(side="left", padx=4)
        tk.Button(btn_frame, text="Executar (Passo a passo GUI)", command=self.run_step).pack(side="left", padx=4)
        self.btn_next = tk.Button(btn_frame, text="Próxima Instrução", command=self.next_step, state="disabled")
        self.btn_next.pack(side="left", padx=4)
        self.btn_stop = tk.Button(btn_frame, text="Parar", command=self.stop, state="disabled")
        self.btn_stop.pack(side="left", padx=4)

        # === Painéis ===
        pane = tk.Frame(root)
        pane.pack(fill="both", expand=True, padx=6, pady=6)

        left = tk.Frame(pane)
        left.pack(side="left", fill="both", expand=True)

        right = tk.Frame(pane)
        right.pack(side="right", fill="both", expand=True)

        # === Área do arquivo ===
        lbl = tk.Label(left, text="Arquivo .obj carregado")
        lbl.pack()
        self.text_file = scrolledtext.ScrolledText(left, width=60, height=30, font=("Consolas", 11))
        self.text_file.pack(fill="both", expand=True)
        self.text_file.tag_config("current", background="#ffd2d2")
        self.text_file.configure(state="disabled")

        # === Saída ===
        lbl2 = tk.Label(right, text="Saída")
        lbl2.pack()
        self.output = scrolledtext.ScrolledText(right, width=60, height=18, font=("Consolas", 11))
        self.output.pack(fill="both", expand=True)
        self.output.configure(state="disabled")

        # === Pilha ===
        lbl3 = tk.Label(right, text="Pilha (top -> bottom)")
        lbl3.pack()
        self.stack_view = scrolledtext.ScrolledText(right, width=60, height=8, font=("Consolas", 11))
        self.stack_view.pack(fill="both", expand=False)
        self.stack_view.configure(state="disabled")

        # === Entrada para RD ===
        input_frame = tk.Frame(root)
        input_frame.pack(fill="x", padx=6, pady=4)

        tk.Label(input_frame, text="Entrada para RD:").pack(side="left")
        self.input_entry = tk.Entry(input_frame, width=20)
        self.input_entry.pack(side="left", padx=6)

        tk.Button(input_frame, text="Enviar Entrada", command=self.send_input).pack(side="left")

        # Mensagem ao lado do campo (NOVO)
        self.input_msg = tk.Label(input_frame, text="", fg="red", font=("Consolas", 10, "bold"))
        self.input_msg.pack(side="left", padx=10)

    # ======================================================================

    def select_file(self):
        path = filedialog.askopenfilename(filetypes=[("Arquivo OBJ","*.obj"), ("Todos","*.*")])
        if not path:
            return
        self.entry_file.delete(0, tk.END)
        self.entry_file.insert(0, path)
      
        try:
            with open(path, "r", encoding="utf-8") as f:
                txt = f.read()
        except:
            txt = "[ERRO ao abrir arquivo]"
        self.text_file.configure(state="normal")
        self.text_file.delete("1.0", tk.END)
        self.text_file.insert("1.0", txt)
        self.text_file.configure(state="disabled")

    # ======================================================================

    def _append_output(self, txt):
        def inner():
            self.output.configure(state="normal")
            self.output.insert(tk.END, txt)
            self.output.see(tk.END)
            self.output.configure(state="disabled")
        self.root.after(0, inner)

    def _set_stack(self, stack_vals):
        def inner():
            self.stack_view.configure(state="normal")
            self.stack_view.delete("1.0", tk.END)
            if stack_vals:
                for v in stack_vals:
                    self.stack_view.insert(tk.END, f"{v}\n")
            self.stack_view.configure(state="disabled")
        self.root.after(0, inner)

    def _highlight_line(self, pc):
        line_no = pc + 1
        def inner():
            self.text_file.configure(state="normal")
            self.text_file.tag_remove("current", "1.0", tk.END)
            try:
                start = f"{line_no}.0"
                end = f"{line_no}.end"
                self.text_file.tag_add("current", start, end)
                self.text_file.see(start)
            except:
                pass
            self.text_file.configure(state="disabled")
        self.root.after(0, inner)

    # ======================================================================

    def run_normal(self):
        self._start_vm(mode=0)

    def run_step(self):
        self._start_vm(mode=2)

    def _start_vm(self, mode):
        path = self.entry_file.get().strip()
        if not path or not os.path.exists(path):
            messagebox.showerror("Erro", "Selecione um arquivo .obj válido.")
            return

        exe = EXECUTABLE_PATH
        if not os.path.exists(exe):
            found = shutil.which(os.path.basename(exe))
            if found:
                exe = found
            else:
                messagebox.showerror("Erro", f"Executável da VM não encontrado: {exe}")
                return

        # Limpa interfaces
        self.output.configure(state="normal")
        self.output.delete("1.0", tk.END)
        self.output.configure(state="disabled")

        self.stack_view.configure(state="normal")
        self.stack_view.delete("1.0", tk.END)
        self.stack_view.configure(state="disabled")

        self.text_file.tag_remove("current", "1.0", tk.END)

        # Limpa estado de input
        self.input_entry.configure(bg="white")
        self.input_msg.config(text="")

        args = [exe, path]
        if mode == 2:
            args.append("2")

        try:
            creation = 0
            if os.name == "nt":
                creation = subprocess.CREATE_NO_WINDOW

            self.proc = subprocess.Popen(
                args,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1,
                creationflags=creation   # <-- ESTA LINHA
            )
        except Exception as e:
            messagebox.showerror("Erro ao iniciar VM", str(e))
            return

        self.mode = mode
        self.btn_stop.config(state="normal")
        self.btn_next.config(state="disabled")

        self.thread = threading.Thread(target=self._reader_thread, daemon=True)
        self.thread.start()

    # ======================================================================

    def stop(self):
        if self.proc:
            try:
                self.proc.kill()
            except:
                pass
        self.proc = None
        self.btn_next.config(state="disabled")
        self.btn_stop.config(state="disabled")

    # ======================================================================

    def next_step(self):
        if not self.proc:
            return
        try:
            self.proc.stdin.write("\n")
            self.proc.stdin.flush()
        except Exception as e:
            self._append_output(f"[Erro ao enviar next] {e}\n")
        self.btn_next.config(state="disabled")

    # ======================================================================

    def send_input(self):
        if not self.proc:
            messagebox.showwarning("Aviso", "Nenhum processo em execução.")
            return

        val = self.input_entry.get().strip()
        if val == "":
            return

        try:
            self.proc.stdin.write(val + "\n")
            self.proc.stdin.flush()

            # Apaga o campo e remove destaque
            self.input_entry.delete(0, tk.END)
            self.input_entry.configure(bg="white")
            self.input_msg.config(text="")
        except Exception as e:
            self._append_output(f"[Erro ao enviar input] {e}\n")

    # ======================================================================

    def _reader_thread(self):
        proc = self.proc
        if not proc:
            return

        try:
            while True:
                line = proc.stdout.readline()
                if line is None:
                    break
                if line == "" and proc.poll() is not None:
                    break
                if not line:
                    continue

                line_strip = line.strip()

                # Atualizar destaque de linha
                if line_strip.startswith("[INSTRUCTION]"):
                    parts = line_strip.split()
                    if len(parts) >= 2:
                        try:
                            pc = int(parts[1])
                            self._highlight_line(pc)
                        except:
                            pass
                    continue 

                # Atualizar pilha
                elif line_strip.startswith("[STACK]"):
                    parts = line_strip.split()
                    vals = []
                    for p in parts[1:]:
                        try:
                            vals.append(int(p))
                        except:
                            pass
                    self._set_stack(vals)
                    continue

                # Habilitar botão de próximo passo
                elif line_strip.startswith("[STEP]"):
                    def enable_next():
                        self.btn_next.config(state="normal")
                    self.root.after(0, enable_next)

                # ======= AQUI ESTÁ A NOVA PARTE DO PEDIDO ========
                elif line_strip == "[INPUT_REQUIRED]":
                    def show_input_request():
                        self.input_msg.config(text="Entrada necessária!")
                        self.input_entry.configure(bg="#fff1a8")
                        self.input_entry.focus()
                    self.root.after(0, show_input_request)
                    continue
                # ==================================================

                elif line_strip == "[END]":
                    self._append_output("[VM] Execução finalizada.\n")
                    break

                else:
                    self._append_output(line)

            err = proc.stderr.read()
            if err:
                self._append_output("\n[STDERR]\n" + err + "\n")

        except Exception as e:
            self._append_output(f"[Erro no reader] {e}\n")

        finally:
            self.btn_stop.config(state="disabled")
            self.btn_next.config(state="disabled")
            try:
                proc.stdout.close()
            except:
                pass
            try:
                proc.stderr.close()
            except:
                pass
            try:
                proc.stdin.close()
            except:
                pass
            self.proc = None


if __name__ == "__main__":
    root = tk.Tk()
    app = VMGui(root)
    root.mainloop()
