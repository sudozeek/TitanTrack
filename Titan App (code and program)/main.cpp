#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>

using namespace std;

// ==========================================
// Console Utility Class for Screen/Buffer
// ==========================================
class ConsoleUtils {
public:
    static const string RESET;
    static const string BOLD;
    static const string RED;
    static const string GREEN;
    static const string YELLOW;
    static const string BLUE;
    static const string MAGENTA;
    static const string CYAN;

    static void clearScreen() {
        system("cls");
    }

    static void pauseScreen() {
        cout << "\n" << YELLOW << "Press Enter to continue..." << RESET;
        cin.get();
    }

    static void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    static void printHeader(const string& title) {
        cout << CYAN << BOLD << "========================================\n" << RESET;
        cout << CYAN << BOLD << "         " << title << "\n" << RESET;
        cout << CYAN << BOLD << "========================================\n\n" << RESET;
    }

    static void printFooter() {
        cout << CYAN << BOLD << "========================================\n" << RESET;
    }

    static void printError(const string& err) {
        cout << RED << BOLD << "Error: " << err << RESET << "\n";
    }

    static void printSuccess(const string& msg) {
        cout << GREEN << BOLD << "Success: " << msg << RESET << "\n";
    }
};

// Define ANSI Colors
const string ConsoleUtils::RESET = "\033[0m";
const string ConsoleUtils::BOLD = "\033[1m";
const string ConsoleUtils::RED = "\033[31m";
const string ConsoleUtils::GREEN = "\033[32m";
const string ConsoleUtils::YELLOW = "\033[33m";
const string ConsoleUtils::BLUE = "\033[34m";
const string ConsoleUtils::MAGENTA = "\033[35m";
const string ConsoleUtils::CYAN = "\033[36m";


// ==========================================
// Encapsulated Workout Data Class
// ==========================================
class Workout {
private:
    string userEmail;
    string exerciseName;
    double weight; // -1 if Cardio
    int sets;      // -1 if Cardio
    int reps;      // -1 if Cardio
    int duration;  // -1 if Strength
    string type;   // "Strength" or "Cardio"
    string timestamp;
    string status; // "Pending" or "Completed"

public:
    // Strength Constructor
    Workout(string email, string name, double w, int s, int r, string time, string stat = "Pending")
        : userEmail(email), exerciseName(name), weight(w), sets(s), reps(r), duration(-1), type("Strength"), timestamp(time), status(stat) {}

    // Cardio Constructor
    Workout(string email, string name, int dur, string time, string stat = "Pending")
        : userEmail(email), exerciseName(name), weight(-1.0), sets(-1), reps(-1), duration(dur), type("Cardio"), timestamp(time), status(stat) {}

    // Database Parsing Constructor
    Workout(string email, string name, double w, int s, int r, int dur, string t, string time, string stat)
        : userEmail(email), exerciseName(name), weight(w), sets(s), reps(r), duration(dur), type(t), timestamp(time), status(stat) {}

    // Getters and Setters
    string getUserEmail() const { return userEmail; }
    string getExerciseName() const { return exerciseName; }
    double getWeight() const { return weight; }
    int getSets() const { return sets; }
    int getReps() const { return reps; }
    int getDuration() const { return duration; }
    string getType() const { return type; }
    string getTimestamp() const { return timestamp; }
    string getStatus() const { return status; }
    void setStatus(const string& s) { status = s; }
};


// ==========================================
// Base User Class (Encapsulation & Validation)
// ==========================================
class User {
protected:
    string email;
    string password;

public:
    User(string e, string p) : email(e), password(p) {}
    virtual ~User() {}

    // Getters and Setters
    string getEmail() const { return email; }
    void setEmail(const string& e) { email = e; }

    string getPassword() const { return password; }
    void setPassword(const string& p) { password = p; }

    bool verifyPassword(const string& inputPass) const {
        return password == inputPass;
    }

    // Static Email Validation Checker
    static bool validateEmail(const string& emailStr) {
        size_t atPos = emailStr.find('@');
        size_t dotPos = emailStr.find('.', atPos);
        return (atPos != string::npos && dotPos != string::npos && atPos > 0 && dotPos > atPos + 1 && dotPos < emailStr.length() - 1);
    }

