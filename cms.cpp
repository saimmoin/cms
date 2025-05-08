#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// Observer interface
class Observer
{
public:
    virtual void update(const string &message) = 0;
};

// Subject class to manage observers
class Subject
{
private:
    vector<Observer *> observers;

public:
    void attach(Observer *observer)
    {
        observers.push_back(observer);
    }
    void detach(Observer *observer)
    {
        for (auto it = observers.begin(); it != observers.end(); ++it)
        {
            if (*it == observer)
            {
                observers.erase(it);
                break;
            }
        }
    }
    void notify(const string &message)
    {
        for (Observer *observer : observers)
        {
            observer->update(message);
        }
    }
};

// Concrete observer: Logger
class Logger : public Observer
{
public:
    void update(const string &message) override
    {
        ofstream logFile("log.txt", ios::app);
        logFile << message << endl;
        logFile.close();
    }
};

// ObserverFactory class
class ObserverFactory
{
public:
    static Observer *createObserver(const string &type)
    {
        if (type == "logger")
            return new Logger();
        return nullptr;
    }
};

// UserManager class (Singleton)
class UserManager : public Subject
{
private:
    static UserManager *instance;
    UserManager() {}

public:
    static UserManager *getInstance()
    {
        if (!instance)
            instance = new UserManager();
        return instance;
    }
    bool doesUsernameExist(const string &username)
    {
        ifstream file("users.txt");
        string storedUsername, storedPassword, storedRole;
        while (file >> storedUsername >> storedPassword >> storedRole)
        {
            if (storedUsername == username)
            {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }
    void registerUser(const string &username, const string &password, const string &role)
    {
        if (doesUsernameExist(username))
        {
            cout << "Username already exists! Try a different one.\n";
            return;
        }
        if (role != "admin" && role != "editor" && role != "viewer")
        {
            cout << "Invalid role! Use admin, editor, or viewer.\n";
            return;
        }
        ofstream file("users.txt", ios::app);
        file << username << " " << password << " " << role << endl;
        file.close();
        string message = "User registered: " + username + " (" + role + ")";
        notify(message);
        cout << "Registration successful!\n";
    }
    bool loginUser(const string &username, const string &password, string &role)
    {
        ifstream file("users.txt");
        string storedUsername, storedPassword, storedRole;
        bool loginSuccess = false;
        while (file >> storedUsername >> storedPassword >> storedRole)
        {
            if (storedUsername == username && storedPassword == password)
            {
                loginSuccess = true;
                role = storedRole;
                break;
            }
        }
        file.close();
        if (loginSuccess)
        {
            string message = "User logged in: " + username + " (" + role + ")";
            notify(message);
            cout << "Login successful! Welcome, " + username + "!\n";
        }
        else
        {
            cout << "Invalid username or password!\n";
        }
        return loginSuccess;
    }
    void logFileAction(const string &username, const string &action, const string &filename)
    {
        string message = "User " + username + ": " + action + " file " + filename;
        notify(message);
    }
};
UserManager *UserManager::instance = nullptr; // This means that the instance is not created yet

// Abstract User class
class User
{
protected:
    string username, password, role;

public:
    User(const string &u, const string &p, const string &r) : username(u), password(p), role(r) {}
    virtual string getRole() const = 0;
    virtual void viewFiles(UserManager &userManager) = 0;
    virtual void viewFileContent(UserManager &userManager, const string &filename) = 0;
    virtual void editFile(UserManager &userManager, const string &filename, const string &content) = 0;
    virtual void createFile(UserManager &userManager, const string &filename, const string &content) = 0;
    virtual void deleteFile(UserManager &userManager, const string &filename) = 0;
    virtual ~User() {}
};

// Concrete User classes
class AdminUser : public User
{
public:
    AdminUser(const string &u, const string &p) : User(u, p, "admin") {}
    string getRole() const override { return "admin"; }
    void viewFiles(UserManager &userManager) override
    {
        ifstream file("files.txt");
        string filename;
        bool hasFiles = false;
        cout << "Available files:\n";
        while (file >> filename)
        {
            cout << filename << endl;
            hasFiles = true;
        }
        file.close();
        if (!hasFiles)
        {
            cout << "No files available.\n";
        }
        else
        {
            userManager.logFileAction(username, "viewed list of files", "");
        }
    }
    void viewFileContent(UserManager &userManager, const string &filename) override
    {
        ifstream file(filename);
        if (!file)
        {
            cout << "File does not exist!\n";
            return;
        }
        string line;
        cout << "Content of " << filename << ":\n";
        while (getline(file, line))
        {
            cout << line << endl;
        }
        file.close();
        userManager.logFileAction(username, "viewed content of", filename);
    }
    void editFile(UserManager &userManager, const string &filename, const string &content) override
    {
        ifstream checkFile(filename);
        if (!checkFile)
        {
            cout << "File does not exist!\n";
            checkFile.close();
            return;
        }
        checkFile.close();
        ofstream file(filename);
        file << content;
        file.close();
        userManager.logFileAction(username, "edited", filename);
        cout << "File edited successfully!\n";
    }
    void createFile(UserManager &userManager, const string &filename, const string &content) override
    {
        ifstream checkFile(filename);
        if (checkFile)
        {
            cout << "File already exists!\n";
            checkFile.close();
            return;
        }
        ofstream file(filename);
        file << content;
        file.close();
        ofstream filesList("files.txt", ios::app);
        filesList << filename << endl;
        filesList.close();
        userManager.logFileAction(username, "created", filename);
        cout << "File created successfully!\n";
    }
    void deleteFile(UserManager &userManager, const string &filename) override
    {
        ifstream checkFile(filename);
        if (!checkFile)
        {
            cout << "File does not exist!\n";
            checkFile.close();
            return;
        }
        checkFile.close();
        if (remove(filename.c_str()) != 0)
        {
            cout << "Error deleting file!\n";
            return;
        }
        ifstream filesList("files.txt");
        ofstream temp("temp.txt");
        string storedFilename;
        while (filesList >> storedFilename)
        {
            if (storedFilename != filename)
            {
                temp << storedFilename << endl;
            }
        }
        filesList.close();
        temp.close();
        remove("files.txt");
        rename("temp.txt", "files.txt");
        userManager.logFileAction(username, "deleted", filename);
        cout << "File deleted successfully!\n";
    }
};

class EditorUser : public User
{
public:
    EditorUser(const string &u, const string &p) : User(u, p, "editor") {}
    string getRole() const override { return "editor"; }
    void viewFiles(UserManager &userManager) override
    {
        AdminUser(username, password).viewFiles(userManager);
    }
    void viewFileContent(UserManager &userManager, const string &filename) override
    {
        AdminUser(username, password).viewFileContent(userManager, filename);
    }
    void editFile(UserManager &userManager, const string &filename, const string &content) override
    {
        AdminUser(username, password).editFile(userManager, filename, content);
    }
    void createFile(UserManager &userManager, const string &filename, const string &content) override
    {
        cout << "Permission denied: Editors cannot create files!\n";
    }
    void deleteFile(UserManager &userManager, const string &filename) override
    {
        cout << "Permission denied: Editors cannot delete files!\n";
    }
};

class ViewerUser : public User
{
public:
    ViewerUser(const string &u, const string &p) : User(u, p, "viewer") {}
    string getRole() const override { return "viewer"; }
    void viewFiles(UserManager &userManager) override
    {
        AdminUser(username, password).viewFiles(userManager);
    }
    void viewFileContent(UserManager &userManager, const string &filename) override
    {
        AdminUser(username, password).viewFileContent(userManager, filename);
    }
    void editFile(UserManager &userManager, const string &filename, const string &content) override
    {
        cout << "Permission denied: Viewers cannot edit files!\n";
    }
    void createFile(UserManager &userManager, const string &filename, const string &content) override
    {
        cout << "Permission denied: Viewers cannot create files!\n";
    }
    void deleteFile(UserManager &userManager, const string &filename) override
    {
        cout << "Permission denied: Viewers cannot delete files!\n";
    }
};

// UserFactory class
class UserFactory
{
public:
    static User *createUser(const string &username, const string &password, const string &role)
    {
        if (role == "admin")
            return new AdminUser(username, password);
        if (role == "editor")
            return new EditorUser(username, password);
        if (role == "viewer")
            return new ViewerUser(username, password);
        return nullptr;
    }
};

// Function to handle logged-in user actions
void userMenu(UserManager &userManager, User *user)
{
    int choice;
    string filename, content, line;
    while (true)
    {
        cout << "\nWelcome, " << user->getRole() << "\n";
        cout << "1. View all files\n";
        cout << "2. View file content\n";
        if (user->getRole() == "admin")
        {
            cout << "3. Create file\n";
            cout << "4. Edit file\n";
            cout << "5. Delete file\n";
            cout << "6. Logout\n";
        }
        else if (user->getRole() == "editor")
        {
            cout << "3. Edit file\n";
            cout << "4. Logout\n";
        }
        else
        {
            cout << "3. Logout\n";
        }
        cout << "Enter your choice: ";
        cin >> choice;
        if (user->getRole() == "admin")
        {
            if (choice == 1)
            {
                user->viewFiles(userManager);
            }
            else if (choice == 2)
            {
                cout << "Enter filename to view: ";
                cin >> filename;
                user->viewFileContent(userManager, filename);
            }
            else if (choice == 3)
            {
                cout << "Enter filename to create: ";
                cin >> filename;
                cout << "Enter content (type 'END' on a new line to finish):\n";
                cin.ignore();
                content = "";
                while (getline(cin, line) && line != "END")
                {
                    content += line + "\n";
                }
                user->createFile(userManager, filename, content);
            }
            else if (choice == 4)
            {
                cout << "Enter filename to edit: ";
                cin >> filename;
                cout << "Enter new content (type 'END' on a new line to finish):\n";
                cin.ignore();
                content = "";
                while (getline(cin, line) && line != "END")
                {
                    content += line + "\n";
                }
                user->editFile(userManager, filename, content);
            }
            else if (choice == 5)
            {
                cout << "Enter filename to delete: ";
                cin >> filename;
                user->deleteFile(userManager, filename);
            }
            else if (choice == 6)
            {
                break;
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        }
        else if (user->getRole() == "editor")
        {
            if (choice == 1)
            {
                user->viewFiles(userManager);
            }
            else if (choice == 2)
            {
                cout << "Enter filename to view: ";
                cin >> filename;
                user->viewFileContent(userManager, filename);
            }
            else if (choice == 3)
            {
                cout << "Enter filename to edit: ";
                cin >> filename;
                cout << "Enter new content (type 'END' on a new line to finish):\n";
                cin.ignore();
                content = "";
                while (getline(cin, line) && line != "END")
                {
                    content += line + "\n";
                }
                user->editFile(userManager, filename, content);
            }
            else if (choice == 4)
            {
                break;
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        }
        else
        { // viewer
            if (choice == 1)
            {
                user->viewFiles(userManager);
            }
            else if (choice == 2)
            {
                cout << "Enter filename to view: ";
                cin >> filename;
                user->viewFileContent(userManager, filename);
            }
            else if (choice == 3)
            {
                break;
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        }
    }
}

// Main function
int main()
{
    UserManager *userManager = UserManager::getInstance();
    Observer *logger = ObserverFactory::createObserver("logger");
    userManager->attach(logger);
    int choice;
    while (true)
    {
        cout << "\nSimple Login System\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        if (choice == 1)
        {
            string username, password, role;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            cout << "Enter role (admin, editor, viewer): ";
            cin >> role;
            userManager->registerUser(username, password, role);
        }
        else if (choice == 2)
        {
            string username, password, role;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            if (userManager->loginUser(username, password, role))
            {
                User *user = UserFactory::createUser(username, password, role);
                if (user)
                {
                    userMenu(*userManager, user);
                    delete user;
                }
            }
        }
        else if (choice == 3)
        {
            cout << "Goodbye!\n";
            break;
        }
        else
        {
            cout << "Invalid choice! Please try again.\n";
        }
    }
    userManager->detach(logger);
    delete logger;
    return 0;
}