
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
import argparse
import sys

def load_results(csv_arg):
    if csv_arg:
        csv_path = Path(csv_arg)
    else:
        script_path = Path(__file__).resolve().parent
        csv_path = script_path / "build" / "Release" / "results.csv"

    if not csv_path.exists():
        raise FileNotFoundError(f"File not found: {csv_path}")

    # Try reading with different encodings, including BOM because sometimes it may not read
    encodings_to_try = ["utf-8-sig", "cp1251", "utf-8"]
    last_exception = None
    for enc in encodings_to_try:
        try:
            df = pd.read_csv(csv_path, encoding=enc)
            return df
        except Exception as e:
            last_exception = e
    raise last_exception

def plot_file_results(df, file_name):
    # Filter by 'File' column
    df_file = df[df["Файл"] == file_name]
    if df_file.empty:
        print(f"No data for file {file_name}")
        return

    # Reading time plot - column 'Чтение(мс)'
    plt.figure()
    for lib in df_file["Библиотека"].unique():
        subset = df_file[df_file["Библиотека"] == lib]
        plt.plot(subset["Запуск"], subset["Чтение(мс)"], marker='o', label=lib)
    plt.title(f"Read time for {file_name}")
    plt.xlabel("Run")
    plt.ylabel("Read (ms)")
    plt.legend()
    plt.grid(True)

    # Write time plot – column 'Запись(мс)'
    plt.figure()
    for lib in df_file["Библиотека"].unique():
        subset = df_file[df_file["Библиотека"] == lib]
        plt.plot(subset["Запуск"], subset["Запись(мс)"], marker='o', label=lib)
    plt.title(f"Write time for {file_name}")
    plt.xlabel("Run")
    plt.ylabel("Write (ms)")
    plt.legend()
    plt.grid(True)

    plt.show()

def main():
    parser = argparse.ArgumentParser(
        description="Visualizer for XML library benchmark results."
    )
    parser.add_argument(
        "-c", "--csv", 
        help="Path to CSV file with results (if not specified, uses build/Release/results.csv)"
    )
    args = parser.parse_args()

    try:
        df = load_results(args.csv)
    except Exception as e:
        print(f"Error loading CSV: {e}")
        sys.exit(1)

    # Check column names
    expected_cols = ["Библиотека", "Файл", "Запуск", "Чтение(мс)", "Запись(мс)"]
    if not all(col in df.columns for col in expected_cols):
        print("Warning: unexpected column names received:")
        print(df.columns.tolist())
        print("Make sure the CSV contains columns:", expected_cols)
        sys.exit(1)

    # Build plots for all unique files
    for fname in df["Файл"].unique():
        plot_file_results(df, fname)

if __name__ == "__main__":
    main()
