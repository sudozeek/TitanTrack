#define RUNNING_TESTS
#include "main.cpp"
#include <cassert>

void testEmailValidation() {
    cout << "Running Email Validation Tests...\n";

    // Valid Emails
    assert(User::validateEmail("athlete@titan.com") == true);
    assert(User::validateEmail("a@b.co") == true);
    assert(User::validateEmail("test.user@domain.org") == true);

    // Invalid Emails
    assert(User::validateEmail("athlete.titan.com") == false); // Missing @
    assert(User::validateEmail("athlete@") == false);          // Missing domain
    assert(User::validateEmail("@domain.com") == false);       // Missing username
    assert(User::validateEmail("athlete@domain") == false);    // Missing dot in domain
    assert(User::validateEmail("athlete@.com") == false);      // Missing domain name between @ and .

    ConsoleUtils::printSuccess("Email validation tests passed!");
}

void testPasswordValidation() {
    cout << "\nRunning Password Strength Validation Tests...\n";

    string errMsg;

    // Valid Passwords
    assert(User::validatePasswordStrength("TitanPass1!", errMsg) == true);
    assert(User::validatePasswordStrength("A_b_c_1", errMsg) == true);
    assert(User::validatePasswordStrength("Strong@123", errMsg) == true);

    // Invalid Passwords - Too short
    assert(User::validatePasswordStrength("Ab1!", errMsg) == false);
    assert(errMsg.find("at least 6 characters") != string::npos);

    // Invalid Passwords - Missing Uppercase
    assert(User::validatePasswordStrength("titanpass1!", errMsg) == false);
    assert(errMsg.find("uppercase letter") != string::npos);

    // Invalid Passwords - Missing Lowercase
    assert(User::validatePasswordStrength("TITANPASS1!", errMsg) == false);
    assert(errMsg.find("lowercase letter") != string::npos);

    // Invalid Passwords - Missing Special Character
    assert(User::validatePasswordStrength("TitanPass123", errMsg) == false);
    assert(errMsg.find("special character") != string::npos);

    ConsoleUtils::printSuccess("Password strength validation tests passed!");
}

void testStorageAndUserSession() {
    cout << "\nRunning Storage and Session Management Tests...\n";

    // Initialize files
    assert(StorageManager::ensureFilesExist() == true);

    string testEmail = "test_unit_athlete@titan.com";
    string testPass = "TitanPass1!";

    // Create user object
    User testUser(testEmail, testPass);
    assert(testUser.getEmail() == testEmail);
    assert(testUser.getPassword() == testPass);
    assert(testUser.verifyPassword(testPass) == true);
    assert(testUser.verifyPassword("wrongpass") == false);

    // Save to storage
    bool saved = StorageManager::saveUserToFile(testUser);
    assert(saved == true);

    // Verify exists
    assert(StorageManager::userExists(testEmail) == true);

    // Verify credentials
    string outPass;
    assert(StorageManager::verifyCredentials(testEmail, testPass, outPass) == true);
    assert(outPass == testPass);
    assert(StorageManager::verifyCredentials(testEmail, "wrongpass", outPass) == false);

    ConsoleUtils::printSuccess("Storage and session tests passed!");
}

void testWorkoutLogging() {
    cout << "\nRunning Polymorphic Workout Logging Tests...\n";

    string testEmail = "test_unit_athlete@titan.com";
    string testPass = "TitanPass1!";

    TitanUser athlete(testEmail, testPass);

    // Clear old logs if any for clean test
    // Log strength workout (Overload 1)
    string liftName = "Test Bench Press";
    double weight = 225.5;
    int sets = 4;
    int reps = 8;
    athlete.logWorkout(liftName, weight, sets, reps);

    // Log cardio workout (Overload 2)
    string cardioName = "Test Running";
    int duration = 45;
    athlete.logWorkout(cardioName, duration);

    // Fetch logs
    vector<Workout> logs = StorageManager::fetchUserLogs(testEmail);
    assert(logs.size() >= 2);

    // Check last two logged workouts
    bool foundStrength = false;
    bool foundCardio = false;

    for (const auto& w : logs) {
        if (w.getExerciseName() == liftName) {
            assert(w.getType() == "Strength");
            assert(w.getWeight() == weight);
            assert(w.getSets() == sets);
            assert(w.getReps() == reps);
            assert(w.getDuration() == -1);
            foundStrength = true;
        } else if (w.getExerciseName() == cardioName) {
            assert(w.getType() == "Cardio");
            assert(w.getWeight() == -1.0);
            assert(w.getSets() == -1);
            assert(w.getReps() == -1);
            assert(w.getDuration() == duration);
            foundCardio = true;
        }
    }

    assert(foundStrength && foundCardio);
    ConsoleUtils::printSuccess("Polymorphic workout logging tests passed!");
}

int main() {
    ConsoleUtils::clearScreen();
    ConsoleUtils::printHeader("Titan Track Unit Test Suite");

    try {
        testEmailValidation();
        testPasswordValidation();
        testStorageAndUserSession();
        testWorkoutLogging();

        cout << "\n";
        ConsoleUtils::printSuccess("ALL UNIT TESTS PASSED SUCCESSFULLY!");
    } catch (const exception& e) {
        cerr << "\nFATAL EXCEPTION DURING TESTING: " << e.what() << "\n";
        return 1;
    } catch (...) {
        cerr << "\nFATAL UNKNOWN ERROR DURING TESTING!\n";
        return 1;
    }

    return 0;
}