    // Static Password Complexity Validator
    static bool validatePasswordStrength(const string& pass, string& errorMsg) {
        errorMsg = "";
        bool hasUpper = false;
        bool hasLower = false;
        bool hasSpecial = false;

        if (pass.length() < 6) {
            errorMsg += " - Password must be at least 6 characters long.\n";
        }
        for (char c : pass) {
            if (isupper(c)) hasUpper = true;
            else if (islower(c)) hasLower = true;
            else if (!isalnum(c)) hasSpecial = true;
        }

        if (!hasUpper) {
            errorMsg += " - Password must contain at least one uppercase letter (A-Z).\n";
        }
        if (!hasLower) {
            errorMsg += " - Password must contain at least one lowercase letter (a-z).\n";
        }
        if (!hasSpecial) {
            errorMsg += " - Password must contain at least one special character (e.g., !, @, #, $, %, ^, *, &, _, +).\n";
        }

        return errorMsg.empty();
    }
};


// ==========================================
// Abstraction: Storage Manager Interface
// ==========================================
class StorageManager {
private:
    static const string USER_FILE;
    static const string WORKOUT_FILE;

    // Sanitize commas to prevent CSV breakdown
    static string sanitizeCsvField(const string& str) {
        string sanitized = str;
        replace(sanitized.begin(), sanitized.end(), ',', ' ');
        return sanitized;
    }

public:
    static bool ensureFilesExist() {
        // Users DB
        ifstream ufile(USER_FILE);
        if (!ufile.is_open()) {
            ofstream createU(USER_FILE);
            if (!createU.is_open()) return false;
            createU << "Email,Password\n"; // Header row for human readability
            createU.close();
        } else {
            ufile.close();
        }

        // Workouts DB
        ifstream wfile(WORKOUT_FILE);
        if (!wfile.is_open()) {
            ofstream createW(WORKOUT_FILE);
            if (!createW.is_open()) return false;
            createW << "UserEmail,ExerciseName,Weight,Sets,Reps,DurationMinutes,Type,Timestamp,Status\n"; // Header row
            createW.close();
        } else {
            wfile.close();
        }
        return true;
    }

    static bool saveUserToFile(const User& user) {
        if (!ensureFilesExist()) return false;
        ofstream file(USER_FILE, ios::app);
        if (!file.is_open()) return false;

        file << sanitizeCsvField(user.getEmail()) << "," << sanitizeCsvField(user.getPassword()) << "\n";
        file.close();
        return true;
    }

