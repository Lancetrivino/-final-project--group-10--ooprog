#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include <map>
#include <limits>

using namespace std;

class InvalidCourseIndexException : public runtime_error {
public:
    InvalidCourseIndexException() : runtime_error("Invalid course index!") {}
};
class Validator {
public:
    static bool isValidEmail(const string& email) {
        // Basic email validation
        size_t atPos = email.find('@');
        size_t dotPos = email.rfind('.');
        return atPos != string::npos && dotPos != string::npos && 
               atPos < dotPos && atPos > 0 && 
               dotPos < email.length() - 1;
    }

    static bool isValidGrade(int grade) {
        return grade >= 0 && grade <= 100;
    }

    static bool isValidIndex(int index, int maxSize) {
        return index >= 0 && index < maxSize;
    }

    static bool isValidString(const string& str) {
        return !str.empty() && str.length() <= 100;  // Arbitrary max length
    }

    static int getValidatedIntInput(const string& prompt, int min, int max) {
        int input;
        bool validInput = false;
        do {
            cout << prompt;
            if (cin >> input) {
                if (input >= min && input <= max) {
                    validInput = true;
                } else {
                    cout << "Please enter a number between " << min << " and " << max << ".\n";
                }
            } else {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number.\n";
                
            }
        } while (!validInput);
        return input;
    }
};
// Base User class
class User {
protected:
    string username;
    string email;
    string password;
public:
    User(string username, string email, string password)
        : username(username), email(email), password(password) {}

    virtual void displayMenu() = 0;  // Abstract method
    string getPassword() { return password; }
    string getEmail() { return email; }
};

using UserPtr = shared_ptr<User>;
vector<UserPtr> users; 

// Forward declarations
class Course;
class LMSManager;

// Admin class
class Admin : public User {
public:
    Admin(string username, string email, string password)
        : User(username, email, password) {}

    void displayMenu() override;
    void manageCourses();
    void addCourse();
    void deleteCourse();
    void editCourse();
    void viewReports();
    void enrollStudent();    
    void removeStudent();   
};

// Teacher class
class Teacher : public User {
public:
    Teacher(string username, string email, string password)
        : User(username, email, password) {}

    void displayMenu() override;
    void manageCourses();
    void viewCourse();
    void viewReports();
    void addGrade();  // Keep only one implementation of adding grades
};

// Student class
class Student : public User {
public:
    Student(string username, string email, string password)
        : User(username, email, password) {}

    void displayMenu() override;
    void viewEnrolledCourses();
    void viewGrades();
    void enrollInCourse();
};

// Course class
class Course {
private:
    string courseName;
    string teacherEmail;
    vector<string> contents;
    vector<pair<string, int>> grades;  // student email and grade
    vector<string> enrolledStudents;

public:
    Course(string courseName, string teacherEmail)
        : courseName(courseName), teacherEmail(teacherEmail) {}

    void addContent(string content) {
        if (!Validator::isValidString(content)) {
            throw runtime_error("Invalid content: Content cannot be empty or too long");
        }
        contents.push_back(content);
    }

    void removeContent(int index) {
        if (index < 0 || index >= contents.size()) {
            throw InvalidCourseIndexException();
        }
        contents.erase(contents.begin() + index);
    }

    void displayContents() {
        for (size_t i = 0; i < contents.size(); ++i) {
            cout << i << ": " << contents[i] << endl;
        }
    }

    void addGrade(string studentEmail, int grade) {
        grades.push_back({studentEmail, grade});
    }

    vector<pair<string, int>>& getGrades() {  // Return reference to grades
        return grades;
    }

    void displayGrades() {
        for (auto& grade : grades) {
            cout << grade.first << ": " << grade.second << "%" << endl;
        }
    }

    void enrollStudent(string studentEmail) {
        enrolledStudents.push_back(studentEmail);
    }

    void removeStudent(string studentEmail) {
        for (auto it = enrolledStudents.begin(); it != enrolledStudents.end(); ++it) {
            if (*it == studentEmail) {
                enrolledStudents.erase(it);
                break;
            }
        }
    }

    void displayStudents() {
        for (auto& student : enrolledStudents) {
            cout << student << endl;
        }
    }

    string getCourseName() { return courseName; }
    string getTeacherEmail() { return teacherEmail; }

    vector<string>& getStudents() {
    return enrolledStudents;
}

    const vector<string>& getContents() const {
        return contents;
    }
};


// LMSManager class (Singleton)
class LMSManager {
private:
    vector<Course> courses;
    static LMSManager* instance;
    LMSManager() {}

public:
    static LMSManager* getInstance() {
        if (!instance) {
            instance = new LMSManager();
        }
        return instance;
    }

