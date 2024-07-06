# DBView

DBView is a command-line interface (CLI) based employee database management system implemented in C. The project is designed to evolve as it incorporates more advanced programming concepts, from basic CRUD operations to a networked, concurrent application. This README provides an overview of the current features and planned features.

## Current Features

- **Database File** (`-f <filename>`): Specify the filename of the database file.

## Planned Features

*Note: These features are planned but not yet implemented.*

### System Capabilities

- **File I/O**: Enhanced file handling for saving and loading the database.
- **Networking**: Allow remote access to the database using TCP/IP.
- **Concurrency**: Implement multithreading to handle multiple clients simultaneously.
- **Security**: Secure communication and data handling.

### Command-line Flags

- **New Database** (`-n`): Create a new database file if it does not already exist and perform the specified action.
- **List Employees** (`-l`): List all employees currently in the database.
- **Add Employee** (`-a`): Add a new employee to the database.
- **Delete Employee** (`-d`): Remove an employee from the database by some parameter yet to be decided.
- **Update Employee** (`-u`): Update details of an existing employee.
