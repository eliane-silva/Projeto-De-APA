import os

def criador_pastas(caminho_pasta):
    # Obtém a lista de arquivos .txt na pasta especificada
    arquivos_txt = [arquivo for arquivo in os.listdir(caminho_pasta) if arquivo.endswith('.txt')]

    # Itera sobre cada arquivo .txt
    for arquivo in arquivos_txt:
        # Obtém o nome do arquivo sem a extensão
        nome_arquivo = os.path.splitext(arquivo)[0]
        
        # Cria uma pasta com o nome do arquivo (se não existir)
        caminho_nova_pasta = os.path.join(caminho_pasta, nome_arquivo)
        if not os.path.exists(caminho_nova_pasta):
            os.makedirs(caminho_nova_pasta)
            print(f"Pasta '{nome_arquivo}' criada em {caminho_pasta}.")
        else:
            print(f"A pasta '{nome_arquivo}' já existe em {caminho_pasta}.")

# Exemplo de uso: substitua 'caminho_da_pasta_de_instancias' pelo caminho da pasta desejada
caminho_lucas = r"C:\Users\Lucas\Documents\GitHub\Projeto-De-APA\instancias"
caminho_eliane = r""
caminho_weslley = r""
caminho_da_pasta_de_instancias = caminho_lucas
criador_pastas(caminho_da_pasta_de_instancias)
