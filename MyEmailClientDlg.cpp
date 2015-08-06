#include "stdafx.h"
#include "MyEmailClient.h"
#include "MyEmailClientDlg.h"
#include "MailMessage.h"
#include "POP3.h"
#include "SMTP.h"
#include "Resource.h"


using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
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
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEmailClientDlg dialog

CMyEmailClientDlg::CMyEmailClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyEmailClientDlg::IDD, pParent)
	, m_recNum(0)
{
	//{{AFX_DATA_INIT(CMyEmailClientDlg)
	m_Body = _T("");
	m_From = _T("");
	m_Password = _T("");
	m_POP3 = _T("");
	m_SMTP = _T("");
	m_To = _T("");
	m_User = _T("");
	m_Subject = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyEmailClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyEmailClientDlg)
	DDX_Text(pDX, IDC_BODY, m_Body);
	DDX_Text(pDX, IDC_FROM, m_From);
	DDX_Text(pDX, IDC_PASSWORD, m_Password);
	DDX_Text(pDX, IDC_POP3, m_POP3);
	DDX_Text(pDX, IDC_SMTP, m_SMTP);
	DDX_Text(pDX, IDC_TO, m_To);
	DDX_Text(pDX, IDC_USER, m_User);
	DDX_Text(pDX, IDC_SUBJECT, m_Subject);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST1, m_listBox);
	DDX_Text(pDX, IDC_RECNUM, m_recNum);
}

