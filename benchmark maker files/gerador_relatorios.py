import os
import csv
import pandas as pd
import numpy as np
from datetime import datetime


def extract_data(file_path):
    with open(file_path, "r", encoding="utf-8") as arq:
        lines = arq.readlines()
        return lines[:3]


def process_files(folder_path, exit_csv):
    actual_time = datetime.now().strftime("%M_%S")

    exit_csv = exit_csv.replace(".csv", f"-{actual_time}.csv")

    with open(exit_csv, "w", newline="", encoding="utf-8") as csvfile:
        csvwriter = csv.writer(csvfile)

        csvwriter.writerow(
            [
                "Instancia",
                "Total de Execucoes - Construcao",
                "Multa Media - Construcao",
                "Menor Multa - Construcao",
                "Tempo Medio - Construcao",
                "Total de Execucoes - RVND",
                "Multa Media - RVND",
                "Menor Multa - RVND",
                "Tempo Medio - RVND",
                "Multa - Metaheuristica",
                "Tempo - Metaheuristica",
            ]
        )

        for root, _, files in os.walk(folder_path):
            for file in files:
                if file.endswith(".txt"):
                    file_path = os.path.join(root, file)

                    parent_folder = os.path.basename(root)

                    lines = extract_data(file_path)
                    lines = [line.strip() for line in lines]
                    lines = [item for line in lines for item in line.split(",")]

                    csvwriter.writerow([parent_folder] + lines)

    df = pd.read_csv(exit_csv)
    os.remove(exit_csv)
    return df


def process_dataframe(entry_df, exit_csv):
    actual_time = datetime.now().strftime("%Y-%m-%d_%H-%M")
    exit_csv = exit_csv.replace(".csv", f"-{actual_time}.csv")

    columns = [
        "Instancia",
        "Total de Execucoes - Construcao",
        "Multa Media - Construcao",
        "Menor Multa - Construcao",
        "Tempo Medio - Construcao",
        "Total de Execucoes - RVND",
        "Multa Media - RVND",
        "Menor Multa - RVND",
        "Tempo Medio - RVND",
        "Multa - Metaheuristica",
        "Tempo - Metaheuristica",
    ]
    int_columns = [columns[1], columns[5]]
    float_columns = columns[2:5] + columns[6:11]

    for column in int_columns:
        entry_df[column] = entry_df[column].astype(int)

    for column in float_columns:
        entry_df[column] = entry_df[column].astype(float)

    aggregated_total_executions = entry_df.groupby("Instancia").agg(
        {
            "Total de Execucoes - Construcao": "sum",
            "Total de Execucoes - RVND": "sum",
        }
    )

    aggregated_data = entry_df.groupby("Instancia").agg(
        {
            "Multa Media - Construcao": lambda x: (
                x * entry_df.loc[x.index, "Total de Execucoes - Construcao"]
            ).sum(),
            "Menor Multa - Construcao": "min",
            "Tempo Medio - Construcao": lambda x: (
                x * entry_df.loc[x.index, "Total de Execucoes - Construcao"]
            ).sum(),
            "Multa Media - RVND": lambda x: (
                x * entry_df.loc[x.index, "Total de Execucoes - RVND"]
            ).sum(),
            "Menor Multa - RVND": "min",
            "Tempo Medio - RVND": lambda x: (
                x * entry_df.loc[x.index, "Total de Execucoes - RVND"]
            ).sum(),
            "Multa - Metaheuristica": ["mean", "min"],
            "Tempo - Metaheuristica": "mean",
        }
    )

    aggregated_data.columns = [
        "Multa Media - Construcao",
        "Menor Multa - Construcao",
        "Tempo Medio - Construcao",
        "Multa Media - RVND",
        "Menor Multa - RVND",
        "Tempo Medio - RVND",
        "Multa Media - Metaheuristica",
        "Menor Multa - Metaheuristica",
        "Tempo Medio - Metaheuristica",
    ]

    aggregated_data["Multa Media - Construcao"] = (
        aggregated_data["Multa Media - Construcao"]
        / aggregated_total_executions["Total de Execucoes - Construcao"]
    )
    aggregated_data["Tempo Medio - Construcao"] = (
        aggregated_data["Tempo Medio - Construcao"]
        / aggregated_total_executions["Total de Execucoes - Construcao"]
    )
    aggregated_data["Multa Media - RVND"] = (
        aggregated_data["Multa Media - RVND"]
        / aggregated_total_executions["Total de Execucoes - RVND"]
    )
    aggregated_data["Tempo Medio - RVND"] = (
        aggregated_data["Tempo Medio - RVND"]
        / aggregated_total_executions["Total de Execucoes - RVND"]
    )

    optimum_values_df = pd.read_csv(r"benchmark maker files/valores_otimos.csv", sep=";")
    optimum_values_df.set_index("Instancia", inplace=True)

    merged_df = pd.merge(
        aggregated_data, optimum_values_df, on="Instancia", how="inner"
    )

    merged_df["GAP - Construcao"] = np.where(
        (merged_df["Menor Multa - Construcao"] == 0) & (merged_df["Otimo"] == 0),
        0,
        (merged_df["Menor Multa - Construcao"] - merged_df["Otimo"]) / merged_df["Otimo"]
    )

    merged_df["GAP - RVND"] = np.where(
        (merged_df["Menor Multa - RVND"] == 0) & (merged_df["Otimo"] == 0),
        0,
        (merged_df["Menor Multa - RVND"] - merged_df["Otimo"]) / merged_df["Otimo"]
    )

    merged_df["GAP - Metaheuristica"] = np.where(
        (merged_df["Menor Multa - Metaheuristica"] == 0) & (merged_df["Otimo"] == 0),
        0,
        (merged_df["Menor Multa - Metaheuristica"] - merged_df["Otimo"]) / merged_df["Otimo"]
    )

    final_columns = [
        "Otimo",
        "Multa Media - Construcao",
        "Menor Multa - Construcao",
        "Tempo Medio - Construcao",
        "GAP - Construcao",
        "Multa Media - RVND",
        "Menor Multa - RVND",
        "Tempo Medio - RVND",
        "GAP - RVND",
        "Multa Media - Metaheuristica",
        "Menor Multa - Metaheuristica",
        "Tempo Medio - Metaheuristica",
        "GAP - Metaheuristica",
    ]

    final_df = merged_df[final_columns]
    
    final_df["Tempo Medio - Construcao"] *= 1000
    final_df["Tempo Medio - RVND"] *= 1000
    final_df["GAP - Construcao"] *= 100
    final_df["GAP - RVND"] *= 100
    final_df["GAP - Metaheuristica"] *= 100
    
    final_df.rename(columns={
        "Tempo Medio - Construcao": "Tempo Medio (ms) - Construcao",
        "Tempo Medio - RVND": "Tempo Medio (ms) - RVND",
        "Tempo Medio - Metaheuristica": "Tempo Medio (s) - Metaheuristica",
        "GAP - Construcao": "GAP (%) - Construcao",
        "GAP - RVND": "GAP (%) - RVND",
        "GAP - Metaheuristica": "GAP (%) - Metaheuristica",
    }, inplace=True)
    
    final_df.to_csv(exit_csv, index=True, sep=";", decimal=".")
    print(f"Arquivo de saída gerado: {exit_csv}")


folders_path_lucas = r"C:\Users\Lucas\Documents\GitHub\Projeto-De-APA\benchmark\instancias"
folders_path_eliane = ""
folders_path_weslley = ""

path_input = folders_path_lucas
relatorio = process_files(path_input, "informacoes.csv")
process_dataframe(relatorio, "relatorio.csv")
