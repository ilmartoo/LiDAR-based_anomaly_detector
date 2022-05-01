# LiDAR-based anomaly detector

Using LiDAR data to detect any anomalies an object of a kind may have in comparison to a predefined model.

*...in-depth explanation to come...*

_**Note:**
This is an end-of-degree project made for my Computer 
Science degree at University of Santiago de Compostela._

## Projects

The main project of this repository is the **Anomaly Detector**. This project uses the obtained LiDAR data to characterize objects and detect any anomalies they may have. [[Project README.md](src/anomaly_detector/README.md)]

The **Error Measuring** project's goal is to measure the cuadratic error a LiDAR dataset may have using plane aproximations. [[Project README.md](src/error_measuring/README.md)]

## Compilation

To compile all the projects at once:
```bash
# Build the projects
cmake -S . -B build
cmake --build build
```