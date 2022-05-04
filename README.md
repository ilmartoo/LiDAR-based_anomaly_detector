# LiDAR-based anomaly detector

Using LiDAR data to detect any anomalies an object of a kind may have in comparison to a predefined model.

This is an end-of-degree project made for my Computer Science degree at University of Santiago de Compostela.

---

## Projects

The main project of this repository is the **Anomaly Detector**. This project uses the obtained LiDAR data to characterize objects and detect any anomalies they may have. [[Project README.md](anomaly_detector/src/README.md)]

The **Error Measuring** project's goal is to measure the cuadratic error a LiDAR dataset may have using plane aproximations. [[Project README.md](error_measuring/src/README.md)]

---

## Compilation

To compile all the projects at once:

```bash
cmake -S . -B build
cmake --build build
```

This will create the projects in their corresponding `build/` subdirectory, both ready to be executed.
