#!/usr/bin/python
# coding=utf-8

import operator
import csv
import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import math
import numpy as np
import scipy.optimize as spo
import jinja2
import sys

########################################################################

# ** Probe geometry **
#
# The probe has a standard 5-hole spherical nose probe with a static
# probe added. The raw measurements are as follows:
#     (dp0, dpA, dpB)
# where the pressures are defined as:
#     dp0 = (center hole) - (static)
#     dpA = (lower hole) - (upper hole)
#     dpB = (right hole) - (left hole)

########################################################################

generate_plots = True

########################################################################

raw_pressures = [
    'c', # center hole
    's', # static probe reading
    'u', # up hole
    'd', # down hole
    'l', # left hole
    'r', # right hole
]

def read_data(csv_file_name):
    d = {
        'alpha': np.array([]),
        'beta': np.array([]),
    }
    for p in raw_pressures:
        d[p] = np.array([])
    with open(csv_file_name, 'r') as csvfile:
        for r in csv.reader(csvfile):
            d['alpha'] = np.append(d['alpha'], int(r[0]))
            d['beta'] = np.append(d['beta'], int(r[1]))
            q = float(r[2])
            for i in range(0, len(raw_pressures)):
                d[raw_pressures[i]] = np.append(
                    d[raw_pressures[i]],
                    float(r[i + 3]) / q)
    return d

########################################################################

def tuples(data):
    n = len(data[list(data.keys())[0]])
    for i in range(0, n):
        t = {}
        for k in data:
            t[k] = data[k][i]
        yield t

def filter(data, f):
    r = {}
    for k in data:
        r[k] = np.array([])
    for t in tuples(data):
        if f(t):
            for k in data:
                r[k] = np.append(r[k], t[k])
    return r

def combine(d0, d1):
    k0 = [k for k in d0]
    k1 = [k for k in d1]
    if not k0 == k1: raise 'Cannot merge incompatible colums'    
    return {k: np.append(d0[k], d1[k]) for k in d0}

def distance(x, y):
    return math.sqrt(math.pow(x, 2) + math.pow(y, 2))

def sign(x):
    if x < 0: return -1
    return 1

########################################################################

raw_data = \
    filter(
        combine(
            filter(
                read_data('staticprobehead_20psf.csv'),
                lambda t: not t['beta'] == -24),
            read_data('staticprobehead_5psf.csv')),
        lambda t: distance(t['alpha'], t['beta']) <= 40)

deltas = {
    'alpha': raw_data['alpha'],
    'beta': raw_data['beta'],
    'dp0': raw_data['c'] - raw_data['s'],
    'dpa' : raw_data['d'] - raw_data['u'],
    'dpb': raw_data['r'] - raw_data['l'],
}

ratios = {
    'alpha': raw_data['alpha'],
    'beta': raw_data['beta'],
    'dpa_over_dp0': deltas['dpa'] / deltas['dp0'],
    'dpb_over_dp0': deltas['dpb'] / deltas['dp0'],
    '1_over_dp0': 1 / deltas['dp0'],
}

ratios_pos = {
    'alpha': raw_data['alpha'],
    'beta': np.abs(raw_data['beta']),
    'dpa_over_dp0': ratios['dpa_over_dp0'],
    'dpb_over_dp0': np.array([
        ratios['dpb_over_dp0'][i] * sign(ratios['beta'][i])
        for i in range(0, len(ratios['beta']))
    ]),
    '1_over_dp0': ratios['1_over_dp0'],
}

########################################################################

def plot_alphabeta(data, name, variable):
    if not generate_plots: return
    fig = plt.figure(figsize=(11, 8.5))
    ax = plt.axes(projection='3d')
    ax.set_xlabel('alpha (degrees)')
    ax.set_ylabel('beta (degrees)')
    ax.set_zlabel(variable)
    ax.scatter3D(
        data['alpha'],
        data['beta'],
        data[variable])
    plt.savefig('alpha_beta_to_' + name + '_' + variable + '.png', dpi=300)
    plt.close()

