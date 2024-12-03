#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include <map>
#include <limits>

using namespace std;

// Forward declarations
class Admin;
class Teacher;
class Student;

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

    virtual void displayMenu() = 0;
    virtual ~User() = default;
    string getEmail() const { return email; }  // Made const
    string getPassword() const { return password; }  // Made const
};


class ValidationException : public runtime_error {
public:
    ValidationException(const string& msg) : runtime_error(msg) {}
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

    void displayMenu();
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
    vector<pair<string, int>> grades;
    vector<string> enrolledStudents;

public:
    Course(string courseName, string teacherEmail) {
        if (!Validator::isValidString(courseName)) {
            throw ValidationException("Invalid course name");
        }
        if (!Validator::isValidEmail(teacherEmail)) {
            throw ValidationException("Invalid teacher email");
        }
        this->courseName = courseName;
        this->teacherEmail = teacherEmail;
    }

    void addContent(string content) {
        if (!Validator::isValidString(content)) {
            throw ValidationException("Invalid content");
        }
        contents.push_back(content);
    }

    void removeContent(int index) {
        if (!Validator::isValidIndex(index, contents.size())) {
            throw InvalidCourseIndexException();
        }
        contents.erase(contents.begin() + index);
    }

    void displayContents() const {
        if (contents.empty()) {
            cout << "No contents available for this course.\n";
            return;
        }
        for (size_t i = 0; i < contents.size(); ++i) {
            cout << i + 1 << ": " << contents[i] << endl;
        }
    }

    void addGrade(const string& studentEmail, int grade) {
        if (!Validator::isValidEmail(studentEmail)) {
            throw ValidationException("Invalid student email");
        }
        if (!Validator::isValidGrade(grade)) {
            throw ValidationException("Invalid grade");
        }
        grades.push_back({studentEmail, grade});
    }

    const vector<pair<string, int>>& getGrades() const {
        return grades;
    }

    void displayGrades() const {
        for (const auto& grade : grades) {
            cout << grade.first << ": " << grade.second << "%" << endl;
        }
    }

    void enrollStudent(const string& studentEmail) {
    if (!Validator::isValidEmail(studentEmail)) {
        throw ValidationException("Invalid student email");
    }

    // Manually check if the student is already enrolled
    for (const auto& enrolledEmail : enrolledStudents) {
        if (enrolledEmail == studentEmail) {
            throw ValidationException("Student already enrolled");
        }
    }

    enrolledStudents.push_back(studentEmail); // Enroll the student
}

   void removeStudent(const string& studentEmail) {
    // Manually search for the studentEmail in the enrolledStudents vector
    for (auto it = enrolledStudents.begin(); it != enrolledStudents.end(); ++it) {
        if (*it == studentEmail) {
            enrolledStudents.erase(it); // Remove the student
            return; // Exit the function after removing
        }
    }
    
    // If we reach here, the student was not found
    throw ValidationException("Student not found");
}

    void displayStudents() const {
        for (const auto& student : enrolledStudents) {
            cout << student << endl;
        }
    }

    string getCourseName() const { return courseName; }
    string getTeacherEmail() const { return teacherEmail; }
    const vector<string>& getStudents() const { return enrolledStudents; }
    const vector<string>& getContents() const { return contents; }
};



// LMSManager class (Singleton)
class LMSManager {
private:
    vector<Course> courses;
    static unique_ptr<LMSManager> instance;
    LMSManager() = default;

public:
    static LMSManager* getInstance() {
        if (!instance) {
            instance = unique_ptr<LMSManager>(new LMSManager());
        }
        return instance.get();
    }

    void addCourse(const Course& course) {
        courses.push_back(course);
    }

    Course& getCourse(int index) {
        if (!Validator::isValidIndex(index, courses.size())) {
            throw InvalidCourseIndexException();
        }
        return courses[index];
    }

    void removeCourse(int index) {
        if (!Validator::isValidIndex(index, courses.size())) {
            throw InvalidCourseIndexException();
        }
        courses.erase(courses.begin() + index);
    }

    void displayCourses() const {
        if (courses.empty()) {
            cout << "There are no courses available.\n";
            return;
        }
        for (size_t i = 0; i < courses.size(); ++i) {
            cout << i + 1 << ": " << courses[i].getCourseName()
                 << " (Teacher: " << courses[i].getTeacherEmail() << ")" << endl;
        }
    }

    vector<Course>& getCourses() { return courses; }
};

