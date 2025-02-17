#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Script to visualize differences of results between stored reference results and
# new simulation results.
# 
# Requires matplotlib:
# 
#   Installation on Windows:
#
#      python -m pip install -U pip
#      python -m pip install -U matplotlib
#
#   Installation on Ubuntu:
#
#      sudo apt install python3-matplotlib
#
# License: 
#   GPL License
#
# Authors: 
#   Hauke Hirsch <hauke.hirsch@tu-dresden.de>  (primary author)
#   Andreas Nicolai <andreas.nicolai@gmx.net>  (patches)
#
# Syntax:
# > python compare_results_against_reference_results.py [--path <path/to/testsuite>] [--suffix <suffix>] [--threshold <difference limit>]
#
# Example:
# > python compare_results_against_reference_results.py --path ../../data/tests --suffix VC2019_win64
#

import os
import argparse
import platform
import matplotlib.pyplot as pl
from matplotlib.ticker import ScalarFormatter
import numpy as np

def configCommandLineArguments():
    """
    This method sets the available input parameters and parses them.

    Returns a configured argparse.ArgumentParser object.
    """

    parser = argparse.ArgumentParser("compare_results_against_reference_results.py")
    parser.description = '''
Processes results from the regression test suite and compares new results against stored reference results.
Generates diagrams for differences.
'''

    parser.add_argument('-p', '--path', dest='path', required=False, type=str, 
                            help='Path to FMU test suite root directory. If missing, the current working directory is being used.')
    parser.add_argument('-s', '--suffix', dest='suffix', required=False, type=str, 
                            help='Reference result suffix. If missing, automatically determined based on OS.')
    parser.add_argument('-t', '--threshold', dest='threshold', required=False, type=float, default=1e-4,
                            help="Threshold to accept as 'correct'. If above, a plot will be generated.")
    parser.add_argument('--png', dest='png', required=False, default=False, action='store_true',
                            help='If given, for each plot a png file is being created.')
    return parser.parse_args()



# *** MAIN PROGRAM START ***

args = configCommandLineArguments()

if not args.path:
    args.path = os.getcwd()

if not args.suffix:

    # process all directories under test suite directory
    currentOS = platform.system()
    compilerID = None
    if currentOS   == "Linux" :
        compilerID = "gcc_linux"
    
    elif currentOS == "Windows" :
        compilerID = "VC2019_win64"
    
    elif currentOS == "Darwin" :
        compilerID = "gcc_mac"
    
    args.suffix = compilerID
    
path = args.path
reference_label = args.suffix                          # compare with reference results with this label
# case_filter = ['ControlledPumpTemperatureDifferenceFollowingElement'] # if empty all cases are considered
case_filter = []                                        # if empty all cases are considered
rmse_threshold = args.threshold                         # only cases with rmse above that will be plotted

plots_per_fig = 5                                       # number of plots in one figure


def read_headers(file):
    with open(file) as f:
        line = f.readline()
        headers = line.split('\t')[1:]
    return  headers


def rmse(file1, file2):
    if os.path.exists(file1):
        d1 = np.loadtxt(file1, skiprows=1, delimiter='\t')
        d2 = np.loadtxt(file2, skiprows=1, delimiter='\t')
        if np.size(d1) != np.size(d2):
            raise Exception('Result and reference files have different size!')
        _rmse = np.sqrt( np.sum( (d1 - d2)**2 ) / d1.size )
        print(f'RMSE = {_rmse}')
        return _rmse
    else:
        return 0


