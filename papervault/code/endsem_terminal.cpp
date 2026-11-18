#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

// EXAM PAPER REPOSITORY SYSTEM
// BASIC OOP VERSION
// Concepts intentionally used:
// 1. Classes and Objects
// 2. Encapsulation
// 3. Abstraction using an abstract class
// 4. Inheritance
// 5. Multiple inheritance
// 6. Polymorphism
// 7. Virtual functions
// 8. Virtual base class / diamond problem demonstration
// 9. Constructors
// 10. this pointer
// 11. Pointers to objects
// 12. Pointers to derived classes
// 13. Function overriding
// 14. Function overloading
// 15. Operator overloading
// 16. Friend function
// 17. Access specifiers
// 18. Multithreading
// 19. Thread creation using callable objects and lambda
// 20. Thread lifecycle, join, sleep and yield
// 21. Thread synchronization using mutex and lock_guard
// 22. Abstract Thread class and Runnable-style interface
//
// Intentionally NOT used as project topics:
// destructors, dynamic_cast, interfaces/realization, composition,
// aggregation, object equality, immutable classes, exceptions,
// file handling, RAII, smart pointers, repository/factory patterns,
// dependency injection, SOLID, and unit testing.

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int readInt(const string& prompt, int minValue, int maxValue) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minValue && value <= maxValue) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Invalid choice. Please enter a number from "
             << minValue << " to " << maxValue << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readText(const string& prompt) {
    string value;
    do {
        cout << prompt;
        getline(cin, value);
        if (value.empty()) cout << "Input cannot be empty. Try again.\n";
    } while (value.empty());
    return value;
}

// SUBJECT CLASS
// Encapsulation: private data + public member functions
class Subject {
private:
    string subjectCode;
    string subjectName;
    string branch;

public:
    Subject() : subjectCode(""), subjectName(""), branch("") {}

    Subject(string code, string name, string branchName)
        : subjectCode(code), subjectName(name), branch(branchName) {}

    string getSubjectCode() const { return subjectCode; }
    string getSubjectName() const { return subjectName; }
    string getBranch() const { return branch; }

    void display() const {
        cout << left << setw(12) << subjectCode
             << setw(28) << subjectName
             << setw(10) << branch << '\n';
    }
};

// EXAM PAPER CLASS
// Encapsulation + constructors + this pointer + friend function
// operator overloading
class ExamPaper {
private:
    int paperId;
    string subjectCode;
    string subjectName;
    string branch;
    int semester;
    string examType;
    int year;
    string fileName;
    string status;
    string uploadedBy;

public:
    ExamPaper()
        : paperId(0), subjectCode(""), subjectName(""), branch(""),
          semester(0), examType(""), year(0), fileName(""),
          status("Pending"), uploadedBy("") {}

    ExamPaper(int id, string code, string name, string branchName,
              int sem, string type, int academicYear,
              string file, string uploader, string paperStatus = "Pending") {
        // this pointer: clearly identifies the current object's data members
        this->paperId = id;
        this->subjectCode = code;
        this->subjectName = name;
        this->branch = branchName;
        this->semester = sem;
        this->examType = type;
        this->year = academicYear;
        this->fileName = file;
        this->uploadedBy = uploader;
        this->status = paperStatus;
    }

    // Getters demonstrate controlled access to private data.
    int getPaperId() const { return paperId; }
    string getSubjectCode() const { return subjectCode; }
    string getSubjectName() const { return subjectName; }
    string getBranch() const { return branch; }
    int getSemester() const { return semester; }
    string getExamType() const { return examType; }
    int getYear() const { return year; }
    string getFileName() const { return fileName; }
    string getStatus() const { return status; }
    string getUploadedBy() const { return uploadedBy; }

    void setStatus(const string& newStatus) {
        this->status = newStatus;
    }

    void setExamDetails(string type, int academicYear) {
        this->examType = type;
        this->year = academicYear;
    }

    void displayDetails() const {
               cout << "              PAPER DETAILS\n";
        cout << "Paper ID     : " << paperId << '\n';
        cout << "Subject Code : " << subjectCode << '\n';
        cout << "Subject      : " << subjectName << '\n';
        cout << "Branch       : " << branch << '\n';
        cout << "Semester     : " << semester << '\n';
        cout << "Exam Type    : " << examType << '\n';
        cout << "Year         : " << year << '\n';
        cout << "File Name    : " << fileName << '\n';
        cout << "Uploaded By  : " << uploadedBy << '\n';
        cout << "Status       : " << status << '\n';

    }

    // Friend function used for operator overloading.
    friend ostream& operator<<(ostream& os, const ExamPaper& paper);

    // Operator overloading using a member function.
    bool operator<(const ExamPaper& other) const {
        return year < other.year;
    }
};

// Friend operator<< has access to private ExamPaper data.
ostream& operator<<(ostream& os, const ExamPaper& paper) {
    os << left << setw(5) << paper.paperId
       << setw(20) << paper.subjectName
       << setw(14) << paper.examType
       << setw(10) << paper.branch
       << setw(8) << paper.semester
       << setw(8) << paper.year
       << setw(12) << paper.status;
    return os;
}

