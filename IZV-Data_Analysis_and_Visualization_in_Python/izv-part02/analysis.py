#!/usr/bin/env python3.9
# coding=utf-8

"""
IZV cast2 projektu

Autor: Dominik Vágner, xvagne10
"""

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import zipfile
import timeit

# Ukol 1: nacteni dat ze ZIP souboru
def load_data(filename: str) -> pd.DataFrame:
    """
    Loads a dataset about accident statistics from the Czech Police
    into a pandas dataframe.

    :param filename: Filename of main zip file with data.
    :return: Constructed dataframe from CSVs.
    """
    # tyto konstanty nemente, pomuzou vam pri nacitani
    # fmt: off
    headers = [
        "p1", "p36", "p37", "p2a", "weekday(p2a)", "p2b", "p6", "p7", "p8",
        "p9", "p10", "p11", "p12", "p13a", "p13b", "p13c", "p14", "p15",
        "p16", "p17", "p18", "p19", "p20", "p21", "p22", "p23", "p24", "p27",
        "p28", "p34", "p35", "p39", "p44", "p45a", "p47", "p48a", "p49",
        "p50a", "p50b", "p51", "p52", "p53", "p55a", "p57", "p58", "a", "b",
        "d", "e", "f", "g", "h", "i", "j", "k", "l", "n", "o", "p", "q", "r",
        "s", "t", "p5a"
    ]
    # fmt: on
    # def get_dataframe(filename: str, verbose: bool = False) -> pd.DataFrame:
    regions = {
        "PHA": "00",
        "STC": "01",
        "JHC": "02",
        "PLK": "03",
        "ULK": "04",
        "HKK": "05",
        "JHM": "06",
        "MSK": "07",
        "OLK": "14",
        "ZLK": "15",
        "VYS": "16",
        "PAK": "17",
        "LBK": "18",
        "KVK": "19",
    }
    new_df = pd.DataFrame()
    with zipfile.ZipFile(filename) as main_zip:
        for nested_zip in main_zip.namelist():
            with zipfile.ZipFile(main_zip.open(nested_zip)) as nested:
                for f_name in nested.namelist():
                    if f_name == "CHODCI.csv":
                        continue
                    elif f_name[0:2] < "08" or f_name[0:2] > "13":
                        df_nested = pd.read_csv(
                            nested.open(f_name),
                            names=headers,
                            encoding="cp1250",
                            delimiter=";",
                            dtype="string"
                        )
                        df_nested["region"] = [
                            k for k, v in regions.items() if v == f_name[0:2]
                        ][0]
                        new_df = pd.concat(
                            [new_df, df_nested], ignore_index=True
                        )
    return new_df


# Ukol 2: zpracovani dat
def parse_data(df: pd.DataFrame, verbose: bool = False) -> pd.DataFrame:
    """
    Parse loaded dataset (output of `load_data()`) into more appropriate
    data types.

    :param df: Loaded dataset as dataframe.
    :param verbose: Calculate and print a size in MB of the dataframe
    before and after parsing.
    :return: Parsed dataframe.
    """
    # fmt: off
    dtype_dict = {
        "p1": "Int64", "p2b": "Int64", "p13a": "Int64", "p13b": "Int64",
        "p13c": "Int64", "p34": "Int64", "p37": "Int64", "p47": "string",
        "p53": "Int64", "a": "float32", "b": "float32", "d": "float32",
        "e": "float32", "f": "float32", "g": "float32", "h": "string",
        "i": "string", "j": "float32", "l": "string", "n": "string",
        "o": "float32", "r": "float32", "s": "float32",
    }
    # fmt: on
    new_df = df.copy()
    names = new_df.columns.values.tolist()
    names.remove("p2a")
    names.remove("region")
    new_df["Date"] = pd.to_datetime(new_df["p2a"], format="%Y-%m-%d")
    problem_row = new_df.loc[new_df["a"] == "A:"].squeeze()
    problem_row[45:63] = problem_row[45:63].str.replace(
        "[a-zA-Z]:+", "nan", regex=True
    )
    new_df.loc[new_df["a"] == "A:"] = [problem_row]
    for col_name in names:
        col_dtype = dtype_dict.get(col_name) \
            if dtype_dict.get(col_name) else "category"
        if col_dtype == "float32":
            new_df[col_name] = new_df[col_name].str.replace(",", ".")
        new_df[col_name] = new_df[col_name].astype(col_dtype)

    if verbose:
        orig_size = df.memory_usage(index=True, deep=True).sum()
        new_size = new_df.memory_usage(index=True, deep=True).sum()
        print(f"orig_size={orig_size / 1000000} MB")
        print(f"new_size={new_size / 1000000} MB")

    return new_df


