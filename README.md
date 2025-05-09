# File Management System

## Overview of the Program
The program is a file management system with user authentication. Users can:

- **Register** with a username, password, and role (admin, editor, or viewer).
- **Log in** to access role-specific functionalities.
- **Perform file operations** based on their role:
  - **Admins**: Can view, create, edit, and delete files.
  - **Editors**: Can view and edit files but cannot create or delete them.
  - **Viewers**: Can only view files.
- **All user actions** (e.g., registration, login, file operations) are logged to a file called `log.txt`.
- The system stores user information in a file (`users.txt`) and tracks available files in another file (`files.txt`). The code uses **object-oriented programming (OOP)** principles and design patterns to make it modular, reusable, and maintainable.

## Key Components and Their Functionality

### 1. Files Used in the System
- **`users.txt`**:
  - Stores user information in the format `username password role` (e.g., `john 123 admin`).
  - Updated when a user registers and read during login to verify credentials.
- **`files.txt`**:
  - Keeps a list of filenames (one per line) that exist in the system.
  - Updated when files are created or deleted.
- **`log.txt`**:
  - Logs all user actions (e.g., registration, login, file operations) with messages like `User john: created file document.txt`.
- **Individual files** (e.g., `document.txt`):
  - Each file created by an admin contains user-provided content.
  - These are plain text files stored in the same directory as the program.

### 2. Classes and Their Roles
The program is divided into several classes, each handling specific responsibilities. Below, each class, its purpose, and its key functions are explained.

#### Observer (Interface)
- **Purpose**: Defines a contract for classes that want to "observe" or react to events (e.g., logging user actions).
- **Key Function**:
  - `virtual void update(const string &message) = 0`:
    - A pure virtual function that must be implemented by any class inheriting from `Observer`.
    - Called whenever an event occurs, passing a message (e.g., "User logged in").
- **Why It Exists**: Allows different types of observers (e.g., a logger) to react to events without hardcoding their behavior.

#### Subject (Class)
- **Purpose**: Manages a list of observers and notifies them when an event happens.
- **Key Functions**:
  - `void attach(Observer *observer)`: Adds an observer to the list so it can receive notifications.
  - `void detach(Observer *observer)`: Removes an observer from the list.
  - `void notify(const string &message)`: Calls the `update` function of all attached observers, passing the event message.
- **Why It Exists**: Acts as the "event broadcaster" that informs observers about user actions (e.g., login, file creation).

#### Logger (Concrete Observer)
- **Purpose**: A specific observer that logs messages to `log.txt`.
- **Key Function**:
  - `void update(const string &message)`: Appends the message to `log.txt` (e.g., `User john: created file document.txt`).
- **Why It Exists**: Provides a way to record all user actions in a file for tracking purposes.

#### ObserverFactory (Class)
- **Purpose**: Creates observer objects based on a type string.
- **Key Function**:
  - `static Observer *createObserver(const string &type)`: Returns a new `Logger` object if type is "logger"; otherwise, returns `nullptr`.
- **Why It Exists**: Simplifies creating observers and allows for future expansion (e.g., adding new observer types like email notifiers).

#### UserManager (Singleton Class, also a Subject)
- **Purpose**: Manages user registration, login, and notifies observers of user actions. It’s a **singleton**, meaning only one instance exists.
- **Key Functions**:
  - `static UserManager *getInstance()`: Returns the single instance of `UserManager`, creating it if it doesn’t exist.
  - `bool doesUsernameExist(const string &username)`: Checks if a username exists in `users.txt` by reading the file.
  - `void registerUser(const string &username, const string &password, const string &role)`:
    - Checks if the username already exists (calls `doesUsernameExist`).
    - Validates the role (must be "admin", "editor", or "viewer").
    - If valid, appends the user’s details to `users.txt`.
    - Notifies observers with a message like `User registered: john (admin)`.
    - Prints "Registration successful!" or an.abstract error message.
  - `bool loginUser(const string &username, const string &password, string &role)`:
    - Reads `users.txt` to find a matching username and password.
    - If found, sets the `role` output parameter and notifies observers with a message like `User logged in: john (admin)`.
    - Returns `true` for success, `false` for failure, and prints appropriate messages.
  - `void logFileAction(const string &username, const string &action, const string &filename)`:
    - Creates a message like `User john: created file document.txt` and notifies observers.
