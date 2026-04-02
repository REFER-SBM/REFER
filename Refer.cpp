//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

//Both the WIMP structure and the database of the application
//were originally developed by Jaunvi Andreu:
	//Juan Vicente Andreu Hernandez
	//Desarrollo, implementacion y prueba de un algoritmo
	//de reconstruccion de objetos
	//a partir de una representacion axonometrica
	//utilizando tecnicas de optimizacion
	//Final career project, 2000.
	//Facultad de Informatica
	//Universidad Politecnica de Valencia

//They were later extensively modified by Pedro Company



// Refer.cpp : Defines the class behaviors for the application.
//

#include <stdafx.h>
#include <fstream>
#include <gl\freeglut.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "resource.h"
#include "ReferDoc.h"
#include "Refer.h"

#include "WinRf_MainFrame.h"

#include "WinRf_FrameSketch2D.h"
#include "WinRf_FrameModelBrep.h"
#include "WinRf_FrameModelCSG.h"
#include "WinRf_FrameOptimization.h"

#include "WinRf_GDI_Transform.h"

#include "DB_Sketch.h"
#include "DialogPreviousNext.h"
#include "WinRf_ViewSketch2D.h"

#include "WinRf_OpenGLDevice.h"

#include "WinRf_OpenGLViewInflation.h"
#include "WinRf_OpenGLViewModelBrep.h"
#include "WinRf_ViewModelBrep.h"

#include "WinRf_OpenGLViewModelCSG.h"
#include "DialogPreviousNextCSG.h"
#include "WinRf_ViewModelCSG.h"

#include "WinRf_ViewOptimization_Graphics.h"
#include "WinRf_ViewOptimization.h"


/////////////////////////////////////////////////////////////////////////////
// The one and only CReferApp object

CReferApp ReferApp;

// Use AfxGetApp() to recover this pointer whenever you need it 

/////////////////////////////////////////////////////////////////////////////
// CReferApp

BEGIN_MESSAGE_MAP(CReferApp, CWinApp)
	//{{AFX_MSG_MAP(CReferApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_REFER_SAVE_SETTINGS, OnSaveSettings)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReferApp construction

CReferApp::CReferApp()
{
	EnableHtmlHelp();
}


/////////////////////////////////////////////////////////////////////////////
// CReferApp initialization
// WinMain calls InitInstance every time a new instance of the application starts
// On termination, WinMain calls the application object's ExitInstance member function

