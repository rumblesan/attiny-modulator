import matplotlib.pyplot as plt
import numpy as np

xdata = np.asarray(range(0, 1024))

def fake_log(val, brk, mult):
    if val < brk:
        return val
    else:
        return ((val - brk) * mult) + brk

log = [((x * x) >> 10) for x in xdata]
fake_log = [fake_log(fake_log(x, 350, 8), 4000, 4) for x in xdata]
#squared = [ x*x for x in xdata]

fig = plt.figure()
ax = fig.add_subplot()

ax.plot(xdata, xdata, color='tab:blue')
ax.plot(xdata, log, color='tab:red')
ax.plot(xdata, fake_log, color='tab:green')

ax.set_title('envelope curves')

plt.show()