// ABSTRACT USER CLASS
// Abstraction + inheritance + virtual functions + polymorphism

class User {
private:
    string userId;
    string name;
    string email;
    string password;

protected:
    string getPassword() const { return password; }

public:
    User(string id, string userName, string userEmail, string userPassword)
        : userId(id), name(userName), email(userEmail), password(userPassword) {}

    string getUserId() const { return userId; }
    string getName() const { return name; }
    string getEmail() const { return email; }

    bool login(const string& enteredPassword) const {
        return enteredPassword == password;
    }

    void logout() const {
        cout << "\nLogged out successfully.\n";
    }

    // Pure virtual functions make User an abstract class.
    virtual string getRole() const = 0;
    virtual void showDashboard() = 0;
};


// STUDENT CLASS
// Single inheritance from User + overridden virtual functions

class Student : public User {
private:
    vector<int> downloadedPaperIds;
    vector<int> savedPaperIds;

public:
    Student(string id, string name, string email, string password)
        : User(id, name, email, password) {}

    string getRole() const override { return "Student"; }

    void showDashboard() override {
        cout << "             STUDENT DASHBOARD\n";

        cout << "Welcome, " << getName() << "!\n";
        cout << "1. Search Papers\n";
        cout << "2. Browse Subjects\n";
        cout << "3. My Downloads\n";
        cout << "4. Saved Papers\n";
        cout << "5. My Profile\n";
        cout << "6. Logout\n";
    }

    void downloadPaper(int paperId) {
        downloadedPaperIds.push_back(paperId);
        cout << "\nPaper " << paperId << " downloaded successfully!\n";
    }

    void savePaper(int paperId) {
        savedPaperIds.push_back(paperId);
        cout << "\nPaper " << paperId << " saved successfully!\n";
    }

    void showDownloads() const {
        
        if (downloadedPaperIds.empty()) {
            cout << "No downloaded papers yet.\n";
            return;
        }
        for (size_t i = 0; i < downloadedPaperIds.size(); ++i)
            cout << i + 1 << ". Paper ID: " << downloadedPaperIds[i] << '\n';
    }

    void showSavedPapers() const {
      
        if (savedPaperIds.empty()) {
            cout << "No saved papers yet.\n";
            return;
        }
        for (size_t i = 0; i < savedPaperIds.size(); ++i)
            cout << i + 1 << ". Paper ID: " << savedPaperIds[i] << '\n';
    }
};

// FACULTY CLASS
// Inheritance + polymorphism + method overloading

class Faculty : public User {
private:
    vector<int> uploadedPaperIds;

public:
    Faculty(string id, string name, string email, string password)
        : User(id, name, email, password) {}

    string getRole() const override { return "Faculty"; }

    void showDashboard() override {
                cout << "             FACULTY DASHBOARD\n";
       
        cout << "Welcome, " << getName() << "!\n";
        cout << "1. Search Papers\n";
        cout << "2. Upload Paper\n";
        cout << "3. My Uploaded Papers\n";
        cout << "4. Update My Paper\n";
        cout << "5. Request Paper Deletion\n";
        cout << "6. My Profile\n";
        cout << "7. Logout\n";
    }

    void addUploadedPaper(int paperId) {
        uploadedPaperIds.push_back(paperId);
    }

    void showUploadedPapers() const {
      
        if (uploadedPaperIds.empty()) {
            cout << "You have not uploaded any papers.\n";
            return;
        }
        for (size_t i = 0; i < uploadedPaperIds.size(); ++i)
            cout << i + 1 << ". Paper ID: " << uploadedPaperIds[i] << '\n';
    }

    // Function overloading: same function name, different parameter list.
    void updatePaper(int paperId) {
        cout << "Update requested for Paper ID: " << paperId << '\n';
    }

    void updatePaper(int paperId, const string& newExamType) {
        cout << "Paper " << paperId << " exam type changed to "
             << newExamType << ".\n";
    }
};

// ADMIN CLASS
// Inheritance + polymorphism
class Admin : public User {
public:
    Admin(string id, string name, string email, string password)
        : User(id, name, email, password) {}

    string getRole() const override { return "Admin"; }

    void showDashboard() override {
       
        cout << "Welcome, " << getName() << "!\n";
        cout << "1. Manage Users\n";
        cout << "2. Manage Papers\n";
        cout << "3. Verify Papers\n";
        cout << "4. Manage Subjects\n";
        cout << "5. Search All Papers\n";
        cout << "6. Sort Papers by Year\n";
        cout << "7. Reports\n";
        cout << "8. OOP Concepts Demonstration\n";
        cout << "9. My Profile\n";
        cout << "10. Logout\n";
    }
};

// VIRTUAL BASE CLASS / DIAMOND DEMONSTRATION
// This is kept separate from the User hierarchy so the main
// project remains simple.