    // Add this static cleanup method
    static void cleanup() {
        delete instance;
        instance = nullptr;
    }

    void addCourse(Course course) {
        courses.push_back(course);
    }

    Course& getCourse(int index) {
        if (index < 0 || index >= courses.size()) {
            throw InvalidCourseIndexException();
        }
        return courses[index];
    }

    void removeCourse(int index) {
        if (index < 0 || index >= courses.size()) {
            throw InvalidCourseIndexException();
        }
        courses.erase(courses.begin() + index);
    }

    void displayCourses() {
    if (courses.empty()) {
        cout << "There are no courses available.\n";
        return;
    }

    for (size_t i = 0; i < courses.size(); ++i) {
        cout << i + 1 << ": " << courses[i].getCourseName()
             << " (Teacher: " << courses[i].getTeacherEmail() << ")" << endl;
    }
}


    vector<Course>& getCourses() {
        return courses;
    }
};

// Initialize static member of LMSManager
LMSManager* LMSManager::instance = nullptr;

// Admin class implementation
void Admin::displayMenu() {
    int choice;
    do {
        system("cls");
        cout << "\nAdmin Menu:\n";
        cout << "1. Manage Courses\n";
        cout << "2. View Reports\n";
        cout << "3. Enroll Student\n";
        cout << "4. Remove Student\n";
        cout << "5. Log Out\n";
        
        choice = Validator::getValidatedIntInput("Enter choice (1-5): ", 1, 5);

        switch (choice) {
            case 1:
                manageCourses();
                break;
            case 2:
                viewReports();
                break;
            case 3:
                enrollStudent();
                break;
            case 4:
                removeStudent();
                system("pause");
                break;
            case 5:
                cout << "Logging out...\n";
                system("pause");
                break;
        }
    } while (choice != 5);
}
void Admin::enrollStudent() {
    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    if (courses.empty()) {
        cout << "There are no courses available for enrollment.\n";
        return;
    }

    LMSManager::getInstance()->displayCourses();
    int userIndex = Validator::getValidatedIntInput(
        "Enter course index to enroll student (1-" + to_string(courses.size()) + "): ",
        1, courses.size());

    try {
        Course& course = LMSManager::getInstance()->getCourse(userIndex - 1);
        
        string studentEmail;
        bool validEmail = false;
        do {
            cout << "Enter student's email: ";
            cin >> studentEmail;
            if (Validator::isValidEmail(studentEmail)) {
                validEmail = true;
            } else {
                cout << "Invalid email format. Please try again.\n";
            }
        } while (!validEmail);
        
        course.enrollStudent(studentEmail);
        cout << "Student enrolled successfully.\n";
    } catch (const exception& e) {
        cout << e.what() << endl;
    }
}

void Admin::removeStudent() {
    // Check if there are courses available
    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    if (courses.empty()) {
        cout << "There are no courses available.\n";
        return;
    }

    LMSManager::getInstance()->displayCourses();
    int userIndex;
    cout << "Enter course index to remove student (1-" << courses.size() << "): ";
    cin >> userIndex;

    try {
        // Convert 1-based user input to 0-based index
        int systemIndex = userIndex - 1;
        Course& course = LMSManager::getInstance()->getCourse(systemIndex);

        // Check if the course has any students
        if (course.getStudents().empty()) {
            cout << "There is no student here.\n";
            return;
        }

        string studentEmail;
        cout << "Enter student's email to remove: ";
        cin >> studentEmail;

        try {
            course.removeStudent(studentEmail);
            cout << "Student removed successfully.\n";
        } catch (const runtime_error&) {
            cout << "Student not found in the course.\n";
        }
    } catch (InvalidCourseIndexException&) {
        cout << "Invalid course index. Please enter a number between 1 and " 
             << courses.size() << ".\n";
    }
}

void Admin::manageCourses() {
    int choice;
    do {
        system("cls");
        cout << "\nManage Courses:\n";
        cout << "1. Add Course\n";
        cout << "2. Delete Course\n";
        cout << "3. Edit Course\n";
        cout << "4. Display Courses\n";
        cout << "5. Back\n";

        choice = Validator::getValidatedIntInput("Enter choice (1-5): ", 1, 5);

        switch (choice) {
            case 1:
                addCourse();
                break;
            case 2:
                deleteCourse();
                break;
            case 3:
                editCourse();
                break;
            case 4:
                LMSManager::getInstance()->displayCourses();
                system("pause");
                break;
            case 5:
                cout << "Returning...\n";
                system("pause");
                break;
        }
    } while (choice != 5);
}