- **Why It Exists**: Centralizes user management and ensures all user-related actions are logged consistently.

#### User (Abstract Base Class)
- **Purpose**: Defines the interface for all user types (admin, editor, viewer) and stores common user data (username, password, role).
- **Key Members**:
  - **Protected variables**: `username`, `password`, `role`.
  - **Constructor**: Initializes the user with a username, password, and role.
  - **Pure virtual functions** (must be implemented by derived classes):
    - `virtual string getRole() const = 0`: Returns the user’s role.
    - `virtual void viewFiles(UserManager &userManager) = 0`: Lists all files.
    - `virtual void viewFileContent(UserManager &userManager, const string &filename) = 0`: Shows a file’s content.
    - `virtual void editFile(UserManager &userManager, const string &filename, const string &content) = 0`: Edits a file.
    - `virtual void createFile(UserManager &userManager, const string &filename, const string &content) = 0`: Creates a file.
    - `virtual void deleteFile(UserManager &userManager, const string &filename) = 0`: Deletes a file.
- **Why It Exists**: Provides a common interface for all user types, ensuring they implement required behaviors.

#### AdminUser (Concrete User Class)
- **Purpose**: Represents an admin user with full file management permissions.
- **Key Functions**:
  - **Constructor**: Initializes with username, password, and sets role to "admin".
  - `string getRole() const`: Returns "admin".
  - `void viewFiles(UserManager &userManager)`:
    - Reads `files.txt` and lists all filenames.
    - If no files exist, prints "No files available."
    - Logs the action via `userManager.logFileAction`.
  - `void viewFileContent(UserManager &userManager, const string &filename)`:
    - Opens the specified file and prints its content line by line.
    - If the file doesn’t exist, prints an error.
    - Logs the action.
  - `void editFile(UserManager &userManager, const string &filename, const string &content)`:
    - Checks if the file exists.
    - Overwrites the file with new content.
    - Logs the action and prints "File edited successfully!"
  - `void createFile(UserManager &userManager, const string &filename, const string &content)`:
    - Checks if the file already exists.
    - Creates a new file with the given content.
    - Appends the filename to `files.txt`.
    - Logs the action and prints "File created successfully!"
  - `void deleteFile(UserManager &userManager, const string &filename)`:
    - Checks if the file exists.
    - Deletes the file using `remove`.
    - Updates `files.txt` to remove the filename.
    - Logs the action and prints "File deleted successfully!"
- **Why It Exists**: Implements full file management capabilities for admin users.

#### EditorUser (Concrete User Class)
- **Purpose**: Represents an editor user who can view and edit files but cannot create or delete them.
- **Key Functions**:
  - **Constructor**: Initializes with username, password, and sets role to "editor".
  - `string getRole() const`: Returns "editor".
  - `void viewFiles(UserManager &userManager)`: Delegates to `AdminUser::viewFiles` to reuse code.
  - `void viewFileContent(UserManager &userManager, const string &filename)`: Delegates to `AdminUser::viewFileContent`.
  - `void editFile(UserManager &userManager, const string &filename, const string &content)`: Delegates to `AdminUser::editFile`.
  - `void createFile(UserManager &userManager, const string &filename, const string &content)`: Prints "Permission denied: Editors cannot create files!"
  - `void deleteFile(UserManager &userManager, const string &filename)`: Prints "Permission denied: Editors cannot delete files!"
- **Why It Exists**: Limits permissions for editors, reusing admin functionality where appropriate.

