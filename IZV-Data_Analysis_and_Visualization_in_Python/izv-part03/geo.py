#!/usr/bin/python3.10
# coding=utf-8
import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily as ctx
import sklearn.cluster
import numpy as np

def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """
    Convert DataFrame to GeoDataFrame with correct encoding.
    
    :param df: DataFrame with data about accidents.
    :return: Parsed DataFrame with data about accidents into GeoDataFrame.
    """   
    gdf = df.dropna(subset=["d", "e"])
    gdf = geopandas.GeoDataFrame(gdf, geometry=geopandas.points_from_xy(gdf["d"], gdf["e"]), crs="EPSG:5514")
    return gdf


def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False):
    """
    Plots accidents that involved actors on drugs or alcohol in the Pardubice
    region. The accidents are plotted on a map of the region. Includes 
    years 2018-2021.
    
    :param gdf: GeoDataFrame with data about accidents.
    :param fig_location: Path where should be the output figure saved to.
    :param show_figure: Flag for showing the output figure.
    :return: None
    """
    gdfl = gdf.copy()
    gdfl = gdfl[gdfl["region"] == "PAK"]
    gdfl = gdfl[gdfl["p11"] >= 3]
    gdfl["date"] = pd.to_datetime(gdfl["p2a"], format="%Y-%m-%d")
    gdfl.to_crs(epsg=3857, inplace=True)
    
    fig, axes = plt.subplots(ncols=2, nrows=2, figsize=(16, 12))
    fig.suptitle(
        "Accidents that involved actors on drugs in the Pardubice region."
    )
    for (i, ax) in enumerate(axes.flat):
        ax.set_title(f"PAK, Pardubice region ({2018+i})")
        ax.set_axis_off()
        gdfl[gdfl["date"].dt.year == 2018+i].plot(
            ax=ax,
            markersize=2.5,
            color="red"
        )
        ctx.add_basemap(ax, source=ctx.providers.Stamen.TonerLite, zoom=10)
    
    plt.tight_layout() 
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()
        

def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str = None,
                 show_figure: bool = False):
    """ 
    Plot clustered accidents in the Pardubice region on roads 
    of 1., 2. or 3. class. Clusters are colored according to the number
    of accidents in them.
    
    :param gdf: GeoDataFrame with data about accidents.
    :param fig_location: Path where should be the output figure saved to.
    :param show_figure: Flag for showing the output figure.
    :return: None
    """
    gdfl = gdf.copy()
    gdfl = gdfl[gdfl["region"] == "PAK"]
    gdfl = gdfl[gdfl["p36"].isin([1, 2, 3])]
    gdfl.to_crs(epsg = 3857, inplace = True)
    
    coords = np.dstack([gdfl.geometry.x, gdfl.geometry.y]).reshape(-1, 2)
    clusters = sklearn.cluster.KMeans(n_init=10, n_clusters=21).fit(coords)
    gdfl["cluster"] = clusters.labels_
    gdfl = gdfl.dissolve(
        by="cluster",
        aggfunc={"p1": "count"}
    ).rename(columns={"p1": "count"})
    gdfl.reset_index(inplace=True)
    
    _, ax = plt.subplots(figsize=(10, 10))
    ax.set_title(
        "Clustered accidents that happened in the Pardubice region"
        "on roads of 1., 2. or 3. class."
    )
    ax.set_axis_off()
    gdfl.plot(
        ax=ax,
        column="count",
        markersize=2,
        legend=True,
        legend_kwds={
            "orientation": "horizontal",
            "label": "Number of accidents",
            "pad": 0.02
        }
    )
    ctx.add_basemap(ax, source=ctx.providers.Stamen.TonerLite, zoom=10)
    
    if fig_location:
        plt.savefig(fig_location)
    if show_figure:
        plt.show()
    
    
if __name__ == "__main__":
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", True)
    plot_cluster(gdf, "geo2.png", True)
