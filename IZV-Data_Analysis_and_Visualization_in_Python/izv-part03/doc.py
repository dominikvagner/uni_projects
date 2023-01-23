#!/usr/bin/python3.10
# coding=utf-8

"""
   ___ _   __
  / _ \ | / /
 / // / |/ /
/____/|___/

:file: doc.py
:author: Dominik Vagner, xvagne10
:brief: Code for the third IZV project, accident data analysis and visualization, extract information from the data for creating a report.
:date: 2022-01-15
:version: 1.0
:dependencies: pandas, matplotlib
"""

import pandas as pd
import matplotlib.pyplot as plt


def prep_data(df: pd.DataFrame) -> pd.DataFrame:
    """
    Prepares the data for further analysis and visualization.

    :param df: DataFrame with the accident data.
    :return: DataFrame with the accident data prepared for further analysis and visualization.
    """
    rename_columns = {
        "p12": "Cause of accident",
        "p13a": "Killed",
        "p13b": "Badly injured",
        "p13c": "Slightly injured",
    }
    dfl = df.copy()
    dfl = dfl[['p12', 'p13a', 'p13b', 'p13c']]
    dfl = dfl[dfl["p12"].between(201, 209)]
    dfl.rename(columns=rename_columns, inplace=True)
    dfl["Cause of accident"] = dfl["Cause of accident"].map({
        201: "Failure to adapt speed to road traffic",
        202: "Failure to adapt speed to visibility",
        203: "Failure to adapt speed to vehicla capabilities",
        204: "Failure to adapt speed to road conditions",
        205: "Failure to adapt speed to road technical conditions",
        206: "Going faster than the rules allow",
        207: "Going faster than the signs allow",
        208: "Failure to adapt speed to side wind",
        209: "Other kind of speeding"
    })
    return dfl
    

def print_values(df: pd.DataFrame, total_accidents: int, start_date: str, end_date: str):
    """
    Extracts and print out interesting values from the DataFrame.

    :param df: DataFrame with the accident data.
    :return: None
    """
    dfl = df.copy()
    print(" Values ".center(99, "*"))
    print("Collected data time period: ", start_date, "-", end_date)
    print("Number of accidents: {}".format(total_accidents))
    print("Number of accidents caused by speed mismatch: {}".format(dfl.shape[0]))
    print("Percentage of accidents caused by speed mismatch: {:.2f}%".format(dfl.shape[0] / total_accidents * 100))
    deadly = dfl[dfl["Killed"] > 0]
    hard_injury = dfl[dfl["Badly injured"] > 0]
    light_injury = dfl[dfl["Slightly injured"] > 0]
    print("Number of killed: {}".format(deadly["Killed"].sum()))
    print("Number of injured: {}".format(hard_injury["Badly injured"].sum() + light_injury["Slightly injured"].sum()), end="\n\n")


def print_table(df: pd.DataFrame):
    """
    Prints table with the number of accidents, deaths and injuries caused by each kind of speeding.

    :param df: DataFrame with the accident data.
    :return: None
    """
    dfl = df.copy()
    print(" Table ".center(99, "*"))
    dfl1 = dfl.groupby("Cause of accident").agg({"Killed": "sum", "Badly injured": "sum", "Slightly injured": "sum"}).reset_index()
    dfl2 = dfl.groupby("Cause of accident").size().reset_index(name="Count")
    dfl = pd.concat([dfl2["Count"], dfl1], axis=1).sort_values(by="Count", ascending=False)
    dfl = dfl[["Cause of accident", "Count", "Killed", "Badly injured", "Slightly injured"]]
    print(dfl.to_string(index=False), end="\n")


def plot_pie(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    Plot pie chart with the percentage of accidents that led to death or injury.
    
    :param df: DataFrame with the accident data.
    :param fig_location: Path where should be the output figure saved to.
    :param show_figure: Flag for showing the output figure.
    :return: None
    """
    dfl = df.copy()
    no_injury_percentage = dfl[(dfl["Killed"] == 0) & (dfl["Badly injured"] == 0) & (dfl["Slightly injured"] == 0)].shape[0] / dfl.shape[0] * 100
    deadly_percentage = dfl[dfl["Killed"] > 0].shape[0] / dfl.shape[0] * 100
    badly_injured_percentage = dfl[dfl["Badly injured"] > 0].shape[0] / dfl.shape[0] * 100
    slightly_injured_percentage = dfl[dfl["Slightly injured"] > 0].shape[0] / dfl.shape[0] * 100
    labels = ["No injury", "Deadly", "Badly injured", "Slightly injured"]
    
    plt.pie(
        [no_injury_percentage, deadly_percentage, badly_injured_percentage, slightly_injured_percentage],
        labels=labels, 
        autopct="%1.1f%%"
    )
    
    plt.tight_layout() 
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()
        

if __name__ == "__main__":
    df_loaded = pd.read_pickle("accidents.pkl.gz")
    df = prep_data(df_loaded)
    print_values(df, df_loaded.shape[0], df_loaded["p2a"].min(), df_loaded["p2a"].max())
    print_table(df)
    plot_pie(df, fig_location="doc-graph.png", show_figure=True)
