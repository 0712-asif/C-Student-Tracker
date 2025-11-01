#include "student_tracker.h"

// --- Login Menu (for startup) ---
void displayLoginMenu() {
    printf("\n==============================================\n");
    printf("         WELCOME TO THE TRACKER SYSTEM\n");
    printf("==============================================\n");
    printf("1. Login\n");
    printf("2. Create New Teacher Account\n");
    printf("3. Exit\n");
    printf("==============================================\n");
    printf("Enter your choice: ");
}

// --- Helper to display the main menu dynamically ---
void displayMainMenu(TeacherNode* teacher) {
    printf("\n==============================================\n");
    printf("        Student Performance Tracker\n");
    printf("==============================================\n");
    printf("--- Student Management ---\n");
    printf(" 1. Add New Student\n");
    printf(" 2. List All Students (Sorted by SRN)\n");
    printf(" 3. Delete a Student\n");
    printf("\n--- Data Modification ---\n");
    printf(" 4. Modify Student Name\n");
    printf(" 5. Modify Student SRN\n");
    printf(" 6. Modify an Existing Mark\n");
    printf("\n--- Data Entry (Subject: %s) ---\n", teacher->subject);
    printf(" 7. Take Class Attendance\n");
    printf(" 8. Log Internal 1 Mark (20)\n");
    printf(" 9. Log Assignment 1 Mark (5)\n");
    printf("10. Log Internal 2 Mark (20)\n");
    printf("11. Log Assignment 2 Mark (5)\n");
    printf("12. Log Semester Exam Mark (100)\n");
    printf("\n--- Reporting ---\n");
    printf("13. Generate Subject Marks Report\n");
    printf("14. Generate Full Master Report\n");
    printf("15. Logout\n");
    printf("==============================================\n");
    printf("Logged in as: %s | Enter your choice: ", teacher->name);
}

// Helper function to read a line of input safely
void readString(char* buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
}

// Helper function to find student
StudentNode* getStudent(StudentNode* root) {
    char srnBuffer[20];
    printf("Enter Student SRN: ");
    readString(srnBuffer, 20);
    StudentNode* student = findStudent(root, srnBuffer);
    if (student == NULL) {
        printf("Error: Student SRN %s not found.\n", srnBuffer);
    }
    return student;
}