BEGIN_MESSAGE_MAP(CMyEmailClientDlg, CDialog)
	//{{AFX_MSG_MAP(CMyEmailClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_BN_CLICKED(IDC_RETR, OnRetr)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHOOSEUSER, &CMyEmailClientDlg::OnBnClickedChooseuser)
	ON_BN_CLICKED(IDC_ADDUSER, &CMyEmailClientDlg::OnBnClickedAdduser)
	ON_BN_CLICKED(IDC_REMOVEUSER, &CMyEmailClientDlg::OnBnClickedRemoveuser)
	ON_LBN_DBLCLK(IDC_LIST1, &CMyEmailClientDlg::OnLbnDblclkList1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEmailClientDlg message handlers

BOOL CMyEmailClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_recNum = 1;
	//get data from "useronfo", a couple of user name and password in a row, only row begin with "#" is vaild
	//thus exchanging "#" with "!" is used as a lazy deletion
	//for example "#grass1field8@gmail.com thisIsPassword"
	CStdioFile myFile;
	CFileException fileException;
	CString dataLine, headChar, nameField;
	if (myFile.Open("userinfo", CFile::typeText | CFile::modeRead | CFile::modeNoTruncate, &fileException))
	{
		myFile.SeekToBegin();
		while (myFile.ReadString(dataLine))
		{
			headChar = dataLine.Mid(0, 1);      //exam the beginning character
			if (headChar != "#")
				continue;
			int breakIndex;
			breakIndex = dataLine.Find(" ");     //get the user name
			nameField = dataLine.Mid(1, breakIndex - 1);
			m_listBox.AddString(nameField);
		}
	}
	else
	{                  
		CString myErro;
		myErro.Format("Fail to open file 'userinfo',erro : %d", fileException.m_cause);
		MessageBox(NULL, myErro, 0);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMyEmailClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyEmailClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyEmailClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMyEmailClientDlg::OnSend()          //send letter
{
	UpdateData( TRUE );       //update data to varibles 

	//create smtp object
	CSMTP smtp( m_SMTP );

	//create a message object
	CMailMessage msg;

	//the data in message object is public, so that it's convenient to get single varible when receive a mail
	msg.m_sFrom = m_From;
	msg.AddMultipleRecipients( m_To );
	msg.m_sSubject = m_Subject;
	msg.m_sBody = m_Body;

	//connect to the server
	if( !smtp.Connect() )
	{
		AfxMessageBox( smtp.GetLastError() );
		return;
	}

	//send the mail now
	if( !smtp.SendMessage( &msg, m_User, m_Password) )
	{
		AfxMessageBox( smtp.GetLastError() );
		return;
	}

	//disconnect
	if( !smtp.Disconnect() )
	{
		AfxMessageBox( smtp.GetLastError() );
		return;
	}
	AfxMessageBox( _T( "Message Sent Successfully") );
}

void CMyEmailClientDlg::OnRetr()        //receive
{
	UpdateData( TRUE );           //update data to varibles 

	CPOP3 pop3( m_POP3 );         //create a pop3 object
	pop3.SetUserProperties(m_User,m_Password);

	//connect
	if (!pop3.Connect())
	{
		AfxMessageBox( pop3.GetLastError() );
		return;
	}
x
	//receive mail
	CMailMessage msg;
	if (!pop3.GetMessage(m_recNum ,&msg))
	{
		AfxMessageBox( pop3.GetLastError() );
		return;
	}

	//get the content
	m_Body=msg.m_sBody;
	m_Subject=msg.m_sSubject;
	m_From=msg.m_sFrom;
	m_To="";
	//get muti receiver
	for (int a=0; a<msg.GetNumRecipients(); a++)
	{
		CString sEmail;
		CString sFriendly;
		msg.GetRecipient(sEmail,sFriendly,a);
		m_To+=sEmail;
		m_To+=" ";
	}
	m_To.TrimRight();

	//disconnect
	if( !pop3.Disconnect() )
	{
		AfxMessageBox( pop3.GetLastError() );
		return;
	}
	AfxMessageBox( _T( "Successfully disconnected" ) );

	UpdateData(FALSE);
}


void CMyEmailClientDlg::OnBnClickedChooseuser()
{
	CStdioFile myFile;           //open a file
	CFileException fileException;
	CString dataLine, headChar, nameField, selectData;
	m_listBox.GetText(m_listBox.GetCurSel(), selectData);     //get the selected row
	if (myFile.Open("userinfo", CFile::typeText | CFile::modeRead | CFile::modeNoTruncate, &fileException))
	{
		myFile.SeekToBegin();
		while (myFile.ReadString(dataLine))
		{
			headChar = dataLine.Mid(0, 1);
			if (headChar != "#")
				continue;
			int breakIndex;
			breakIndex = dataLine.Find(" ");
			nameField = dataLine.Mid(1, breakIndex - 1);
			if (nameField == selectData)
			{
				m_User = nameField;
				m_Password = dataLine.Mid(breakIndex + 1);
			}
		}
	}
	else
	{
		CString myErro;
		myErro.Format("Fail to open file 'userinfo',erro : %d", fileException.m_cause);
		MessageBox(NULL, myErro, 0);
	}
	UpdateData(FALSE);
}


void CMyEmailClientDlg::OnBnClickedAdduser()       
{
	UpdateData(TRUE);
	m_listBox.AddString(m_User);      //add the user name in the MFCControl' varible
	CStdioFile myFile;
	CFileException fileException;
	CString dataWrite;
	
	dataWrite = "#" + m_User + " " + m_Password + "\n";     //write the user name and password into the "userinfo" 
	if (myFile.Open("userinfo", CFile::typeText | CFile::modeWrite | CFile::modeNoTruncate , &fileException))
	{
		myFile.SeekToEnd();
		myFile.WriteString(dataWrite);
	}
	else
	{
		CString myErro;
		myErro.Format("Fail to open file 'userinfo',erro : %d", fileException.m_cause);
		MessageBox(NULL, myErro, 0);
	}
	UpdateData(FALSE);
}


void CMyEmailClientDlg::OnBnClickedRemoveuser()      //delete a user
{
	CStdioFile myFile;
	CFileException fileException;
	CString dataLine, headChar, nameField, selectData;
	m_listBox.GetText(m_listBox.GetCurSel(), selectData); //get the selected user
	m_listBox.DeleteString(m_listBox.GetCurSel());         //delete the user name from the MFCControl' varible
	if (myFile.Open("userinfo", CFile::typeText | CFile::modeReadWrite | CFile::modeNoTruncate, &fileException))
	{
		myFile.SeekToBegin();
		while (myFile.ReadString(dataLine))
		{
			headChar = dataLine.Mid(0, 1);
			if (headChar != "#")
				continue;
			int breakIndex;
			breakIndex = dataLine.Find(" ");
			nameField = dataLine.Mid(1, breakIndex - 1);
			if (nameField == selectData)
			{                  //find the user in file "userinfo" and delete it
				CString temp;
				int offset = -dataLine.GetLength() - 2;
				myFile.Seek(offset, CFile::current);
				myFile.Write("!", 1);
				break;
			}

		}
	}
	else
	{
		CString myErro;
		myErro.Format("Fail to open file 'userinfo',erro : %d", fileException.m_cause);
		MessageBox(NULL, myErro, 0);
	}
	UpdateData(FALSE);
}


void CMyEmailClientDlg::OnLbnDblclkList1()      //double click the item in the list box control
//perform the same function as function OnBnClickedChooseuser
{
	CStdioFile myFile;
	CFileException fileException;
	CString dataLine, headChar, nameField, selectData;
	m_listBox.GetText(m_listBox.GetCurSel(), selectData);  
	if (myFile.Open("userinfo", CFile::typeText | CFile::modeRead | CFile::modeNoTruncate, &fileException))
	{
		myFile.SeekToBegin();
		while (myFile.ReadString(dataLine))
		{
			headChar = dataLine.Mid(0, 1);
			if (headChar != "#")
				continue;
			int breakIndex;
			breakIndex = dataLine.Find(" ");
			nameField = dataLine.Mid(1, breakIndex - 1);
			if (nameField == selectData)
			{
				m_User = nameField;
				m_Password = dataLine.Mid(breakIndex + 1);
			}
		}
	}
	else
	{
		CString myErro;
		myErro.Format("Fail to open file 'userinfo',erro : %d", fileException.m_cause);
		MessageBox(NULL, myErro, 0);
	}
	UpdateData(FALSE);
}