#### ViewerUser (Concrete User Class)
- **Purpose**: Represents a viewer user who can only view files.
- **Key Functions**:
  - **Constructor**: Initializes with username, password, and sets role to "viewer".
  - `string getRole() const`: Returns "viewer".
  - `void viewFiles(UserManager &userManager)`: Delegates to `AdminUser::viewFiles`.
  - `void viewFileContent(UserManager &userManager, const string &filename)`: Delegates to `AdminUser::viewFileContent`.
  - `void editFile(UserManager &userManager, const string &filename, const string &content)`: Prints "Permission denied: Viewers cannot edit files!"
  - `void createFile(UserManager &userManager, const string &filename, const string &content)`: Prints "Permission denied: Viewers cannot create files!"
  - `void deleteFile(UserManager &userManager, const string &filename)`: Prints "Permission denied: Viewers cannot delete files!"
- **Why It Exists**: Restricts viewers to read-only access, reusing admin functionality for viewing.

#### UserFactory (Class)
- **Purpose**: Creates user objects based on their role.
- **Key Function**:
  - `static User *createUser(const string &username, const string &password, const string &role)`:
    - Returns an `AdminUser`, `EditorUser`, or `ViewerUser` based on the role ("admin", "editor", or "viewer").
    - Returns `nullptr` for invalid roles.
- **Why It Exists**: Simplifies user object creation and allows for easy extension (e.g., adding new user types).

#### userMenu (Function)
- **Purpose**: Displays a menu for logged-in users and handles their actions based on their role.
- **How It Works**:
  - Takes a `UserManager` and a `User` object as parameters.
  - Shows a role-specific menu:
    - **Admin**: Options to view files, view file content, create, edit, delete files, or logout.
    - **Editor**: Options to view files, view file content, edit files, or logout.
    - **Viewer**: Options to view files, view file content, or logout.
  - Reads the user’s choice and calls the appropriate `User` method (e.g., `viewFiles`, `createFile`).
  - For file creation/editing, prompts for filename and content (until "END" is typed).
  - Continues until the user chooses to logout.
- **Why It Exists**: Provides an interactive interface for users to perform actions.

#### main (Function)
- **Purpose**: The entry point of the program, handling the main menu and program flow.
- **How It Works**:
  - Creates a `UserManager` instance (singleton).
  - Creates a `Logger` observer using `ObserverFactory` and attaches it to `UserManager`.
  - Shows a menu with options: **Register**, **Login**, **Exit**.
  - **Register**:
    - Prompts for username, password, and role.
    - Calls `userManager->registerUser`.
  - **Login**:
    - Prompts for username and password.
    - Calls `userManager->loginUser` to verify credentials.
    - If successful, creates a `User` object using `UserFactory` and calls `userMenu`.
  - **Exit**: Breaks the loop and ends the program.
  - Detaches and deletes the logger before exiting.
- **Why It Exists**: Orchestrates the entire program flow.

## How the Program Works (User Perspective)

### Starting the Program
- The user sees a menu: **Register**, **Login**, **Exit**.
- They choose an option by entering a number (1, 2, or 3).

### Registering
- The user enters a **username**, **password**, and **role** (admin, editor, or viewer).
- If the username is unique and the role is valid, the user is added to `users.txt`, and a log entry is created in `log.txt`.
- They see **"Registration successful!"** or an error message.

### Logging In
- The user enters a **username** and **password**.
- If correct, they see **"Login successful!"** and a role-specific menu.
- A log entry is created in `log.txt`.

### Using the System
- **Admins** can:
  - View all filenames (from `files.txt`).
  - View a file’s content.
  - Create a new file (adds to `files.txt` and creates the file).
  - Edit an existing file’s content.
  - Delete a file (removes from `files.txt` and deletes the file).
- **Editors** can view files and edit existing files but cannot create or delete.
- **Viewers** can only view files and their content.
- All actions are logged to `log.txt`.

### Logging Out
- The user selects the **logout** option, returning to the main menu.

### Exiting
- The user chooses **Exit**, and the program ends with **"Goodbye!"**.

## Design Patterns Used
The code uses several design patterns to make it modular, reusable, and maintainable. Here’s a simple explanation of each pattern and how it’s applied:

