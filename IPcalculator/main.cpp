//#define _CRT_SECURE_NO_WARNINGS
//#include<Windows.h>
//#include<CommCtrl.h>
//#include<cstdio>
//#include"resource.h"
//
//#pragma comment(lib, "Comctl32.lib")
//
//BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
//{
//	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
//	return 0;
//}
//
//BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	switch (uMsg)
//	{
//	case WM_INITDIALOG:
//	{
//		InitCommonControls();
//		HWND hSpinPrefix = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
//		SendMessage(hSpinPrefix, UDM_SETRANGE, 0, 32);
//	}
//	break;
//	case WM_COMMAND:
//	{
//		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
//		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
//		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
//		DWORD dwIPaddress = 0;
//		DWORD dwIPmask = UINT_MAX;
//		DWORD dwIPprefix = 0;
//		CHAR szIPprefix[3] = {};
//		switch (LOWORD(wParam))
//		{
//		case IDC_IPADDRESS:
//		{
//			if (HIWORD(wParam) == EN_CHANGE)
//			{
//				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
//				DWORD dwFirst = FIRST_IPADDRESS(dwIPaddress);
//				if (dwFirst < 128) dwIPprefix = 8;
//				else if (dwFirst < 192)dwIPprefix = 16;
//				else if (dwFirst < 224)dwIPprefix = 24;
//
//				sprintf(szIPprefix, "%i", dwIPprefix);
//				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)szIPprefix);
//				dwIPmask <<= (32 - dwIPprefix);
//				SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
//			}
//		}
//		break;
//		case IDC_IPMASK:
//		{
//			if (HIWORD(wParam) == EN_CHANGE)
//			{
//				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
//				for (; dwIPmask; dwIPmask <<= 1)dwIPprefix++;
//				sprintf(szIPprefix, "%i", dwIPprefix);
//				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)szIPprefix);
//			}
//		}
//		break;
//		case IDOK:
//			break;
//		case IDCANCEL:
//			EndDialog(hwnd, 0);
//			break;
//		}
//	}
//	break;
//	case WM_CLOSE:
//		EndDialog(hwnd, 0);
//	}
//	return FALSE;
//}
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <CommCtrl.h>
#include <cstdio>
#include <cstdlib> 
#include "resource.h"

#pragma comment(lib, "Comctl32.lib")

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static BOOL g_bUpdating = FALSE;

DWORD HostToNetworkLong(DWORD hostlong) {
    return ((hostlong & 0x000000FF) << 24) |
        ((hostlong & 0x0000FF00) << 8) |
        ((hostlong & 0x00FF0000) >> 8) |
        ((hostlong & 0xFF000000) >> 24);
}

DWORD NetworkToHostLong(DWORD netlong) {
    return ((netlong & 0x000000FF) << 24) |
        ((netlong & 0x0000FF00) << 8) |
        ((netlong & 0x00FF0000) >> 8) |
        ((netlong & 0xFF000000) >> 24);
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR lpCmdLine, _In_ INT nCmdShow)
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
        SendMessage(hSpinPrefix, UDM_SETRANGE, 0, MAKELONG(32, 0));

        SetDlgItemText(hwnd, IDC_EDIT_PREFIX, "24");

        HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
        SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFFFF00);
    }
    break;
    case WM_COMMAND:
    {
        if (g_bUpdating) break;

        HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
        HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
        HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
        DWORD dwIPaddress = 0;
        DWORD dwIPmask = 0;
        DWORD dwIPprefix = 0;
        CHAR szIPprefix[10] = {};

        g_bUpdating = TRUE; 

        switch (LOWORD(wParam))
        {
        case IDC_IPADDRESS:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                if (SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress) != 0)
                {
                    BYTE firstOctet = FIRST_IPADDRESS(dwIPaddress);

                    if (firstOctet < 128) dwIPprefix = 8;
                    else if (firstOctet < 192) dwIPprefix = 16;
                    else if (firstOctet < 224) dwIPprefix = 24;
                    else dwIPprefix = 24; 

                    if (dwIPprefix == 0) {
                        dwIPmask = 0;
                    }
                    else if (dwIPprefix == 32) {
                        dwIPmask = 0xFFFFFFFF;
                    }
                    else {
                        dwIPmask = (0xFFFFFFFF << (32 - dwIPprefix));
                        dwIPmask = HostToNetworkLong(dwIPmask);
                    }

                    sprintf_s(szIPprefix, sizeof(szIPprefix), "%lu", dwIPprefix);
                    SetWindowText(hEditPrefix, szIPprefix);
                    SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
                }
            }
        }
        break;

        case IDC_IPMASK:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                if (SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask) != 0)
                {
                    dwIPprefix = 0;

                    DWORD hostMask = NetworkToHostLong(dwIPmask);

                    for (int i = 31; i >= 0; i--) {
                        if (hostMask & (1UL << i)) {
                            dwIPprefix++;
                        }
                        else {
                            break;
                        }
                    }

                    sprintf_s(szIPprefix, sizeof(szIPprefix), "%lu", dwIPprefix);
                    SetWindowText(hEditPrefix, szIPprefix);
                }
            }
        }
        break;

        case IDC_EDIT_PREFIX:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                GetWindowText(hEditPrefix, szIPprefix, sizeof(szIPprefix));
                dwIPprefix = atoi(szIPprefix);

                if (dwIPprefix > 32) dwIPprefix = 32;
                if (dwIPprefix < 0) dwIPprefix = 0;

                if (dwIPprefix == 0) {
                    dwIPmask = 0;
                }
                else if (dwIPprefix == 32) {
                    dwIPmask = 0xFFFFFFFF;
                }
                else {
                    dwIPmask = (0xFFFFFFFF << (32 - dwIPprefix));
                    dwIPmask = HostToNetworkLong(dwIPmask);
                }

                SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);

                sprintf_s(szIPprefix, sizeof(szIPprefix), "%lu", dwIPprefix);
                SetWindowText(hEditPrefix, szIPprefix);
            }
        }
        break;

        case IDC_SPIN_PREFIX:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                GetWindowText(hEditPrefix, szIPprefix, sizeof(szIPprefix));
                dwIPprefix = atoi(szIPprefix);

                if (dwIPprefix > 32) {
                    dwIPprefix = 32;
                }
                else if (dwIPprefix < 0) {
                    dwIPprefix = 0;
                }

                if (dwIPprefix == 0) {
                    dwIPmask = 0;
                }
                else if (dwIPprefix == 32) {
                    dwIPmask = 0xFFFFFFFF;
                }
                else {
                    dwIPmask = (0xFFFFFFFF << (32 - dwIPprefix));
                    dwIPmask = HostToNetworkLong(dwIPmask);
                }

                SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);

                sprintf_s(szIPprefix, sizeof(szIPprefix), "%lu", dwIPprefix);
                SetWindowText(hEditPrefix, szIPprefix);
            }
        }
        break;

        case IDOK:
            break;
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        }

        g_bUpdating = FALSE;
    }
    break;
    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    }
    return FALSE;
}