    static bool userExists(const string& email) {
        if (!ensureFilesExist()) return false;
        ifstream file(USER_FILE);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string fileEmail;
            if (getline(ss, fileEmail, ',')) {
                if (fileEmail == "Email") continue; // Skip header row
                if (fileEmail == email) {
                    file.close();
                    return true;
                }
            }
        }
        file.close();
        return false;
    }

    static bool verifyCredentials(const string& email, const string& password, string& outPassword) {
        if (!ensureFilesExist()) return false;
        ifstream file(USER_FILE);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string fileEmail, filePass;
            if (getline(ss, fileEmail, ',') && getline(ss, filePass, ',')) {
                if (fileEmail == "Email") continue; // Skip header row
                if (fileEmail == email && filePass == password) {
                    outPassword = filePass;
                    file.close();
                    return true;
                }
            }
        }
        file.close();
        return false;
    }

    static bool saveWorkoutToFile(const Workout& workout) {
        if (!ensureFilesExist()) return false;
        ofstream file(WORKOUT_FILE, ios::app);
        if (!file.is_open()) return false;

        file << sanitizeCsvField(workout.getUserEmail()) << ","
             << sanitizeCsvField(workout.getExerciseName()) << ","
             << workout.getWeight() << ","
             << workout.getSets() << ","
             << workout.getReps() << ","
             << workout.getDuration() << ","
             << sanitizeCsvField(workout.getType()) << ","
             << sanitizeCsvField(workout.getTimestamp()) << ","
             << sanitizeCsvField(workout.getStatus()) << "\n";
        file.close();
        return true;
    }

    static vector<Workout> fetchUserLogs(const string& email) {
        vector<Workout> logs;
        if (!ensureFilesExist()) return logs;
        ifstream file(WORKOUT_FILE);
        if (!file.is_open()) return logs;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string fileEmail, name, weightStr, setsStr, repsStr, durStr, type, timestamp, status;

            if (getline(ss, fileEmail, ',') &&
                getline(ss, name, ',') &&
                getline(ss, weightStr, ',') &&
                getline(ss, setsStr, ',') &&
                getline(ss, repsStr, ',') &&
                getline(ss, durStr, ',') &&
                getline(ss, type, ',') &&
                getline(ss, timestamp, ',')) {
                
                if (fileEmail == "UserEmail") continue; // Skip header row

                if (!getline(ss, status, ',')) {
                    status = "Pending";
                }
                if (!status.empty() && status.back() == '\n') status.pop_back();
                if (!status.empty() && status.back() == '\r') status.pop_back();

                if (fileEmail == email) {
                    try {
                        double weight = stod(weightStr);
                        int sets = stoi(setsStr);
                        int reps = stoi(repsStr);
                        int duration = stoi(durStr);
                        logs.push_back(Workout(fileEmail, name, weight, sets, reps, duration, type, timestamp, status));
                    } catch (...) {
                        // Safe skip on corrupt parse values
                        continue;
                    }
                }
            }
        }
        file.close();
        return logs;
    }

    static bool updateWorkoutStatus(const string& email, int indexToComplete) {
        if (!ensureFilesExist()) return false;
        
        vector<Workout> allWorkouts;
        ifstream file(WORKOUT_FILE);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string fileEmail, name, weightStr, setsStr, repsStr, durStr, type, timestamp, status;

            if (getline(ss, fileEmail, ',') &&
                getline(ss, name, ',') &&
                getline(ss, weightStr, ',') &&
                getline(ss, setsStr, ',') &&
                getline(ss, repsStr, ',') &&
                getline(ss, durStr, ',') &&
                getline(ss, type, ',') &&
                getline(ss, timestamp, ',')) {
                
                if (fileEmail == "UserEmail") continue; // Skip header row

                if (!getline(ss, status, ',')) {
                    status = "Pending";
                }
                if (!status.empty() && status.back() == '\n') status.pop_back();
                if (!status.empty() && status.back() == '\r') status.pop_back();

                try {
                    double weight = stod(weightStr);
                    int sets = stoi(setsStr);
                    int reps = stoi(repsStr);
                    int duration = stoi(durStr);
                    allWorkouts.push_back(Workout(fileEmail, name, weight, sets, reps, duration, type, timestamp, status));
                } catch (...) {
                    continue;
                }
            }
        }
        file.close();

        // Find the user's N-th workout in allWorkouts
        int userWorkoutCount = 0;
        bool updated = false;
        for (auto& w : allWorkouts) {
            if (w.getUserEmail() == email) {
                if (userWorkoutCount == indexToComplete) {
                    w.setStatus("Completed");
                    updated = true;
                    break;
                }
                userWorkoutCount++;
            }
        }

        if (!updated) return false;

        // Write all workouts back to file
        ofstream outFile(WORKOUT_FILE, ios::trunc);
        if (!outFile.is_open()) return false;

        for (const auto& w : allWorkouts) {
            outFile << sanitizeCsvField(w.getUserEmail()) << ","
                    << sanitizeCsvField(w.getExerciseName()) << ","
                    << w.getWeight() << ","
                    << w.getSets() << ","
                    << w.getReps() << ","
                    << w.getDuration() << ","
                    << sanitizeCsvField(w.getType()) << ","
                    << sanitizeCsvField(w.getTimestamp()) << ","
                    << sanitizeCsvField(w.getStatus()) << "\n";
        }
        outFile.close();
        return true;
    }
};

// Static Constants Initialization
const string StorageManager::USER_FILE = "users.txt";
const string StorageManager::WORKOUT_FILE = "workouts.txt";


// ==========================================
// Derived TitanUser Class (Inheritance)
// ==========================================
class TitanUser : public User {
public:
    TitanUser(string e, string p) : User(e, p) {}

    // Polymorphism: Method Overloading for Strength
    void logWorkout(string name, double weight, int sets, int reps) {
        time_t now = time(0);
        char buf[80];
        // Formatting timestamp safely
        #ifdef _MSC_VER
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        #else
        struct tm* timeinfo = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
        #endif
        string timestamp(buf);

        Workout workoutObj(email, name, weight, sets, reps, timestamp);
        StorageManager::saveWorkoutToFile(workoutObj);
    }

    // Polymorphism: Method Overloading for Cardio
    void logWorkout(string name, int durationMinutes) {
        time_t now = time(0);
        char buf[80];
        #ifdef _MSC_VER
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        #else
        struct tm* timeinfo = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
        #endif
        string timestamp(buf);

        Workout workoutObj(email, name, durationMinutes, timestamp);
        StorageManager::saveWorkoutToFile(workoutObj);
    }
};


// ==========================================
// Polymorphism: Virtual Interface View
// ==========================================
class View {
public:
    virtual ~View() {}
    virtual void displayMenu() = 0; // Pure Virtual Function
};