class RepositoryItem {
protected:
    int itemId;
    string title;

public:
    RepositoryItem(int id = 0, string itemTitle = "") {
        this->itemId = id;
        this->title = itemTitle;
    }

    virtual void displayInfo() const {
        cout << "Repository Item: " << title << " (ID: " << itemId << ")\n";
    }
};

class PrintableItem : virtual public RepositoryItem {
protected:
    int pageCount;

public:
    PrintableItem(int id = 0, string title = "", int pages = 0)
        : RepositoryItem(id, title), pageCount(pages) {}

    void printPreview() const {
        cout << "Print Preview: " << pageCount << " pages\n";
    }
};

class DigitalItem : virtual public RepositoryItem {
protected:
    double fileSizeMB;

public:
    DigitalItem(int id = 0, string title = "", double size = 0.0)
        : RepositoryItem(id, title), fileSizeMB(size) {}

    void digitalInfo() const {
        cout << "Digital File Size: " << fileSizeMB << " MB\n";
    }
};

class DemoExamPaper : public PrintableItem, public DigitalItem {
public:
    DemoExamPaper(int id, string title, int pages, double size)
        : RepositoryItem(id, title),
          PrintableItem(id, title, pages),
          DigitalItem(id, title, size) {}

    void displayInfo() const override {
        cout << "\n=== VIRTUAL BASE CLASS DEMO ===\n";
        cout << "ID: " << itemId << '\n';
        cout << "Title: " << title << '\n';
        cout << "Pages: " << pageCount << '\n';
        cout << "File Size: " << fileSizeMB << " MB\n";
        cout << "RepositoryItem exists only once because of virtual inheritance.\n";
    }
};

// PAPER REPOSITORY DATA MANAGER
// This is a normal class, NOT the Repository Pattern.
// It simply stores papers in memory for this console project.

class PaperManager {
private:
    vector<ExamPaper> papers;
    int nextPaperId;

public:
    PaperManager() : nextPaperId(1001) {
        // Sample data makes the application immediately usable.
        papers.push_back(ExamPaper(
            nextPaperId++, "CS301", "Data Structures", "CSE", 3,
            "Mid Semester", 2025, "DS_MidSem_2025.pdf", "F014", "Verified"));
        papers.push_back(ExamPaper(
            nextPaperId++, "CS201", "Object Oriented Programming", "CSE", 4,
            "Mid Semester", 2026, "OOP_MidSem_2026.pdf", "F014", "Verified"));
        papers.push_back(ExamPaper(
            nextPaperId++, "CS202", "Database Management System", "CSE", 4,
            "End Semester", 2025, "DBMS_EndSem_2025.pdf", "F014", "Pending"));
        papers.push_back(ExamPaper(
            nextPaperId++, "EC201", "Digital Logic", "ECE", 3,
            "Mid Semester", 2026, "DL_MidSem_2026.pdf", "F014", "Verified"));
    }

    vector<ExamPaper>& getPapers() { return papers; }
    const vector<ExamPaper>& getPapers() const { return papers; }

    ExamPaper* findById(int id) {
        for (auto& paper : papers) {
            if (paper.getPaperId() == id) return &paper;
        }
        return nullptr;
    }

    void addPaper(const ExamPaper& paper) {
        papers.push_back(paper);
    }

    int generateId() { return nextPaperId++; }

    void displayHeader() const {
        cout << left << setw(5) << "ID"
             << setw(20) << "Subject"
             << setw(14) << "Exam Type"
             << setw(10) << "Branch"
             << setw(8) << "Sem"
             << setw(8) << "Year"
             << setw(12) << "Status" << '\n';
        cout << string(77, '-') << '\n';
    }

    void displayAll() const {
                if (papers.empty()) {
            cout << "No papers available.\n";
            return;
        }
        displayHeader();
        for (const auto& paper : papers) cout << paper << '\n';
    }

    vector<ExamPaper> search(const string& branch, int semester,
                             const string& subject, int year,
                             const string& examType) const {
        vector<ExamPaper> result;
        for (const auto& p : papers) {
            bool match = true;
            if (branch != "Any" && p.getBranch() != branch) match = false;
            if (semester != 0 && p.getSemester() != semester) match = false;
            if (subject != "Any" && p.getSubjectName() != subject) match = false;
            if (year != 0 && p.getYear() != year) match = false;
            if (examType != "Any" && p.getExamType() != examType) match = false;
            if (match) result.push_back(p);
        }
        return result;
    }

    void sortByYear() {
        sort(papers.begin(), papers.end(),
             [](const ExamPaper& a, const ExamPaper& b) {
                 return a.getYear() < b.getYear();
             });
    }
};


