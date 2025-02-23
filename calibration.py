import numpy as np
import matplotlib.pyplot as plt
import os
from sklearn.preprocessing import PolynomialFeatures
from sklearn.pipeline import make_pipeline
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error

# Manually input the calibration data
bh1750_values = np.array([
    2.5, 3.33, 10.38, 17.5, 26.67, 33.33, 39.17, 46.67, 50.83, 60.0, 65.83, 74.17,
    85.0, 90.83, 97.5, 104.17, 110.0, 118.3, 171.33, 198.33, 239.17, 270.83, 305.83, 
    340.0, 441.67, 486.67, 442.5, 430.0, 450.0, 488.0, 525.0, 567.0, 599.0, 625.0, 
    659.0, 697.0, 728.0, 758.0, 789.0, 827.0, 858.0, 895.0, 927.5, 963.0, 1011.0, 
    1128.0, 1140.0, 1310.0, 1382.0
])
spherical_values = np.array([
    10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 
    250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000, 
    1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, 1600, 1700, 
    1800, 1900
])

# Reshape for sklearn
X = bh1750_values.reshape(-1, 1)
y = spherical_values

# Define paths for saving the report and SVG files
report_path = "calibration_report1.txt"
svg_output_dir = "model_graphs"
os.makedirs(svg_output_dir, exist_ok=True)

# Initialize report content
report_content = "LoRa Sensor Calibration Report\n"
report_content += "====================================\n\n"

# Try polynomial degrees from 1 to 5 and store results
best_degree = 1
best_mse = float('inf')
best_model = None

for degree in range(1, 6):
    model = make_pipeline(PolynomialFeatures(degree), LinearRegression())
    model.fit(X, y)
    predicted_values = model.predict(X)
    
    mse = mean_squared_error(y, predicted_values)
    intercept = model.named_steps["linearregression"].intercept_
    coefficients = model.named_steps["linearregression"].coef_

    # Store details for each model
    report_content += f"\nPolynomial Degree: {degree}\n"
    report_content += f"MSE: {mse:.4f}\n"
    report_content += f"Intercept: {intercept:.4f}\n"
    report_content += f"Coefficients: {', '.join(map(str, coefficients))}\n"
    report_content += "------------------------------------\n"

    # Find the best model
    if mse < best_mse:
        best_mse = mse
        best_degree = degree
        best_model = model

    # Generate and save SVG plots
    plt.figure(figsize=(8, 6))
    plt.scatter(bh1750_values, spherical_values, label="Calibration Data", color='blue')
    plt.plot(bh1750_values, predicted_values, color='red', linestyle='dashed', label=f"Polynomial Degree {degree}")
    plt.xlabel("BH1750 Measured (Lux)")
    plt.ylabel("Corrected Lux (Spherical Illuminator)")
    plt.legend()
    plt.title(f"Calibration Curve - Polynomial Degree {degree}")

    # Save the figure in SVG format
    svg_path = os.path.join(svg_output_dir, f"calibration_degree_{degree}.svg")
    plt.savefig(svg_path, format="svg")
    plt.close()

# Store best model summary
report_content += f"\nBest Polynomial Degree: {best_degree} with MSE: {best_mse:.4f}\n"

# Save the report to a text file
with open(report_path, "w") as file:
    file.write(report_content)

print(f"Calibration report saved as {report_path}")
print(f"SVG graphs saved in {svg_output_dir}/")