// ==========================================
// Derived View: Login Screen System
// ==========================================
class LoginScreen : public View {
private:
    TitanUser*& sessionUser;

    void handleRegister() {
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("Titan Track - Registration");

        string emailStr, passStr, confirmPassStr;

        cout << "Enter User Email: ";
        getline(cin, emailStr);

        // Validation Loop for Email format
        if (!User::validateEmail(emailStr)) {
            ConsoleUtils::printError("Email must contain '@' and '.' characters (e.g. user@domain.com).");
            ConsoleUtils::pauseScreen();
            return;
        }

        // Check if user is already registered
        if (StorageManager::userExists(emailStr)) {
            ConsoleUtils::printError("An account with this Email is already registered!");
            ConsoleUtils::pauseScreen();
            return;
        }

        // Rigorous Validation Loop for Password
        while (true) {
            cout << "Enter Password: ";
            getline(cin, passStr);

            cout << "Confirm Password: ";
            getline(cin, confirmPassStr);

            if (passStr != confirmPassStr) {
                ConsoleUtils::printError("Confirm Password must exactly match the initial password input.\n");
                continue;
            }

            string errorMsg;
            if (!User::validatePasswordStrength(passStr, errorMsg)) {
                ConsoleUtils::printError("Password fails cryptographic verification rules:\n" + errorMsg);
                continue;
            }

            break; // Validation passed
        }

        // Save User Data to file
        User newUser(emailStr, passStr);
        if (StorageManager::saveUserToFile(newUser)) {
            ConsoleUtils::printSuccess("Athlete registered successfully! Please log in.");
        } else {
            ConsoleUtils::printError("Fatal: Storage system failed to write credentials database.");
        }
        ConsoleUtils::pauseScreen();
    }

    void handleLogin() {
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("Titan Track - Authenticate");

        string emailStr, passStr;

        cout << "Enter Registered Email: ";
        getline(cin, emailStr);

        cout << "Enter Password: ";
        getline(cin, passStr);

        string verifiedPassword;
        if (StorageManager::verifyCredentials(emailStr, passStr, verifiedPassword)) {
            // Success: Open Session state
            sessionUser = new TitanUser(emailStr, verifiedPassword);
            ConsoleUtils::printSuccess("Access Granted. Starting Athlete Session.");
            ConsoleUtils::pauseScreen();
        } else {
            ConsoleUtils::printError("Access Denied! Invalid credentials provided.");
            ConsoleUtils::pauseScreen();
        }
    }

public:
    LoginScreen(TitanUser*& user) : sessionUser(user) {}

    void displayMenu() override {
        while (sessionUser == nullptr) {
            ConsoleUtils::clearScreen();
            ConsoleUtils::printHeader("Titan Track - Gatekeeper System");
            cout << "1. Register Active Athlete Profile\n";
            cout << "2. Authenticate & Log In\n";
            cout << "3. Exit System\n";
            ConsoleUtils::printFooter();
            cout << "Choose Command Sequence: ";

            int choice;
            if (!(cin >> choice)) {
                ConsoleUtils::printError("Invalid command input type!");
                ConsoleUtils::clearInputBuffer();
                ConsoleUtils::pauseScreen();
                continue;
            }
            ConsoleUtils::clearInputBuffer();

            switch (choice) {
                case 1:
                    handleRegister();
                    break;
                case 2:
                    handleLogin();
                    break;
                case 3:
                    cout << "\n" << ConsoleUtils::MAGENTA << "Exiting Titan Track. Build your strength tomorrow!" << ConsoleUtils::RESET << "\n";
                    exit(0);
                default:
                    ConsoleUtils::printError("Invalid option choice. Input 1, 2, or 3.");
                    ConsoleUtils::pauseScreen();
                    break;
            }
        }
    }
};


// ==========================================
// Derived View: Fitness Dashboard Screen
// ==========================================
class DashboardScreen : public View {
private:
    TitanUser*& sessionUser;