BOOL CReferApp::InitInstance()
{
//*** TO DEBUG MEMORY ***********************
	// AfxEnableMemoryTracking(false);
	// Activates or deactivates memory diagnosis
	// Desactivated in version Release
	// Active in version Debug. Runs faster if desctivated

	//afxMemDF = allocMemDF | checkAlwaysMemDF;

	//_CrtSetBreakAlloc( 424 );
//*** END MEMORY DEBUG ************************

	LoadParamsFromINI();
	SaveParamsToINI();  //Creates an initialization file, useful just in case this is the first time that the program runs 

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("REGEO. Pedro Company SOFTWARE"));

	LoadStdProfileSettings(ReferApp.GeneralParams.MRU_Files);  // Load standard INI file options (including MRU)
															   // Loads the most recently used files (MRU's)

	// Register the application's document templates
	// (Document templates serve as the connection between documents, frame windows and views)
	// ---------------------------------------------------------------------------------------
	m_pTemplateFrSketch2D = new CMultiDocTemplate(
		IDR_2D_SKETCH,
		RUNTIME_CLASS(CReferDoc),
		RUNTIME_CLASS(CWinRf_FrameSketch2D),
		RUNTIME_CLASS(CWinRf_ViewSketch2D));
	AddDocTemplate(m_pTemplateFrSketch2D);

	m_pTemplateFrModelBrep = new CMultiDocTemplate(
		IDR_BREP_MODEL,
		RUNTIME_CLASS(CReferDoc),
		RUNTIME_CLASS(CWinRf_FrameModelBrep),
		RUNTIME_CLASS(CWinRf_ViewModelBrep));
	AddDocTemplate(m_pTemplateFrModelBrep);

	m_pTemplateFrModelCSG = new CMultiDocTemplate(
		IDR_CSG_MODEL,
		RUNTIME_CLASS(CReferDoc),
		RUNTIME_CLASS(CWinRf_FrameModelCSG),
		RUNTIME_CLASS(CWinRf_ViewModelCSG));
	AddDocTemplate(m_pTemplateFrModelCSG);

	m_pTemplateFrOptimization = new CMultiDocTemplate(
		IDR_OPTIMIZATION,
		RUNTIME_CLASS(CReferDoc),
		RUNTIME_CLASS(CWinRf_FrameOptimization),
		RUNTIME_CLASS(CWinRf_ViewOptimization));
	AddDocTemplate(m_pTemplateFrOptimization);

	// create main MDI Frame window
	CWinRf_MainFrame* pMainFrame = new CWinRf_MainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return false;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(true);  // Register all of your application's document types

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// DON'T display a new MDI child window during startup!!!
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return false;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	//Set the help file name for MFC calls
		////First free the string allocated by MFC at CWinApp startup
		////(The string is allocated before InitInstance is called)
		//free((void*)m_pszHelpFilePath);

		////Change the name of the help file
		////(The CWinApp destructor will free the memory)
		//m_pszHelpFilePath=_tcsdup(_T("c:\\D\\Refer 7.0\\Refer.chm"));

	CString strHelpFile = m_pszHelpFilePath;
	strHelpFile.MakeLower();
	if (strHelpFile.Right(4) == ".hlp") {
		strHelpFile = strHelpFile.Left(strHelpFile.GetLength()-4) + ".chm";
		free((void*)m_pszHelpFilePath);
		m_pszHelpFilePath = _tcsdup(strHelpFile);
	}

	CFileStatus FileStatus;
	if (!CFile::GetStatus(m_pszHelpFilePath, FileStatus)) {
		CString Message("Help file not found:\r\n");
		AfxMessageBox(Message+m_pszHelpFilePath);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
//	void Versiones(CString sPrograma, CString sOptimizador, CString sImportarExportar);
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString m_sVersionProgram;
	CString m_sVersionOpenGL;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//m_sVersionPrograma = _T("");
	//}}AFX_DATA_INIT

	m_sVersionProgram = "REFER Version 7.0   ";
	#ifdef _DEBUG
		m_sVersionProgram += "*******   Compilation: DEBUG (";
	#else
		m_sVersionProgram += "*******   Compilation: RELEASE  (";
	#endif

	time_t t = time(0);
	struct tm now;
	localtime_s(&now, &t);
	CString date;
	date.Format("%04d-%02d-%02d", (now.tm_year + 1900), (now.tm_mon + 1), now.tm_mday);
	m_sVersionProgram += date;
	m_sVersionProgram += ")";


	m_sVersionOpenGL = (CString) "OPENGL Version " + gluGetString(GLU_VERSION) + "\r\n" +
	                             "               " + gluGetString(GLU_EXTENSIONS);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_EDIT_VersionProgram, m_sVersionProgram);
	DDX_Text(pDX, IDC_EDIT_VersionOpenGL, m_sVersionOpenGL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CReferApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
//	aboutDlg.Versiones( VERSION_PROGRAMA, VERSION_OPTIMIZADOR, VERSION_IMPORTAR_EXPORTAR );
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CReferApp commands

int CReferApp::ExitInstance() 
// Called when ID_APP_EXIT
{
	//// Delete the application's document templates
	//delete m_pTemplateFrSketch2D;
	//delete m_pTemplateFrModelBrep;
	//delete m_pTemplateFrModelCSG;
	//delete m_pTemplateFrOptimization;

	return CWinApp::ExitInstance();
}


CDocument* CReferApp::OpenDocumentFile(LPCTSTR lpszFileName)
// Called by the framework as part of the File Open command (See CDocManager::OnFileOpen)

// Creates the child windows:
//		2D sketch
//		model Brep
{

	// As the application supports multiple document templates,
	// the framework uses the file name extension to find the appropriate
	// document template to try to load the document.
	// If successful, the document template then creates a frame window and view for the document.
	CReferDoc* pDoc = (CReferDoc*)CWinApp::OpenDocumentFile(lpszFileName);
	if (pDoc == NULL)
		return NULL;

	// Searches the 2D sketch window (assuming than this is the frame that has been created by default),
	// to store its frame handler
	CView* pView;
	POSITION pos = pDoc->GetFirstViewPosition();
	ASSERT(pos != NULL);
	pView = pDoc->GetNextView(pos);
	pDoc->m_pFrameSketch2D = (CMDIChildWnd*) pView->GetParentFrame();

	// Creates the child windows
	// in reverse order to which they should occur in the TILE_VERTICAL

	// MDI child window aimed at "Model Brep"
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pDoc->m_pFrameModelBrep= pFrame->OnCreateWindowModelBrep(pDoc);

	// Activates now the 2D Sketch window, to ensure it to be the first in the TILE VERTICAL
	pDoc->m_pFrameSketch2D->MDIActivate();

	// Tile the MDI children windows within the MDI frame window
	ASSERT_KINDOF(CMDIChildWnd, pDoc->m_pFrameModelBrep);
	CMDIFrameWnd* pMDIFrameWnd = pDoc->m_pFrameModelBrep->GetMDIFrame();
	ASSERT(pMDIFrameWnd != NULL);
	pMDIFrameWnd->MDITile(MDITILE_VERTICAL);

	return pDoc;
}


void CReferApp::LoadParamsFromINI()
// Reads default setting from file "Refer.ini"
// The file must be in the same directory than the executable Refer.exe

// Related: SaveParamsToINI()
{
	// Reads the file and updates the settings
	std::ifstream in ("Refer.ini");
	if (in.is_open()){
		char sRefer[256];
		char cTrue='1';

		//Vectorization
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bRefine2D_OnLine= (sRefer[50]==cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bRefine2D_OffLine= (sRefer[50]==cTrue);

		//Detection
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bDetectCuesAndFeatures= (sRefer[50]==cTrue);

		//Detect cues
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectCues= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectValences= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectParallelEdges= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectVanishingPoints= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectGraphs= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectPerimeter= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectLabels= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectMainAxes= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectFaces= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectTypology= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectHiddenEdges= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectSymmetry= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectLevels= (sRefer[50]==cTrue);

		//Detect features
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectFeatures= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectRibsAndDividers= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectRailsAndSlots= (sRefer[50]==cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectStepsAndPockets= (sRefer[50]==cTrue);

		//Reconstruction flow
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bInflateBRep= (sRefer[50]==cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DirectInflation= (sRefer[50]==cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.Optimize= (sRefer[50]==cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bRefine3D= (sRefer[50]==cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bTessellate3D= (sRefer[50]==cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bBuildModelCSG= (sRefer[50]==cTrue);

		//Formats
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.MRU_Files= sRefer[50] -'0';  //Convert char to int

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bIGS_Type_143= (sRefer[50]==cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bIGS_NgonFaces= (sRefer[50]==cTrue);

		in.close ();
	}
	else{
		//Display the credits if settings file does not exist
		//(because it means that the application is running for the first time)
		CAboutDlg aboutDlg;
		aboutDlg.DoModal();
	}
}


void CReferApp::SaveParamsToINI()
// Updates default setting to file "Refer.ini"
// The file must be in the same directory than the executable Refer.exe

// Related: LoadParamsFromINI()
{
	// Writes the file with the current settings
	std::ofstream out ("Refer.ini");
	if (out.is_open ()) {
		out << "Online refine                                    ="
			<< ReferApp.GeneralParams.bRefine2D_OnLine
			<< std::endl;
		out << "Offline refine                                   ="
			<< ReferApp.GeneralParams.bRefine2D_OffLine
			<< std::endl;

		out << "Detect cues and features                         ="
			<< ReferApp.GeneralParams.bDetectCuesAndFeatures
			<< std::endl;

		out << "     Detect cues                                 ="
			<< ReferApp.GeneralParams.DetectCues
			<< std::endl;
		out << "        Detect valences                          ="
			<< ReferApp.GeneralParams.DetectValences
			<< std::endl;
		out << "        Detect parallel edges                    ="
			<< ReferApp.GeneralParams.DetectParallelEdges
			<< std::endl;
		out << "        Detect vanishing points                  ="
			<< ReferApp.GeneralParams.DetectVanishingPoints
			<< std::endl;
		out << "        Detect graphs                            ="
			<< ReferApp.GeneralParams.DetectGraphs
			<< std::endl;
		out << "        Detect perimeter                         ="
			<< ReferApp.GeneralParams.DetectPerimeter
			<< std::endl;
		out << "        Detect labels                            ="
			<< ReferApp.GeneralParams.DetectLabels
			<< std::endl;
		out << "        Detect main axes                         ="
			<< ReferApp.GeneralParams.DetectMainAxes
			<< std::endl;
		out << "        Detect faces                             ="
			<< ReferApp.GeneralParams.DetectFaces
			<< std::endl;
		out << "        Detect typology                          ="
			<< ReferApp.GeneralParams.DetectTypology
			<< std::endl;
		out << "        Detect hidden edges                      ="
			<< ReferApp.GeneralParams.DetectHiddenEdges
			<< std::endl;
		out << "        Detect symmetry                          ="
			<< ReferApp.GeneralParams.DetectSymmetry
			<< std::endl;
		out << "        Detect levels                            ="
			<< ReferApp.GeneralParams.DetectLevels
			<< std::endl;

		out << "     Detect features                             ="
			<< ReferApp.GeneralParams.DetectFeatures
			<< std::endl;
		out << "        Detect ribs and dividers                 ="
			<< ReferApp.GeneralParams.DetectRibsAndDividers
			<< std::endl;
		out << "        Detect rails and slots                   ="
			<< ReferApp.GeneralParams.DetectRailsAndSlots
			<< std::endl;
		out << "        Detect steps and pockets                 ="
			<< ReferApp.GeneralParams.DetectStepsAndPockets
			<< std::endl;

		out << "BRep Inflation                                   ="
			<< ReferApp.GeneralParams.bInflateBRep
			<< std::endl;
		out << "     Direct inflation                            ="
			<< ReferApp.GeneralParams.DirectInflation
			<< std::endl;
		out << "     Optimization inflation                      ="
			<< ReferApp.GeneralParams.Optimize
			<< std::endl;
		out << "3D Refine                                        ="
			<< ReferApp.GeneralParams.bRefine3D
			<< std::endl;
		out << "3D Tessellate                                    ="
			<< ReferApp.GeneralParams.bTessellate3D
			<< std::endl;
		out << "CSG-based modeling                               ="
			<< ReferApp.GeneralParams.bBuildModelCSG
			<< std::endl;

		out << "Load Most Recently Used files (MRU)              ="
			<< ReferApp.GeneralParams.MRU_Files
			<< std::endl;
		out << "Save IGS files as type 143(1) or 144(0)          ="
			<< ReferApp.GeneralParams.bIGS_Type_143
			<< std::endl;
		out << "Save IGS files as polygons(1) or triangles(0)    ="
			<< ReferApp.GeneralParams.bIGS_NgonFaces
			<< std::endl;
		out.close ();
	}
}


void CReferApp::OnSaveSettings() 
// Saves current settings
{
	SaveParamsToINI();
}
