# 🎯 Student Performance & Attendance Tracker

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Build](https://img.shields.io/badge/Build-Compiles-brightgreen.svg)](https://gcc.gnu.org/)
[![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)](https://opensource.org/licenses/MIT)

A comprehensive, console-based application written in C to securely manage and track student academic performance, attendance, and records. This project demonstrates the power of combining data structures, featuring two Binary Search Trees (BSTs) and multiple Linked Lists, all managed through a secure, multi-user teacher login system with data persistence.

---

## ✨ Key Features

* **Secure Teacher Portal:**
    * Teacher account creation with ID, name, and password.
    * Secure login system.
    * **3-Try Lockout:** Account is locked after 3 failed login attempts (simulates an SMS alert).
    * **Auto-Subject:** Each teacher's subject is stored, automatically tagging new marks.

* **Student Management (BST):**
    * Add, delete, and list all students.
    * All students are stored in a **Binary Search Tree** sorted by SRN for $O(\log n)$ search, insertion, and deletion.
    * Modify student names or SRNs (SRN modification rebuilds the tree node).

* **Performance Tracking (Linked Lists):**
    * Each student has their own **Singly Linked List** to store a dynamic, unlimited history of records.
    * Log all 5 mark components: Internal 1 (20), Assignment 1 (5), Internal 2 (20), Assignment 2 (5), and Semester Exam (100).
    * Modify existing marks for a specific student and subject.

* **Roll-Call Attendance:**
    * A function to traverse the student list (in sorted order) and log daily attendance (1 for Present, 0 for Absent) for the entire class.

* **Advanced Reporting:**
    * **Subject Report:** Generates a detailed report for one student in one subject, calculating the final mark (out of 100) based on all components.
    * **Master Report:** Dumps every single record (all subjects, all marks, all attendance) for a single student.

* **Data Persistence:**
    * All teacher and student data is saved to a local `data.dat` text file.
    * Data is **loaded** from the file on program start.
    * On logout, the teacher is **prompted to save** their changes, overwriting the file.

---

## 💡 Core Concept: Data Structure Design

The efficiency of this system comes from a three-tiered data structure design:

1.  **Teacher BST (Root 1):** The primary `TeacherNode* teacherRoot` is a BST that sorts teachers by their **Teacher ID**. This allows for fast, secure authentication.

2.  **Student BST (Root 2):** The primary `StudentNode* studentRoot` is a BST that sorts students by their **SRN** (a string). This allows for instant lookup of any student.

3.  **Performance Linked Lists (The History):** This is the key insight. **Every single `StudentNode` in the Student BST contains a pointer (`historyHead`) to the head of its own, separate Singly Linked List.** This list stores that student's performance records.

This hybrid approach gives us the best of all worlds:
* **Fast lookup** (BSTs for teachers and students).
* **Dynamic, unlimited storage** (Linked Lists for history).

📈 Usage Walkthrough (Tracing)
Here is a typical "trace" of a user session:

Program Start:

The program calls loadData().

It searches for data.dat. If found, it reads all teachers, students, and records, rebuilding the two BSTs and all linked lists in memory.

Login Menu (Loop 1):

User sees: (1) Login, (2) Create Account, (3) Exit.

User selects 2 (Create Account).

User enters ID: T-101, Name: Prof. Alex, Pass: pass123, Subject: C Programming.

The new TeacherNode is added to the teacherRoot BST.

Login:

User sees the login menu again.

User selects 1 (Login).

Tries ID: T-101, Pass: wrongpass. (Fails, failedAttempts = 1).

Tries ID: T-101, Pass: pass123. (Succeeds, failedAttempts = 0).

currentTeacher pointer is set to Prof. Alex's node. The main menu loop begins.

Main Menu (Loop 2):

User sees the main menu, which now says "Data Entry (Subject: C Programming)".

User selects 1 (Add New Student).

Enters SRN: R24EF062, Name: Student One.

A new StudentNode is added to the studentRoot BST.

Log Marks:

User selects 8 (Log Internal 1).

Program asks for SRN: R24EF062. The findStudent() function traverses the student BST.

Program asks for marks. User enters 18.

A new PerformanceNode is created with type: INTERNAL_1, value: 18, subject: C Programming (from currentTeacher->subject).

This node is added to the front of R24EF062's personal linked list.

Take Attendance:

User selects 7 (Take Class Attendance).

Program asks for a date: 2025-11-01.

The takeAttendance() function does an in-order traversal of the studentRoot BST.

It prints "Student: R24EF062 (Student One)". User enters 1.

A new PerformanceNode (type ATTENDANCE, value 1) is added to that student's list.

Generate Report:

User selects 13 (Generate Subject Report).

Asks for SRN: R24EF062.

Asks for Subject: User just hits Enter. The program auto-fills C Programming.

The generateSubjectReport() function traverses R24EF062's linked list, picking out only the "C Programming" marks.

It prints the full report, calculating the final grade (e.g., N/A because other marks are missing).

Logout & Save:

User selects 15 (Logout).

Program asks: "Do you want to save your changes? (y/n)". User types y.

The saveData() function is called.

It opens data.dat in "write" mode (overwriting).

It traverses the teacherRoot BST (pre-order) and writes "TEACHER", T-101, etc.

It traverses the studentRoot BST (pre-order) and writes "STUDENT", R24EF062, etc.

It then traverses that student's linked list, writing "RECORD", ATTENDANCE, etc.

It prints "All data saved." and returns to the Login Menu.

Exit:

User selects 3 (Exit) from the Login Menu.

The freeTree() and freeTeacherTree() functions are called, which recursively free all nodes and their linked lists (post-order traversal) to prevent memory leaks.

Program terminates.

---

## 📁 File Structure
Here is an overview of the project's file structure and the purpose of each file:
         
├── main.c\
├── data.dat\
├── student_tracker.h\
└── student_tracker.c


---
## ⚖️ License
This project is distributed under MIT License. Please see LICENSE file for details on permissions and limitations.