    void handleLogWorkout() {
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("Log Daily Metrics");

        cout << "1. Log Strength Session (Weights & Sets)\n";
        cout << "2. Log Cardiovascular Endurance Session (Duration)\n";
        cout << "Choose Workout Vector: ";

        int subChoice;
        if (!(cin >> subChoice)) {
            ConsoleUtils::printError("Invalid Choice. Aborting logging command.");
            ConsoleUtils::clearInputBuffer();
            ConsoleUtils::pauseScreen();
            return;
        }
        ConsoleUtils::clearInputBuffer();

        if (subChoice == 1) {
            string exName;
            double weight;
            int sets, reps;

            cout << "\nEnter Exercise / Lift Name: ";
            getline(cin, exName);

            if (exName.empty()) {
                ConsoleUtils::printError("Exercise name cannot be empty.");
                ConsoleUtils::pauseScreen();
                return;
            }

            cout << "Enter Weight Load (lbs or kg): ";
            while (!(cin >> weight) || weight < 0) {
                ConsoleUtils::printError("Weight load must be a positive decimal number.");
                cout << "Re-enter Weight: ";
                ConsoleUtils::clearInputBuffer();
            }

            cout << "Enter Completed Sets: ";
            while (!(cin >> sets) || sets <= 0) {
                ConsoleUtils::printError("Sets count must be a positive integer.");
                cout << "Re-enter Sets: ";
                ConsoleUtils::clearInputBuffer();
            }

            cout << "Enter Completed Repetitions: ";
            while (!(cin >> reps) || reps <= 0) {
                ConsoleUtils::printError("Reps count must be a positive integer.");
                cout << "Re-enter Reps: ";
                ConsoleUtils::clearInputBuffer();
            }
            ConsoleUtils::clearInputBuffer();

            // Calling strength logging overload
            sessionUser->logWorkout(exName, weight, sets, reps);
            ConsoleUtils::printSuccess("Strength Metrics saved to database successfully.");

        } else if (subChoice == 2) {
            string exName;
            int duration;

            cout << "\nEnter Cardio Activity Name: ";
            getline(cin, exName);

            if (exName.empty()) {
                ConsoleUtils::printError("Activity name cannot be empty.");
                ConsoleUtils::pauseScreen();
                return;
            }

            cout << "Enter Duration (Total Minutes): ";
            while (!(cin >> duration) || duration <= 0) {
                ConsoleUtils::printError("Duration must be a positive integer.");
                cout << "Re-enter Duration (mins): ";
                ConsoleUtils::clearInputBuffer();
            }
            ConsoleUtils::clearInputBuffer();

            // Calling cardio logging overload
            sessionUser->logWorkout(exName, duration);
            ConsoleUtils::printSuccess("Cardiovascular Metrics saved to database successfully.");

        } else {
            ConsoleUtils::printError("Unknown workout vector selection.");
        }
        ConsoleUtils::pauseScreen();
    }

    void handleViewHistory() {
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("Athlete Workout Chronicle");

        vector<Workout> logs = StorageManager::fetchUserLogs(sessionUser->getEmail());

        if (logs.empty()) {
            cout << ConsoleUtils::YELLOW << "No metrics found on local database. Begin your first workout!" << ConsoleUtils::RESET << "\n";
        } else {
            cout << ConsoleUtils::BOLD << left << setw(20) << "Exercise / Activity" 
                 << setw(12) << "Type" 
                 << setw(12) << "Weight" 
                 << setw(8) << "Sets" 
                 << setw(8) << "Reps" 
                 << setw(12) << "Duration" 
                 << setw(12) << "Status"
                 << "Timestamp" << ConsoleUtils::RESET << "\n";
            cout << string(97, '-') << "\n";

            for (const auto& w : logs) {
                cout << left << setw(20) << w.getExerciseName()
                     << setw(12) << w.getType();
                
                if (w.getType() == "Strength") {
                    cout << setw(12) << to_string(w.getWeight()) + " kg/lb"
                         << setw(8) << w.getSets()
                         << setw(8) << w.getReps()
                         << setw(12) << "-";
                } else {
                    cout << setw(12) << "-"
                         << setw(8) << "-"
                         << setw(8) << "-"
                         << setw(12) << to_string(w.getDuration()) + " min";
                }

                // Display colored status
                string statusColor = (w.getStatus() == "Completed") ? ConsoleUtils::GREEN : ConsoleUtils::YELLOW;
                cout << statusColor << setw(12) << w.getStatus() << ConsoleUtils::RESET;

                cout << w.getTimestamp() << "\n";
            }
        }
        ConsoleUtils::pauseScreen();
    }

