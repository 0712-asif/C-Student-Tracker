#include "student_tracker.h"
#include <string.h>

#define SAVE_FILE "data.dat" // Define the save file name

// --- File I/O Function Implementations ---

// Helper to save a single student's history
void saveHistory(PerformanceNode* head, FILE* fp) {
    PerformanceNode* current = head;
    while (current != NULL) {
        fprintf(fp, "RECORD\n");
        fprintf(fp, "%d\n", (int)current->type);
        fprintf(fp, "%d\n", current->value);
        fprintf(fp, "%s\n", current->subject);
        fprintf(fp, "%s\n", current->description);
        current = current->next;
    }
}

// Helper to save the student tree (pre-order traversal)
void saveStudentTree(StudentNode* root, FILE* fp) {
    if (root == NULL) return;
    
    fprintf(fp, "STUDENT\n");
    fprintf(fp, "%s\n", root->studentSRN);
    fprintf(fp, "%s\n", root->name);
    
    saveHistory(root->historyHead, fp);
    
    saveStudentTree(root->left, fp);
    saveStudentTree(root->right, fp);
}

// Helper to save the teacher tree (pre-order traversal)
void saveTeacherTree(TeacherNode* root, FILE* fp) {
    if (root == NULL) return;
    
    fprintf(fp, "TEACHER\n");
    fprintf(fp, "%s\n", root->teacherID);
    fprintf(fp, "%s\n", root->name);
    fprintf(fp, "%s\n", root->password);
    fprintf(fp, "%s\n", root->subject);
    
    saveTeacherTree(root->left, fp);
    saveTeacherTree(root->right, fp);
}

// Main save function
void saveData(TeacherNode* teacherRoot, StudentNode* studentRoot) {
    FILE* fp = fopen(SAVE_FILE, "w");
    if (fp == NULL) {
        printf("Error: Could not open file %s for writing.\n", SAVE_FILE);
        return;
    }
    
    saveTeacherTree(teacherRoot, fp);
    saveStudentTree(studentRoot, fp);
    
    fprintf(fp, "END_OF_FILE\n");
    
    fclose(fp);
    printf("All data saved successfully to %s.\n", SAVE_FILE);
}

// Helper function to read a line safely from a file
void readLineFromFile(char* buffer, int size, FILE* fp) {
    fgets(buffer, size, fp);
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
}

// Main load function
void loadData(TeacherNode** teacherRoot, StudentNode** studentRoot) {
    FILE* fp = fopen(SAVE_FILE, "r");
    if (fp == NULL) {
        printf("No save file found. Starting with a fresh database.\n");
        return;
    }
    
    char lineBuffer[100];
    char nameBuffer[100], idBuffer[20], pwBuffer[50], subjectBuffer[50], descBuffer[50];
    int tempType, tempValue;
    
    StudentNode* lastStudent = NULL;
    
    while (fscanf(fp, "%s", lineBuffer) != EOF && strcmp(lineBuffer, "END_OF_FILE") != 0) {
        fgetc(fp); // Consume the newline
        
        if (strcmp(lineBuffer, "TEACHER") == 0) {
            readLineFromFile(idBuffer, 20, fp);
            readLineFromFile(nameBuffer, 100, fp);
            readLineFromFile(pwBuffer, 50, fp);
            readLineFromFile(subjectBuffer, 50, fp);
            *teacherRoot = addTeacher(*teacherRoot, idBuffer, nameBuffer, pwBuffer, subjectBuffer);
            
        } else if (strcmp(lineBuffer, "STUDENT") == 0) {
            readLineFromFile(idBuffer, 20, fp);
            readLineFromFile(nameBuffer, 100, fp);
            *studentRoot = addStudent(*studentRoot, idBuffer, nameBuffer);
            lastStudent = findStudent(*studentRoot, idBuffer);
            
        } else if (strcmp(lineBuffer, "RECORD") == 0) {
            if (lastStudent == NULL) continue;
            
            fscanf(fp, "%d", &tempType); fgetc(fp);
            fscanf(fp, "%d", &tempValue); fgetc(fp);
            readLineFromFile(subjectBuffer, 50, fp);
            readLineFromFile(descBuffer, 50, fp);
            addPerformanceRecord(lastStudent, (RecordType)tempType, tempValue, subjectBuffer, descBuffer);
        }
    }
    
    fclose(fp);
    printf("Data loaded successfully from %s.\n", SAVE_FILE);
}

// --- Teacher Function Implementations ---