void Admin::addCourse() {
    system("cls");  // Clear screen
    string courseName, teacherEmail;
    cout << "Enter course name: ";
    cin.ignore();
    getline(cin, courseName);
    cout << "Enter teacher's email: ";
    cin >> teacherEmail;

    Course newCourse(courseName, teacherEmail);
    LMSManager::getInstance()->addCourse(newCourse);
    cout << "Course added successfully.\n";
    system("pause");  // Pause the console to see the message
}

void Admin::deleteCourse() {
    // Check if there are any courses
    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    if (courses.empty()) {
        cout << "There are no courses to delete.\n";
        system("pause");  // Wait for the user to see the message
        return;  // Exit the function if there are no courses
    }

    // Display the list of courses if there are courses
    LMSManager::getInstance()->displayCourses();
    int index;
    cout << "Enter course index to delete: ";
    cin >> index;

    try {
        LMSManager::getInstance()->removeCourse(index);
        cout << "Course deleted successfully.\n";
    } catch (InvalidCourseIndexException&) {
        cout << "Invalid course index.\n";
    }
}

void Admin::editCourse() {
    system("cls");  // Clear screen

    // Check if there are courses available
    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    if (courses.empty()) {
        cout << "There are no courses available.\n";
        system("pause");  // Wait for user to see the message
        return;  // Exit the function if there are no courses
    }

    // If there are courses, proceed with editing
    LMSManager::getInstance()->displayCourses();
    int userIndex;
    cout << "Enter course index to edit (1-" << courses.size() << "): ";
    cin >> userIndex;

    try {
        // Convert 1-based user input to 0-based index
        int systemIndex = userIndex - 1;
        Course& course = LMSManager::getInstance()->getCourse(systemIndex);
        cout << "Editing course: " << course.getCourseName() << endl;
        
        cout << "Would you like to edit the course content? (y/n): ";
        char choice;
        cin >> choice;
        
        if (tolower(choice) == 'y') {
            int contentChoice;
            cout << "1. Add content\n2. Remove content\nEnter choice: ";
            cin >> contentChoice;

            if (contentChoice == 1) {
                string content;
                cout << "Enter content: ";
                cin.ignore();
                getline(cin, content);
                course.addContent(content);
                cout << "Content added successfully.\n";
            } 
            else if (contentChoice == 2) {
                // Check if there's any content to remove
                vector<string> contents = course.getContents();
                if (contents.empty()) {
                    cout << "There is no content to remove.\n";
                } 
                else {
                    // Display current content with 1-based indexing
                    cout << "\nCurrent content:\n";
                    for (size_t i = 0; i < contents.size(); i++) {
                        cout << i + 1 << ". " << contents[i] << endl;
                    }

                    int userContentIndex;
                    cout << "Enter content index to remove (1-" << contents.size() << "): ";
                    cin >> userContentIndex;

                    try {
                        // Convert to 0-based index for internal use
                        course.removeContent(userContentIndex - 1);
                        cout << "Content removed successfully.\n";
                    } 
                    catch (const out_of_range&) {
                        cout << "Invalid content index. Please enter a number between 1 and " 
                             << contents.size() << ".\n";
                    }
                }
            } 
            else {
                cout << "Invalid choice. Please select 1 or 2.\n";
            }
        }
    } 
    catch (InvalidCourseIndexException&) {
        cout << "Invalid course index. Please enter a number between 1 and " 
             << courses.size() << ".\n";
    }
    
    system("pause");  // Pause the console to see the message
}


void Admin::viewReports() {
    system("cls");  // Clear screen
    vector<Course>& courses = LMSManager::getInstance()->getCourses();

    if (courses.empty()) {
        cout << "No courses available to generate reports.\n";
        system("pause");
        return;
    }

    cout << "Courses Report:\n";
    for (auto& course : courses) {
        cout << "Course: " << course.getCourseName() << " (Teacher: " << course.getTeacherEmail() << ")\n";
        cout << "Enrolled Students:\n";
        course.displayStudents();
        cout << "Grades:\n";
        course.displayGrades();
        cout << "----------------------\n";
    }
    system("pause");
}

// Teacher class implementation
void Teacher::displayMenu() {
    int choice;
    do {
        system("cls");
        cout << "\nTeacher Menu:\n";
        cout << "1. Manage Courses\n";
        cout << "2. View Reports\n";
        cout << "3. Add Grade\n";
        cout << "4. Log Out\n";

        choice = Validator::getValidatedIntInput("Enter choice (1-4): ", 1, 4);

        switch (choice) {
            case 1:
                manageCourses();
                break;
            case 2:
                viewReports();
                break;
            case 3:
                addGrade();
                break;
            case 4:
                cout << "Logging out...\n";
                system("pause");
                break;
        }
    } while (choice != 4);
}

