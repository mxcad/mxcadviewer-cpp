# MxCAD Java API Test Guide
## Prerequisites
Download and extract dependencies as specified in:
- `../../mx/code/download_address.txt`
- `../../3rdparty/download_address.txt`

## Step 1: Copy Resource Files
Copy the following files to the **current working directory**:
1. All contents from `../../mx/code/build_mxcadlib/bin/Release`
2. The executable `../../3rdparty/mxconvert/mxconvert.exe`

## Step 2: Compile the Program
Run the following command in the current directory via command line to compile the code:
```bash
javac -d . TestMxCAD.java com/mxcad/wrapper/*.java
```

## Step 3: Run the Program
Execute the below command in the current directory to launch the test program:
```bash
java -Djava.library.path=. com.mxcad.wrapper.TestMxCAD
```

## Expected Output
The program will output results similar to:
```
--- Start testing MxCAD Java API ---
App initialized successfully
MxCADObject created successfully
Draw line status: 0
Draw circle status: 0
File saved successfully: D:\test_draw.mxweb
Starting convert mxweb to dwg...
Convert success! DWG file saved to: D:\test_draw.dwg
--- Test finished ---
```

### Summary
1. Download and extract dependencies using the provided `download_address.txt` files first.
2. Copy required runtime resources (Release folder contents + `mxconvert.exe`) to the working directory.
3. Compile Java files with the specified `javac` command, then run the test program with the `java` command (ensure `java.library.path` points to the current directory).
4. Verify the test success by checking the expected output log (including drawing, file saving and format conversion results).