void showSearch(PaperManager& manager) {
    clearScreen();
   
    cout << "Available branches: CSE, ECE, IT\n";
    string branch = readText("Enter branch or Any: ");
    int semester = readInt("Enter semester (1-8, or 0 for Any): ", 0, 8);
    string subject = readText("Enter subject name or Any: ");
    int year = readInt("Enter year (2020-2030, or 0 for Any): ", 0, 2030);
    string examType = readText("Enter exam type or Any: ");

    vector<ExamPaper> results = manager.search(
        branch, semester, subject, year, examType);

    clearScreen();
    
    if (results.empty()) {
        cout << "No papers found matching your criteria.\n";
        pauseScreen();
        return;
    }

    cout << "Found " << results.size() << " paper(s).\n\n";
    manager.displayHeader();
    for (const auto& paper : results) cout << paper << '\n';

    int id = readInt("\nEnter Paper ID to open it (0 to go back): ", 0, 100000);
    if (id == 0) return;

    ExamPaper* selected = manager.findById(id);
    if (!selected) {
        cout << "Paper not found.\n";
        pauseScreen();
        return;
    }
    selected->displayDetails();
    pauseScreen();
}


void studentBrowseSubjects() {
    clearScreen();

    cout << left << setw(12) << "Code"
         << setw(28) << "Subject"
         << setw(10) << "Branch" << '\n';
    cout << string(50, '-') << '\n';

    Subject subjects[] = {
        Subject("CS201", "Object Oriented Programming", "CSE"),
        Subject("CS301", "Data Structures", "CSE"),
        Subject("CS202", "Database Management System", "CSE"),
        Subject("EC201", "Digital Logic", "ECE")
    };

    for (const auto& subject : subjects) subject.display();
    pauseScreen();
}

void studentProfile(const Student& student) {
    clearScreen();
    
    cout << "Role       : " << student.getRole() << '\n';
    cout << "User ID    : " << student.getUserId() << '\n';
    cout << "Name       : " << student.getName() << '\n';
    cout << "Email      : " << student.getEmail() << '\n';
    pauseScreen();
}

void studentPaperAction(PaperManager& manager, Student& student) {
    clearScreen();
    manager.displayAll();
    int id = readInt("\nEnter Paper ID (0 to go back): ", 0, 100000);
    if (id == 0) return;

    ExamPaper* paper = manager.findById(id);
    if (!paper) {
        cout << "Paper not found.\n";
        pauseScreen();
        return;
    }

    clearScreen();
    paper->displayDetails();
    cout << "1. Download Paper\n";
    cout << "2. Save Paper\n";
    cout << "3. Back\n";
    int choice = readInt("Enter choice: ", 1, 3);

    if (choice == 1) student.downloadPaper(id);
    else if (choice == 2) student.savePaper(id);
    else return;

    pauseScreen();
}

void studentDashboard(PaperManager& manager, Student& student) {
    int choice;
    do {
        clearScreen();
        student.showDashboard();
        choice = readInt("\nEnter choice: ", 1, 6);

        switch (choice) {
        case 1:
            showSearch(manager);
            break;
        case 2:
            studentBrowseSubjects();
            break;
        case 3:
            clearScreen();
            student.showDownloads();
            pauseScreen();
            break;
        case 4:
            clearScreen();
            student.showSavedPapers();
            pauseScreen();
            break;
        case 5:
            studentProfile(student);
            break;
        case 6:
            student.logout();
            pauseScreen();
            break;
        }

        // Give students an actual paper-selection path after the dashboard.
        // Search is already the primary path; direct download/view is shown here
        // as an additional operation after menu navigation.
        if (choice == 1) {
            cout << "\nSearch screen completed.\n";
        }
    } while (choice != 6);
}

void facultyProfile(const Faculty& faculty) {
    clearScreen();
 
    cout << "Role       : " << faculty.getRole() << '\n';
    cout << "User ID    : " << faculty.getUserId() << '\n';
    cout << "Name       : " << faculty.getName() << '\n';
    cout << "Email      : " << faculty.getEmail() << '\n';
    pauseScreen();
}

void facultyUpload(PaperManager& manager, Faculty& faculty) {
    clearScreen();
   
    string code = readText("Subject code: ");
    string name = readText("Subject name: ");
    string branch = readText("Branch: ");
    int semester = readInt("Semester (1-8): ", 1, 8);
    string examType = readText("Exam type: ");
    int year = readInt("Academic year: ", 2020, 2030);
    string fileName = readText("File name (example: OOP_MidSem_2026.pdf): ");

    int id = manager.generateId();
    ExamPaper paper(id, code, name, branch, semester, examType,
                    year, fileName, faculty.getUserId(), "Pending");
    manager.addPaper(paper);
    faculty.addUploadedPaper(id);

    cout << "\nPaper uploaded successfully!\n";
    cout << "Paper ID: " << id << '\n';
    cout << "Status: Pending admin verification\n";
    pauseScreen();
}

void facultyMyPapers(PaperManager& manager, Faculty& faculty) {
    clearScreen();
    faculty.showUploadedPapers();
    cout << "\nPapers currently stored under your user ID:\n";
    bool found = false;
    manager.displayHeader();
    for (const auto& paper : manager.getPapers()) {
        if (paper.getUploadedBy() == faculty.getUserId()) {
            cout << paper << '\n';
            found = true;
        }
    }
    if (!found) cout << "No papers found.\n";
    pauseScreen();
}