// Initialize static member of LMSManager
unique_ptr<LMSManager> LMSManager::instance;

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
        string studentPassword;
        bool validEmail = false;
        
        // Email validation
        do {
            cout << "Enter student's email: ";
            cin >> studentEmail;
            if (Validator::isValidEmail(studentEmail)) {
                
                // Check if student already exists
                bool studentExists = false;
                for (const auto& user : users) {
                    if (user->getEmail() == studentEmail) {
                        studentExists = true;
                        break;
                    }
                }
                
                if (studentExists) {
                    cout << "Student with this email already exists. Cannot create a duplicate account.\n";
                    return;
                }
                
                validEmail = true;
            } else {
                cout << "Invalid email format. Please try again.\n";
            }
        } while (!validEmail);
        
        // Password input
        cout << "Enter password for the student: ";
        cin >> studentPassword;
        
        // Create new student
        UserPtr newStudent = make_shared<Student>(
            studentEmail.substr(0, studentEmail.find('@')),  // Use email prefix as username
            studentEmail, 
            studentPassword
        );
        
        // Add to users list
        users.push_back(newStudent);
        
        // Enroll in the course
        course.enrollStudent(studentEmail);
        
        cout << "Student enrolled successfully and account created.\n";
        cout << "Username: " << newStudent->getEmail() << endl;
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
                system("pause");
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
    system("cls");
    string courseName, teacherEmail;
    cout << "Enter course name: ";
    cin.ignore();
    getline(cin, courseName);
    cout << "Enter teacher's email: ";
    cin >> teacherEmail;

    // Ensure teacher is not managing multiple subjects
    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    for (const auto& course : courses) {
        if (course.getTeacherEmail() == teacherEmail) {
            cout << "Error: Teacher is already assigned to another course.\n";
            system("pause");
            return;
        }
    }

    Course newCourse(courseName, teacherEmail);
    LMSManager::getInstance()->addCourse(newCourse);
    cout << "Course added successfully.\n";
    system("pause");
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
        // Validate user input and adjust for 0-based indexing
        if (index < 1 || index > courses.size()) {
            throw out_of_range("Invalid index");
        }
        
        // Subtract 1 to convert to 0-based index
        Course courseToDelete = courses[index - 1];  // Get a copy of the course
        
        LMSManager::getInstance()->removeCourse(index - 1);  // Pass 0-based indexad
        cout << "Successfully deleted course: " << courseToDelete.getCourseName() << endl;
    } catch (InvalidCourseIndexException&) {
        cout << "Invalid course index.\n";
    } catch (out_of_range&) {
        // Handle case where index is out of vector bounds
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
                system("pause");
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

    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    if (courses.empty()) {
        cout << "No courses available to view.\n";
        system("pause");
        return;  // Exit the function if there are no courses
    }

    // Display the list of courses with 1-based indexing
    cout << "Courses:\n";
    for (size_t i = 0; i < courses.size(); ++i) {
        cout << i + 1 << ". " << courses[i].getCourseName() << endl;
    }

    int index;
    cout << "Enter course index to view (1-based): ";
    cin >> index;

    try {
        // Validate and adjust for 0-based indexing
        if (index < 1 || index > courses.size()) {
            throw out_of_range("Invalid index");
        }

        // Get the course using 0-based index
        Course& course = LMSManager::getInstance()->getCourse(index - 1);
        cout << "Viewing course: " << course.getCourseName() << endl;
        course.displayContents();
        system("pause");  // Wait for the user to see the course contents
    } catch (InvalidCourseIndexException&) {
        cout << "Invalid course index.\n";
    } catch (out_of_range&) {
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
        cout << "3. Log Out\n";

        choice = Validator::getValidatedIntInput("Enter choice (1-3): ", 1, 3);

        switch (choice) {
            case 1:
                viewEnrolledCourses();
                break;
            case 2:
                viewGrades();
                break;
            case 3:
                cout << "Logging out...\n";
                system("pause");
                break;
        }
    } while (choice != 3);
}

void Student::viewEnrolledCourses() {
    vector<Course>& allCourses = LMSManager::getInstance()->getCourses();
    vector<Course> enrolledCourses;

    // Find courses where the student is enrolled
    for (Course& course : allCourses) {
        for (const string& studentEmail : course.getStudents()) {
            if (studentEmail == email) {
                enrolledCourses.push_back(course);
                break;
            }
        }
    }

    // Check if student is enrolled in any courses
    if (enrolledCourses.empty()) {
        cout << "You are not enrolled in any courses.\n";
        return;
    }

    // Display enrolled courses
    cout << "Your Enrolled Courses:\n";
    for (size_t i = 0; i < enrolledCourses.size(); ++i) {
        cout << i + 1 << ": " << enrolledCourses[i].getCourseName() 
             << " (Teacher: " << enrolledCourses[i].getTeacherEmail() << ")\n";
    }

    int index = Validator::getValidatedIntInput(
        "Enter course index to view content (or 0 to go back): ", 
        0, enrolledCourses.size()
    );

    if (index == 0) return;

    // Display course contents
    try {
        Course& selectedCourse = enrolledCourses[index - 1];
        selectedCourse.displayContents();
    } catch (const exception& e) {
        cout << "Error viewing course contents: " << e.what() << endl;
    }
}

