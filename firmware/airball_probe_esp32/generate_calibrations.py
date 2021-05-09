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

# Subsequent functions refer to two probe geometries. These are:
#
# ** Probe v1 **
#
# This is a 5-hole sphere plus a static source. The raw measurements are:
#     (dp0, dpA, dpB)
# where the pressures are defined as:
#     dp0 = (center hole) - (static)
#     dpA = (lower hole) - (upper hole)
#     dpB = (right hole) - (left hole)
#
# ** Probe v2 **
#
# This is a 5-hole probe where the static pressure is inferred from one
# hole on the centerline at 90 degrees from the centerline, on the bottom
# of the spherical nose. The raw measurements are:
#     (dp0, dpA, dpB)
# where the pressures are defined as:
#     dp0 = (center hole) - (upper hole)
#     dpA = (lower hole) - (upper hole)
#     dpB = (right hole) - (left hole)

########################################################################

generate_plots = True

########################################################################

csvfiles = [
    '5psf,smooth,fairing.csv',
    '10psf,smooth,fairing.csv',
    '15psf,smooth,fairing.csv',
    '20psf,smooth,fairing.csv',
]

raw_pressures = ['c', 'b', 'u', 'd', 'l', 'r']

def read_data():
    d = {
        'alpha': np.array([]),
        'beta': np.array([]),
    }
    for p in raw_pressures:
        d[p] = np.array([])
    for f in csvfiles:
        with open(f, 'r') as csvfile:
            for r in csv.reader(csvfile):
                d['alpha'] = np.append(d['alpha'], int(r[0]))
                d['beta'] = np.append(d['beta'], int(r[1]))
                q = float(r[2])
                for i in range(0, len(raw_pressures)):
                    d[raw_pressures[i]] = np.append(
                        d[raw_pressures[i]],
                        float(r[i + 3]) / q)
    return d

raw_data = read_data()

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
        
def distance(x, y):
    return math.sqrt(math.pow(x, 2) + math.pow(y, 2))

def mirror_neg_beta(data):
    data = filter(data, lambda t: t['beta'] >= 0)
    r = {}
    for k in data:
        r[k] = np.array([])
    for t in tuples(data):
        for k in data:
            r[k] = np.append(r[k], t[k])
        if t['beta'] == 0:
            continue
        t['beta'] = -1 * t['beta']
        t['r'] = -1 * t['r']
        t['l'] = -1 * t['l']
        for k in data:
            r[k] = np.append(r[k], t[k])
    return r

raw_data = filter(raw_data, lambda t: t['alpha'] >= -15)
raw_data = filter(raw_data, lambda t: t['alpha'] <= 25)
raw_data = filter(raw_data, lambda t: distance(t['alpha'], t['beta']) <= 45)
raw_data = mirror_neg_beta(raw_data)

########################################################################

def plot_alphabeta(zlabel, data):
    if not generate_plots: return
    fig = plt.figure(figsize=(11, 8.5))
    ax = plt.axes(projection='3d')
    ax.set_xlabel('alpha (degrees)')
    ax.set_ylabel('beta (degrees)')
    ax.set_zlabel(zlabel)
    ax.scatter3D(
        raw_data['alpha'],
        raw_data['beta'],
        data)
    plt.savefig('alpha_beta_to_' + zlabel + '.png', dpi=300)

for p in raw_pressures:
    plot_alphabeta(p + '_over_q', raw_data[p])

dp0 = raw_data['c'] - raw_data['b']
dpa_over_dp0 = (raw_data['d'] - raw_data['u']) / dp0
dpb_over_dp0 = (raw_data['r'] - raw_data['l']) / dp0
q_over_dp0 = 1 / dp0

plot_alphabeta('q_over_dp0', q_over_dp0)
plot_alphabeta('dpa_over_dp0', dpa_over_dp0)
plot_alphabeta('dpb_over_dp0', dpb_over_dp0)
               
########################################################################

def plot_scatter(plot_name, z, zlabel):
    if not generate_plots: return
    fig = plt.figure(figsize=(11, 8.5))
    ax = plt.axes(projection='3d')
    ax.set_xlabel('dpa / dp0')
    ax.set_ylabel('dpb / dp0')
    ax.set_zlabel(zlabel)
    ax.scatter3D(dpa_over_dp0, dpb_over_dp0, z)
    plt.savefig(plot_name + '.png', dpi=300)

plot_scatter('pressure_ratios_to_alpha',
             raw_data['alpha'],
             'alpha (degrees)')

plot_scatter('pressure_ratios_to_beta',
             raw_data['beta'],
             'beta (degrees)')

