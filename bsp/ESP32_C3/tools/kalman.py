import re
import matplotlib.pyplot as plt
from collections import defaultdict
import numpy as np

# 原始数据
data = """Step 0: m1x=-0.075, m1y=0.000, m1z=0.000, m2x=0.012, m2y=0.000, m2z=0.000
Step 0: px=0.000, vx=-0.000, f1_ax=-0.004, f2_ax=-0.005
Step 0: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 0: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 1: m1x=0.049, m1y=0.000, m1z=0.000, m2x=0.028, m2y=0.000, m2z=0.000
Step 1: px=0.000, vx=0.000, f1_ax=0.038, f2_ax=-0.005
Step 1: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 1: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 2: m1x=0.255, m1y=0.000, m1z=0.000, m2x=0.147, m2y=0.000, m2z=0.000
Step 2: px=-0.000, vx=0.002, f1_ax=0.179, f2_ax=-0.005
Step 2: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 2: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 3: m1x=0.202, m1y=0.000, m1z=0.000, m2x=0.171, m2y=0.000, m2z=0.000
Step 3: px=0.000, vx=0.003, f1_ax=0.182, f2_ax=-0.005
Step 3: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 3: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 4: m1x=0.401, m1y=0.000, m1z=0.000, m2x=0.308, m2y=0.000, m2z=0.000
Step 4: px=0.000, vx=0.006, f1_ax=0.249, f2_ax=-0.005
Step 4: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 4: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 5: m1x=0.399, m1y=0.000, m1z=0.000, m2x=0.203, m2y=0.000, m2z=0.000
Step 5: px=0.000, vx=0.009, f1_ax=0.252, f2_ax=-0.005
Step 5: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 5: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 6: m1x=0.469, m1y=0.000, m1z=0.000, m2x=0.385, m2y=0.000, m2z=0.000
Step 6: px=0.000, vx=0.012, f1_ax=0.317, f2_ax=-0.005
Step 6: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 6: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 7: m1x=0.374, m1y=0.000, m1z=0.000, m2x=0.221, m2y=0.000, m2z=0.000
Step 7: px=0.000, vx=0.015, f1_ax=0.295, f2_ax=-0.005
Step 7: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 7: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 8: m1x=0.283, m1y=0.000, m1z=0.000, m2x=0.375, m2y=0.000, m2z=0.000
Step 8: px=0.001, vx=0.018, f1_ax=0.321, f2_ax=-0.005
Step 8: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 8: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 9: m1x=0.344, m1y=0.000, m1z=0.000, m2x=0.370, m2y=0.000, m2z=0.000
Step 9: px=0.001, vx=0.022, f1_ax=0.340, f2_ax=-0.005
Step 9: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 9: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 10: m1x=0.475, m1y=0.000, m1z=0.000, m2x=0.256, m2y=0.000, m2z=0.000
Step 10: px=0.001, vx=0.025, f1_ax=0.330, f2_ax=-0.005
Step 10: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 10: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 11: m1x=0.593, m1y=0.000, m1z=0.000, m2x=0.290, m2y=0.000, m2z=0.000
Step 11: px=0.001, vx=0.029, f1_ax=0.346, f2_ax=-0.005
Step 11: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 11: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 12: m1x=0.454, m1y=0.000, m1z=0.000, m2x=0.214, m2y=0.000, m2z=0.000
Step 12: px=0.001, vx=0.032, f1_ax=0.319, f2_ax=-0.005
Step 12: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 12: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 13: m1x=0.410, m1y=0.000, m1z=0.000, m2x=0.161, m2y=0.000, m2z=0.000
Step 13: px=0.002, vx=0.034, f1_ax=0.282, f2_ax=-0.005
Step 13: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 13: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 14: m1x=0.051, m1y=0.000, m1z=0.000, m2x=0.126, m2y=0.000, m2z=0.000
Step 14: px=0.002, vx=0.036, f1_ax=0.213, f2_ax=-0.005
Step 14: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 14: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 15: m1x=0.035, m1y=0.000, m1z=0.000, m2x=-0.139, m2y=0.000, m2z=0.000
Step 15: px=0.002, vx=0.037, f1_ax=0.089, f2_ax=-0.005
Step 15: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 15: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 16: m1x=0.067, m1y=0.000, m1z=0.000, m2x=-0.034, m2y=0.000, m2z=0.000
Step 16: px=0.002, vx=0.036, f1_ax=0.051, f2_ax=-0.005
Step 16: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 16: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 17: m1x=-0.159, m1y=0.000, m1z=0.000, m2x=-0.049, m2y=0.000, m2z=0.000
Step 17: px=0.003, vx=0.036, f1_ax=0.001, f2_ax=-0.005
Step 17: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 17: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 18: m1x=-0.236, m1y=0.000, m1z=0.000, m2x=-0.109, m2y=0.000, m2z=0.000
Step 18: px=0.003, vx=0.034, f1_ax=-0.055, f2_ax=-0.005
Step 18: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 18: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 19: m1x=-0.330, m1y=0.000, m1z=0.000, m2x=-0.175, m2y=0.000, m2z=0.000
Step 19: px=0.003, vx=0.032, f1_ax=-0.118, f2_ax=-0.005
Step 19: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 19: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 20: m1x=-0.463, m1y=0.000, m1z=0.000, m2x=-0.330, m2y=0.000, m2z=0.000
Step 20: px=0.003, vx=0.028, f1_ax=-0.216, f2_ax=-0.005
Step 20: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 20: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 21: m1x=-0.397, m1y=0.000, m1z=0.000, m2x=-0.341, m2y=0.000, m2z=0.000
Step 21: px=0.003, vx=0.024, f1_ax=-0.270, f2_ax=-0.005
Step 21: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 21: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 22: m1x=-0.531, m1y=0.000, m1z=0.000, m2x=-0.338, m2y=0.000, m2z=0.000
Step 22: px=0.003, vx=0.020, f1_ax=-0.316, f2_ax=-0.005
Step 22: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 22: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 23: m1x=-0.549, m1y=0.000, m1z=0.000, m2x=-0.317, m2y=0.000, m2z=0.000
Step 23: px=0.003, vx=0.016, f1_ax=-0.338, f2_ax=-0.005
Step 23: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 23: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 24: m1x=-0.443, m1y=0.000, m1z=0.000, m2x=-0.286, m2y=0.000, m2z=0.000
Step 24: px=0.003, vx=0.013, f1_ax=-0.331, f2_ax=-0.005
Step 24: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 24: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 25: m1x=-0.396, m1y=0.000, m1z=0.000, m2x=-0.243, m2y=0.000, m2z=0.000
Step 25: px=0.004, vx=0.010, f1_ax=-0.309, f2_ax=-0.005
Step 25: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 25: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 26: m1x=-0.336, m1y=0.000, m1z=0.000, m2x=-0.226, m2y=0.000, m2z=0.000
Step 26: px=0.004, vx=0.008, f1_ax=-0.284, f2_ax=-0.005
Step 26: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 26: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 27: m1x=-0.519, m1y=0.000, m1z=0.000, m2x=-0.125, m2y=0.000, m2z=0.000
Step 27: px=0.004, vx=0.006, f1_ax=-0.257, f2_ax=-0.005
Step 27: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 27: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 28: m1x=-0.193, m1y=0.000, m1z=0.000, m2x=-0.172, m2y=0.000, m2z=0.000
Step 28: px=0.004, vx=0.004, f1_ax=-0.223, f2_ax=-0.005
Step 28: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 28: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 29: m1x=-0.325, m1y=0.000, m1z=0.000, m2x=-0.140, m2y=0.000, m2z=0.000
Step 29: px=0.004, vx=0.003, f1_ax=-0.206, f2_ax=-0.005
Step 29: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 29: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 30: m1x=-0.079, m1y=0.000, m1z=0.000, m2x=-0.074, m2y=0.000, m2z=0.000
Step 30: px=0.005, vx=0.002, f1_ax=-0.151, f2_ax=-0.005
Step 30: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 30: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 31: m1x=-0.236, m1y=0.000, m1z=0.000, m2x=-0.004, m2y=0.000, m2z=0.000
Step 31: px=0.005, vx=0.002, f1_ax=-0.113, f2_ax=-0.005
Step 31: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 31: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 32: m1x=0.123, m1y=0.000, m1z=0.000, m2x=0.087, m2y=0.000, m2z=0.000
Step 32: px=0.006, vx=0.005, f1_ax=-0.026, f2_ax=-0.005
Step 32: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 32: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 33: m1x=0.153, m1y=0.000, m1z=0.000, m2x=0.076, m2y=0.000, m2z=0.000
Step 33: px=0.006, vx=0.008, f1_ax=0.025, f2_ax=-0.005
Step 33: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 33: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 34: m1x=0.317, m1y=0.000, m1z=0.000, m2x=0.167, m2y=0.000, m2z=0.000
Step 34: px=0.007, vx=0.012, f1_ax=0.100, f2_ax=-0.005
Step 34: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 34: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 35: m1x=0.342, m1y=0.000, m1z=0.000, m2x=0.303, m2y=0.000, m2z=0.000
Step 35: px=0.008, vx=0.018, f1_ax=0.189, f2_ax=-0.005
Step 35: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 35: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 36: m1x=0.209, m1y=0.000, m1z=0.000, m2x=0.275, m2y=0.000, m2z=0.000
Step 36: px=0.008, vx=0.022, f1_ax=0.219, f2_ax=-0.005
Step 36: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 36: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 37: m1x=0.360, m1y=0.000, m1z=0.000, m2x=0.303, m2y=0.000, m2z=0.000
Step 37: px=0.008, vx=0.026, f1_ax=0.261, f2_ax=-0.005
Step 37: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 37: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 38: m1x=0.352, m1y=0.000, m1z=0.000, m2x=0.232, m2y=0.000, m2z=0.000
Step 38: px=0.009, vx=0.029, f1_ax=0.263, f2_ax=-0.005
Step 38: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 38: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 39: m1x=0.504, m1y=0.000, m1z=0.000, m2x=0.291, m2y=0.000, m2z=0.000
Step 39: px=0.009, vx=0.034, f1_ax=0.298, f2_ax=-0.005
Step 39: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 39: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 40: m1x=0.529, m1y=0.000, m1z=0.000, m2x=0.268, m2y=0.000, m2z=0.000
Step 40: px=0.010, vx=0.038, f1_ax=0.314, f2_ax=-0.005
Step 40: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 40: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 41: m1x=0.434, m1y=0.000, m1z=0.000, m2x=0.315, m2y=0.000, m2z=0.000
Step 41: px=0.010, vx=0.041, f1_ax=0.329, f2_ax=-0.005
Step 41: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 41: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 42: m1x=0.238, m1y=0.000, m1z=0.000, m2x=0.250, m2y=0.000, m2z=0.000
Step 42: px=0.010, vx=0.043, f1_ax=0.298, f2_ax=-0.005
Step 42: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 42: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 43: m1x=0.346, m1y=0.000, m1z=0.000, m2x=0.279, m2y=0.000, m2z=0.000
Step 43: px=0.011, vx=0.046, f1_ax=0.299, f2_ax=-0.005
Step 43: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 43: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 44: m1x=0.268, m1y=0.000, m1z=0.000, m2x=0.182, m2y=0.000, m2z=0.000
Step 44: px=0.011, vx=0.047, f1_ax=0.262, f2_ax=-0.005
Step 44: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 44: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 45: m1x=0.071, m1y=0.000, m1z=0.000, m2x=0.063, m2y=0.000, m2z=0.000
Step 45: px=0.012, vx=0.045, f1_ax=0.184, f2_ax=-0.005
Step 45: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 45: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 46: m1x=0.067, m1y=0.000, m1z=0.000, m2x=0.086, m2y=0.000, m2z=0.000
Step 46: px=0.012, vx=0.045, f1_ax=0.144, f2_ax=-0.005
Step 46: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 46: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 47: m1x=0.031, m1y=0.000, m1z=0.000, m2x=0.090, m2y=0.000, m2z=0.000
Step 47: px=0.013, vx=0.045, f1_ax=0.118, f2_ax=-0.005
Step 47: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 47: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 48: m1x=-0.051, m1y=0.000, m1z=0.000, m2x=-0.057, m2y=0.000, m2z=0.000
Step 48: px=0.013, vx=0.042, f1_ax=0.049, f2_ax=-0.005
Step 48: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 48: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 49: m1x=-0.393, m1y=0.000, m1z=0.000, m2x=-0.079, m2y=0.000, m2z=0.000
Step 49: px=0.014, vx=0.038, f1_ax=-0.033, f2_ax=-0.005
Step 49: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 49: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 50: m1x=-0.208, m1y=0.000, m1z=0.000, m2x=-0.136, m2y=0.000, m2z=0.000
Step 50: px=0.014, vx=0.035, f1_ax=-0.080, f2_ax=-0.005
Step 50: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 50: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 51: m1x=-0.299, m1y=0.000, m1z=0.000, m2x=-0.077, m2y=0.000, m2z=0.000
Step 51: px=0.014, vx=0.033, f1_ax=-0.100, f2_ax=-0.005
Step 51: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 51: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 52: m1x=-0.413, m1y=0.000, m1z=0.000, m2x=-0.283, m2y=0.000, m2z=0.000
Step 52: px=0.015, vx=0.028, f1_ax=-0.186, f2_ax=-0.005
Step 52: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 52: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 53: m1x=-0.376, m1y=0.000, m1z=0.000, m2x=-0.402, m2y=0.000, m2z=0.000
Step 53: px=0.015, vx=0.021, f1_ax=-0.269, f2_ax=-0.005
Step 53: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 53: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 54: m1x=-0.583, m1y=0.000, m1z=0.000, m2x=-0.295, m2y=0.000, m2z=0.000
Step 54: px=0.015, vx=0.016, f1_ax=-0.307, f2_ax=-0.005
Step 54: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 54: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 55: m1x=-0.333, m1y=0.000, m1z=0.000, m2x=-0.300, m2y=0.000, m2z=0.000
Step 55: px=0.015, vx=0.013, f1_ax=-0.306, f2_ax=-0.005
Step 55: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 55: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 56: m1x=-0.551, m1y=0.000, m1z=0.000, m2x=-0.258, m2y=0.000, m2z=0.000
Step 56: px=0.015, vx=0.010, f1_ax=-0.314, f2_ax=-0.005
Step 56: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 56: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 57: m1x=-0.557, m1y=0.000, m1z=0.000, m2x=-0.305, m2y=0.000, m2z=0.000
Step 57: px=0.015, vx=0.006, f1_ax=-0.334, f2_ax=-0.005
Step 57: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 57: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 58: m1x=-0.267, m1y=0.000, m1z=0.000, m2x=-0.119, m2y=0.000, m2z=0.000
Step 58: px=0.016, vx=0.006, f1_ax=-0.260, f2_ax=-0.005
Step 58: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 58: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 59: m1x=-0.210, m1y=0.000, m1z=0.000, m2x=-0.177, m2y=0.000, m2z=0.000
Step 59: px=0.016, vx=0.006, f1_ax=-0.227, f2_ax=-0.005
Step 59: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 59: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 60: m1x=-0.263, m1y=0.000, m1z=0.000, m2x=-0.140, m2y=0.000, m2z=0.000
Step 60: px=0.016, vx=0.005, f1_ax=-0.202, f2_ax=-0.005
Step 60: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 60: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 61: m1x=-0.212, m1y=0.000, m1z=0.000, m2x=-0.067, m2y=0.000, m2z=0.000
Step 61: px=0.016, vx=0.005, f1_ax=-0.160, f2_ax=-0.005
Step 61: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 61: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 62: m1x=-0.131, m1y=0.000, m1z=0.000, m2x=-0.030, m2y=0.000, m2z=0.000
Step 62: px=0.016, vx=0.006, f1_ax=-0.116, f2_ax=-0.005
Step 62: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 62: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 63: m1x=0.025, m1y=0.000, m1z=0.000, m2x=0.018, m2y=0.000, m2z=0.000
Step 63: px=0.016, vx=0.008, f1_ax=-0.059, f2_ax=-0.005
Step 63: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 63: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 64: m1x=-0.082, m1y=0.000, m1z=0.000, m2x=0.122, m2y=0.000, m2z=0.000
Step 64: px=0.016, vx=0.013, f1_ax=-0.004, f2_ax=-0.005
Step 64: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 64: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 65: m1x=0.297, m1y=0.000, m1z=0.000, m2x=0.081, m2y=0.000, m2z=0.000
Step 65: px=0.016, vx=0.020, f1_ax=0.055, f2_ax=-0.005
Step 65: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 65: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 66: m1x=0.277, m1y=0.000, m1z=0.000, m2x=0.106, m2y=0.000, m2z=0.000
Step 66: px=0.016, vx=0.025, f1_ax=0.095, f2_ax=-0.005
Step 66: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 66: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 67: m1x=0.440, m1y=0.000, m1z=0.000, m2x=0.257, m2y=0.000, m2z=0.000
Step 67: px=0.016, vx=0.035, f1_ax=0.182, f2_ax=-0.005
Step 67: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 67: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 68: m1x=0.453, m1y=0.000, m1z=0.000, m2x=0.248, m2y=0.000, m2z=0.000
Step 68: px=0.017, vx=0.043, f1_ax=0.231, f2_ax=-0.005
Step 68: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 68: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 69: m1x=0.603, m1y=0.000, m1z=0.000, m2x=0.269, m2y=0.000, m2z=0.000
Step 69: px=0.017, vx=0.051, f1_ax=0.283, f2_ax=-0.005
Step 69: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 69: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 70: m1x=0.565, m1y=0.000, m1z=0.000, m2x=0.358, m2y=0.000, m2z=0.000
Step 70: px=0.018, vx=0.060, f1_ax=0.336, f2_ax=-0.005
Step 70: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 70: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 71: m1x=0.357, m1y=0.000, m1z=0.000, m2x=0.200, m2y=0.000, m2z=0.000
Step 71: px=0.018, vx=0.059, f1_ax=0.299, f2_ax=-0.005
Step 71: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 71: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 72: m1x=0.414, m1y=0.000, m1z=0.000, m2x=0.281, m2y=0.000, m2z=0.000
Step 72: px=0.019, vx=0.063, f1_ax=0.307, f2_ax=-0.005
Step 72: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 72: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 73: m1x=0.455, m1y=0.000, m1z=0.000, m2x=0.253, m2y=0.000, m2z=0.000
Step 73: px=0.020, vx=0.066, f1_ax=0.307, f2_ax=-0.005
Step 73: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 73: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 74: m1x=0.611, m1y=0.000, m1z=0.000, m2x=0.266, m2y=0.000, m2z=0.000
Step 74: px=0.020, vx=0.071, f1_ax=0.328, f2_ax=-0.005
Step 74: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 74: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 75: m1x=0.034, m1y=0.000, m1z=0.000, m2x=0.205, m2y=0.000, m2z=0.000
Step 75: px=0.021, vx=0.067, f1_ax=0.262, f2_ax=-0.005
Step 75: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 75: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 76: m1x=0.333, m1y=0.000, m1z=0.000, m2x=0.242, m2y=0.000, m2z=0.000
Step 76: px=0.022, vx=0.070, f1_ax=0.265, f2_ax=-0.005
Step 76: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 76: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 77: m1x=0.319, m1y=0.000, m1z=0.000, m2x=0.105, m2y=0.000, m2z=0.000
Step 77: px=0.022, vx=0.068, f1_ax=0.224, f2_ax=-0.005
Step 77: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 77: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 78: m1x=0.111, m1y=0.000, m1z=0.000, m2x=-0.062, m2y=0.000, m2z=0.000
Step 78: px=0.023, vx=0.060, f1_ax=0.127, f2_ax=-0.005
Step 78: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 78: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 79: m1x=-0.130, m1y=0.000, m1z=0.000, m2x=-0.016, m2y=0.000, m2z=0.000
Step 79: px=0.024, vx=0.053, f1_ax=0.059, f2_ax=-0.005
Step 79: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 79: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 80: m1x=0.030, m1y=0.000, m1z=0.000, m2x=-0.108, m2y=0.000, m2z=0.000
Step 80: px=0.024, vx=0.048, f1_ax=0.007, f2_ax=-0.005
Step 80: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 80: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 81: m1x=-0.315, m1y=0.000, m1z=0.000, m2x=-0.098, m2y=0.000, m2z=0.000
Step 81: px=0.025, vx=0.041, f1_ax=-0.056, f2_ax=-0.005
Step 81: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 81: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 82: m1x=-0.145, m1y=0.000, m1z=0.000, m2x=-0.223, m2y=0.000, m2z=0.000
Step 82: px=0.025, vx=0.034, f1_ax=-0.114, f2_ax=-0.005
Step 82: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 82: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 83: m1x=-0.256, m1y=0.000, m1z=0.000, m2x=-0.297, m2y=0.000, m2z=0.000
Step 83: px=0.025, vx=0.025, f1_ax=-0.183, f2_ax=-0.005
Step 83: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 83: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 84: m1x=-0.366, m1y=0.000, m1z=0.000, m2x=-0.231, m2y=0.000, m2z=0.000
Step 84: px=0.026, vx=0.020, f1_ax=-0.214, f2_ax=-0.005
Step 84: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 84: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 85: m1x=-0.556, m1y=0.000, m1z=0.000, m2x=-0.327, m2y=0.000, m2z=0.000
Step 85: px=0.026, vx=0.010, f1_ax=-0.282, f2_ax=-0.005
Step 85: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 85: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 86: m1x=-0.535, m1y=0.000, m1z=0.000, m2x=-0.318, m2y=0.000, m2z=0.000
Step 86: px=0.026, vx=0.004, f1_ax=-0.317, f2_ax=-0.005
Step 86: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 86: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 87: m1x=-0.422, m1y=0.000, m1z=0.000, m2x=-0.294, m2y=0.000, m2z=0.000
Step 87: px=0.026, vx=0.000, f1_ax=-0.319, f2_ax=-0.005
Step 87: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 87: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 88: m1x=-0.463, m1y=0.000, m1z=0.000, m2x=-0.250, m2y=0.000, m2z=0.000
Step 88: px=0.026, vx=-0.002, f1_ax=-0.310, f2_ax=-0.005
Step 88: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 88: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 89: m1x=-0.428, m1y=0.000, m1z=0.000, m2x=-0.226, m2y=0.000, m2z=0.000
Step 89: px=0.026, vx=-0.003, f1_ax=-0.294, f2_ax=-0.005
Step 89: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 89: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 90: m1x=-0.324, m1y=0.000, m1z=0.000, m2x=-0.233, m2y=0.000, m2z=0.000
Step 90: px=0.026, vx=-0.004, f1_ax=-0.277, f2_ax=-0.005
Step 90: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 90: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 91: m1x=-0.287, m1y=0.000, m1z=0.000, m2x=-0.187, m2y=0.000, m2z=0.000
Step 91: px=0.026, vx=-0.004, f1_ax=-0.249, f2_ax=-0.005
Step 91: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 91: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 92: m1x=-0.299, m1y=0.000, m1z=0.000, m2x=-0.106, m2y=0.000, m2z=0.000
Step 92: px=0.026, vx=-0.002, f1_ax=-0.208, f2_ax=-0.005
Step 92: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 92: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 93: m1x=-0.047, m1y=0.000, m1z=0.000, m2x=-0.084, m2y=0.000, m2z=0.000
Step 93: px=0.026, vx=0.002, f1_ax=-0.152, f2_ax=-0.005
Step 93: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 93: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 94: m1x=0.020, m1y=0.000, m1z=0.000, m2x=0.070, m2y=0.000, m2z=0.000
Step 94: px=0.026, vx=0.010, f1_ax=-0.065, f2_ax=-0.005
Step 94: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 94: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 95: m1x=0.244, m1y=0.000, m1z=0.000, m2x=0.058, m2y=0.000, m2z=0.000
Step 95: px=0.026, vx=0.018, f1_ax=0.006, f2_ax=-0.005
Step 95: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 95: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 96: m1x=0.138, m1y=0.000, m1z=0.000, m2x=0.115, m2y=0.000, m2z=0.000
Step 96: px=0.026, vx=0.023, f1_ax=0.054, f2_ax=-0.005
Step 96: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 96: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 97: m1x=0.166, m1y=0.000, m1z=0.000, m2x=0.062, m2y=0.000, m2z=0.000
Step 97: px=0.026, vx=0.025, f1_ax=0.070, f2_ax=-0.005
Step 97: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 97: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 98: m1x=0.445, m1y=0.000, m1z=0.000, m2x=0.230, m2y=0.000, m2z=0.000
Step 98: px=0.027, vx=0.036, f1_ax=0.159, f2_ax=-0.005
Step 98: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 98: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000
Step 99: m1x=0.344, m1y=0.000, m1z=0.000, m2x=0.305, m2y=0.000, m2z=0.000
Step 99: px=0.027, vx=0.045, f1_ax=0.224, f2_ax=-0.005
Step 99: py=0.000, vy=0.000, f1_ay=0.000, f2_ay=0.000
Step 99: pz=0.000, vz=0.000, f1_az=0.000, f2_az=0.000"""

