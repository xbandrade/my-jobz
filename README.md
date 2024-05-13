# <img src="https://raw.githubusercontent.com/xbandrade/my-jobz/main/resources/application.ico" width=48px> MyJobz
### ❕A job application manager made with C++ and Qt

### Releases of this project are available for download [here](https://github.com/xbandrade/my-jobz/releases/tag/v0.0.1)

## ➡️ Dependencies and Tools
- Qt Creator 12.0.2
- Qt 6.6.0
- gcc 12.2.0
- SQLite 3.40.1
- cmake 3.27.7

## ➡️ Local Setup 
- Clone this repository to your machine
- In Qt Creator:
    - Go to File > Open File or Project
    - Navigate to the project's folder and open the `CMakeLists.txt` file
    - Build the project and run the application
- Via CLI environment:
    - Navigate to the solution's folder
    - Create a `build` directory and navigate to it
    - Configure the Qt path in `CMakeLists.txt` by adding the line `set(CMAKE_PREFIX_PATH "/path/to/Qt")`
    - Run the command `cmake ..; cmake --build .` and execute the output application

     <img src="https://raw.githubusercontent.com/xbandrade/my-jobz/main/resources/img/myjobz.png">

## ➡️ Job Application Model Schema
```
    {
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        job_title TEXT,
        company TEXT,
        status TEXT,
        application_date TEXT,
        url_email TEXT,
        details TEXT,
        is_finished BOOLEAN DEFAULT 0
    }
```

## ➡️ App Features
### ➜ Import and export to SQLite3 database or CSV
  - All job applications can be exported to a database or a CSV file, a text file that can be viewed and edited in popular spreadsheet platforms

### ➜ Clipboard monitoring
  - When enabled and granted permission, it will listen for changes in the clipboard, and when a URL or email is detected, a notification will be displayed and a new job application entry can be added from there. This feature will work even when the application is minimized in the system tray

### ➜ Database table filtering and sorting options
  - The database table view can be sorted by double clicking any of its column headers. Jobs can be filtered by using the search box, and jobs marked as finished can also be hidden from the table view

### ➜ Job application entry editing
  - A job application can be edited by double clicking on any of the values in its row. A job entry can be marked as finished in the context menu shown by right clicking on the row
    
    <img src="https://raw.githubusercontent.com/xbandrade/my-jobz/main/resources/img/details.png" width=40%>
  
### ➜ Redirection to URL or Email
  - When right clicking on an item and choosing `Go to URL`, if it has a valid URL, the application will open the browser and go to that URL, and if it has a valid email, it will use the application's email as a search query in the user's email platform.
    
### ➜ Table view pagination
  - Items in the table view are limited to 10 per page by default, but this value can be changed in the menu bar `View > Items per Page`
