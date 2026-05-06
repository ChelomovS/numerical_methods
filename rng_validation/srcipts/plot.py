import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set_theme(style="whitegrid")

df = pd.read_csv('../build/rng_validation.csv') 

fig, axs = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle('Двухуровневое тестирование генератора случ чисел', fontsize=16)

sns.histplot(data=df, x='Chi2', hue='Generator', kde=True, ax=axs[0, 0], bins=20)
axs[0, 0].set_title('Chi-squared test')
axs[0, 0].set_xlabel('Chi2 statistic')

sns.histplot(data=df, x='KS', hue='Generator', kde=True, ax=axs[0, 1], bins=20)
axs[0, 1].set_title('2. Kolmogorov-Smirnov test')
axs[0, 1].set_xlabel('KS Distance (lower is better)')

sns.histplot(data=df, x='RunsZScore', hue='Generator', kde=True, ax=axs[1, 0], bins=20)
axs[1, 0].set_title('Runs Test (independence / H0A)')
axs[1, 0].set_xlabel('|Z-score| (expected < 3.0)')

sns.histplot(data=df, x='AutoCorrDev', hue='Generator', kde=True, ax=axs[1, 1], bins=20)
axs[1, 1].set_title('Bitwise autocorrelation (H0B)')
axs[1, 1].set_xlabel('Deviation from perfect 0.5 ratio')

plt.tight_layout()
plt.savefig('rng_validation_report.png', dpi=300)
plt.show()