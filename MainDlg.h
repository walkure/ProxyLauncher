// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(IDC_BROWSE, OnBrowse)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		LoadPreferences();

		return TRUE;
	}

	void OnBrowse(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		CFileDialog dlg(TRUE,_T("exe"),NULL, OFN_HIDEREADONLY | OFN_CREATEPROMPT,
			_T("実行ファイル (*.exe)\0*.exe\0すべてのファイル (*.*)\0*.*\0\0"));
		if(dlg.DoModal() == IDOK)
			SetDlgItemText(IDC_LAUNCH_APP,dlg.m_szFileName);
		
	}

	void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		// TODO: Add validation code 
		SavePreferences();
		LaunchApplication();
		EndDialog(IDOK);
	}

	void OnClose()
	{
		SavePreferences();
		EndDialog(IDCANCEL);
	}

	void SavePreferences()
	{
		CRegKey cReg;
		if( cReg.Create(HKEY_CURRENT_USER,_T("Software\\3pf.jp\\ProxyLauncher"),
			REG_NONE,REG_OPTION_NON_VOLATILE,KEY_WRITE) != ERROR_SUCCESS)
			return;

		ATL::CString param;
		CButton cbHttp = GetDlgItem(IDC_CBHTTP);
		CButton cbHttps = GetDlgItem(IDC_CBHTTPS);

		cReg.SetDWORDValue(_T("http_proxy_use"),cbHttp.GetCheck());
		cReg.SetDWORDValue(_T("https_proxy_use"),cbHttps.GetCheck());
		
		GetDlgItemText(IDC_EBHTTP,param);
		cReg.SetStringValue(_T("http_proxy"),param);
		GetDlgItemText(IDC_EBHTTPS,param);
		cReg.SetStringValue(_T("https_proxy"),param);

		GetDlgItemText(IDC_LAUNCH_APP,param);
		cReg.SetStringValue(_T("app_path"),param);

		cReg.Close();

	}

	void LoadPreferences()
	{
		CRegKey cReg;
		if( cReg.Open(HKEY_CURRENT_USER,_T("Software\\3pf.jp\\ProxyLauncher"),KEY_READ) != ERROR_SUCCESS)
			return;

		DWORD dwValue;
		ULONG paramLength;
		LPWSTR param = new TCHAR[MAX_PATH];

		CButton cbHttp = GetDlgItem(IDC_CBHTTP);
		CButton cbHttps = GetDlgItem(IDC_CBHTTPS);

		if(cReg.QueryDWORDValue(_T("http_proxy_use"),dwValue) == ERROR_SUCCESS)
			cbHttp.SetCheck(dwValue);
		if(cReg.QueryDWORDValue(_T("https_proxy_use"),dwValue) == ERROR_SUCCESS)
			cbHttps.SetCheck(dwValue);

		paramLength = MAX_PATH;
		if(cReg.QueryStringValue(_T("http_proxy"),param,&paramLength) == ERROR_SUCCESS)
			SetDlgItemText(IDC_EBHTTP,param);

		paramLength = MAX_PATH;
		if(cReg.QueryStringValue(_T("https_proxy"),param,&paramLength) == ERROR_SUCCESS)
			SetDlgItemText(IDC_EBHTTPS,param);

		paramLength = MAX_PATH;
		if(cReg.QueryStringValue(_T("app_path"),param,&paramLength) == ERROR_SUCCESS)
			SetDlgItemText(IDC_LAUNCH_APP,param);


	}

	void LaunchApplication()
	{
		CButton cbHttp = GetDlgItem(IDC_CBHTTP);
		CButton cbHttps = GetDlgItem(IDC_CBHTTPS);

		ATL::CString param,argv;

		if(cbHttp.GetCheck())
		{
			GetDlgItemText(IDC_EBHTTP,param);
			SetEnvironmentVariable(_T("http_proxy"),param);
		}

		if(cbHttps.GetCheck())
		{
			GetDlgItemText(IDC_EBHTTPS,param);
			SetEnvironmentVariable(_T("https_proxy"),param);
		}

		GetDlgItemText(IDC_LAUNCH_APP,param);
		argv.Format(_T("\"%s\""),param);

		STARTUPINFO stInfo;
		ZeroMemory(&stInfo,sizeof(STARTUPINFO));
		stInfo.cb = sizeof(STARTUPINFO);
		stInfo.dwFlags = STARTF_USESHOWWINDOW;
		stInfo.wShowWindow = SW_SHOW;

		PROCESS_INFORMATION procInfo;

		CreateProcess(NULL,argv.GetBuffer(1024),NULL,NULL,false,
			NORMAL_PRIORITY_CLASS,NULL,NULL,&stInfo,&procInfo);
		argv.ReleaseBuffer();

	}

};
