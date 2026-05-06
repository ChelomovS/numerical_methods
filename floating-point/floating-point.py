import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

np.random.seed(42)
array_size = 1000

def fast(x):
    n = x.dtype.type(x.size)
    sum1 = np.sum(x, dtype=x.dtype)
    sum2 = np.sum(x ** 2, dtype=x.dtype)
    return (sum2 / n) - (sum1 / n) ** x.dtype.type(2.0)

def twopass(x):
    n = x.dtype.type(x.size)
    mean = np.sum(x, dtype=x.dtype) / n
    return np.sum((x - mean) ** 2, dtype=x.dtype) / n

def onepass(x):
    n = x.dtype.type(x.size)
    M = x[0]
    D = x.dtype.type(0.0)
    for k in range(2, x.size + 1):
        xk = x[k-1]
        delta = xk - M
        M = M + delta / x.dtype.type(k)
        D = D + delta * (xk - M)
    return D / n

configs = [
    {"mean": 1, "scale": 1},
    {"mean": 10, "scale": 0.1},
    {"mean": 100, "scale": 0.01},
    {"mean": 10000, "scale": 0.001}
]

results = []

for i, config in enumerate(configs):
    mean, scale = config["mean"], config["scale"]
    data = np.random.normal(mean, scale, array_size)
    
    data_f32 = data.astype(np.float32)
    data_f64 = data.astype(np.float64)
    
    ref_var = np.var(data_f64, dtype=np.float64)
    
    vars_f32 = {
        "Fast": fast(data_f32),
        "Two pass": twopass(data_f32),
        "One pass": onepass(data_f32)
    }
    
    vars_f64 = {
        "Fast": fast(data_f64),
        "Two pass": twopass(data_f64),
        "One pass": onepass(data_f64)
    }
    
    for method, val in vars_f32.items():
        rel_error = abs(val - ref_var) / ref_var
        results.append({"Dataset": f"M={mean}, S={scale}", "Type": "float32", "Method": method, "Variance": val, "Rel. Error": rel_error})
        
    for method, val in vars_f64.items():
        rel_error = abs(val - ref_var) / ref_var
        results.append({"Dataset": f"M={mean}, S={scale}", "Type": "float64", "Method": method, "Variance": val, "Rel. Error": rel_error})

df = pd.DataFrame(results)

with open('results_table.txt', 'w', encoding='utf-8') as f:
    f.write("Результаты вычислений дисперсии\n")
    f.write("="*80 + "\n")
    f.write(df.to_string(index=False))
    f.write("\n\nПри M=10000 метод Fast на float32 дает критическую ошибку.")

methods = ["Fast", "Two pass", "One pass"]
datasets = [f"μ={c['mean']}, σ={c['scale']}" for c in configs]

fig, axes = plt.subplots(1, 2, figsize=(14, 6), sharey=True)
fig.suptitle('Относительная ошибка вычисления дисперсии (шкала log-координаты)', fontsize=16)

for ax, dtype in zip(axes, ["float32", "float64"]):
    x_pos = np.arange(len(datasets))
    width = 0.25
    
    for i, method in enumerate(methods):
        subset = df[(df["Type"] == dtype) & (df["Method"] == method)]
        errors = subset["Rel. Error"].values + 1e-18
        
        ax.bar(x_pos + (i - 1) * width, errors, width, label=method)
    
    ax.set_title(f"Тип данных: {dtype}")
    ax.set_xticks(x_pos)
    ax.set_xticklabels(datasets)
    ax.set_yscale('log')
    ax.set_ylabel('Относительная ошибка')
    ax.legend()
    ax.grid(True, which="both", ls="--", alpha=0.5)

plt.tight_layout()
plt.savefig('variance_error_comparison.png', dpi=300, bbox_inches='tight')
print("График сохранен в 'variance_error_comparison.png'")

plt.show()