#!/usr/bin/env python3
"""
IZV cast1 projektu
Autor: Dominik Vágner, xvagne10

Detailni zadani projektu je v samostatnem projektu e-learningu.
Nezapomente na to, ze python soubory maji dane formatovani.

Muzete pouzit libovolnou vestavenou knihovnu a knihovny predstavene na prednasce
"""


from bs4 import BeautifulSoup
import requests
import numpy as np
import matplotlib.pyplot as plt
from typing import List


def integrate(x: np.array, y: np.array) -> float:
    """
    Calculate an integral between two NumPy arrays where one represents
    the points of integration and the other theirs values.

    :param x: NumPy array of integration points.
    :param y: NumPy array with the values of the points from x.
    :return: Integration result.
    """
    return ((x[1:] - x[:-1]) * ((y[:-1] + y[1:]) * 0.5)).sum()


def generate_graph(
    a: List[float], show_figure: bool = False, save_path: str | None = None
):
    r"""
    Generate a matplotlib figure/graph for a predefined function:
    :math:`f_{a}(x) = a \cdot x^2` with the definitional scope of ≤-3, 3>.
    One parameter of the function can be specified with the "a" argument.

    :param a: Values for the "a" param of the predefined math function.
    :param show_figure: Flag for showing the output figure.
    :param save_path: Path where should be the output figure saved to.
    :return: None
    """
    x = np.arange(-3, 3.1, 0.1)
    f = (
        np.resize(np.power(x, 2), (len(a), len(x)))
        * np.array(a)[:, np.newaxis]
    )

    plt.figure(figsize=(10, 5))
    plt.plot(x, f[0], "b", label=r"$y_{1.0}(x)$")
    plt.fill_between(x, f[0], color="b", alpha=0.2, edgecolor=None)
    plt.plot(x, f[1], "orange", label=r"$y_{2.0}(x)$")
    plt.fill_between(x, f[1], color="orange", alpha=0.2, edgecolor=None)
    plt.plot(x, f[2], "g", label=r"$y_{-2.0}(x)$")
    plt.fill_between(x, f[2], color="g", alpha=0.2, edgecolor=None)
    plt.xlabel("x")
    plt.ylabel(r"$f_{a}(x)$")
    plt.yticks(ticks=[-20.0, -15.0, -10.0, -5.0, 0.0, 5.0, 10.0, 15.0, 20.0])
    plt.legend(loc="upper center", bbox_to_anchor=(0.5, 1.15), ncol=3)
    plt.annotate(
        r"$\int_{}^{}f_{1.0}(x)\mathrm{d}x$", xy=(1, 1), xytext=(3, 9)
    )
    plt.annotate(
        r"$\int_{}^{}f_{2.0}(x)\mathrm{d}x$", xy=(1, 1), xytext=(3, 18)
    )
    plt.annotate(
        r"$\int_{}^{}f_{-2.0}(x)\mathrm{d}x$", xy=(1, 1), xytext=(3, -18)
    )
    ax = plt.gca()
    ax.spines["top"].set_bounds(-3, 3.65)
    ax.spines["bottom"].set_bounds(-3, 3.65)
    ax.spines["right"].set_position(("data", 3.65))

    plt.margins(0, 0.1)

    if show_figure:
        plt.show()
    if save_path:
        plt.savefig(save_path)


def generate_sinus(show_figure: bool = False, save_path: str | None = None):
    r"""
    | Generate a matplotlib figure (3 graphs) for a set of predefined
      functions with the definitional scope of ≤0, 100>:
    | :math:`f_1(t)=0.5 \cdot \sin (\frac{1}{50} \pi t),
      f_2(t)=0.25 \cdot \sin (\pi t)`
    | 1. Graph show the first function.
    | 2. Graph shows the second function.
    | 3. Graph shows the function added together with values lower than
      the first one masked in a different color.

    :param show_figure: Flag for showing the output figure.
    :param save_path: Path where should be the output figure saved to.
    :return: None
    """
    t = np.linspace(0, 100, 7500)
    f1 = 0.5 * np.sin(0.02 * np.pi * t)
    f2 = 0.25 * np.sin(np.pi * t)
    f3 = np.add(f1, f2)

    fig = plt.figure(figsize=(8, 12))
    ax1, ax2, ax3 = fig.subplots(3, 1)

    ax1.plot(t, f1, "b")
    ax1.set_xlabel("t")
    ax1.set_ylabel(r"$f_{1}(x)$")
    ax1.set_yticks(ticks=[-0.8, -0.4, 0.0, 0.4, 0.8])
    ax1.margins(0, None)

    ax2.plot(t, f2, "b")
    ax2.set_xlabel("t")
    ax2.set_ylabel(r"$f_{2}(x)$")
    ax2.set_yticks(ticks=[-0.8, -0.4, 0.0, 0.4, 0.8])
    ax2.margins(0, None)

    mask = np.ma.masked_greater(f3, f1)
    ax3.plot(t, f3, "g")
    ax3.plot(t, mask, "r")
    ax3.set_xlabel("t")
    ax3.set_ylabel(r"$f_{1}(x) + f_{2}(x)$")
    ax3.set_yticks(ticks=[-0.8, -0.4, 0.0, 0.4, 0.8])
    ax3.margins(0, None)

    if show_figure:
        plt.show()
    if save_path:
        plt.savefig(save_path, bbox_inches="tight", pad_inches=0.25)


def download_data(url="https://ehw.fit.vutbr.cz/izv/temp.html"):
    """
    Send a get request to a specified url and finds a table in the response
    which it parses into a list of dictionaries.

    :param url: URL for a site where the table should be read from.
    :return: Parsed data into a List[Dict[String, Union[int, np.array]]]
    """
    page = requests.get(url)
    soup = BeautifulSoup(page.text, features="html.parser")
    rows = soup.find("table").find_all("tr")
    rows = [row.text.strip().split("\n\n\n") for row in rows]
    rows = [
        [x.split("\n")[0].strip().replace(",", ".") for x in row]
        for row in rows
    ]
    return [
        {
            "year": int(row[0]),
            "month": int(row[1]),
            "temp": np.array(row[2:], dtype=float),
        }
        for row in rows
    ]


def get_avg_temp(data, year=None, month=None) -> float:
    """
    Calculates an average temp from a list of dictionaries.
    The dictionaries can be filtered by year and/or month.

    :param data: Input data with the temperatures.
    :param year: Year for which we should calculate the average temperature.
    :param month: Month for which we should calculate the average temperature.
    :return: Calculated average temperature.
    """
    if year and month:
        return np.average(
            np.concatenate(
                [
                    x.get("temp")
                    for x in data
                    if x.get("year") == year and x.get("month") == month
                ]
            )
        )
    if year:
        return np.average(
            np.concatenate(
                [x.get("temp") for x in data if x.get("year") == year]
            )
        )
    if month:
        return np.average(
            np.concatenate(
                [x.get("temp") for x in data if x.get("month") == month]
            )
        )
    return np.average(np.concatenate([x.get("temp") for x in data]))