# Ukol 3: počty nehod v jednotlivých regionech podle viditelnosti
def plot_visibility(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    Create a graph for selected regions showing a count of accidents
    grouped by time and visibility.

    :param df: Parsed dataset as dataframe.
    :param fig_location: Path where should be the output figure saved to.
    :param show_figure: Flag for showing the output figure.
    :return: None
    """
    visibility_time = {
        "1": "Day", "2": "Day", "3": "Day", "4": "Night",
        "5": "Night", "6": "Night", "7": "Night"
    }
    visibility_quality = {
        "1": "Not impaired", "2": "Impaired", "3": "Impaired",
        "4": "Not impaired", "5": "Impaired", "6": "Impaired",
        "7": "Impaired"
    }
    regions = ["HKK", "PAK", "OLK", "JHM"]
    regions_full = [
        "Královehradecký", "Pardubický", "Olomoucký", "Jihomoravský"
    ]

    dfl = df.copy()
    dfl["Time"] = dfl["p19"].map(visibility_time)
    dfl["Visibility"] = dfl["p19"].map(visibility_quality)
    dfl.rename(columns={"region": "Region"}, inplace=True)
    grouped = dfl.groupby(
        ["Region", "Time", "Visibility"]
    ).size().reset_index(name='Accident count')
    grouped = grouped.loc[grouped["Region"].isin(regions)]

    sns.set_theme()
    g = sns.FacetGrid(
        grouped,
        col="Time",
        row="Visibility",
        margin_titles=True,
        hue="Region",
        hue_order=regions,
        palette="deep",
        ylim=[0, 30000],
        height=3.8
    )
    g.map(sns.barplot, "Region", "Accident count", order=regions)
    g.add_legend()
    g.legend.get_texts()[0].set_text(regions_full[0])
    g.legend.get_texts()[1].set_text(regions_full[1])
    g.legend.get_texts()[2].set_text(regions_full[2])
    g.legend.get_texts()[3].set_text(regions_full[3])
    sns.move_legend(g, "lower center", ncol=4, frameon=True, title="Regions")
    g.fig.subplots_adjust(top=0.875, hspace=0.2, bottom=0.16)
    g.fig.suptitle("Count of accidents grouped by time and visibility")

    if show_figure:
        plt.show()
    if fig_location:
        g.savefig(fig_location)


# Ukol4: druh srážky jedoucích vozidel
def plot_direction(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    Create a graph for selected regions showing a count of accidents per
    month and their collision direction

    :param df: Parsed dataset as dataframe.
    :param fig_location: Path where should be the output figure saved to.
    :param show_figure: Flag for showing the output figure.
    :return: None
    """
    collision_direction = {
        "1": "Rear",
        "2": "Lateral",
        "3": "Lateral",
        "4": "Frontal"
    }
    regions = ["HKK", "PAK", "OLK", "JHM"]
    direction_order = ["Lateral", "Rear", "Frontal"]

    dfl = df.copy()
    dfl = dfl.loc[dfl["p7"].isin(["1", "2", "3", "4"])]
    dfl["Collision direction"] = dfl["p7"].map(collision_direction)
    dfl.rename(columns={"region": "Region"}, inplace=True)
    dfl = dfl.loc[dfl["Region"].isin(regions)]
    dfl = dfl.assign(Month=lambda d: d['Date'].dt.month)
    grouped = dfl.groupby(
        ["Region", "Month", "Collision direction"]
    ).size().reset_index(name='Accident count')

    sns.set_theme()
    g = sns.catplot(
        data=grouped, col="Region", col_wrap=2, x="Month", y="Accident count",
        hue_order=direction_order, hue="Collision direction", kind="bar",
        palette="deep", height=3.5, aspect=1.5, sharex=False, sharey=False
    )
    g.fig.subplots_adjust(top=0.875, hspace=0.45, wspace=0.2, right=0.85)
    g.legend.set(frame_on=True)
    g.fig.suptitle(
        "Count of accidents per month in selected regions and their "
        "collision direction"
    )
    g.set(ylim=(0, 800), xlabel="Month", ylabel="Accident count")

    if show_figure:
        plt.show()
    if fig_location:
        g.savefig(fig_location)


# Ukol 5: Následky v čase
def plot_consequences(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    Create a graph for selected regions showing timeline with a count of
    accidents and their consequences.

    :param df: Parsed dataset as dataframe.
    :param fig_location: Path where should be the output figure saved to.
    :param show_figure: Flag for showing the output figure.
    :return: None
    """
    rename_columns = {
        "region": "Region",
        "p13a": "Killed",
        "p13b": "Badly injured",
        "p13c": "Slightly injured",
    }
    regions = ["HKK", "PAK", "OLK", "JHM"]

    dfl = df.copy()
    dfl = dfl[["Date", "region", "p13a", "p13b", "p13c"]]
    dfl.rename(columns=rename_columns, inplace=True)
    dfl = dfl.loc[dfl["Region"].isin(regions)]
    dfl = pd.pivot_table(
        dfl,
        index=["Region", "Date"],
        values=["Killed", "Badly injured", "Slightly injured"],
        aggfunc=np.sum
    )
    dfl = dfl.groupby([
        dfl.index.get_level_values("Region"),
        dfl.index.get_level_values("Date").year,
        dfl.index.get_level_values("Date").month
    ]).sum()
    dfl = dfl.stack().to_frame()
    dfl.columns = ["Accident count"]
    dfl['Date'] = pd.to_datetime(
        dfl.index.get_level_values(1).astype(str) + "/" +
        dfl.index.get_level_values(2).astype(str) + "/01"
    )

    sns.set_theme()
    g = sns.relplot(
        data=dfl, col=dfl.index.get_level_values("Region"), col_wrap=2,
        kind="line", x=dfl["Date"], y=dfl["Accident count"],
        hue=dfl.index.get_level_values(3), palette="deep", height=3.5,
        aspect=1.5, facet_kws={"sharex": False}
    )
    g.fig.subplots_adjust(top=0.875, hspace=0.3, wspace=0.1, right=0.825)
    g.set(
        ylim=(0, 350), xlabel="",
        xlim=(dfl['Date'].min(), dfl['Date'].max() + pd.DateOffset(months=1))
    )
    g.legend.set(frame_on=True, title="Consequences")
    g.set_xticklabels([
        "01/16", "01/17", "01/18", "01/19", "01/20", "01/21", "01/22"
    ])
    g.fig.suptitle("Count of accidents and their consequences")

    if show_figure:
        plt.show()
    if fig_location:
        g.savefig(fig_location)


if __name__ == "__main__":
    start_time = timeit.default_timer()
    df1 = load_data("data.zip")
    print("Load: ", timeit.default_timer() - start_time)
    df2 = parse_data(df1, True)
    print("Parse: ", timeit.default_timer() - start_time)
    # df2.to_pickle("df2.pkl")
    # df2 = pd.read_pickle("df2.pkl")

    plot_visibility(df2, "01_visibility.png", True)
    plot_direction(df2, "02_direction.png", True)
    plot_consequences(df2, "03_consequences.png", True)
