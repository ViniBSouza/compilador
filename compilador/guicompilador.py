"""
  Programa: guicompilador.py
  Descrição: Interface gráfica para compilador
"""

import subprocess
import tkinter as tk
from tkinter import filedialog, messagebox
from tkinter import scrolledtext


class CompiladorGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Compilador")

        # Caminho do arquivo atualmente carregado
        self.arquivo_atual = None

        # Área onde fica o código e os números de linha 
        frame_codigo = tk.Frame(root)
        frame_codigo.pack(padx=10, pady=10)

        # Caixa só para números de linha (read-only)
        self.numeros = tk.Text(
            frame_codigo,
            width=5, height=30,
            font=("Consolas", 12),
            bg="#eee",
            state="disabled"
        )
        self.numeros.pack(side=tk.LEFT, fill="y")

        # Caixa onde o usuário escreve/visualiza o código
        self.texto = tk.Text(
            frame_codigo,
            width=100, height=30,
            font=("Consolas", 12)
        )
        self.texto.pack(side=tk.LEFT)

        # Scrollbar sincronizada para as duas caixas
        scroll = tk.Scrollbar(frame_codigo, command=self._scroll_both)
        scroll.pack(side=tk.RIGHT, fill="y")

        self.texto.config(yscrollcommand=scroll.set)
        self.numeros.config(yscrollcommand=scroll.set)

        # Atualiza números de linhas sempre que o usuário digita algo
        self.texto.bind("<KeyRelease>", self._update_line_numbers)

        # ======== Botões principais ========
        frame_btn = tk.Frame(root)
        frame_btn.pack()

        tk.Button(frame_btn, text="Carregar Arquivo", command=self.carregar_arquivo).pack(side=tk.LEFT, padx=5)
        tk.Button(frame_btn, text="Compilar", command=self.compilar).pack(side=tk.LEFT, padx=5)

        #Saída do compilador
        self.saida = scrolledtext.ScrolledText(
            root,
            width=100, height=12,
            font=("Consolas", 11),
            fg="white",
            bg="black"
        )
        self.saida.pack(padx=10, pady=10)

    # Sincroniza o scroll das duas caixas (texto e números)
    def _scroll_both(self, *args):
        self.texto.yview(*args)
        self.numeros.yview(*args)

    # Atualiza a numeração das linhas conforme o texto muda
    def _update_line_numbers(self, event=None):
        self.numeros.config(state="normal")
        self.numeros.delete("1.0", tk.END)

        # Número total de linhas (o "end" sempre tem +1)
        total = int(self.texto.index("end").split(".")[0]) - 1

        numeros_texto = "\n".join(str(i) for i in range(1, total + 1))
        self.numeros.insert("1.0", numeros_texto)
        self.numeros.config(state="disabled")

    # Carrega arquivo .txt ou outro e joga na área de edição

    def carregar_arquivo(self):
        caminho = filedialog.askopenfilename(
            filetypes=[("Arquivos de texto", "*.txt"), ("Todos", "*.*")]
        )

        if not caminho:
            return  # Usuário cancelou

        self.arquivo_atual = caminho

        # Lê o arquivo
        with open(caminho, "r", encoding="utf-8") as f:
            conteudo = f.read()

        # Coloca o conteúdo no campo de texto
        self.texto.delete("1.0", tk.END)
        self.texto.insert(tk.END, conteudo)

        self._update_line_numbers()


    # Executa o compilador externo e exibe a saída
    def compilar(self):
        if not self.arquivo_atual:
            messagebox.showerror("Erro", "Nenhum arquivo carregado.")
            return

        # Remove tags de highlight antigo
        self.texto.tag_delete("erro")
        self.texto.tag_config("erro", background="red", foreground="white")

        import os
        import sys

        # Função usada para localizar arquivos incluídos em executáveis criados pelo PyInstaller
        def caminho_base():
            if getattr(sys, 'frozen', False):
                return sys._MEIPASS  # Caminho temporário criado pelo PyInstaller
            return os.path.dirname(os.path.abspath(__file__))

        # Caminho para o compilador.exe
        caminho_exe = os.path.join(caminho_base(), "compilador.exe")

        # Tenta executar o compilador
        try:
            resultado = subprocess.run(
                [caminho_exe, self.arquivo_atual],
                capture_output=True,
                text=True
            )
        except FileNotFoundError:
            messagebox.showerror("Erro", f"Não encontrei o arquivo:\n{caminho_exe}")
            return

        # Combina stdout e stderr
        saida = resultado.stdout + "\n" + resultado.stderr

        # Mostra saída na caixa inferior
        self.saida.delete("1.0", tk.END)
        self.saida.insert(tk.END, saida)

        # Procura linha do erro para destacar
        linha_erro = self.extrair_linha_erro(saida)
        if linha_erro:
            self.destacar_linha(linha_erro)

    # Extrai número da linha a partir da mensagem do compilador
    def extrair_linha_erro(self, texto):
        import re
        padrao = r"Linha\s+(\d+)"  # Procura "Linha 10"
        m = re.search(padrao, texto)
        if m:
            return int(m.group(1))
        return None

    # Destaca visualmente a linha com erro
    def destacar_linha(self, numero):
        inicio = f"{numero}.0"
        fim = f"{numero}.end"

        try:
            self.texto.tag_add("erro", inicio, fim)
            self.texto.see(inicio)  
        except:
            pass



# Inicializa a janela principal
if __name__ == "__main__":
    root = tk.Tk()
    CompiladorGUI(root)
    root.mainloop()