void facultyUpdatePaper(PaperManager& manager, Faculty& faculty) {
    clearScreen();
       bool found = false;
    manager.displayHeader();
    for (const auto& paper : manager.getPapers()) {
        if (paper.getUploadedBy() == faculty.getUserId()) {
            cout << paper << '\n';
            found = true;
        }
    }
    if (!found) {
        cout << "You have no papers to update.\n";
        pauseScreen();
        return;
    }

    int id = readInt("Enter your Paper ID (0 to go back): ", 0, 100000);
    if (id == 0) return;

    ExamPaper* paper = manager.findById(id);
    if (!paper || paper->getUploadedBy() != faculty.getUserId()) {
        cout << "You can update only your own paper.\n";
        pauseScreen();
        return;
    }

    string newType = readText("New exam type: ");
    int newYear = readInt("New academic year: ", 2020, 2030);
    paper->setExamDetails(newType, newYear);
    paper->setStatus("Pending");

    // Demonstrating overloaded Faculty::updatePaper functions.
    faculty.updatePaper(id);
    faculty.updatePaper(id, newType);

    cout << "Paper updated successfully.\n";
    cout << "Status changed to Pending for verification.\n";
    pauseScreen();
}

void facultyRequestDeletion(PaperManager& manager, Faculty& faculty) {
    clearScreen();
  
    manager.displayHeader();
    bool found = false;
    for (const auto& paper : manager.getPapers()) {
        if (paper.getUploadedBy() == faculty.getUserId()) {
            cout << paper << '\n';
            found = true;
        }
    }
    if (!found) {
        cout << "No papers available.\n";
        pauseScreen();
        return;
    }

    int id = readInt("Enter Paper ID to request deletion (0 to back): ", 0, 100000);
    if (id == 0) return;
    ExamPaper* paper = manager.findById(id);
    if (!paper || paper->getUploadedBy() != faculty.getUserId()) {
        cout << "Invalid paper selection.\n";
    } else {
        paper->setStatus("Deletion Requested");
        cout << "Deletion request sent to Admin.\n";
    }
    pauseScreen();
}

void facultyDashboard(PaperManager& manager, Faculty& faculty) {
    int choice;
    do {
        clearScreen();
        faculty.showDashboard();
        choice = readInt("\nEnter choice: ", 1, 7);

        switch (choice) {
        case 1:
            showSearch(manager);
            break;
        case 2:
            facultyUpload(manager, faculty);
            break;
        case 3:
            facultyMyPapers(manager, faculty);
            break;
        case 4:
            facultyUpdatePaper(manager, faculty);
            break;
        case 5:
            facultyRequestDeletion(manager, faculty);
            break;
        case 6:
            facultyProfile(faculty);
            break;
        case 7:
            faculty.logout();
            pauseScreen();
            break;
        }
    } while (choice != 7);
}

void adminProfile(const Admin& admin) {
    clearScreen();
 
    cout << "Role       : " << admin.getRole() << '\n';
    cout << "User ID    : " << admin.getUserId() << '\n';
    cout << "Name       : " << admin.getName() << '\n';
    cout << "Email      : " << admin.getEmail() << '\n';
    pauseScreen();
}

void adminManageUsers() {
    clearScreen();
    cout << "1. Student - Riya Sharma (S1023)\n";
    cout << "2. Faculty - Dr. A. Verma (F014)\n";
    cout << "3. Admin - Mr. R. Singh (A002)\n";
    cout << "\nSelect a user to view details.\n";
    int choice = readInt("Enter choice (1-3): ", 1, 3);
    if (choice == 1)
        cout << "\nStudent: Riya Sharma | S1023 | riya.sharma@college.edu\n";
    else if (choice == 2)
        cout << "\nFaculty: Dr. A. Verma | F014 | verma@college.edu\n";
    else
        cout << "\nAdmin: Mr. R. Singh | A002 | singh@college.edu\n";
    pauseScreen();
}

void adminVerifyPapers(PaperManager& manager) {
    clearScreen();
        manager.displayHeader();
    bool found = false;
    for (const auto& paper : manager.getPapers()) {
        if (paper.getStatus() == "Pending" ||
            paper.getStatus() == "Deletion Requested") {
            cout << paper << '\n';
            found = true;
        }
    }
    if (!found) {
        cout << "No pending papers.\n";
        pauseScreen();
        return;
    }

    int id = readInt("Enter Paper ID (0 to back): ", 0, 100000);
    if (id == 0) return;
    ExamPaper* paper = manager.findById(id);
    if (!paper) {
        cout << "Paper not found.\n";
        pauseScreen();
        return;
    }

    cout << "1. Verify\n2. Reject\n3. Back\n";
    int choice = readInt("Enter choice: ", 1, 3);
    if (choice == 1) {
        paper->setStatus("Verified");
        cout << "Paper verified successfully.\n";
    } else if (choice == 2) {
        paper->setStatus("Rejected");
        cout << "Paper rejected.\n";
    }
    pauseScreen();
}