void Teacher::addGrade() {
    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    if (courses.empty()) {
        cout << "No courses available.\n";
        return;
    }

    LMSManager::getInstance()->displayCourses();
    int courseIndex = Validator::getValidatedIntInput(
        "Enter course index (1-" + to_string(courses.size()) + "): ",
        1, courses.size());

    try {
        Course& course = LMSManager::getInstance()->getCourse(courseIndex - 1);
        
        string studentEmail;
        bool validEmail = false;
        do {
            cout << "Enter student's email: ";
            cin >> studentEmail;
            if (Validator::isValidEmail(studentEmail)) {
                validEmail = true;
            } else {
                cout << "Invalid email format. Please try again.\n";
            }
        } while (!validEmail);

        // Validate student enrollment
        bool studentFound = false;
        for (const string& student : course.getStudents()) {
            if (student == studentEmail) {
                studentFound = true;
                break;
            }
        }

        if (!studentFound) {
            cout << "Student is not enrolled in this course.\n";
            return;
        }

        int grade = Validator::getValidatedIntInput(
            "Enter grade (0-100): ",
            0, 100);

        course.addGrade(studentEmail, grade);
        cout << "Grade added successfully for student: " << studentEmail << endl;
    } catch (const exception& e) {
        cout << e.what() << endl;
    }
}

void Teacher::manageCourses() {
    int choice;
    do {
        system("cls");
        cout << "\nManage Courses:\n";
        cout << "1. View Course\n";
        cout << "2. Add Content\n";
        cout << "3. Add Grade\n";
        cout << "4. Display Students\n";
        cout << "5. Back\n";

        choice = Validator::getValidatedIntInput("Enter choice (1-5): ", 1, 5);

        switch (choice) {
            case 1:
                viewCourse();
                break;
            case 2: {
                LMSManager::getInstance()->displayCourses();
                vector<Course>& courses = LMSManager::getInstance()->getCourses();
                if (courses.empty()) {
                    cout << "No courses available.\n";
                    system("pause");
                    break;
                }
                int index = Validator::getValidatedIntInput(
                    "Enter course index (1-" + to_string(courses.size()) + "): ",
                    1, courses.size());
                
                try {
                    Course& course = LMSManager::getInstance()->getCourse(index - 1);
                    string content;
                    cout << "Enter the content to add: ";
                    cin.ignore();
                    getline(cin, content);
                    course.addContent(content);
                    cout << "Content added to the course: " << course.getCourseName() << endl;
                    system("pause");
                } catch (const exception& e) {
                    cout << e.what() << endl;
                    system("pause");
                }
                break;
            }
            case 3:
                addGrade();
                break;
            case 4: {
                LMSManager::getInstance()->displayCourses();
                vector<Course>& courses = LMSManager::getInstance()->getCourses();
                if (courses.empty()) {
                    cout << "No courses available.\n";
                    system("pause");
                    break;
                }
                int index = Validator::getValidatedIntInput(
                    "Enter course index (1-" + to_string(courses.size()) + "): ",
                    1, courses.size());
                
                try {
                    Course& course = LMSManager::getInstance()->getCourse(index - 1);
                    course.displayStudents();
                    system("pause");
                } catch (const exception& e) {
                    cout << e.what() << endl;
                    system("pause");
                }
                break;
            }
            case 5:
                cout << "Returning...\n";
                system("pause");
                break;
        }
    } while (choice != 5);
}


void Teacher::viewCourse() {
    system("cls");  // Clear screen
    LMSManager::getInstance()->displayCourses();
    int index;
    cout << "Enter course index to view: ";
    cin >> index;

    try {
        Course& course = LMSManager::getInstance()->getCourse(index);
        cout << "Viewing course: " << course.getCourseName() << endl;
        course.displayContents();
        system("pause");  // Wait for the user to see the course contents
    } catch (InvalidCourseIndexException&) {
        cout << "Invalid course index.\n";
    }
}

void Teacher::viewReports() {
    system("cls");  // Clear screen
    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    string teacherEmail = getEmail();

    bool hasCourses = false;
    cout << "Courses Report for " << teacherEmail << ":\n";
    for (auto& course : courses) {
        if (course.getTeacherEmail() == teacherEmail) {
            hasCourses = true;
            cout << "Course: " << course.getCourseName() << "\n";
            cout << "Enrolled Students:\n";
            course.displayStudents();
            cout << "Grades:\n";
            course.displayGrades();
            cout << "----------------------\n";
        }
    }

    if (!hasCourses) {
        cout << "No courses assigned to you.\n";
    }
    system("pause");
}

