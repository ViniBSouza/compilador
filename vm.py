import tkinter as tk
from tkinter import filedialog, scrolledtext, messagebox
import subprocess
import re
import os

CAMINHO_EXECUTAVEL = "./vm.exe"   # ajuste se estiver no Windows (vm.exe)


def selecionar_arquivo():
    caminho = filedialog.askopenfilename(
        title="Selecione o arquivo .obj",
        filetypes=[("Arquivo OBJ", "*.obj")]
    )

    if caminho:
        entrada_arquivo.delete(0, tk.END)
        entrada_arquivo.insert(0, caminho)


def executar_vm():
    caminho = entrada_arquivo.get().strip()

    if not os.path.exists(caminho):
        messagebox.showerror("Erro", "Arquivo não encontrado!")
        return

    try:
        # Executa o programa C com o caminho do arquivo
        processo = subprocess.Popen(
            [CAMINHO_EXECUTAVEL, caminho],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        saida, erro = processo.communicate()

        if erro:
            caixa_saida.insert(tk.END, "ERRO:\n" + erro + "\n")

        # Filtrar apenas o que foi printado pelo PRN
        # O PRN imprime "\n%d\n", então vamos pegar números entre quebras de linha
        linhas = saida.splitlines()
        valores_prn = []

        for linha in linhas:
            linha = linha.strip()
            if re.fullmatch(r"-?\d+", linha):
                valores_prn.append(linha)

        caixa_saida.insert(tk.END, "Saída PRN:\n")
        for v in valores_prn:
            caixa_saida.insert(tk.END, f"{v}\n")

        caixa_saida.insert(tk.END, "\nExecução finalizada.\n\n")

    except Exception as e:
        messagebox.showerror("Erro ao executar", str(e))


# Interface Tkinter
janela = tk.Tk()
janela.title("Executar Máquina Virtual")

frame = tk.Frame(janela)
frame.pack(pady=10)

tk.Label(frame, text="Arquivo OBJ:").grid(row=0, column=0)

entrada_arquivo = tk.Entry(frame, width=50)
entrada_arquivo.grid(row=0, column=1, padx=5)

btn_selecionar = tk.Button(frame, text="Selecionar", command=selecionar_arquivo)
btn_selecionar.grid(row=0, column=2)

btn_executar = tk.Button(janela, text="Executar", command=executar_vm)
btn_executar.pack(pady=10)

caixa_saida = scrolledtext.ScrolledText(janela, width=60, height=20)
caixa_saida.pack()

janela.mainloop()