TeacherNode* createTeacher(char* id, char* name, char* password, char* subject) {
    TeacherNode* newNode = (TeacherNode*)malloc(sizeof(TeacherNode));
    if (newNode == NULL) return NULL;
    
    strcpy(newNode->teacherID, id);
    strcpy(newNode->name, name);
    strcpy(newNode->password, password);
    strcpy(newNode->subject, subject);
    newNode->failedAttempts = 0; // Initialize security counter
    newNode->left = NULL;
    newNode->right = NULL;
    
    return newNode;
}

TeacherNode* addTeacher(TeacherNode* root, char* id, char* name, char* password, char* subject) {
    if (root == NULL) {
        return createTeacher(id, name, password, subject);
    }
    int compare = strcmp(id, root->teacherID);
    if (compare < 0) {
        root->left = addTeacher(root->left, id, name, password, subject);
    } else if (compare > 0) {
        root->right = addTeacher(root->right, id, name, password, subject);
    } else {
        printf("Error: Teacher ID %s already exists.\n", id);
    }
    return root;
}

TeacherNode* findTeacher(TeacherNode* root, char* id) {
    if (root == NULL) return NULL;
    int compare = strcmp(id, root->teacherID);
    if (compare == 0) return root;
    if (compare < 0) return findTeacher(root->left, id);
    return findTeacher(root->right, id);
}

TeacherNode* login(TeacherNode* root, char* id, char* password) {
    TeacherNode* teacher = findTeacher(root, id);
    
    if (teacher == NULL) {
        printf("Login Failed: Teacher ID %s not found.\n", id);
        return NULL;
    }
    
    if (teacher->failedAttempts >= 3) {
        printf("Login Failed: Account for %s is LOCKED.\n", id);
        return NULL;
    }
    
    if (strcmp(teacher->password, password) == 0) {
        printf("Login Successful. Welcome, %s.\n", teacher->name);
        teacher->failedAttempts = 0; // Reset counter
        return teacher;
    } else {
        teacher->failedAttempts++;
        int attempts_left = 3 - teacher->failedAttempts;
        printf("Login Failed: Incorrect password.\n");
        if (attempts_left > 0) {
            printf("You have %d attempt(s) remaining.\n", attempts_left);
        } else {
            printf("********************************************************\n");
            printf("  SECURITY ALERT: 3 failed login attempts.\n");
            printf("  Account for %s is now LOCKED.\n", id);
            printf("  [SIMULATING SMS ALERT TO ADMINISTRATOR...]\n");
            printf("********************************************************\n");
        }
        return NULL;
    }
}

void freeTeacherTree(TeacherNode* root) {
    if (root == NULL) return;
    freeTeacherTree(root->left);
    freeTeacherTree(root->right);
    free(root);
}

// --- Student Function Implementations ---

StudentNode* createStudent(char* srn, char* name) {
    StudentNode* newNode = (StudentNode*)malloc(sizeof(StudentNode));
    if (newNode == NULL) return NULL;
    strcpy(newNode->studentSRN, srn);
    strcpy(newNode->name, name);
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->historyHead = NULL;
    return newNode;
}

StudentNode* addStudent(StudentNode* root, char* srn, char* name) {
    if (root == NULL) {
        return createStudent(srn, name);
    }
    int compare = strcmp(srn, root->studentSRN);
    if (compare < 0) {
        root->left = addStudent(root->left, srn, name);
    } else if (compare > 0) {
        root->right = addStudent(root->right, srn, name);
    } else {
        printf("Error: Student SRN %s already exists.\n", srn);
    }
    return root;
}

StudentNode* findStudent(StudentNode* root, char* srn) {
    if (root == NULL) return NULL;
    int compare = strcmp(srn, root->studentSRN);
    if (compare == 0) return root;
    if (compare < 0) return findStudent(root->left, srn);
    return findStudent(root->right, srn);
}