def plot_results(file_ref, file_res):
    if os.path.exists(file_ref) and os.path.exists(file_res):
        # read and check
        headers = read_headers(file_ref)
        data_ref = np.loadtxt(file_ref, skiprows=1, delimiter='\t')
        data_res = np.loadtxt(file_res, skiprows=1, delimiter='\t')
        if data_ref.ndim != 2:
            raise Exception('Ref file faulty, error in simulation!')
        elif data_res.ndim != 2:
            raise Exception('Result file faulty, error in simulation!')
        elif data_res.size != data_ref.size:
            raise Exception('Result an reference have different size!')

        # plot
        cols = np.size(data_ref, axis=1) - 1
        if cols > 1:
            num_figs = cols // plots_per_fig
            for num in range(num_figs):
                f, ax = pl.subplots(plots_per_fig, 1, figsize=(10, 10))
                ax[0].set_title(base_name)
                # dummy plots for legend
                ax[0].plot([],[], 'r-', label=reference_label)
                ax[0].plot([],[], 'b--', label='current')
                # actual plots
                for i in range(plots_per_fig):
                    col_index = num * plots_per_fig + i
                    ax[i].plot(data_ref[:, 0], data_ref[:, col_index + 1], 'r-')
                    ax[i].plot(data_res[:, 0], data_res[:, col_index + 1], 'b--')
                    ax[i].text(0.5, 0.5, headers[col_index], transform=ax[i].transAxes, ha='center', va='center', fontsize=8)
                    ticks = ax[i].get_yticks()
                    formatted_ticks = ['{:.1e}'.format(tick) for tick in ticks]
                    ax[i].set_yticklabels(formatted_ticks)
                # some adjustments and legend
                handles, labels = ax[0].get_legend_handles_labels()
                f.legend(handles, labels, loc='lower center', ncol=2, bbox_to_anchor=(0.5, -0.0001))
                f.tight_layout()
                if args.png:
                    pathParts = file_res.split("\\")
                    if len(pathParts)==1:
                        pathParts = file_res.split("/")
                    if len(pathParts) > 2:
                        pngFile = os.path.split(file_res)[0] + "/" + pathParts[-3] + "_" + pathParts[-1] + "-{}.png".format(num+1)
                    else:
                        pngFile = file_res + "-{}.png".format(num+1)
                    print("-> {}".format(pngFile))
                    f.savefig(pngFile)
        else:
            f, ax = pl.subplots(1, 1, figsize=(10, 10))
            ax.set_title(base_name)
            ax.plot(data_ref[:, 0], data_ref[:, 1], 'r-')
            ax.plot(data_res[:, 0], data_res[:, 1], 'b--')
            ax.text(0.5, 0.5, headers[0], transform=ax.transAxes, ha='center', va='center', fontsize=8)
            ticks = ax.get_yticks()
            formatted_ticks = ['{:.1e}'.format(tick) for tick in ticks]
            ax.set_yticklabels(formatted_ticks)
            if args.png:
                pathParts = file_res.split("\\")
                if len(pathParts)==1:
                    pathParts = file_res.split("/")
                    if len(pathParts) > 2:
                        pngFile = os.path.split(file_res)[0] + "/" + pathParts[-3] + "_" + pathParts[-1]
                    else:
                        pngFile = file_res
                print("-> {}".format(pngFile))
                f.savefig(pngFile)


def cpu_time(prj):
    sum_file = os.path.join(prj, 'log', 'summary.txt')
    if not os.path.exists(sum_file):
        return 0
    with open(sum_file) as f:
        l = f.readline()
        return float(l.split('=')[1])


count_faster = 0
count_slower = 0
total_cpu_diff = 0

categories = os.listdir(path)
for cat in categories:

    if cat=='climate' or not os.path.isdir(os.path.join(path, cat)):
        continue

    dir_content = os.listdir(os.path.join(path, cat))
    for f in dir_content:
        base_name, ext = os.path.splitext(f)

        if len(case_filter) > 0 and base_name not in case_filter:
            continue
        if ext != '.nandrad':
            continue

        print('\n' + base_name)

        # we have a result dir ...
        res_file_ref = os.path.join(path, cat, base_name + '.' + reference_label, 'results', 'network_elements.tsv')
        states_file_ref = os.path.join(path, cat, base_name + '.' + reference_label, 'results', 'states.tsv')
        res_file_new = os.path.join(path, cat, base_name, 'results', 'network_elements.tsv')
        states_file_new = os.path.join(path, cat, base_name, 'results', 'states.tsv')

        if not os.path.exists(res_file_ref):
            continue

        rmse_res = rmse(res_file_ref, res_file_new)
        if rmse_res > rmse_threshold:

            plot_results(res_file_ref, res_file_new)

        rmse_states = rmse(states_file_ref, states_file_new)
        if rmse_states > 0.00001:
            print(rmse_states)
            plot_results(states_file_ref, states_file_new)

        cpu_time_ref = cpu_time(os.path.join(path, cat, base_name + '.' + reference_label))
        cpu_time_new = cpu_time(os.path.join(path, cat, base_name))
        cpu_diff = cpu_time_ref - cpu_time_new
        # print(f'sse = {np.sum(sse)}')
        if cpu_time_new < cpu_time_ref:
            print(f'+{cpu_diff} s faster than reference')
            count_faster+=1
        else:
            print(f'{-cpu_diff} s slower than reference')
            count_slower += 1

        total_cpu_diff += cpu_diff

print('\n\n\n--------------------')
print(f'{count_faster} times faster, {count_slower} times slower, {total_cpu_diff} total diff')

pl.show()