plot_scatter('pressure_ratios_to_q_over_dp0',
             q_over_dp0,
             'q / dp0')

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

model_q_over_dp0 = make_fit(dpa_over_dp0, dpb_over_dp0, q_over_dp0)
model_alpha = make_fit(dpa_over_dp0, dpb_over_dp0, raw_data['alpha'])
model_beta = make_fit(dpa_over_dp0, dpb_over_dp0, raw_data['beta'])

def plot_data_model(plot_name, z, zlabel, model):
    if not generate_plots: return
    X, Y = np.meshgrid(
        np.arange(-1, 3, 0.1),
        np.arange(-4, 4, 0.1))
    Z = model(X, Y)
    fig = plt.figure(figsize=(11, 8.5))
    ax = plt.axes(projection='3d')
    ax.set_xlabel('dpa / dp0')
    ax.set_ylabel('dpb / dp0')
    ax.set_zlabel(zlabel)
    ax.scatter3D(dpa_over_dp0, dpb_over_dp0, z, color='blue')
    ax.plot_surface(X, Y, Z, color='yellow')
    plt.savefig(plot_name + '.png', dpi=300)
             
plot_data_model('calibration_alpha',
                raw_data['alpha'],
                'alpha',
                model_alpha)

plot_data_model('calibration_beta',
                raw_data['beta'],
                'beta',
                model_beta)

plot_data_model('calibration_q_over_dp0',
                q_over_dp0,
                'q / dp0',
                model_q_over_dp0)

########################################################################

# Load the calibration data template

def load_template():
    templateLoader = jinja2.FileSystemLoader(searchpath="./")
    templateEnv = jinja2.Environment(loader=templateLoader)
    return templateEnv.get_template("calibration_data.jinja")

########################################################################

# Plot raw calibration data for quality control

def plot_calibration(file_prefix, var_name, nx, ny, linear_data):
    if not generate_plots: return
    xx, yy = np.meshgrid(range(0, nx), range(0, ny))
    f = lambda ix, iy: linear_data[iy + ny * ix]
    zz = np.vectorize(f)(xx, yy)

    title = file_prefix + "_" + var_name
        
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.set_xlabel('x index')
    ax.set_ylabel('y index')
    ax.set_zlabel(var_name)
    ax.plot_surface(xx, yy, zz, rstride=1, cstride=1)
    fake2Dline = matplotlib.lines.Line2D([0],[0], linestyle="none", c='b', marker = 'o')
    ax.legend([fake2Dline], [title], numpoints = 1)
    plt.savefig(title + ".png", dpi=600)

########################################################################

# Generate a calibration data file for a given probe design

def generate_file(fileprefix, raw2data):

    data_alpha = []
    data_beta = []
    data_q_over_dp0 = []

    dp_step = 0.1
    
    dp_alpha_min = -1.0
    dp_alpha_max = 3.0
    dp_alpha_zero_offset = 1.0
    n_alpha = int(round((dp_alpha_max - dp_alpha_min) / dp_step) + 1)
    
    dp_beta_min = -4.0
    dp_beta_max = 4.0
    dp_beta_zero_offset = 4.0
    n_beta = int(round((dp_beta_max - dp_beta_min) / dp_step) + 1)    

    for i in range(0, n_alpha):
        dpa = dp_alpha_min + dp_step * i
        for j in range(0, n_beta):
            dpb = dp_beta_min + dp_step * j
            [alpha, beta, q_over_dp0] = raw2data(dpa, dpb)
            data_alpha.append(alpha);
            data_beta.append(beta);
            data_q_over_dp0.append(q_over_dp0)

    plot_calibration(fileprefix, 'alpha', n_alpha, n_beta, data_alpha)
    plot_calibration(fileprefix, 'beta', n_alpha, n_beta, data_beta)
    plot_calibration(fileprefix, 'q_over_dp0', n_alpha, n_beta, data_q_over_dp0)    
            
    outfile = open(fileprefix + '_calibration.h', 'w')
            
    outfile.write(load_template().render(
        fileprefix = fileprefix,
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
                'data': data_q_over_dp0,
            },
        }))
    
    outfile.close();

########################################################################

# Generate calibration files for our two known probe designs

def v2_probe_raw2data(dpa_over_dp0, dpb_over_dp0):
    return [
        model_alpha(dpa_over_dp0, dpb_over_dp0),
        model_beta(dpa_over_dp0, dpb_over_dp0),
        model_q_over_dp0(dpa_over_dp0, dpb_over_dp0),
    ]

generate_file('v2_probe', v2_probe_raw2data)
