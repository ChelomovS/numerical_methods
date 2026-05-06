import numpy as np
import matplotlib.pyplot as plt

T = 1.0
v0 = 10.0
k = 10.0
theta = 50.0
q = 1.0
delta = 0.0001
M = 1000
N = int(T / delta)

np.random.seed(42)
dt = delta
t_axis = np.linspace(0, T, N + 1)

# dv_t = k * (theta - v_t) * dt + q * dW_t
v = np.zeros((M, N + 1))
v[:, 0] = v0

for t in range(1, N + 1):
    dW = np.random.normal(0, np.sqrt(dt), M)
    # итерация процесса Орнштейна Уленбека
    v[:, t] = v[:, t-1] + k * (theta - v[:, t-1]) * dt + q * dW

plt.figure(figsize=(12, 7))

plt.plot(t_axis, v.T, color='darkblue', alpha=0.02)

plt.axhline(theta, color='red', linestyle='--', label=f'Уровень theta ({theta})')

plt.title(f"1000 траекторий процесса Орнштейна-Уленбека\n(T={T}, k={k}, theta={theta}, delta={delta})", fontsize=14)
plt.xlabel("Время", fontsize=12)
plt.ylabel("Значение процесса v(t)", fontsize=12)
plt.legend()
plt.grid(True, alpha=0.3)

plt.savefig("ou_process_trajectories.png", dpi=150)
plt.show()