# MxCAD WinForms Test Project Setup Guide
## Prerequisites
1. Download and extract dependencies as specified in the files:
   - `../../mx/code/download_address.txt`
   - `../../3rdparty/download_address.txt`

## Step-by-Step Project Creation
### 1. Create a New Project
Create a new **WinForms Application** project in C#.

### 2. Add Wrapper Classes
- Copy the following C# wrapper class files to the project folder:
  - `MxCADAppWrapper.cs`
  - `MxCADViewWrapper.cs`
  - `MxCADWrapperLibPINVOKE.cs`
- Add these files to the WinForms project.

### 3. Replace Test Code
Overwrite the content of the auto-generated `Form1.cs` file with the code from `./test.cs`.

### 4. Save and Build the Project
Save all project files and build the project to generate executable files.

### 5. Copy Dependencies to Output Directory
Copy the following files to the directory where the project’s executable is generated:
- All contents from `../../mx/code/build_mxcadlib/bin/Release`
- The executable `../../3rdparty/mxconvert/mxconvert.exe`
- The DLL `./MxCADWrapperLib.dll`

### 6. Run the Application
Execute the generated program to test opening CAD files (supports `mxweb`, `dwg`, `dxf` formats).

### Summary
1. Download dependencies via the specified `download_address.txt` files and extract them.
2. Set up the WinForms project with wrapper classes and test code, then build it.
3. Copy required runtime dependencies (Release folder contents + `mxconvert.exe` + `MxCADWrapperLib.dll`) to the output directory and run the app to test CAD file opening.