void adminManagePapers(PaperManager& manager) {
    clearScreen();
   
    manager.displayAll();
    int id = readInt("\nEnter Paper ID to open (0 to back): ", 0, 100000);
    if (id == 0) return;
    ExamPaper* paper = manager.findById(id);
    if (!paper) {
        cout << "Paper not found.\n";
        pauseScreen();
        return;
    }
    paper->displayDetails();
    cout << "1. Verify\n2. Reject\n3. Mark Deletion Requested\n4. Back\n";
    int choice = readInt("Enter choice: ", 1, 4);
    if (choice == 1) {
        paper->setStatus("Verified");
        cout << "Paper verified.\n";
    } else if (choice == 2) {
        paper->setStatus("Rejected");
        cout << "Paper rejected.\n";
    } else if (choice == 3) {
        paper->setStatus("Deletion Requested");
        cout << "Paper marked for deletion.\n";
    }
    pauseScreen();
}

void adminManageSubjects() {
    clearScreen();
    
    cout << "1. CS201 - Object Oriented Programming - CSE\n";
    cout << "2. CS301 - Data Structures - CSE\n";
    cout << "3. CS202 - Database Management System - CSE\n";
    cout << "4. EC201 - Digital Logic - ECE\n";
    cout << "\nSubject list displayed successfully.\n";
    pauseScreen();
}

void adminReports(PaperManager& manager) {
    clearScreen();
    int verified = 0, pending = 0, rejected = 0, deletion = 0;
    for (const auto& paper : manager.getPapers()) {
        if (paper.getStatus() == "Verified") ++verified;
        else if (paper.getStatus() == "Pending") ++pending;
        else if (paper.getStatus() == "Rejected") ++rejected;
        else if (paper.getStatus() == "Deletion Requested") ++deletion;
    }

   
    cout << "Total Papers       : " << manager.getPapers().size() << '\n';
    cout << "Verified Papers    : " << verified << '\n';
    cout << "Pending Papers     : " << pending << '\n';
    cout << "Rejected Papers    : " << rejected << '\n';
    cout << "Deletion Requests  : " << deletion << '\n';
    pauseScreen();
}

void demonstrateEncapsulation() {
    clearScreen();
        ExamPaper paper(500, "CS201", "OOP", "CSE", 4,
                    "Mid Semester", 2026, "OOP.pdf", "F014");
    cout << "ExamPaper contains private data members.\n";
    cout << "They are accessed through public member functions.\n";
    paper.displayDetails();
    pauseScreen();
}

void demonstrateAbstractionAndPolymorphism() {
    clearScreen();
    
    Student student("S1023", "Riya Sharma", "riya@college.edu", "1234");
    Faculty faculty("F014", "Dr. A. Verma", "verma@college.edu", "1234");
    Admin admin("A002", "Mr. R. Singh", "singh@college.edu", "1234");

    // Base-class pointers point to different derived objects.
    User* users[] = {&student, &faculty, &admin};

    for (User* user : users) {
        cout << "\nRole through User pointer: " << user->getRole() << '\n';
        user->showDashboard();
    }

    cout << "\nSame virtual call, different behavior = runtime polymorphism.\n";
    pauseScreen();
}

void demonstratePointers() {
    clearScreen();
 

    ExamPaper paper(501, "CS301", "Data Structures", "CSE", 3,
                    "Mid Semester", 2026, "DS.pdf", "F014", "Verified");

    // Pointer to object.
    ExamPaper* paperPointer = &paper;

    cout << "Address stored in pointer: " << paperPointer << '\n';
    cout << "Accessing object through pointer:\n";
    paperPointer->displayDetails();

    // Pointer to derived object through base class pointer.
    Student student("S1023", "Riya Sharma", "riya@college.edu", "1234");
    User* basePointer = &student;
    cout << "Base pointer is pointing to Student object.\n";
    cout << "Role: " << basePointer->getRole() << '\n';
    pauseScreen();
}

void demonstrateThisPointer() {
    clearScreen();
   
    cout << "The this pointer stores the address of the current object.\n";
    cout << "It is useful when constructor parameter names and member names are same.\n\n";

    ExamPaper paper(502, "CS202", "DBMS", "CSE", 4,
                    "End Semester", 2026, "DBMS.pdf", "F014");
    cout << "Current ExamPaper object address: " << &paper << '\n';
    cout << "Constructor used this->member = parameter.\n";
    paper.displayDetails();
    pauseScreen();
}

void demonstrateOperatorOverloading() {
    clearScreen();
    
    ExamPaper paper(503, "CS201", "OOP", "CSE", 4,
                    "Mid Semester", 2026, "OOP.pdf", "F014", "Verified");

    cout << "Friend operator<< output:\n";
    cout << paper << '\n';

    ExamPaper older(504, "CS301", "DSA", "CSE", 3,
                    "Final", 2025, "DSA.pdf", "F014", "Verified");

    if (older < paper)
        cout << "operator< says the DSA paper is older than the OOP paper.\n";

    pauseScreen();
}