void Student::viewGrades() {
    vector<Course>& allCourses = LMSManager::getInstance()->getCourses();
    vector<Course> enrolledCourses;

    // Find courses where the student is enrolled
    for (Course& course : allCourses) {
        for (const string& studentEmail : course.getStudents()) {
            if (studentEmail == email) {
                enrolledCourses.push_back(course);
                break;
            }
        }
    }

    // Check if student is enrolled in any courses
    if (enrolledCourses.empty()) {
        cout << "You are not enrolled in any courses.\n";
        return;
    }

    // Display enrolled courses
    cout << "Your Enrolled Courses:\n";
    for (size_t i = 0; i < enrolledCourses.size(); ++i) {
        cout << i + 1 << ": " << enrolledCourses[i].getCourseName() 
             << " (Teacher: " << enrolledCourses[i].getTeacherEmail() << ")\n";
    }

    int index = Validator::getValidatedIntInput(
        "Enter course index to view grades (or 0 to go back): ", 
        0, enrolledCourses.size()
    );

    if (index == 0) return;

    // Display course grades
    try {
        Course& selectedCourse = enrolledCourses[index - 1];
        
        // Find and display only this student's grade
        bool gradeFound = false;
        for (auto& grade : selectedCourse.getGrades()) {
            if (grade.first == email) {
                cout << "Your Grade in " << selectedCourse.getCourseName() 
                     << ": " << grade.second << "%" << endl;
                gradeFound = true;
                break;
            }
        }

        if (!gradeFound) {
            cout << "No grade available for this course.\n";
        }
    } catch (const exception& e) {
        cout << "Error viewing grades: " << e.what() << endl;
    }
}

void Student::enrollInCourse() {
    vector<Course>& courses = LMSManager::getInstance()->getCourses();
    vector<Course> unenrolledCourses;

    // Find courses student is not already enrolled in
    for (Course& course : courses) {
        bool alreadyEnrolled = false;
        for (const string& studentEmail : course.getStudents()) {
            if (studentEmail == email) {
                alreadyEnrolled = true;
                break;
            }
        }
        
        if (!alreadyEnrolled) {
            unenrolledCourses.push_back(course);
        }
    }

    // Check if there are courses available for enrollment
    if (unenrolledCourses.empty()) {
        cout << "No courses available for enrollment.\n";
        return;
    }

    // Display unenrolled courses
    cout << "Available Courses:\n";
    for (size_t i = 0; i < unenrolledCourses.size(); ++i) {
        cout << i + 1 << ": " << unenrolledCourses[i].getCourseName() 
             << " (Teacher: " << unenrolledCourses[i].getTeacherEmail() << ")\n";
    }

    int courseIndex = Validator::getValidatedIntInput(
        "Enter course index to enroll (or 0 to go back): ", 
        0, unenrolledCourses.size()
    );

    if (courseIndex == 0) return;

    try {
        Course& course = unenrolledCourses[courseIndex - 1];
        course.enrollStudent(email);
        cout << "Successfully enrolled in the course: " 
             << course.getCourseName() << endl;
    } catch (const exception& e) {
        cout << e.what() << endl;
    }
}

// Main function for login and menu display
int main() {
    try {
        LMSManager* lms = LMSManager::getInstance();

        // Sample data initialization
        Course course1("Mathematics", "teacher1@example.com");
        course1.addContent("Introduction to Algebra");
        course1.addContent("Advanced Calculus");

        Course course2("Physics", "teacher2@example.com");
        course2.addContent("Newton's Laws");
        course2.addContent("Thermodynamics");

        lms->addCourse(course1);
        lms->addCourse(course2);

        // Initialize users
        vector<shared_ptr<User>> users;
        users.push_back(make_shared<Admin>("admin1", "admin1@example.com", "adminpass"));
        users.push_back(make_shared<Teacher>("teacher1", "teacher1@example.com", "teacherpass"));
        users.push_back(make_shared<Teacher>("teacher2", "teacher2@example.com", "teacherpass"));

        string email, password;
        bool loggedIn = false;

        while (true) {
            while (!loggedIn) {
                system("cls");
                cout << "Learning Management System Login\n";
                cout << "================================\n";
                cout << "Enter your email (or type '0' to exit): ";
                cin >> email;

                if (email == "0") {
                    cout << "Exiting program...\n";
                    return 0;
                }

                cout << "Enter your password: ";
                cin >> password;

                for (const auto& user : users) {
                    if (user->getEmail() == email && user->getPassword() == password) {
                        loggedIn = true;
                        user->displayMenu();
                        break;
                    }
                }

                if (!loggedIn) {
                    cout << "Invalid login credentials. Please try again.\n";
                    system("pause");
                }
            }

            char changeRole;
            cout << "Do you want to log in as a different role? (y/n): ";
            cin >> changeRole;

            if (tolower(changeRole) == 'n') {
                cout << "Logging out...\n";
                break;
            }
            loggedIn = false;
        }
    }
    catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }

    return 0;
}