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
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
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

	//Initialization
	//--------------
	SetRegistryKey(_T("Refer 7.0")); //Set label m_pszRegistryKey to later create the root key under HKEY_CURRENT_USER\Software (This fucntion sets the label, but does NOT create the key in the registry)
	LoadParamsFromRegistry(); //(Root Key is created here, if it does not yet exist)
	LoadParamsFromINI(); //REFER parameters initialization
	LoadStdProfileSettings(ReferApp.GeneralParams.MRU_Files);  // Keys to Load Recent File List (MRU) and Settings

	//RenameFolder(ReferApp.GeneralParams.PathExamples_OLD, ReferApp.GeneralParams.PathExamples_NEW);
	//DeleteFolder(ReferApp.GeneralParams.PathExamples);

	HKEY key;
	CString path;
	path.Format(_T("SOFTWARE\\%s"), AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszAppName);
	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_ALL_ACCESS, &key);
	if (lResult != ERROR_SUCCESS){
		AfxMessageBox("Registry initialization failure (HKEY_CURRENT_USER->"+path);
	}
	RegCloseKey(key);

	////Use this code if you need to delete the application registry key
	//CRegKey Rkey1;
	//Rkey1.Open(HKEY_CURRENT_USER, _T("SOFTWARE"), KEY_ALL_ACCESS);
	//LONG lResult1 = Rkey1.RecurseDeleteKey(AfxGetApp()->m_pszRegistryKey);
	//if (lResult1 != ERROR_SUCCESS){
	//	AfxMessageBox("Registry deletion failure (HKEY_CURRENT_USER->software->Refer 7.0)!");
	//}

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


void CReferApp::OnFileOpen()
{
	//CWinApp::OnFileOpen();

	// Create a file open dialog
	CString sMensaje;
	sMensaje = "Refer sketch files(*.stk)|*.stk|";
	sMensaje += "Refer line-drawing files(*.dxf)|*.dxf|";
	sMensaje += "Varley's sketch files(*.Sketch)|*.Sketch|";
	//sMensaje += "STL files(*.stl)|*.stl|";
	//sMensaje += "OBJ files(*.obj)|*.obj|";
	//sMensaje += "IGES files(*.igs)|*.igs|";
	//sMensaje += "CSG files(*.csg)|*.csg|";
	sMensaje += "All files(*.*)|*.*||";

	CFileDialog fdOpen(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, sMensaje);

	// Set current directory for file open dialog
	static CString strLastDir = ReferApp.GeneralParams.PathExamples;
	fdOpen.m_ofn.lpstrInitialDir = strLastDir;

	// Open dialog
	if (fdOpen.DoModal() == IDOK) {
		CString fileName = fdOpen.GetPathName();

		//SetPathName(fileName);
		OpenDocumentFile(fileName);
		AddToRecentFileList(fileName);

		//Update last dir
		strLastDir = fileName.Left(fdOpen.m_ofn.nFileOffset);
	}
}


void CReferApp::LoadParamsFromRegistry()
// Reads default setting from Registry
// Related: SaveParamsToRegistry()
{
	HKEY key;

	CString path;
	path.Format(_T("SOFTWARE\\%s\\%s\\Configuration"), AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszAppName);

	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_ALL_ACCESS, &key);

	if (lResult == ERROR_SUCCESS){
		BYTE data[512];
		DWORD type = REG_SZ;

		DWORD size = sizeof(data);
		RegQueryValueEx(key, "PathSetup", 0, &type, data, &size);
		CString PathINI(data);
		ReferApp.GeneralParams.PathINI = PathINI;

		size = sizeof(data);
		RegQueryValueEx(key, "PathExamples", 0, &type, (BYTE*)data, &size);
		CString PathExamples(data);
		ReferApp.GeneralParams.PathExamples = PathExamples;
	}
	//else{
	//	AfxMessageBox("REFER registry access failure (HKEY_CURRENT_USER->software->Refer 7.0)!");
	//}

	RegCloseKey(key);
}


void CReferApp::SaveParamsToRegistry()
// Updates default setting to file "ReferPaths.ini"
// The file must be in the same directory than the executable Refer.exe

