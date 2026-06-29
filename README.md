# 🏋️‍♂️ Titan Track - CLI Fitness Tracker

Welcome to **Titan Track**, a lightweight, high-performance Command Line Interface (CLI) application built to help athletes log, track, and manage their daily fitness metrics. 

This application was developed as a semester project for the **Object-Oriented Programming (OOP)** course during our **2nd Semester** at **Pak-Austria Fachhochschule: Institute of Applied Sciences and Technology (PAF-IAST)**.

---

## 👥 Project Team
* **Developer**: Zarak Khan Afridi
* **Project Partner**: Amar Sadat
* **Faculty Instructor**: Mr. Aadil Khan

---

## 🚀 Key Features

* **Secure Authentication Layer**: An interactive gatekeeper system featuring athlete registration, login, and strict password complexity validation[cite: 1].
* **Polymorphic Metric Logging**: Dedicated tracking paths for both **Strength Sessions** (Weight, Sets, Reps) and **Cardiovascular Endurance** (Duration in minutes)[cite: 1].
* **Live State Tracking**: Seamlessly view your history and update pending workouts to `Completed` status in real-time[cite: 1].
* **Flat-File Data Persistence**: Auto-sanitizes CSV inputs to safely store profiles and logs locally without database corruption[cite: 1].
* **Automated Unit Tests**: Built-in testing suite to verify architectural logic and data handling out of the box[cite: 2].

---

## 🛠️ OOP Architectural Pillars Showcase

This project serves as a practical deployment of fundamental software engineering and computer science concepts:

* **Encapsulation**: Data fields within classes (like `User` and `Workout`) are protected using private/protected modifiers, accessible only via robust input-validating getters and setters[cite: 1].
* **Inheritance**: The `TitanUser` class extends the base `User` class, cleanly reusing core authentication properties while implementing advanced athlete session routines[cite: 1].
* **Polymorphism**: 
  * *Compile-time (Method Overloading)*: The `logWorkout()` function is overloaded to cleanly digest completely different parameter sets for strength or cardio logic[cite: 1].
  * *Runtime (Virtual Interfaces)*: Utilizes a pure virtual `View` interface to dynamically drive distinct interface layouts (`LoginScreen` and `DashboardScreen`)[cite: 1].
* **Abstraction**: Implements a dedicated `StorageManager` utility layer to isolate complex flat-file string parsing and I/O tasks away from the core UI loops[cite: 1].

---

## 📂 Repository Structure & Deliverables

The project files are structurally organized to separate our source implementation from our formal academic defense assets:

### 💻 Core Source Files (`TitanTrack/`)
* **`main.cpp`** — The standalone system core[cite: 1]. It bundles our validation utilities, the foundational `User` templates, file I/O sanitization algorithms, and the polymorphic screen navigation views that drive the console dashboard interface[cite: 1].
* **`test.cpp`** — The automated test suite engineered for system validation[cite: 2]. It isolates internal function calls and forces execution constraints to systematically assert email structures, password strength conditions, and data output handling[cite: 2].

### 📊 Academic System Defense (`documentation/`)
* **`Titan_Track_Final_Project_Proposal.pdf`** — The definitive technical design manual. This official report documents our complete development roadmap, data schemas, code flowcharts, and engineering rationales justifying why this architecture meets enterprise-level software requirements.
* **`Titan_Track_Complete_Final presentation.pptx`** — Our semester project presentation deck used during the oral evaluation. It features clean slide breakdowns mapping our system requirements, class hierarchy charts, structural designs, and an overview of how we applied OOP pillars to handle persistent user logging.
* **`Titan_Track_video_demonstaration.mp4`** — A complete runtime video walkthrough of the operational ecosystem. This records live system validation, showing account generation, standard dashboard navigation, functional log entries for strength and cardio modules, database file creation, and the immediate execution of our unit test suite.

---

## 💻 Installation & How to Run

### Prerequisites
To build and execute this project locally, you will need a C++ compiler supporting **C++11 or higher** (such as GCC/G++ on Linux/MinGW or MSVC on Windows).

### Full Setup and Execution Sequence
Copy and execute the entire block below in your terminal environment to clone, build, run the core program, and trigger the verification test suite:

```bash
# Clone the repository
git clone https://github.com/sudozeek/TitanTrack.git

# Navigate directly into the root directory
cd TitanTrack

# Compile the core application
g++ -std=c++11 main.cpp -o TitanTrack

# Launch the live CLI fitness ecosystem
./TitanTrack

# Compile the automated test platform
g++ -std=c++11 test.cpp -o TestSuite

# Run structural architecture integrity assertions
./TestSuite
