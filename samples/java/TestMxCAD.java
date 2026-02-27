package com.mxcad.wrapper;

import java.io.IOException;

public class TestMxCAD {
    // Load DLL
    static {
        try {
            System.loadLibrary("MxCADWrapperLib");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Failed to load DLL: " + e);
            System.exit(1);
        }
    }

    public static void main(String[] args) {
        System.out.println("--- Start testing MxCAD Java API ---");

        MxCADAppWrapper app = null;
        MxCADObjectWrapper obj = null;

        try {
            // Init App
            app = new MxCADAppWrapper();
            System.out.println("App initialized successfully");

            // Init object wrapper
            obj = new MxCADObjectWrapper();
            
            // Create internal object
            if (obj.Create()) {
                System.out.println("MxCADObject created successfully");

                // Draw line
                int status = obj.DrawLine(0, 0, 100, 100);
                System.out.println("Draw line status: " + status);

                // Draw circle
                status = obj.DrawCircle(50, 50, 25);
                System.out.println("Draw circle status: " + status);

                // Save file, only mxweb
                String savePath = "D:\\test_draw.mxweb";
                int saveRet = obj.WriteFile(savePath);
                if (saveRet != 0) {
                    System.out.println("File save failed! Ret: " + saveRet);
                } else {
                    System.out.println("File saved successfully: " + savePath);
                    
                    // Convert to dwg using command line tool
                    convertMxwebToDwg(savePath, "D:\\test_draw.dwg");
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            // Release resources
            if (obj != null) obj.delete();
            if (app != null) {
				app.Free();
				app.delete();
			}
        }
        
        System.out.println("--- Test finished ---");
    }
    
    /**
     * Convert mxweb file to dwg using mxconvert.exe
     * @param sourcePath Path of mxweb file
     * @param outputPath Path of dwg file
     */
    private static void convertMxwebToDwg(String sourcePath, String outputPath) {
        System.out.println("Starting convert mxweb to dwg...");
        
		String converterPath = ".\\mxconvert.exe";
		
        // Build command line
        String[] cmd = {
            "cmd.exe", "/c", 
            converterPath, 
            "-s", sourcePath, 
            "-o", outputPath
        };
        
        try {
            // Execute command
            Process process = Runtime.getRuntime().exec(String.join(" ", cmd));
            int exitCode = process.waitFor();
            
            // Check result
            if (exitCode == 0) {
                System.out.println("Convert success! DWG file saved to: " + outputPath);
            } else {
                System.err.println("Convert failed! Exit code: " + exitCode);
            }
        } catch (IOException e) {
            System.err.println("IO error when converting file: " + e.getMessage());
        } catch (InterruptedException e) {
            System.err.println("Convert process interrupted: " + e.getMessage());
            Thread.currentThread().interrupt();
        }
    }
}