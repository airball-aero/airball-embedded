#!/usr/bin/python
# coding=utf-8

import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import math
import numpy as np
import scipy.optimize as spo
import jinja2

########################################################################

# This is the core formula that computes the pressure at a given point
# on a sphere as a function of angular offset from the stagnation point.
# The result is a ratio of the stagnation pressure _q_. The angle is
# given in radians.

def sphere_pressure_coefficient_polar(angle):
    def cos_sq(x):
        y = math.cos(x)
        return y * y
    return 1.0 - 9.0 / 4.0 * cos_sq(angle - math.pi / 2.0);

# This formula takes two angles, alpha and beta, and computes the
# pressure coefficient at the given point using the distance formula.

def sphere_pressure_coefficient_cartesian(alpha, beta):
    return sphere_pressure_coefficient_polar(
        math.sqrt(alpha * alpha + beta * beta))

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
#     dp0 = (center hole) - (bottom 90 degree hole)
#     dpA = (lower hole) - (upper hole)
#     dpB = (right hole) - (left hole)

########################################################################

# The following functions takes a triple of airflow parameters
#     (alpha, beta, q)
# where alpha and beta are in radians, and q can be in any units but,
# for the purposes of this module, we always use pascals. They return
# the expected "raw" measurements from potential flow theory.

# Probe v1 version

def v1_probe_data2raw(alpha, beta, q):
    p0 = q * sphere_pressure_coefficient_cartesian(
        alpha,
        beta)
    pUpr = q * sphere_pressure_coefficient_cartesian(
        alpha - math.pi / 4,
        beta)
    pLwr = q * sphere_pressure_coefficient_cartesian(
        alpha + math.pi / 4,
        beta)
    pLft = q * sphere_pressure_coefficient_cartesian(
        alpha,
        beta - math.pi / 4)
    pRgt = q * sphere_pressure_coefficient_cartesian(
        alpha,
        beta + math.pi / 4)
    return [p0, pLwr - pUpr, pRgt - pLft]

# Probe v2 version

def v2_probe_data2raw(alpha, beta, q):
    p0 = q * sphere_pressure_coefficient_cartesian(
        alpha,
        beta)
    pUpr = q * sphere_pressure_coefficient_cartesian(
        alpha - math.pi / 4,
        beta)
    pLwr = q * sphere_pressure_coefficient_cartesian(
        alpha + math.pi / 4,
        beta)
    pBtm = q * sphere_pressure_coefficient_cartesian(
        alpha + math.pi / 2,
        beta)
    pLft = q * sphere_pressure_coefficient_cartesian(
        alpha,
        beta - math.pi / 4)
    pRgt = q * sphere_pressure_coefficient_cartesian(
        alpha,
        beta + math.pi / 4)
    return [p0 - pBtm, pLwr - pUpr, pRgt - pLft]

########################################################################

# The following functions take a triple of raw pressure measurements
#     (dp0, dpa, dpb)
# where these pressures can be in any units but, for the purposes of
# this module, we always use pascals. They return the airdata parameters
#     (alpha, beta, q)
# from potential flow theory.

# Generic case version

def generic_probe_raw2data(dp0, dpa, dpb, data2raw):

    ra0 = dpa / dp0
    rb0 = dpb / dp0

    def to_optimize(angles):
        [dp0, dpa, dpb] = data2raw(angles[0], angles[1], 1.0)
        return [dpa / dp0 - ra0, dpb / dp0 - rb0]

    [alpha, beta] = spo.root(to_optimize, [0, 0]).x
    dp0_over_q = data2raw(alpha, beta, 1.0)[0]
    q = dp0 / dp0_over_q

    return [alpha, beta, q]

# Probe v1 version

def v1_probe_raw2data(dp0, dpa, dpb):
    return generic_probe_raw2data(dp0, dpa, dpb, v1_probe_data2raw)
    
# Probe v2 version

def v2_probe_raw2data(dp0, dpa, dpb):
    return generic_probe_raw2data(dp0, dpa, dpb, v2_probe_data2raw)

########################################################################

# Load the calibration data template

def load_template():
    templateLoader = jinja2.FileSystemLoader(searchpath="./")
    templateEnv = jinja2.Environment(loader=templateLoader)
    return templateEnv.get_template("calibration_data.jinja")

########################################################################

# Plot raw calibration data for quality control

def plot_calibration(file_prefix, var_name, nx, ny, linear_data):
    xx = range(0, nx)
    yy = range(0, ny)
    xx, yy = np.meshgrid(xx, yy)

    gridded_data = []
    for i in range(0, ny):
        x_row = []
        for j in range(0, nx):
            x_row.append(linear_data[i + ny * j])
        gridded_data.append(x_row)

    title = file_prefix + "_" + var_name
        
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.set_xlabel('x index')
    ax.set_ylabel('y index')
    ax.set_zlabel(var_name)
    ax.plot_surface(
        xx,
        yy,
        gridded_data,
        rstride=1, cstride=1)
    fake2Dline = matplotlib.lines.Line2D([0],[0], linestyle="none", c='b', marker = 'o')
    ax.legend([fake2Dline], [title], numpoints = 1)
    plt.savefig(title + ".png", dpi=600)

########################################################################

# Generate a calibration data file for a given probe design

dp_range = 1.40   # The absolute maximum for (dpx/dp0) we expect
dp_step  = 0.05   # The step of (dpx/dp0) to generate precise data points

def generate_file(fileprefix, raw2data):
    
    data_alpha = []
    data_beta = []
    data_q_over_dp0 = []

    dp_alpha_min = -dp_range
    dp_alpha_max = dp_range
    dp_alpha_zero_offset = dp_range
    n_alpha = int(round((dp_alpha_max - dp_alpha_min) / dp_step) + 1)
    
    dp_beta_min = 0.0
    dp_beta_max = dp_range
    dp_beta_zero_offset = 0.0
    n_beta = int(round((dp_beta_max - dp_beta_min) / dp_step) + 1)    

    for i in range(0, n_alpha):
        dpa = dp_alpha_min + dp_step * i
        for j in range(0, n_beta):
            dpb = dp_beta_min + dp_step * j
            [alpha, beta, q_over_dp0] = raw2data(1, dpa, dpb)
            data_alpha.append(math.degrees(alpha));
            data_beta.append(math.degrees(beta));
            data_q_over_dp0.append(q_over_dp0)

    plot_calibration(fileprefix, "alpha", n_alpha, n_beta, data_alpha)
    plot_calibration(fileprefix, "beta", n_alpha, n_beta, data_beta)
    plot_calibration(fileprefix, "q_over_dp0", n_alpha, n_beta, data_q_over_dp0)    
            
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

generate_file("v1_probe", v1_probe_raw2data)
generate_file("v2_probe", v2_probe_raw2data)