// Student class implementation
void Student::displayMenu() {
    int choice;
    do {
        system("cls");
        cout << "\nStudent Menu:\n";
        cout << "1. View Enrolled Courses\n";
        cout << "2. View Grades\n";
        cout << "3. Enroll in a Course\n";
        cout << "4. Log Out\n";

        choice = Validator::getValidatedIntInput("Enter choice (1-4): ", 1, 4);

        switch (choice) {
            case 1:
                viewEnrolledCourses();
                break;
            case 2:
                viewGrades();
                break;
            case 3:
                enrollInCourse();
                break;
            case 4:
                cout << "Logging out...\n";
                system("pause");
                break;
        }
    } while (choice != 4);
}

void Student::enrollInCourse() {
    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    if (courses.empty()) {
        cout << "No courses available for enrollment.\n";
        return;
    }

    LMSManager::getInstance()->displayCourses();
    int courseIndex = Validator::getValidatedIntInput(
        "Enter course index (1-" + to_string(courses.size()) + "): ",
        1, courses.size());

    try {
        Course& course = LMSManager::getInstance()->getCourse(courseIndex - 1);
        
        // Check if already enrolled
        for (const string& student : course.getStudents()) {
            if (student == email) {
                cout << "You are already enrolled in this course.\n";
                return;
            }
        }

        course.enrollStudent(email);
        cout << "Successfully enrolled in the course.\n";
    } catch (const exception& e) {
        cout << e.what() << endl;
    }
}

void Student::viewGrades() {
    LMSManager::getInstance()->displayCourses();
    int index;
    cout << "Enter course index to view grades: ";
    cin >> index;

    try {
        Course& course = LMSManager::getInstance()->getCourse(index);
        course.displayGrades();
    } catch (InvalidCourseIndexException&) {
        cout << "Invalid course index.\n";
    }
}
void Student::viewEnrolledCourses() {
    LMSManager::getInstance()->displayCourses();
    int index;
    cout << "Enter course index to view content: ";
    cin >> index;

    try {
        Course& course = LMSManager::getInstance()->getCourse(index);
        course.displayContents();
    } catch (InvalidCourseIndexException&) {
        cout << "Invalid course index.\n";
    }
}

// Main function for login and menu display
int main() {
   LMSManager* lms = LMSManager::getInstance();

    // Adding some courses
    Course course1("Mathematics", "teacher1@example.com");
    course1.addContent("Introduction to Algebra");
    course1.addContent("Advanced Calculus");

    Course course2("Physics", "teacher2@example.com");
    course2.addContent("Newton's Laws");
    course2.addContent("Thermodynamics");

    lms->addCourse(course1);
    lms->addCourse(course2);

    // Creating users with shared pointers
    users.push_back(make_shared<Admin>("admin1", "admin1@example.com", "adminpass"));
    users.push_back(make_shared<Teacher>("teacher1", "teacher1@example.com", "teacherpass"));
    users.push_back(make_shared<Teacher>("teacher2", "teacher2@example.com", "teacherpass"));
    users.push_back(make_shared<Student>("student1", "student1@example.com", "studentpass"));
    users.push_back(make_shared<Student>("student2", "student2@example.com", "studentpass"));

    // Main login loop
    string email, password;
    bool loggedIn = false;

    while (true) {  // Outer loop to allow logging in with different roles
        while (!loggedIn) {  // Inner loop for user login
            system("cls");  // Clear screen
            cout << "Learning Management System Login\n";
            cout << "================================\n";
            cout << "Enter your email (or type '0' to exit): ";
            cin >> email;

        if (email == "0") {
        cout << "Exiting program...\n";
        LMSManager::cleanup();  // Use cleanup instead of direct delete
        return 0;
    }
            cout << "Enter your password: ";
            cin >> password;

            // Find matching user and check password
            for (const auto& user : users) {
                if (user->getEmail() == email && user->getPassword() == password) {
                    loggedIn = true;
                    user->displayMenu();  // Display appropriate menu based on user type
                    break;
                }
            }

            if (!loggedIn) {
                cout << "Invalid login credentials. Please try again.\n";
                system("pause");
            }
        }

        // Ask if the user wants to login as a different role
        char changeRole;
        cout << "Do you want to log in as a different role? (y/n): ";
        cin >> changeRole;

        if (changeRole == 'n' || changeRole == 'N') {
            cout << "Logging out...\n";
            break;
        } else {
            loggedIn = false;  // Reset login status to allow another login
        }
    }
  LMSManager::cleanup(); 
    return 0;
}

