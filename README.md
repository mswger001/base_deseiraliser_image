

![logo](docs/resources/ARU_logo_rectangle.png)

# M2S2 eCAL Deserializers Docker Image

This Docker image provides a complete environment to build and run the M2S2 eCAL deserializers. It includes ROS 2 Humble, eCAL, the XIMEA SDK, OpenCV, PCL, and all required libraries to deserialize and process measurement data from various M2S2 sensor interfaces.

---

## 🧰 Included Features

* Ubuntu 22.04 base
* ROS 2 Humble ROS-Base
* eCAL communication framework
* OpenCV, PCL, Boost, and other computer vision dependencies
* XIMEA SDK and libraries
* CMake-based build system for all deserializers
* Non-root `rosuser` with sudo access
* Pre-cloned `m2s2_ecal_deserializers` repository

---

## 🧪 Usage Guide

### 🐳 Running the Container

To start the container and mount a local directory (e.g. your eCAL measurements) persistently:

```bash
docker build -t m2s2-deserializer .
```

Run with volume linking:

```bash
docker run -it --rm \
  -v $(pwd)/my_local_data:/home/rosuser/data \
  --name m2s2_container \
  m2s2-deserializer
```

This mounts your local folder `my_local_data` to `/home/rosuser/data` in the container.

---

## 📁 Directory Structure Inside the Container

* `/home/rosuser/m2s2_ecal_deserializers`: Source code and build directory for all deserializers
* `/home/rosuser/data`: Mounted persistent directory for input/output data
* `/usr/include/ximea`: XIMEA SDK headers
* `/usr/lib/libm3api.so`: XIMEA SDK shared library

---

## 🔧 Building Deserializers

Inside the container:

```bash
cd ~/m2s2_ecal_deserializers/build
cmake .. -DM3API_LIBRARY=/usr/lib/libm3api.so -DM3API_INCLUDE_DIR=/usr/include/ximea
make
```

Alternatively, use:

```bash
cmake --build . --target bme280  # Replace with target: bme280, boson, ximea, radar, lidar
```

---

## ▶️ Running Deserializers

> All deserializer binaries are built in `~/m2s2_ecal_deserializers/build/`.

### BME280

```bash
./ecal_sample_bme280 data/my_measurement.hdf5 rt/bme280 data/bme280_output.json
```

### FLIR Boson

```bash
./ecal_sample_boson data/my_measurement.hdf5 rt/thermal_raw data/flir_raw/ data/flir_rgb/
```

### Ximea

```bash
./ecal_sample_ximea data/my_measurement.hdf5 rt/ximea data/cam_context.txt data/ximea_rgb/
```

### Radar

```bash
./ecal_sample_radar data/my_measurement.hdf5 rt/radar data/radar_output.json
```

### Lidar

```bash
./ecal_sample_lidar data/my_measurement.hdf5 rt/lidar data/pointclouds/
```

---

## 💾 Persistent Volume Setup

To persist and share your data with the container:

1. Create a local directory:

   ```bash
   mkdir -p ~/m2s2_data
   ```

2. Start the container with volume link:

   ```bash
   docker run -it --rm \
     -v ~/m2s2_data:/home/rosuser/data \
     m2s2-deserializer
   ```

All input/output files can be placed in `~/m2s2_data` and accessed at `/home/rosuser/data` inside the container.

---

## 🛠️ Notes

* ROS 2 topics in eCAL are often named like `rt/<ros2_topic_name>`.
* Make sure `channel_name` passed to deserializers matches what's recorded in `.hdf5`.
* The XIMEA SDK is pre-installed and available at standard paths.

---

## 📚 References

* [eCAL on GitHub](https://github.com/eclipse-ecal/ecal)
* [ROS 2 Documentation](https://docs.ros.org/en/humble/)
* [XIMEA API Docs](https://www.ximea.com/support/wiki/apis/XiAPI)

---

Let me know if you'd like a `docker-compose.yml` or sample scripts to streamline usage.