# 解析数据，PV为1可以输出速度位移图像，PV为0则输出所有传感器的数据及其滤波后的结果
PV = 0
if PV == 0:
    steps = defaultdict(dict)
    pattern = re.compile(r"Step (\d+): (.*)")
    for line in data.strip().split('\n'):
        if not line.startswith("Step"):
            continue
        match = pattern.match(line)
        if match:
            step = int(match.group(1))
            entries = match.group(2).split(', ')
            for entry in entries:
                key, value = entry.split('=')
                steps[step][key.strip()] = float(value)

    # 提取变量名
    m_keys = sorted(k for k in set().union(*steps.values()) if k.startswith('m'))
    other_keys = sorted(k for k in set().union(*steps.values()) if not k.startswith('m'))
    # 准备绘图数据
    steps_sorted = sorted(steps.keys())
    m_data = {k: [steps[step].get(k, None) for step in steps_sorted] for k in m_keys}
    other_data = {k: [steps[step].get(k, None) for step in steps_sorted] for k in other_keys}
    # 定义样式
    measured_styles = {'marker': ['o', 's', '^', 'D', 'v', 'p', '*', 'X', 'h', '8'], 'linestyle': '-', 'linewidth': 2}
    filtered_styles = {'marker': ['x', '+', '1', '2', '3', '4', '|', '_', '.', ','], 'linestyle': '--', 'linewidth': 1.5}
    def get_styles(keys, marker_list, linestyle, linewidth):
        return [{'marker': marker_list[i % len(marker_list)], 'linestyle': linestyle, 'linewidth': linewidth}
                for i in range(len(keys))]
    m_styles = get_styles(m_keys, measured_styles['marker'], measured_styles['linestyle'], measured_styles['linewidth'])
    other_styles = get_styles(other_keys, filtered_styles['marker'], filtered_styles['linestyle'], filtered_styles['linewidth'])
    # 颜色（使用色图扩展）
    m_colors = plt.cm.tab20(np.linspace(0, 1, len(m_keys)))  # 20种颜色
    other_colors = plt.cm.tab20b(np.linspace(0, 1, len(other_keys)))  # 另外20种颜色
    # 绘图
    plt.figure(figsize=(15, 8))
    # 测量数据
    for (key, values), style, color in zip(m_data.items(), m_styles, m_colors):
        plt.plot(steps_sorted, values,
                 marker=style['marker'], linestyle=style['linestyle'],
                 linewidth=style['linewidth'], color=color,
                 label=f'Measured {key}')
    # 滤波数据
    for (key, values), style, color in zip(other_data.items(), other_styles, other_colors):
        plt.plot(steps_sorted, values,
                 marker=style['marker'], linestyle=style['linestyle'],
                 linewidth=style['linewidth'], color=color,
                 label=f'Filtered {key}')
    # 图表装饰
    plt.title('Data Comparison: Measurements vs Filtered (Enhanced Styles)')
    plt.xlabel('Step')
    plt.ylabel('Value')
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.grid(True, alpha=0.3)
    plt.tight_layout()


    plt.show()
else:
    steps = defaultdict(dict)
    pattern = re.compile(r"Step (\d+): (.*)")
    for line in data.strip().split('\n'):
        if not line.startswith("Step"):
            continue
        match = pattern.match(line)
        if match:
            step = int(match.group(1))
            entries = match.group(2).split(', ')
            for entry in entries:
                key, value = entry.split('=')
                steps[step][key.strip()] = float(value)
    # 提取 px 和 vx 数据
    steps_sorted = sorted(steps.keys())
    px_values = [steps[step].get('px', None) for step in steps_sorted]
    vx_values = [steps[step].get('vx', None) for step in steps_sorted]
    # 绘图
    plt.figure(figsize=(12, 6))
    # 绘制 px（位置）
    plt.plot(steps_sorted, px_values,
             marker='o', linestyle='-', linewidth=2, color='blue',
             label='Filtered px (Position)')
    # 绘制 vx（速度）
    plt.plot(steps_sorted, vx_values,
             marker='s', linestyle='--', linewidth=2, color='red',
             label='Filtered vx (Velocity)')
    # 图表装饰
    plt.title('Filtered Position (px) and Velocity (vx)')
    plt.xlabel('Step')
    plt.ylabel('Value')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()