// Related: LoadParamsFromRegisytry()
{
	HKEY key;
	DWORD flag;
	CString path;
	path.Format(_T("SOFTWARE\\%s\\%s\\Configuration"), AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszAppName);
	LPCTSTR lpSubKey = path;

	LONG lResult = RegCreateKeyEx(HKEY_CURRENT_USER, 
								  lpSubKey,
								  0,
								  NULL,
								  REG_OPTION_NON_VOLATILE,
								  KEY_ALL_ACCESS,
								  NULL,
								  &key,
								  &flag);

	if (lResult == ERROR_SUCCESS){
		//if (flag == REG_CREATED_NEW_KEY) {
		//	//Key was created from scratch
		//}
		//else if (flag == REG_OPENED_EXISTING_KEY) {
		//	//Already existing key was open
		//}

		LPCTSTR lpValueName = _T("PathSetup");
		LPCTSTR lpData = ReferApp.GeneralParams.PathINI;
		DWORD size = (DWORD)((_tcslen(lpData) + 1) * sizeof(TCHAR));
		RegSetValueEx(key, lpValueName, 0, REG_SZ, (BYTE*)lpData, size);

		lpValueName = _T("PathExamples");
		lpData = ReferApp.GeneralParams.PathExamples;
		size = (DWORD)((_tcslen(lpData) + 1) * sizeof(TCHAR));
		RegSetValueEx(key, lpValueName, 0, REG_SZ, (BYTE*)lpData, size);
	}
	else {
		AfxMessageBox("REFER registry access failure (HKEY_CURRENT_USER->software->Refer 7.0)!");
	}

	RegCloseKey(key);
}


//bool CReferApp::DeleteFolder(const CString FolderPath)
////Delete folder "strFolderPath"
////Return TRUE if successful
//{
//	//SHFileOperation requires that the path end in a double null character '\0'
//	int nLen = FolderPath.GetLength();
//	TCHAR* pszFrom = new TCHAR[nLen + 2];
//
//	_tcscpy_s(pszFrom, nLen + 1, FolderPath);
//	pszFrom[nLen] = _T('\0');
//	pszFrom[nLen + 1] = _T('\0');
//
//	SHFILEOPSTRUCT fileOp;
//	ZeroMemory(&fileOp, sizeof(SHFILEOPSTRUCT));
//
//	fileOp.hwnd = NULL;
//	fileOp.wFunc = FO_DELETE; // Delete flag
//	fileOp.pFrom = pszFrom;   // path to delete
//	fileOp.pTo = NULL;        // Not required
//
//	//Flags to delete without asking for confirmation and hide the UI
//	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
//
//	int Result = SHFileOperation(&fileOp);
//
//	//Free memory
//	delete[] pszFrom;
//
//	return (Result == 0);
//}


bool CReferApp::MoveFolder(CString oldFolder, CString newFolder)
{
	BOOL moved = MoveFile(oldFolder, newFolder);

	if (moved){
		//AfxMessageBox(_T("Folder successfully renamed!"));
		return true;
	}
	else{
		DWORD dwError = GetLastError();
		CString Messaje;
		Messaje.Format(_T("Error while moving folder %s to %s. Code: %d"), oldFolder, newFolder, dwError);
		AfxMessageBox(Messaje);
		return false;
	}
}


void CReferApp::LoadParamsFromINI()
// Reads default setting from file "Refer.ini"
// The file must be:
// - in the "PathINI" folder, or 
// - in the same folder than the executable Refer.exe

