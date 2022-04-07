"""
Created on Fri Mar 25 2022

@author: Cheng
"""

import numpy as np
import matplotlib.pyplot as plt
import os

os.system("awk '{print $5}' enbs.txt > enbs_tem.txt && sed 's/,/ /g' enbs_tem.txt > enbs_loc.txt")
os.system("awk '{print $5}' ues.txt > ues_tem.txt && sed 's/,/ /g' ues_tem.txt > ues_loc.txt")

enbs = "./enbs_loc.txt"
ues = "./ues_loc.txt"

enb_loc = np.loadtxt(enbs)
ue_loc = np.loadtxt(ues)


# print(enb_loc)
# print(ue_loc)

plt.grid()


if len(ue_loc) == 1:
    plt.plot(enb_loc[0], enb_loc[1], 's', marker="o", markersize=20, markeredgecolor="black", markerfacecolor="orange")
else:
    plt.plot(enb_loc[:,0], enb_loc[:,1], 's', marker="o", markersize=20, markeredgecolor="black", markerfacecolor="orange")

if len(ue_loc) == 1:
    plt.plot(ue_loc[0], ue_loc[1], 's', marker="o", markersize=10, markeredgecolor="black", markerfacecolor="green")
else:
    plt.plot(ue_loc[:,0], ue_loc[:,1], 's', marker="o", markersize=10, markeredgecolor="black", markerfacecolor="green")

plt.savefig("./topo.png",format="png",dpi=500,bbox_inches = 'tight')










