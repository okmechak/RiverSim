import math


v = [-1, 0];

alpha = math.acos(v[0]/math.sqrt(v[0]**2+v[1]**2))
if v[1] < 0:
    alpha = -alpha;

print(alpha/math.pi*180)