for p in raw_pressures: plot_alphabeta(raw_data, 'raw_data', p)

plot_alphabeta(deltas, 'deltas', 'dp0')
plot_alphabeta(deltas, 'deltas', 'dpa')
plot_alphabeta(deltas, 'deltas', 'dpb')

plot_alphabeta(ratios, 'ratios', '1_over_dp0')
plot_alphabeta(ratios, 'ratios', 'dpa_over_dp0')
plot_alphabeta(ratios, 'ratios', 'dpb_over_dp0')

plot_alphabeta(ratios_pos, 'ratios_pos', '1_over_dp0')
plot_alphabeta(ratios_pos, 'ratios_pos', 'dpa_over_dp0')
plot_alphabeta(ratios_pos, 'ratios_pos', 'dpb_over_dp0')
               
########################################################################

def plot_scatter(data, variable):
    if not generate_plots: return
    fig = plt.figure(figsize=(11, 8.5))
    ax = plt.axes(projection='3d')
    ax.set_xlabel('dpa_over_dp0')
    ax.set_ylabel('dpb_over_dp0')
    ax.set_zlabel(variable)
    ax.scatter3D(
        data['dpa_over_dp0'],
        data['dpb_over_dp0'],
        data[variable])
    plt.savefig('pressure_ratios_to_' + variable + '.png', dpi=300)
    plt.close()
    
plot_scatter(ratios_pos, 'alpha')
plot_scatter(ratios_pos, 'beta')
plot_scatter(ratios_pos, '1_over_dp0')

########################################################################

def make_fit(x, y, z):

    def generic_fit_function(xvalues, a, b, c, d, e, f, g, h, i, j, k, l):
        x = xvalues[0]
        y = xvalues[1]
        return (a +
                b * np.power(x, 1) +
                c * np.power(x, 2) +
                d * np.power(x, 3) +
                e * np.power(y, 1) +
                f * np.power(y, 2) +
                g * np.power(y, 3) +
                h * np.power(x, 1) * np.power(y, 1) +
                i * np.power(x, 2) * np.power(y, 2) +
                j * np.power(x, 3) * np.power(y, 3) +
                k * np.sin(x) +
                l * np.cos(x))

    popt, pcov = spo.curve_fit(
        generic_fit_function,
        [x, y],
        z,
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1])

    def result_function(x, y):
        return generic_fit_function([x, y], *popt)

    return result_function

########################################################################

model = {
    '1_over_dp0': make_fit(
        ratios_pos['dpa_over_dp0'],
        ratios_pos['dpb_over_dp0'],
        ratios_pos['1_over_dp0']),
    'alpha': make_fit(
        ratios_pos['dpa_over_dp0'],
        ratios_pos['dpb_over_dp0'],
        ratios_pos['alpha']),
    'beta': make_fit(
        ratios_pos['dpa_over_dp0'],
        ratios_pos['dpb_over_dp0'],
        ratios_pos['beta'])
}

def plot_curve_fit(data, model, variable):
    if not generate_plots: return
    X, Y = np.meshgrid(
        np.arange(-4, 4, 0.1),
        np.arange( 0, 6, 0.1))
    Z = model[variable](X, Y)
    fig = plt.figure(figsize=(11, 8.5))
    ax = plt.axes(projection='3d')
    ax.set_xlabel('dpa_over_dp0')
    ax.set_ylabel('dpb_over_dp0')
    ax.set_zlabel(variable)
    ax.scatter3D(
        data['dpa_over_dp0'],
        data['dpb_over_dp0'],
        data[variable],
        color='blue')
    ax.plot_surface(X, Y, Z, color='yellow')
    plt.savefig('curve_fit_' + variable + '.png', dpi=300)
    plt.close()
             
plot_curve_fit(ratios_pos, model, '1_over_dp0')
plot_curve_fit(ratios_pos, model, 'alpha')
plot_curve_fit(ratios_pos, model, 'beta')

########################################################################

# Load the calibration data template

