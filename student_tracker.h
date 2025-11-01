#ifndef STUDENT_TRACKER_H
#define STUDENT_TRACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Data Structures ---

// Enum for all record types
typedef enum {
    ATTENDANCE,     // value = 1 (Present), 0 (Absent)
    INTERNAL_1,     // For 20 marks
    ASSIGNMENT_1,   // For 5 marks
    INTERNAL_2,     // For 20 marks
    ASSIGNMENT_2,   // For 5 marks
    SEMESTER_EXAM   // For 100 marks (will be scaled to 50)
} RecordType;

// Linked List node for a student's history
typedef struct PerformanceNode {
    RecordType type;
    int value;
    char subject[50];
    char description[50];      // e.g., "2025-11-01" or "Internal 1"
    struct PerformanceNode* next;
} PerformanceNode;

// BST node for a student
typedef struct StudentNode {
    char studentSRN[20];
    char name[100];
    struct StudentNode* left;
    struct StudentNode* right;
    PerformanceNode* historyHead;
} StudentNode;

// BST node for a teacher
typedef struct TeacherNode {
    char teacherID[20];  // The teacher's unique ID
    char name[100];
    char password[50];   // Stores the password
    char subject[50];    // The subject they teach
    int failedAttempts;  // Tracks failed login tries
    
    struct TeacherNode* left;
    struct TeacherNode* right;
} TeacherNode;


// --- Function Prototypes ---

/* --- NEW: File I/O Functions --- */
void saveData(TeacherNode* teacherRoot, StudentNode* studentRoot);
void loadData(TeacherNode** teacherRoot, StudentNode** studentRoot);

/* --- Teacher Functions --- */
TeacherNode* createTeacher(char* id, char* name, char* password, char* subject);
TeacherNode* addTeacher(TeacherNode* root, char* id, char* name, char* password, char* subject);
TeacherNode* findTeacher(TeacherNode* root, char* id);
TeacherNode* login(TeacherNode* root, char* id, char* password);
void freeTeacherTree(TeacherNode* root);

/* --- Student Functions --- */
StudentNode* createStudent(char* srn, char* name);
StudentNode* addStudent(StudentNode* root, char* srn, char* name);
StudentNode* findStudent(StudentNode* root, char* srn);
StudentNode* findMin(StudentNode* node);
StudentNode* deleteStudent(StudentNode* root, char* srn);
void listAllStudents(StudentNode* root);

/* --- Data Modification Functions --- */
void modifyStudentName(StudentNode* root, char* srn);
void modifyMark(StudentNode* root, char* srn);
StudentNode* modifyStudentSRN(StudentNode* root, char* oldSRN, char* newSRN);

/* --- Linked List (Performance) Functions --- */
PerformanceNode* createPerformanceNode(RecordType type, int value, char* subject, char* desc);
void addPerformanceRecord(StudentNode* student, RecordType type, int value, char* subject, char* desc);

/* --- Attendance Function --- */
void takeAttendance(StudentNode* root, char* date);

/* --- Reporting Functions --- */
void generateSubjectReport(StudentNode* student, char* subjectName);
void generateMasterReport(StudentNode* student);

/* --- Memory Freeing Functions --- */
void freeHistory(PerformanceNode* head);
void freeTree(StudentNode* root);


#endif // STUDENT_TRACKER_H