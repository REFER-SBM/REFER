
				REFER 7.0 (see https://doi.org/10.1142/14656)


The folder containing this ReadMe also contains two subfolders:

1. The first folder contains the installer needed to prepare the application for use. The compressed file (ReferInstaller.zip) includes everything required to install the REFER 7.0 executable.

	Steps to install the application:
	1. Download the ZIP file.
	2. Place it in a local folder (for example, Downloads or Desktop/REFER).
	3. Extract the contents of the ZIP file.
	4. Run the installer setup.exe. During the installation process, permission may be requested to install Visual Studio Redistributable libraries.
	5. Once the installation is complete, the installer files may be safely removed
	6. Run Refer.exe.
	7. Browse, search, and open any of the examples in the "Examples" folder (default location: C:/REFER Data/Examples).

	WARNINGS:
	(1) REFER requires .NET Framework 4.7. If the automatic installation through the VS2026 installer fails, it is recommended to enable .NET Framework 3.5 by following these steps:
		1. Press the Windows key, type "Turn Windows features on or off".
		2. In the list, locate and check the box for .NET Framework 4.7 (includes .NET 2.0 and 3.0).
		3. Click OK and select "Download files from Windows Update" if prompted.
		4. Restart the computer if necessary.

	(2) REFER supports multiple execution modes, and an INI file stores the execution preferences. By default, the installation places the executable in a subfolder of the Program Files directory. Because this directory is protected, the refer.ini configuration file is stored in "C:/REFER Data." You can change this default path to your preferred folder. Although customizing the application through the File -> General_Settings menu is recommended, the INI file may also be edited with any text editor to modify the default execution preferences.

	(3) Since REFER is an MFC application, installation of the Visual C++ Redistributable Runtime Libraries may be required. If necessary, visit the "Microsoft Visual C++ Redistributable latest supported downloads" page to download and install the appropriate library.

	(4) REFER includes a setting that allows adjustment of the menu button size. If buttons appear too small at high screen resolutions, you can adjust the scaling settings via Start -> Settings -> System -> Display -> Scale. Changing this setting requires restarting Refer.



2. The second subfolder contains the source code of the REFER application. The file "Solution Refer.sln" can be opened directly with Microsoft Visual Studio Community 2026. For compatibility with previous versions, the solution can also be opened with Microsoft Visual Studio Community 2015 or Visual Studio 2008 using the corresponding solution files located in the "Solutions previous versions" folder.

	WARNINGS:
	(1) The application uses two different approaches to generate vectorized line drawings. One of these approaches is based on a third-party contribution:

		Manuel J. Fonseca, Cesar Pimentel and Joaquim A. Jorge
		CALI: An Online Scribble Recognizer for Calligraphic Interface
		AAAI Technical Report SS-02-08. Compilation copyright 2002, AAAI (www.aaai.org)

	For backward compatibility, the CALI library must be rebuilt using the corresponding version of Microsoft Visual Studio 2008 or 2015. To do so, open and build the solution file Recognizer.sln located in the RECOGNIZER folder. After generating the resulting library file (Recognizer.lib), copy it into the lib folder of the REFER application, where the freeglut library is also located.

	(2) The help documentation was built using Microsoft's HTML Help Workshop compiler. The HTML help file is available as Refer.chm in the Help folder and may also be accessed directly from within the application. In addition, the file Refer.hhp is provided for users interested in editing or rebuilding the help documentation.



				**********
				

REFER is a computer software for the automatic reconstruction of polyhedral shapes from sketched axonometric views. This is version 7.0 for PC, under Windows operating system.


The following book serves not only as a handbook for REFER, but also demonstrates how procedural modeling, graph-based methods, and established CAD principles can contribute to the training of AI systems and provide a pathway toward more robust and scalable SBM solutions:

	Company P., Camba J.D.
	"Handbook of Sketch-Based Modeling for Engineering Design: A Practical Approach with Refer".
	World Scientific Publishing Company, 2026. (456 pages).
	ISBN-10: ‎ 9819825652. ISBN-13: ‎ 978-9819825653. https://doi.org/10.1142/14656.


REFER is an academic software application designed to serve as a laboratory for ideas related to Sketch-Based Modeling (SBM). Information about the REGEO research group that developed the application can be found at: http://www.regeo.uji.es.

REFER is an interactive program based on a Microsoft Foundation Classes (MFC) interface. It was originally implemented in Microsoft Visual Studio 2008 and was later successfully ported to Microsoft Visual Studio Community 2015 and 2026.

Currently REFER is built on 69294 lines of C++ code and 11429 lines of C code (more than 47000 logic lines, and some 220 classes), organized in approximately 360 files. An extensive set of 20000 lines of comments helps to better understand the code.


				**********


REFER is not intended to be commercial software.
This program is free software and is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY, including the implied warranties of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
All open-source code provided here may be freely used, modified, and redistributed. However, users are requested to cite: https://doi.org/10.1142/14656
as well as the original contributions identified in the comments at the beginning of the different sections of the code.