### 1. Singleton Pattern (UserManager)
- **What It Is**: Ensures a class has only one instance and provides a global point of access to it.
- **How It’s Used**:
  - `UserManager` is a singleton. The `getInstance` function creates a single instance and returns it every time.
  - This ensures all parts of the program use the same `UserManager` to manage users and notify observers.
- **Why It’s Useful**: Prevents multiple `UserManager` instances from causing inconsistencies (e.g., different user lists).

### 2. Observer Pattern (Subject, Observer, Logger)
- **What It Is**: Allows objects (observers) to be notified of changes or events in another object (subject).
- **How It’s Used**:
  - `Subject` maintains a list of `Observer` objects (e.g., `Logger`).
  - `UserManager` inherits from `Subject` and calls `notify` to inform observers of events (e.g., user registration, file actions).
  - `Logger` implements `Observer` and writes messages to `log.txt` when notified.
- **Why It’s Useful**: Decouples event producers (e.g., `UserManager`) from event consumers (e.g., `Logger`), making it easy to add new observers (e.g., a database logger) without changing `UserManager`.

### 3. Factory Pattern (ObserverFactory, UserFactory)
- **What It Is**: Provides a way to create objects without specifying their exact class, using a factory method.
- **How It’s Used**:
  - `ObserverFactory::createObserver` creates a `Logger` object based on the type string ("logger").
  - `UserFactory::createUser` creates `AdminUser`, `EditorUser`, or `ViewerUser` based on the role string.
- **Why It’s Useful**: Centralizes object creation, making it easy to add new observer or user types without changing other code.

### 4. Template Method Pattern (User Hierarchy)
- **What It Is**: Defines a skeleton of an algorithm in a base class, allowing subclasses to override specific steps.
- **How It’s Used**:
  - `User` is an abstract base class with pure virtual functions for file operations.
  - `AdminUser`, `EditorUser`, and `ViewerUser` implement these functions differently based on their permissions.
  - For example, `EditorUser` and `ViewerUser` reuse `AdminUser`’s `viewFiles` and `viewFileContent` methods but override `createFile` and `deleteFile` to deny access.
- **Why It’s Useful**: Ensures a consistent interface for all user types while allowing role-specific behavior.

## How Design Patterns Work Together
- **Singleton** (`UserManager`): Ensures one central manager for users and events.
- **Observer** (`Subject`, `Logger`): Allows logging of all actions without tightly coupling `UserManager` to `Logger`.
- **Factory** (`UserFactory`, `ObserverFactory`): Simplifies creating the right user or observer objects.
- **Template Method** (`User` hierarchy): Organizes user roles with shared and role-specific behaviors.

This combination makes the code:

- **Modular**: Each class has a single responsibility (e.g., `Logger` for logging, `UserManager` for user management).
- **Extensible**: Easy to add new user roles (e.g., a "manager" role) or observers (e.g., an email notifier).
- **Maintainable**: Changes to one part (e.g., adding a new observer) don’t affect others.

## Example Workflow (Step-by-Step)
Let’s walk through a scenario to see how everything ties together:

1. User runs the program and chooses **Register**.
2. Enters **username**: `john`, **password**: `123`, **role**: `admin`.
3. `UserManager::registerUser` checks if `john` exists in `users.txt` (it doesn’t).
4. Writes `john 123 admin` to `users.txt`.
5. Notifies the `Logger`, which writes `User registered: john (admin)` to `log.txt`.
6. User chooses **Login** and enters `john`, `123`.
7. `UserManager::loginUser` verifies credentials, sets role to "admin", and logs `User logged in: john (admin)`.
8. `UserFactory::createUser` creates an `AdminUser` object.
9. `userMenu` shows the admin menu. User chooses **Create File**.
10. Enters **filename**: `document.txt`, **content**: `Hello, world!`, and types `END`.
11. `AdminUser::createFile` creates `document.txt` with `Hello, world!`, adds `document.txt` to `files.txt`, and logs `User john: created file document.txt`.
12. User logs out and exits.