StudentNode* findMin(StudentNode* node) {
    StudentNode* current = node;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

StudentNode* deleteStudent(StudentNode* root, char* srn) {
    if (root == NULL) {
        printf("Error: Student SRN %s not found for deletion.\n", srn);
        return root;
    }
    int compare = strcmp(srn, root->studentSRN);
    if (compare < 0) {
        root->left = deleteStudent(root->left, srn);
    } else if (compare > 0) {
        root->right = deleteStudent(root->right, srn);
    } else {
        // Node found
        if (root->left == NULL && root->right == NULL) {
            freeHistory(root->historyHead);
            free(root);
            return NULL;
        }
        else if (root->left == NULL) {
            StudentNode* temp = root->right;
            freeHistory(root->historyHead);
            free(root);
            return temp;
        } else if (root->right == NULL) {
            StudentNode* temp = root->left;
            freeHistory(root->historyHead);
            free(root);
            return temp;
        }
        // Two children case
        StudentNode* temp = findMin(root->right);
        strcpy(root->studentSRN, temp->studentSRN);
        strcpy(root->name, temp->name);
        PerformanceNode* tempListHead = root->historyHead;
        root->historyHead = temp->historyHead;
        temp->historyHead = tempListHead;
        root->right = deleteStudent(root->right, temp->studentSRN);
    }
    return root;
}

// --- Data Modification Functions ---

void modifyStudentName(StudentNode* root, char* srn) {
    StudentNode* student = findStudent(root, srn);
    if (student == NULL) {
        printf("Error: Student SRN %s not found.\n", srn);
        return;
    }
    char newName[100];
    printf("Current name is: %s\n", student->name);
    printf("Enter new name: ");
    fgets(newName, 100, stdin);
    newName[strcspn(newName, "\n")] = 0;
    strcpy(student->name, newName);
    printf("Name updated successfully.\n");
}

StudentNode* modifyStudentSRN(StudentNode* root, char* oldSRN, char* newSRN) {
    StudentNode* student = findStudent(root, oldSRN);
    if (student == NULL) {
        printf("Error: Student SRN %s not found.\n", oldSRN);
        return root;
    }
    char tempName[100];
    strcpy(tempName, student->name);
    PerformanceNode* tempHistory = student->historyHead;
    
    student->historyHead = NULL; // Prevent history from being freed
    root = deleteStudent(root, oldSRN);
    
    root = addStudent(root, newSRN, tempName);
    student = findStudent(root, newSRN);
    if (student) {
        student->historyHead = tempHistory; // Re-attach history
        printf("SRN modified successfully.\n");
    }
    return root;
}

void modifyMark(StudentNode* root, char* srn) {
    StudentNode* student = findStudent(root, srn);
    if (student == NULL) {
        printf("Error: Student SRN %s not found.\n", srn);
        return;
    }
    char subject[50];
    printf("Enter subject of mark to modify: ");
    fgets(subject, 50, stdin);
    subject[strcspn(subject, "\n")] = 0;
    
    int typeChoice, newValue;
    printf("Which mark type?\n1=I1, 2=A1, 3=I2, 4=A2, 5=SEM: ");
    scanf("%d", &typeChoice);
    getchar();
    
    RecordType typeToFind;
    switch(typeChoice) {
        case 1: typeToFind = INTERNAL_1; break;
        case 2: typeToFind = ASSIGNMENT_1; break;
        case 3: typeToFind = INTERNAL_2; break;
        case 4: typeToFind = ASSIGNMENT_2; break;
        case 5: typeToFind = SEMESTER_EXAM; break;
        default: printf("Invalid type.\n"); return;
    }
    
    PerformanceNode* current = student->historyHead;
    int found = 0;
    while (current != NULL) {
        if (current->type == typeToFind && strcmp(current->subject, subject) == 0) {
            printf("Found existing mark for %s. Current value: %d\n", subject, current->value);
            printf("Enter new value: ");
            scanf("%d", &newValue);
            getchar();
            current->value = newValue;
            printf("Mark updated.\n");
            found = 1;
            break;
        }
        current = current->next;
    }
    if (!found) printf("No matching mark found to update.\n");
}

// --- Linked List (Performance) Functions ---

PerformanceNode* createPerformanceNode(RecordType type, int value, char* subject, char* desc) {
    PerformanceNode* newNode = (PerformanceNode*)malloc(sizeof(PerformanceNode));
    if (newNode == NULL) return NULL;
    newNode->type = type;
    newNode->value = value;
    strcpy(newNode->subject, subject);
    strcpy(newNode->description, desc);
    newNode->next = NULL;
    return newNode;
}

void addPerformanceRecord(StudentNode* student, RecordType type, int value, char* subject, char* desc) {
    if (student == NULL) return;
    PerformanceNode* newNode = createPerformanceNode(type, value, subject, desc);
    if (newNode == NULL) return;
    newNode->next = student->historyHead; // Add to front
    student->historyHead = newNode;
}

// --- Attendance and Reporting Functions ---

void listAllStudents(StudentNode* root) {
    if (root != NULL) {
        listAllStudents(root->left); // In-order traversal
        printf("  -> SRN: %-15s | Name: %s\n", root->studentSRN, root->name);
        listAllStudents(root->right);
    }
}

void takeAttendance(StudentNode* root, char* date) {
    if (root != NULL) {
        takeAttendance(root->left, date);
        
        int status = -1;
        printf("Student: %-15s (%s)\n", root->studentSRN, root->name);
        while(status != 0 && status != 1) {
            printf("  Enter 1 (Present) or 0 (Absent): ");
            scanf("%d", &status);
            getchar();
        }
        addPerformanceRecord(root, ATTENDANCE, status, "Attendance", date);
        
        takeAttendance(root->right, date);
    }
}

void generateSubjectReport(StudentNode* student, char* subjectName) {
    if (student == NULL) return;
    printf("\n============================================\n");
    printf("            STUDENT SUBJECT REPORT\n");
    printf("============================================\n");
    printf("SRN:    %s\n", student->studentSRN);
    printf("Name:   %s\n", student->name);
    printf("Subject: %s\n", subjectName);
    printf("--------------------------------------------\n");
    printf("Detailed Marks:\n\n");
    
    PerformanceNode* current = student->historyHead;
    int i1_mark = -1, a1_mark = -1, i2_mark = -1, a2_mark = -1, sem_mark = -1;
    
    while (current != NULL) {
        if (strcmp(current->subject, subjectName) == 0) {
            switch (current->type) {
                case INTERNAL_1:   i1_mark = current->value;   printf("  [Internal 1]   : %d / 20\n", i1_mark);   break;
                case ASSIGNMENT_1: a1_mark = current->value;   printf("  [Assignment 1] : %d / 5\n", a1_mark);   break;
                case INTERNAL_2:   i2_mark = current->value;   printf("  [Internal 2]   : %d / 20\n", i2_mark);   break;
                case ASSIGNMENT_2: a2_mark = current->value;   printf("  [Assignment 2] : %d / 5\n", a2_mark);   break;
                case SEMESTER_EXAM: sem_mark = current->value; printf("  [Semester Exam]: %d / 100\n", sem_mark); break;
                case ATTENDANCE: break;
            }
        }
        current = current->next;
    }
    
    printf("\n--- Summary & Calculation ---\n");
    double total_cie = 0;
    int cie_complete = 1;
    if (i1_mark != -1) total_cie += i1_mark; else cie_complete = 0;
    if (a1_mark != -1) total_cie += a1_mark; else cie_complete = 0;
    if (i2_mark != -1) total_cie += i2_mark; else cie_complete = 0;
    if (a2_mark != -1) total_cie += a2_mark; else cie_complete = 0;
    
    if(cie_complete) printf("Total CIE (I1+A1+I2+A2):      %.2f / 50\n", total_cie);
    else printf("Total CIE (I1+A1+I2+A2):      N/A (Missing components)\n");
    
    double sem_scaled = -1;
    if (sem_mark != -1) {
        sem_scaled = sem_mark / 2.0;
        printf("Semester Mark (Scaled):       %.2f / 50\n", sem_scaled);
    } else {
        printf("Semester Mark (Scaled):       N/A (Missing SEM score)\n");
    }
    
    if (cie_complete && sem_mark != -1) printf("\nFINAL SUBJECT MARK (Total):   %.2f / 100\n", (total_cie + sem_scaled));
    else printf("\nFINAL SUBJECT MARK (Total):   N/A (Missing components)\n");
    printf("============================================\n");
}

void generateMasterReport(StudentNode* student) {
    if (student == NULL) {
        printf("Error: Student not found.\n");
        return;
    }
    printf("\n============================================\n");
    printf("           MASTER STUDENT REPORT\n");
    printf("============================================\n");
    printf("SRN:    %s\n", student->studentSRN);
    printf("Name:   %s\n", student->name);
    printf("--------------------------------------------\n");
    
    PerformanceNode* current = student->historyHead;
    if (current == NULL) {
        printf("...No records found for this student.\n");
        printf("============================================\n");
        return;
    }
    
    while (current != NULL) {
        printf("\nSubject: %s\n", current->subject);
        printf("  Description: %s\n", current->description);
        switch (current->type) {
            case ATTENDANCE:   printf("  Type:  Attendance\n  Value: %s\n", (current->value == 1 ? "Present" : "Absent")); break;
            case INTERNAL_1:   printf("  Type:  Internal 1\n  Value: %d / 20\n", current->value);   break;
            case ASSIGNMENT_1: printf("  Type:  Assignment 1\n  Value: %d / 5\n", current->value);   break;
            case INTERNAL_2:   printf("  Type:  Internal 2\n  Value: %d / 20\n", current->value);   break;
            case ASSIGNMENT_2: printf("  Type:  Assignment 2\n  Value: %d / 5\n", current->value);   break;
            case SEMESTER_EXAM: printf("  Type:  Semester Exam\n  Value: %d / 100\n", current->value); break;
        }
        current = current->next;
    }
    printf("============================================\n");
}

// --- Memory Freeing Functions ---

void freeHistory(PerformanceNode* head) {
    PerformanceNode* current = head;
    PerformanceNode* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
}

void freeTree(StudentNode* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    freeHistory(root->historyHead);
    free(root);
}