def load_template():
    templateLoader = jinja2.FileSystemLoader(searchpath="./")
    templateEnv = jinja2.Environment(loader=templateLoader)
    return templateEnv.get_template("calibration_data.jinja")

########################################################################

# Plot raw calibration data for quality control

def plot_calibration(var_name, nx, ny, linear_data):
    if not generate_plots: return
    xx, yy = np.meshgrid(range(0, nx), range(0, ny))
    f = lambda ix, iy: linear_data[iy + ny * ix]
    zz = np.vectorize(f)(xx, yy)

    title = 'probe_calibration_' + var_name
        
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.set_xlabel('x index')
    ax.set_ylabel('y index')
    ax.set_zlabel(var_name)
    ax.plot_surface(xx, yy, zz, rstride=1, cstride=1)
    fake2Dline = matplotlib.lines.Line2D([0],[0], linestyle="none", c='b', marker = 'o')
    ax.legend([fake2Dline], [title], numpoints = 1)
    plt.savefig(title + '.png', dpi=600)
    plt.close()

########################################################################

# Generate a calibration data file for a given probe design

def generate_file(raw2data):

    data_alpha = []
    data_beta = []
    data_1_over_dp0 = []

    dp_step = 0.1
    
    dp_alpha_min = -4.0
    dp_alpha_max = 4.0
    dp_alpha_zero_offset = 4.0
    n_alpha = int(round((dp_alpha_max - dp_alpha_min) / dp_step) + 1)
    
    dp_beta_min = 0
    dp_beta_max = 4.0
    dp_beta_zero_offset = 0.0
    n_beta = int(round((dp_beta_max - dp_beta_min) / dp_step) + 1)    

    for i in range(0, n_alpha):
        dpa = dp_alpha_min + dp_step * i
        for j in range(0, n_beta):
            dpb = dp_beta_min + dp_step * j
            d = raw2data(dpa, dpb)
            data_alpha.append(d[0]);
            data_beta.append(d[1]);
            data_1_over_dp0.append(d[2])

    plot_calibration('alpha', n_alpha, n_beta, data_alpha)
    plot_calibration('beta', n_alpha, n_beta, data_beta)
    plot_calibration('1_over_dp0', n_alpha, n_beta, data_1_over_dp0)    
            
    outfile = open('probe_calibration.h', 'w')
            
    outfile.write(load_template().render(
        fileprefix = 'probe',
        structs = {
            'alpha': {
                'comment': 'Alpha as a function of (dpa/dp0, dpb/dp0)',
                'x': {
                    'size': n_alpha,
                    'step': dp_step,
                    'zero_offset': dp_alpha_zero_offset,
                },
                'y': {
                    'size': n_beta,
                    'step': dp_step,
                    'zero_offset': dp_beta_zero_offset,
                },
                'data': data_alpha,
            },
            'beta': {
                'comment': 'Beta as a function of (dpa/dp0, dpb/dp0)',
                'x': {
                    'size': n_alpha,
                    'step': dp_step,
                    'zero_offset': dp_alpha_zero_offset,
                },
                'y': {
                    'size': n_beta,
                    'step': dp_step,
                    'zero_offset': dp_beta_zero_offset,
                },
                'data': data_beta,
            },
            'q_over_dp0': {
                'comment': 'q/dp0 as a function of (dpa/dp0, dpb/dp0)',
                'x': {
                    'size': n_alpha,
                    'step': dp_step,
                    'zero_offset': dp_alpha_zero_offset,
                },
                'y': {
                    'size': n_beta,
                    'step': dp_step,
                    'zero_offset': dp_beta_zero_offset,
                },
                'data': data_1_over_dp0,
            },
        }))
    
    outfile.close();

########################################################################

# Generate calibration files for our two known probe designs

def probe_raw2data(dpa_over_dp0, dpb_over_dp0):
    return [
        model['alpha'](dpa_over_dp0, dpb_over_dp0),
        model['beta'](dpa_over_dp0, dpb_over_dp0),
        model['1_over_dp0'](dpa_over_dp0, dpb_over_dp0),
    ]

generate_file(probe_raw2data)