void demonstrateConstructors() {
    clearScreen();
   
    ExamPaper defaultPaper;
    cout << "1. Default constructor created a paper object.\n";
    defaultPaper.displayDetails();

    ExamPaper parameterPaper(505, "EC201", "Digital Logic", "ECE", 3,
                             "Mid Semester", 2026, "DL.pdf", "F014");
    cout << "2. Parameterized constructor created a fully initialized object.\n";
    parameterPaper.displayDetails();
    pauseScreen();
}

void demonstrateVirtualBaseClass() {
    clearScreen();
    DemoExamPaper paper(700, "Virtual Inheritance Demo", 10, 3.5);
    RepositoryItem* basePointer = &paper;
    basePointer->displayInfo();
    paper.printPreview();
    paper.digitalInfo();
    pauseScreen();
}

void runTypeCastingDemo();
void runTemplateDemo();
void runExceptionDemo();
void runMultithreadingDemo();

void oopDemoMenu() {
    int choice;
    do {
        clearScreen();
        cout << "========== OOP CONCEPTS ==========" << '\n';
        cout << "1. Encapsulation\n";
        cout << "2. Abstraction & Polymorphism\n";
        cout << "3. Inheritance\n";
        cout << "4. this Pointer\n";
        cout << "5. Pointer to Object / Derived Object\n";
        cout << "6. Virtual Function\n";
        cout << "7. Virtual Base Class / Diamond Problem\n";
        cout << "8. Function Overloading\n";
        cout << "9. Friend Operator Overloading\n";
        cout << "10. Constructors\n";
        cout << "11. Type Casting\n";
        cout << "12. Templates\n";
        cout << "13. Exception Handling\n";
        cout << "14. Multithreading\n";
        cout << "15. Back\n";

        choice = readInt("Enter choice: ", 1, 15);
        switch (choice) {
        case 1: demonstrateEncapsulation(); break;
        case 2: demonstrateAbstractionAndPolymorphism(); break;
        case 3:
            clearScreen();
            cout << "User is the base class.\n";
            cout << "Student, Faculty and Admin inherit from User.\n\n";
            cout << "              User\n";
            cout << "          /     |     \\\n";
            cout << "     Student  Faculty  Admin\n";
            pauseScreen();
            break;
        case 4: demonstrateThisPointer(); break;
        case 5: demonstratePointers(); break;
        case 6: demonstrateAbstractionAndPolymorphism(); break;
        case 7: demonstrateVirtualBaseClass(); break;
        case 8:
            clearScreen();
            cout << "Faculty has updatePaper(int) and updatePaper(int, string).\n";
            cout << "Same function name with different parameters.\n";
            pauseScreen();
            break;
        case 9: demonstrateOperatorOverloading(); break;
        case 10: demonstrateConstructors(); break;
        case 11: runTypeCastingDemo(); break;
        case 12: runTemplateDemo(); break;
        case 13: runExceptionDemo(); break;
        case 14: runMultithreadingDemo(); break;
        case 15: break;
        }
    } while (choice != 15);
}

// ---------- TYPE CASTING ----------
class PaperNumber {
public:
    int value;
    PaperNumber(int v = 0) : value(v) {}
    operator int() const { return value; } // Class-to-basic
};

class PaperInfo {
public:
    int id;
    PaperInfo(int v = 0) : id(v) {}
};

class PaperRecord {
public:
    int paperId;
    PaperRecord(PaperInfo info) : paperId(info.id) {} // Class-to-class
};

void runTypeCastingDemo() {
    clearScreen();
    cout << "TYPE CASTING\n\n";
    PaperNumber paper = 501; // Basic-to-class: int to PaperNumber
    cout << "Basic-to-class: int 501 to PaperNumber = " << paper.value << '\n';

    int paperId = paper; // Class-to-basic: PaperNumber to int
    cout << "Class-to-basic: PaperNumber to int = " << paperId << '\n';

    PaperInfo info(502);
    PaperRecord record = info; // Class-to-class
    cout << "Class-to-class: PaperInfo to PaperRecord, ID = "
         << record.paperId << '\n';
    pauseScreen();
}

// ---------- TEMPLATES ----------
template <typename T>
T showLatestValue(T value) {
    return value;
}

void runTemplateDemo() {
    clearScreen();
    cout << "TEMPLATE FUNCTION\n\n";
    cout << "Integer value: " << showLatestValue<int>(501) << '\n';
    cout << "String value: " << showLatestValue<string>("DAA Paper") << '\n';
    cout << "The same function works with different data types.\n";
    pauseScreen();
}

// ---------- EXCEPTION HANDLING ----------
class PaperException : public exception {
public:
    const char* what() const noexcept override {
        return "Paper record could not be found.";
    }
};

class InvalidPaperId : public PaperException {
public:
    const char* what() const noexcept override {
        return "Invalid paper ID.";
    }
};

void checkPaperId(int id) {
    if (id <= 0) throw InvalidPaperId();
}