// Related: SaveParamsToINI()
{
	// Reads the file and updates the settings
	std::ifstream in;
	in.open((ReferApp.GeneralParams.PathINI + "\\Refer.ini"));
	if (!in.is_open()) {
		in.open("Refer.ini");  //alternatively, searches the file in the local folder
	}

	if (in.is_open()) {
		char sRefer[256];
		char cTrue = '1';

		//Vectorization
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bRefine2D_OnLine = (sRefer[50] == cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bRefine2D_OffLine = (sRefer[50] == cTrue);

		//Detection
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bDetectCuesAndFeatures = (sRefer[50] == cTrue);

		//Detect cues
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectCues = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectValences = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectParallelEdges = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectVanishingPoints = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectGraphs = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectPerimeter = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectLabels = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectMainAxes = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectFaces = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectTypology = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectHiddenEdges = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectSymmetry = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectLevels = (sRefer[50] == cTrue);

		//Detect features
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectFeatures = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectRibsAndDividers = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectRailsAndSlots = (sRefer[50] == cTrue);
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DetectStepsAndPockets = (sRefer[50] == cTrue);

		//Reconstruction flow
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bInflateBRep = (sRefer[50] == cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.DirectInflation = (sRefer[50] == cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.Optimize = (sRefer[50] == cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bRefine3D = (sRefer[50] == cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bTessellate3D = (sRefer[50] == cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bBuildModelCSG = (sRefer[50] == cTrue);

		//Formats
		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bIGS_Type_143 = (sRefer[50] == cTrue);

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.bIGS_NgonFaces = (sRefer[50] == cTrue);


		in.getline(sRefer, 256);
		ReferApp.GeneralParams.MRU_Files = sRefer[50] - '0';  //Convert char to int

		in.getline(sRefer, 256);
		ReferApp.GeneralParams.toolbarButtonsSize = sRefer[50] - '0';  //Convert char to int

		in.getline(sRefer, 256);
		CString linea = sRefer;
		ReferApp.GeneralParams.PathINI = linea.Mid(50, linea.GetLength() - 49);

		in.getline(sRefer, 256);
		linea = sRefer;
		ReferApp.GeneralParams.PathExamples = linea.Mid(50, linea.GetLength() - 49);

		in.close ();

		//Synchronize the registry keys with the INI parameters
		SaveParamsToRegistry();
	}
	else{
		//Display the credits if settings file does not exist
		//(because it means that the application is running for the first time)
		CAboutDlg aboutDlg;
		aboutDlg.DoModal();

		//Save default params
		SaveParamsToINI();
	}
}


void CReferApp::SaveParamsToINI()
// Updates default setting to file "Refer.ini"
// The file is placed:
// - in the "PathINI" folder, or 
// - in the same folder than the executable Refer.exe

// Related: LoadParamsFromINI()
{
	// Writes the file with the current settings
	std::ofstream out;
	out.open((ReferApp.GeneralParams.PathINI + "\\Refer.ini"));
	if (!out.is_open()) {
		CString Message("Failure in saving Refer.ini\r\n");
		Message += "Folder ";
		Message += ReferApp.GeneralParams.PathINI;
		Message += " not available\r\n";
		Message += "Create the folder, or review data folder assignment in Menu->File->General Settings\r\n";
		Message += "\r\n";
		Message += "Now trying to save ini file in the application local folder:\r\n";
		CString strModule = AfxGetApp()->m_pszExeName;
		strModule.Append(_T(".exe"));
		HMODULE hmod = GetModuleHandle(strModule);
		TCHAR szPath[MAX_PATH];
		GetModuleFileName(hmod, szPath, MAX_PATH);
		Message += szPath;
		AfxMessageBox(Message);

		//If the path folder does not exist, tries to save the file in the local folder
		out.open("Refer.ini");
	}

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

		out << "Save IGS files as type 143(1) or 144(0)          ="
			<< ReferApp.GeneralParams.bIGS_Type_143
			<< std::endl;
		out << "Save IGS files as polygons(1) or triangles(0)    ="
			<< ReferApp.GeneralParams.bIGS_NgonFaces
			<< std::endl;

		out << "Load Most Recently Used files (MRU)              ="
			<< ReferApp.GeneralParams.MRU_Files
			<< std::endl;
		out << "Toolbar buttons size (0 default 24x22, >0 scale) ="
			<< ReferApp.GeneralParams.toolbarButtonsSize
			<< std::endl;
		out << "REFER data folder, path for the INI file         ="
			<< ReferApp.GeneralParams.PathINI
			<< std::endl;
		out << "REFER default path for application files         ="
			<< ReferApp.GeneralParams.PathExamples
			<< std::endl;
		out.close();
	}

	else {
		CString Message("Failure in locally saving Refer.ini\r\n");
		Message += "Application local folder not available (read only?):\r\n";
		CString strModule = AfxGetApp()->m_pszExeName;
		strModule.Append(_T(".exe"));
		HMODULE hmod = GetModuleHandle(strModule);
		TCHAR szPath[MAX_PATH];
		GetModuleFileName(hmod, szPath, MAX_PATH);
		Message += szPath;
		AfxMessageBox(Message);
	}

	//Update registry params
	SaveParamsToRegistry();
}