int main() {
    StudentNode* studentRoot = NULL; 
    TeacherNode* teacherRoot = NULL; 
    TeacherNode* currentTeacher = NULL;
    
    int loginChoice = 0;
    char idBuffer[20], nameBuffer[100], pwBuffer[50], subjectBuffer[50];

    // Load all data from file at startup
    loadData(&teacherRoot, &studentRoot);
    printf("\nPress Enter to continue...");
    getchar();

    // Main application loop
    while (loginChoice != 3) {
        
        // --- LOGIN MENU LOOP ---
        currentTeacher = NULL;
        while (currentTeacher == NULL && loginChoice != 3) {
            displayLoginMenu();
            if (scanf("%d", &loginChoice) != 1) {
                while(getchar() != '\n'); 
                loginChoice = 0;
            }
            getchar(); // Consume newline

            switch (loginChoice) {
                case 1: // Login
                    printf("\n--- Login ---\n");
                    printf("Enter Teacher ID: ");
                    readString(idBuffer, 20);
                    printf("Enter Password: ");
                    readString(pwBuffer, 50);
                    currentTeacher = login(teacherRoot, idBuffer, pwBuffer);
                    break;
                
                case 2: // Create Teacher Account
                    printf("\n--- Create Teacher Account ---\n");
                    printf("Enter New Teacher ID: ");
                    readString(idBuffer, 20);
                    printf("Enter Full Name: ");
                    readString(nameBuffer, 100);
                    printf("Enter New Password: ");
                    readString(pwBuffer, 50);
                    printf("Enter Subject Taught: ");
                    readString(subjectBuffer, 50);
                    teacherRoot = addTeacher(teacherRoot, idBuffer, nameBuffer, pwBuffer, subjectBuffer);
                    printf("Account created successfully. Please login.\n");
                    break;
                
                case 3: // Exit
                    printf("Preparing to exit...\n");
                    break;
                
                default:
                    printf("Invalid choice. Please try again.\n");
                    break;
            }
        }
        
        if (loginChoice == 3) {
            break; // Exit main application loop
        }

        // --- MAIN MENU LOOP ---
        int menuChoice = 0;
        while (menuChoice != 15) { // 15 is "Logout"
            
            displayMainMenu(currentTeacher); // Pass teacher to display subject

            if (scanf("%d", &menuChoice) != 1) {
                while(getchar() != '\n');
                menuChoice = 0;
            }
            getchar(); // Consume newline

            char srnBuffer[20], srnBuffer2[20], dateBuffer[50], descBuffer[50];
            int tempValue;
            StudentNode* foundStudent = NULL;

            switch (menuChoice) {
                case 1: // Add Student
                    printf("\n--- Add New Student ---\n");
                    printf("Enter Student SRN: ");
                    readString(srnBuffer, 20);
                    printf("Enter Student Name: ");
                    readString(nameBuffer, 100);
                    studentRoot = addStudent(studentRoot, srnBuffer, nameBuffer);
                    break;
                
                case 2: // List Students
                    printf("\n--- All Students (Sorted by SRN) ---\n");
                    if (studentRoot == NULL) printf("No students in the system.\n");
                    else listAllStudents(studentRoot);
                    break;
                
                case 3: // Delete Student
                    printf("\n--- Delete Student ---\n");
                    foundStudent = getStudent(studentRoot);
                    if (foundStudent) {
                        studentRoot = deleteStudent(studentRoot, foundStudent->studentSRN);
                        printf("Student %s deleted.\n", foundStudent->studentSRN);
                    }
                    break;

                case 4: // Modify Name
                    printf("\n--- Modify Student Name ---\n");
                    printf("Enter Student SRN: ");
                    readString(srnBuffer, 20);
                    modifyStudentName(studentRoot, srnBuffer);
                    break;

                case 5: // Modify SRN
                    printf("\n--- Modify Student SRN ---\n");
                    printf("Enter OLD Student SRN: ");
                    readString(srnBuffer, 20);
                    printf("Enter NEW Student SRN: ");
                    readString(srnBuffer2, 20);
                    studentRoot = modifyStudentSRN(studentRoot, srnBuffer, srnBuffer2);
                    break;
                
                case 6: // Modify Mark
                    printf("\n--- Modify Existing Mark ---\n");
                    printf("Enter Student SRN: ");
                    readString(srnBuffer, 20);
                    modifyMark(studentRoot, srnBuffer);
                    break;
                
                case 7: // Take Attendance
                    printf("\n--- Take Class Attendance ---\n");
                    printf("Enter Date (e.g., 2025-11-01): ");
                    readString(dateBuffer, 50);
                    if (studentRoot == NULL) printf("No students to take attendance for.\n");
                    else takeAttendance(studentRoot, dateBuffer);
                    printf("...Attendance complete.\n");
                    break;

                // --- MARK ENTRY CASES (8-12) ---
                case 8: case 9: case 10: case 11: case 12:
                    foundStudent = getStudent(studentRoot);
                    if (foundStudent == NULL) break;
                    
                    RecordType currentType;
                    int maxMark = 0;
                    
                    if(menuChoice == 8)  { currentType = INTERNAL_1;   maxMark = 20; strcpy(descBuffer, "Internal 1"); }
                    if(menuChoice == 9)  { currentType = ASSIGNMENT_1; maxMark = 5;  strcpy(descBuffer, "Assignment 1"); }
                    if(menuChoice == 10) { currentType = INTERNAL_2;   maxMark = 20; strcpy(descBuffer, "Internal 2"); }
                    if(menuChoice == 11) { currentType = ASSIGNMENT_2; maxMark = 5;  strcpy(descBuffer, "Assignment 2"); }
                    if(menuChoice == 12) { currentType = SEMESTER_EXAM;maxMark = 100;strcpy(descBuffer, "Semester Exam"); }

                    printf("Enter %s Score for %s (out of %d): ", descBuffer, currentTeacher->subject, maxMark);
                    scanf("%d", &tempValue);
                    getchar(); // Consume newline

                    if (tempValue < 0 || tempValue > maxMark) {
                        printf("Error: Mark must be between 0 and %d.\n", maxMark);
                    } else {
                        addPerformanceRecord(foundStudent, currentType, tempValue, currentTeacher->subject, descBuffer);
                        printf("Mark added successfully.\n");
                    }
                    break;
                
                case 13: // Subject Report
                    foundStudent = getStudent(studentRoot);
                    if (foundStudent == NULL) break;
                    printf("Enter Subject Name for report (or leave blank for %s): ", currentTeacher->subject);
                    readString(subjectBuffer, 50);
                    
                    if (strlen(subjectBuffer) == 0) {
                        strcpy(subjectBuffer, currentTeacher->subject);
                    }
                    generateSubjectReport(foundStudent, subjectBuffer);
                    break;

                case 14: // Master Report
                    foundStudent = getStudent(studentRoot);
                    if (foundStudent) generateMasterReport(foundStudent);
                    break;

                case 15: // Logout
                    printf("You are logging out, %s.\n", currentTeacher->name);
                    char saveChoice = ' ';
                    while (saveChoice != 'y' && saveChoice != 'n') {
                        printf("Do you want to save your changes to disk? (y/n): ");
                        scanf(" %c", &saveChoice);
                        getchar(); // Consume newline
                    }
                    
                    if (saveChoice == 'y') {
                        saveData(teacherRoot, studentRoot);
                    } else {
                        printf("Changes not saved. Loading from last save on next login.\n");
                    }
                    
                    printf("Logging out...\n");
                    currentTeacher = NULL; // Breaks this loop, goes to login menu
                    break;

                default:
                    printf("Invalid choice. Please enter a number between 1 and 15.\n");
            }
        }
    }

    // --- FINAL CLEANUP (Only on Exit from Login Menu) ---
    printf("Freeing all allocated memory.\n");
    freeTree(studentRoot);
    freeTeacherTree(teacherRoot);
    printf("All memory freed. Goodbye.\n");
    
    return 0;
}