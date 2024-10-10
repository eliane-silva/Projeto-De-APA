import os
from datetime import datetime


def extract_data(file_path):
    with open(file_path, "r", encoding="utf-8") as arq:
        lines = arq.readlines()[3:5]
        lines[0] = lines[0].replace("Sequence: ", "").strip()
        lines[1] = lines[1].replace("Total Penalty: ", "").replace(",", ".").strip()

        sequence = ", ".join([str(int(num) + 1) for num in lines[0].split(" - ")])
        penalty = float(lines[1])
        return (penalty, sequence)


def process_files(folder_path):
    instances = dict()
    for root, _, files in os.walk(folder_path):
        for file in files:
            if file.endswith(".txt"):
                file_path = os.path.join(root, file)
                parent_folder = os.path.basename(root)

                penalty, sequence = extract_data(file_path)

                if (
                    parent_folder not in instances
                    or instances[parent_folder][0] > penalty
                ):
                    instances[parent_folder] = (penalty, sequence)

    for k, v in instances.items():
        print(f"Instancia: {k} - Sequencia:\n{v[1]}\n\n")

folders_path_lucas = r"C:\Users\Lucas\Documents\GitHub\Projeto-De-APA\benchmark"
folders_path_eliane = ""
folders_path_weslley = ""

path_input = folders_path_lucas
process_files(path_input)
