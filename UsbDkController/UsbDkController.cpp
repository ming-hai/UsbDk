// UsbDkController.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "UsbDkHelper.h"

using namespace std;
//-------------------------------------------------------------------------------

void ShowUsage()
{
    tcout << endl;
    tcout << TEXT("              UsbDkController usage:") << endl;
    tcout << endl;
    tcout << TEXT("UsbDkController -i  - install UsbDk driver") << endl;
    tcout << TEXT("UsbDkController -u  - uninstall UsbDk driver") << endl;
    tcout << TEXT("UsbDkController -n  - enumerate USB devices") << endl;
    tcout << TEXT("UsbDkController -r ID SN - Redirect USB device (start-stop) by ID and serial number") << endl;
    tcout << endl;
}
//-------------------------------------------------------------------------------

void Controller_InstallDriver()
{
    tcout << TEXT("Installing UsbDk driver") << endl;
    auto res = InstallDriver();
    switch (res)
    {
    case InstallSuccess:
        tcout << TEXT("UsbDk driver installation succeeded") << endl;
        break;
    case InstallFailure:
        tcout << TEXT("UsbDk driver installation failed") << endl;
        break;
    case InstallFailureNeedReboot:
        tcout << TEXT("UsbDk driver installation failed. Reboot your machine") << endl;
        break;
    default:
        tcout << TEXT("UsbDk driver installation returns unknown value") << endl;
        assert(0);
    }
}
//-------------------------------------------------------------------------------

void Controller_UninstallDriver()
{
    tcout << TEXT("Uninstalling UsbDk driver") << endl;
    if (UninstallDriver())
    {
        tcout << TEXT("UsbDk driver uninstall succeeded") << endl;
    }
    else
    {
        tcout << TEXT("UsbDk driver uninstall failed") << endl;
    }
}
//-------------------------------------------------------------------------------

void Controller_EnumerateDevices()
{
    PUSB_DK_DEVICE_INFO devicesArray;
    ULONG               numberDevices;
    tcout << TEXT("Enumerate USB devices") << endl;
    if (GetDevicesList(&devicesArray, &numberDevices))
    {
        tcout << TEXT("Found ") << to_tstring(numberDevices) << TEXT(" USB devices:") << endl;

        for (ULONG deviceIndex = 0; deviceIndex < numberDevices; ++deviceIndex)
        {
            tcout << to_tstring(deviceIndex) << TEXT(". ")
                  << devicesArray[deviceIndex].ID.DeviceID << TEXT(" ")
                  << devicesArray[deviceIndex].ID.InstanceID
                  << endl;
        }

        ReleaseDeviceList(devicesArray);
    }
    else
    {
        tcout << TEXT("Enumeration failed") << endl;
    }
}
//-------------------------------------------------------------------------------

void Controller_RedirectDevice(_TCHAR *DeviceID, _TCHAR *InstanceID)
{
    USB_DK_DEVICE_ID   deviceID;
    wcscpy_s(deviceID.DeviceID, MAX_DEVICE_ID_LEN, tstring2wstring(DeviceID));
    wcscpy_s(deviceID.InstanceID, MAX_DEVICE_ID_LEN, tstring2wstring(InstanceID));

    tcout << TEXT("Redirect USB device ") << deviceID.DeviceID << TEXT(", ") << deviceID.InstanceID << endl;

    HANDLE redirectedDevice = StartRedirect(&deviceID);
    if (nullptr == redirectedDevice)
    {
        tcout << TEXT("Redirect of USB device failed") << endl;
        return;
    }
    tcout << TEXT("USB device was redirected successfully. Redirected device handle = ") << redirectedDevice << endl;
    tcout << TEXT("Press some key to stop redirection");
    getchar();

    // STOP redirect
    tcout << TEXT("Restore USB device ") << redirectedDevice << endl;
    if (TRUE == StopRedirect(redirectedDevice))
    {
        tcout << TEXT("USB device redirection was stopped successfully.") << endl;
    }
    else
    {
        tcout << TEXT("Stopping of USB device redirection failed.") << endl;
    }

}
//-------------------------------------------------------------------------------

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
    if (argc > 1)
    {
        if (_tcsicmp(L"-i", argv[1]) == 0)
        {
            Controller_InstallDriver();
        }
        else if (_tcsicmp(L"-u", argv[1]) == 0)
        {
            Controller_UninstallDriver();
        }
        else if (_tcsicmp(L"-n", argv[1]) == 0)
        {
            Controller_EnumerateDevices();
        }
        else if (_tcsicmp(L"-r", argv[1]) == 0)
        {
            if (argc < 4)
            {
                ShowUsage();
                return 0;
            }

            Controller_RedirectDevice(argv[2], argv[3]);
        }
        else
        {
            ShowUsage();
        }
    }
    else
    {
        ShowUsage();
    }

    return 0;
}
//-------------------------------------------------------------------------------
