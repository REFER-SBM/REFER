
				REFER 7.0


REFER is a computer software for the automatic reconstruction of polyhedral shapes from sketched axonometric views. This is version 7.0 for PC, under Windows operating system.

REFER is an academic software application, aimed at serving as a laboratory of ideas about Sketch-Based Modeling.
Information about REFER, as much as the group REGEO that implemented the application, can be found at: http://www.regeo.uji.es.

REFER is not intended as a commercial software:
This program is free software,
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

REFER is an interactive program based on a Microsoft Foundation Classes interface.

It was implemented in Microsoft Visual studio 2008. But it has been tested to succesfully export to Microsoft Visual Studio Community 2015 and 2019. 

The folder that contains this ReadMe also containts the code of the Refer application. You may directly open the solution Refer.sln within Microsoft Visual studio 2008 and run it. Alternatively, you may export the solution to Microsoft Visual Studio Community 2015 or 2019, and safely ignore the warnings produced during the migration process. 

However, The application uses two different approaches to produce a Vectorized Line-Drawing. One of the two approaches is a third party contribution

Manuel J. Fonseca, Cesar Pimentel and Joaquim A. Jorge
CALI: An Online Scribble Recognizer for Calligraphic Interface
AAAI Technical Report SS-02-08. Compilation copyright   2002, AAAI (www.aaai.org)

Thus, you must remember to rebuid the CALI library with Microsoft Visual Studio Community 2015 or 2019. At this end, you should run the Recognizer.sln that you will find in the folder RECOGNIZER. After producing the resulting library (Recognizer.lib), do not forget to copy it in the folder lib of the Refer application, where the freeglut lib is already copied. 

A set of examples is provided in the subfolder "Examples", and an html help facility can be found at Refer.chm (folder Help), or directly asking for help from inside the application.

A help document is also provided. The help document is linked to the application, and can be opened through the "help" command you can find in the end of the main menu. The help file also opens as a stand alone application, by running Refer.chm located in the Help folder.

The help document was buid with the Microsoft's HTML Help Workshop compiler(R). And the file Refer.hhp is provided for those users interested in editing the help. 

You can freely use, edit and copy all the open code provided here. But do not forget to cite the original contributions claimed in the comments added at the beginning of the different parts of the code.