void runExceptionDemo() {
    clearScreen();
    cout << "EXCEPTION HANDLING\n\n";
    try {
        int id;
        cout << "Enter paper ID (positive number): ";
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        checkPaperId(id);
        cout << "Paper ID accepted.\n";
    }
    catch (const InvalidPaperId& e) { // Derived exception first
        cout << "Derived exception: " << e.what() << '\n';
    }
    catch (const PaperException& e) { // Base exception after derived
        cout << "Base exception: " << e.what() << '\n';
    }
    catch (const exception& e) {
        cout << "Standard exception: " << e.what() << '\n';
    }
    cout << "Finally-style cleanup: exception handling section finished.\n";
    cout << "C++ has no finally/throws keywords; throw and catch are used here.\n";
    pauseScreen();
}

// ---------- MULTITHREADING ----------
mutex repositoryMutex;

class RunnableTask {
public:
    virtual void run() = 0;
    virtual ~RunnableTask() = default;
};

class DownloadTask : public RunnableTask {
public:
    void run() override {
        lock_guard<mutex> lock(repositoryMutex);
        cout << "Runnable task: downloading paper... Thread ID: "
             << this_thread::get_id() << '\n';
    }
};

class ThreadTask {
private:
    thread worker;
public:
    virtual void run() = 0;
    void start() { worker = thread(&ThreadTask::run, this); }
    void join() { if (worker.joinable()) worker.join(); }
    virtual ~ThreadTask() = default;
};

class UploadTask : public ThreadTask {
public:
    void run() override {
        lock_guard<mutex> lock(repositoryMutex);
        cout << "Extended Thread class: uploading paper... Thread ID: "
             << this_thread::get_id() << '\n';
    }
};

void runMultithreadingDemo() {
    clearScreen();
    cout << "MULTITHREADING\n\n";

    UploadTask upload;
    DownloadTask download;

    cout << "Thread life cycle: NEW -> RUNNING -> TERMINATED\n";
    upload.start(); // start() creates a new thread; run() executes in it
    thread downloadThread(&DownloadTask::run, &download); // Runnable-style task

    this_thread::sleep_for(chrono::milliseconds(100)); // sleep()
    cout << "Main thread yields execution.\n";
    this_thread::yield(); // yield()

    upload.join(); // join() waits for upload thread
    downloadThread.join();
    cout << "Both threads completed.\n";
    cout << "Mutex provides synchronization for shared output.\n";
    cout << "Note: C++ uses std::thread; it has no built-in Java Thread/Runnable classes.\n";
    pauseScreen();
}

bool loginUser(User& user) {
    clearScreen();
    cout << "============================================\n";
    cout << "              " << user.getRole() << " LOGIN\n";
    cout << "============================================\n";
    cout << "User ID: " << user.getUserId() << '\n';
    string password = readText("Password: ");

    if (user.login(password)) {
        cout << "\nLogin successful!\n";
        cout << "Logged in as: " << user.getRole() << '\n';
        pauseScreen();
        return true;
    }

    cout << "\nIncorrect password.\n";
    pauseScreen();
    return false;
}

void loginStudent(PaperManager& manager) {
    Student student("S1023", "Riya Sharma", "riya.sharma@college.edu", "1234");
    if (loginUser(student)) studentDashboard(manager, student);
}

void loginFaculty(PaperManager& manager) {
    Faculty faculty("F014", "Dr. A. Verma", "verma@college.edu", "1234");
    if (loginUser(faculty)) facultyDashboard(manager, faculty);
}

void loginAdmin(PaperManager& manager) {
    Admin admin("A002", "Mr. R. Singh", "singh@college.edu", "1234");
    if (loginUser(admin)) {
        int choice;
        do {
            clearScreen();
            admin.showDashboard();
            choice = readInt("\nEnter choice: ", 1, 10);

            switch (choice) {
            case 1:
                adminManageUsers();
                break;
            case 2:
                adminManagePapers(manager);
                break;
            case 3:
                adminVerifyPapers(manager);
                break;
            case 4:
                adminManageSubjects();
                break;
            case 5:
                showSearch(manager);
                break;
            case 6:
                clearScreen();
                manager.sortByYear();
                cout << "Papers sorted by academic year.\n";
                manager.displayAll();
                pauseScreen();
                break;
            case 7:
                adminReports(manager);
                break;
            case 8:
                oopDemoMenu();
                break;
            case 9:
                adminProfile(admin);
                break;
            case 10:
                admin.logout();
                pauseScreen();
                break;
            }
        } while (choice != 10);
    }
}


int main() {
    PaperManager manager;

    int choice;
    do {
        clearScreen();
   
        cout << "A console-based OOP project for Students, Faculty and Admin\n\n";
        cout << "1. Student Login\n";
        cout << "2. Faculty Login\n";
        cout << "3. Admin Login\n";
        cout << "4. Exit\n";

        choice = readInt("\nEnter choice: ", 1, 4);

        switch (choice) {
        case 1:
            loginStudent(manager);
            break;
        case 2:
            loginFaculty(manager);
            break;
        case 3:
            loginAdmin(manager);
            break;
        case 4:
            clearScreen();
                     break;
        }
    } while (choice != 4);

    return 0;
}