    void handleMarkWorkoutDone() {
        ConsoleUtils::clearScreen();
        ConsoleUtils::printHeader("Mark Workout as Completed");

        vector<Workout> logs = StorageManager::fetchUserLogs(sessionUser->getEmail());

        // Find which workouts are still Pending
        vector<int> pendingIndices;
        for (int i = 0; i < logs.size(); ++i) {
            if (logs[i].getStatus() == "Pending") {
                pendingIndices.push_back(i);
            }
        }

        if (pendingIndices.empty()) {
            cout << ConsoleUtils::YELLOW << "You have no pending workouts to complete!" << ConsoleUtils::RESET << "\n";
            ConsoleUtils::pauseScreen();
            return;
        }

        cout << "Select a pending workout to mark as Completed:\n\n";
        for (int i = 0; i < pendingIndices.size(); ++i) {
            int originalIdx = pendingIndices[i];
            const auto& w = logs[originalIdx];
            cout << ConsoleUtils::BOLD << i + 1 << "." << ConsoleUtils::RESET << " " << w.getExerciseName() << " [";
            if (w.getType() == "Strength") {
                cout << w.getWeight() << " kg/lb, " << w.getSets() << " sets x " << w.getReps() << " reps";
            } else {
                cout << w.getDuration() << " mins duration";
            }
            cout << "] - Logged on " << w.getTimestamp() << "\n";
        }
        
        cout << "\nEnter selection number (or 0 to cancel): ";
        int sel;
        if (!(cin >> sel) || sel < 0 || sel > pendingIndices.size()) {
            ConsoleUtils::printError("Invalid Selection. Aborting.");
            ConsoleUtils::clearInputBuffer();
            ConsoleUtils::pauseScreen();
            return;
        }
        ConsoleUtils::clearInputBuffer();

        if (sel == 0) {
            cout << "Cancelled.\n";
            ConsoleUtils::pauseScreen();
            return;
        }

        int targetIndexInUserLogs = pendingIndices[sel - 1];

        // Call StorageManager update
        if (StorageManager::updateWorkoutStatus(sessionUser->getEmail(), targetIndexInUserLogs)) {
            ConsoleUtils::printSuccess("Workout marked as Completed successfully!");
        } else {
            ConsoleUtils::printError("Failed to update database.");
        }
        ConsoleUtils::pauseScreen();
    }

    void handleLogout() {
        // Reset session securely and deallocate
        delete sessionUser;
        sessionUser = nullptr;
        ConsoleUtils::printSuccess("Session cleared. Athlete logged out securely.");
        ConsoleUtils::pauseScreen();
    }

public:
    DashboardScreen(TitanUser*& user) : sessionUser(user) {}

    void displayMenu() override {
        while (sessionUser != nullptr) {
            ConsoleUtils::clearScreen();
            ConsoleUtils::printHeader("Titan Track - Fitness Dashboard");
            cout << ConsoleUtils::BOLD << "Authenticated Athlete: " << ConsoleUtils::YELLOW << sessionUser->getEmail() << ConsoleUtils::RESET << "\n\n";
            cout << "1. Log a New Workout\n";
            cout << "2. View Workout History\n";
            cout << "3. Mark a Workout as Completed / Done\n";
            cout << "4. Secure Log Out / Reset Session\n";
            ConsoleUtils::printFooter();
            cout << "Choose Action Sequence: ";

            int choice;
            if (!(cin >> choice)) {
                ConsoleUtils::printError("Invalid command input type!");
                ConsoleUtils::clearInputBuffer();
                ConsoleUtils::pauseScreen();
                continue;
            }
            ConsoleUtils::clearInputBuffer();

            switch (choice) {
                case 1:
                    handleLogWorkout();
                    break;
                case 2:
                    handleViewHistory();
                    break;
                case 3:
                    handleMarkWorkoutDone();
                    break;
                case 4:
                    handleLogout();
                    break;
                default:
                    ConsoleUtils::printError("Invalid option choice. Input 1, 2, 3, or 4.");
                    ConsoleUtils::pauseScreen();
                    break;
            }
        }
    }
};


// ==========================================
// Main System Bootstrapper
// ==========================================
#ifndef RUNNING_TESTS
int main() {
    // Initial Verification of Database Integrity
    if (!StorageManager::ensureFilesExist()) {
        ConsoleUtils::printError("Fatal Error: Unable to access or create local persistence databases.");
        ConsoleUtils::pauseScreen();
        return 1;
    }

    // Active Athlete Session Pointer
    TitanUser* sessionUser = nullptr;

    // Polymorphic views initialized with session reference
    LoginScreen loginView(sessionUser);
    DashboardScreen dashboardView(sessionUser);

    // Dynamic program cycle
    while (true) {
        if (sessionUser == nullptr) {
            loginView.displayMenu();
        } else {
            dashboardView.displayMenu();
        }
    }

    return 0;
}
#endif
