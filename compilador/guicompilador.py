import subprocess
import tkinter as tk
from tkinter import filedialog, messagebox
from tkinter import scrolledtext


class CompiladorGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Compilador")

        self.arquivo_atual = None

    
        frame_codigo = tk.Frame(root)
        frame_codigo.pack(padx=10, pady=10)


        self.numeros = tk.Text(frame_codigo, width=5, height=30, font=("Consolas", 12),
                               bg="#eee", state="disabled")
        self.numeros.pack(side=tk.LEFT, fill="y")

      
        self.texto = tk.Text(frame_codigo, width=100, height=30, font=("Consolas", 12))
        self.texto.pack(side=tk.LEFT)

        
        scroll = tk.Scrollbar(frame_codigo, command=self._scroll_both)
        scroll.pack(side=tk.RIGHT, fill="y")

        self.texto.config(yscrollcommand=scroll.set)
        self.numeros.config(yscrollcommand=scroll.set)

        self.texto.bind("<KeyRelease>", self._update_line_numbers)

     
        frame_btn = tk.Frame(root)
        frame_btn.pack()

        tk.Button(frame_btn, text="Carregar Arquivo", command=self.carregar_arquivo).pack(side=tk.LEFT, padx=5)
        tk.Button(frame_btn, text="Compilar", command=self.compilar).pack(side=tk.LEFT, padx=5)

    
        self.saida = scrolledtext.ScrolledText(root, width=100, height=12, font=("Consolas", 11),
                                               fg="white", bg="black")
        self.saida.pack(padx=10, pady=10)

  
    def _scroll_both(self, *args):
        """Sincroniza o scroll das duas caixas."""
        self.texto.yview(*args)
        self.numeros.yview(*args)


    def _update_line_numbers(self, event=None):
        """Atualiza numeração de linhas."""
        self.numeros.config(state="normal")
        self.numeros.delete("1.0", tk.END)

        total = int(self.texto.index("end").split(".")[0]) - 1
        numeros_texto = "\n".join(str(i) for i in range(1, total + 1))

        self.numeros.insert("1.0", numeros_texto)
        self.numeros.config(state="disabled")


    def carregar_arquivo(self):
        caminho = filedialog.askopenfilename(
            filetypes=[("Arquivos de texto", "*.txt"), ("Todos", "*.*")]
        )

        if not caminho:
            return

        self.arquivo_atual = caminho

        with open(caminho, "r", encoding="utf-8") as f:
            conteudo = f.read()

        self.texto.delete("1.0", tk.END)
        self.texto.insert(tk.END, conteudo)

        self._update_line_numbers()

   
    def compilar(self):
        if not self.arquivo_atual:
            messagebox.showerror("Erro", "Nenhum arquivo carregado.")
            return

        # Remover destaques anteriores
        self.texto.tag_delete("erro")
        self.texto.tag_config("erro", background="red", foreground="white")

        import os
        import sys

        # Função correta para achar arquivos incluídos no .exe do PyInstaller
        def caminho_base():
            if getattr(sys, 'frozen', False):
                return sys._MEIPASS  # pasta temporária criada pelo PyInstaller
            return os.path.dirname(os.path.abspath(__file__))  # rodando normal

        # Agora ele vai achar o compilador.exe em qualquer modo
        caminho_exe = os.path.join(caminho_base(), "compilador.exe")

        # Debug opcional (pode remover):
        # print("Usando compilador:", caminho_exe)

        try:
            resultado = subprocess.run(
                [caminho_exe, self.arquivo_atual],
                capture_output=True,
                text=True
            )
        except FileNotFoundError:
            messagebox.showerror("Erro", f"Não encontrei o arquivo:\n{caminho_exe}")
            return

        saida = resultado.stdout + "\n" + resultado.stderr

        self.saida.delete("1.0", tk.END)
        self.saida.insert(tk.END, saida)

        linha_erro = self.extrair_linha_erro(saida)
        if linha_erro:
            self.destacar_linha(linha_erro)



    def extrair_linha_erro(self, texto):
        import re
        padrao = r"Linha\s+(\d+)"
        m = re.search(padrao, texto)
        if m:
            return int(m.group(1))
        return None

    def destacar_linha(self, numero):
        """Destaca linha com erro."""
        inicio = f"{numero}.0"
        fim = f"{numero}.end"

        try:
            self.texto.tag_add("erro", inicio, fim)
            self.texto.see(inicio)   # rola até o erro
        except:
            pass



if __name__ == "__main__":
    root = tk.Tk()
    CompiladorGUI(root)
    root.mainloop()
