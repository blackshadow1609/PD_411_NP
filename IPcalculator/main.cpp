#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<CommCtrl.h>
#include<cstdio>
#include"resource.h"

#pragma comment(lib, "Comctl32.lib")

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);					

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		InitCommonControls();
		HWND hSpinPrefix = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hSpinPrefix, UDM_SETRANGE, 0, 32);
	}
		break;
	case WM_COMMAND:
	{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				DWORD dwIPaddress = 0;
				DWORD dwIPmask = UINT_MAX;
				DWORD dwIPprefix = 0;
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				DWORD dwFirst = FIRST_IPADDRESS(dwIPaddress);
				if (dwFirst < 128) dwIPprefix = 8;
				else if (dwFirst < 192)dwIPprefix = 16;
				else if (dwFirst < 224)dwIPprefix = 24;

				CHAR szIPprefix[3] = {};
				sprintf(szIPprefix, "%i", dwIPprefix);
				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)szIPprefix);
				dwIPmask <<= (32 - dwIPprefix);
				SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
			}
		}
			break;
		case IDOK:
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
	}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
	}
	return FALSE;
}