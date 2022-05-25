#include <iostream>
#include <windows.h>
#include <fstream>
#include <process.h>

using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::fstream;
CRITICAL_SECTION criticalSection;
const size_t MAX_MESSAGE_SIZE = 200;

int main(){
    InitializeCriticalSection(&criticalSection);
    char* fileName = new char[MAX_MESSAGE_SIZE];
    cout << "Enter name of file:\n";
    cin >> fileName;
    int sendersNumber;
    cout << "Enter number of senders:\n";
    cin >> fileName;

    STARTUPINFO* startupInfo = new STARTUPINFO[sendersNumber];
    PROCESS_INFORMATION* processInformation = new PROCESS_INFORMATION[sendersNumber];
    HANDLE* events = new HANDLE[sendersNumber];
    HANDLE* senders = new HANDLE[sendersNumber];
    SECURITY_ATTRIBUTES securityAttributes = {sizeof(SECURITY_ATTRIBUTES), 0, TRUE};

    HANDLE startEvent = CreateEvent(&securityAttributes, FALSE, FALSE, "StartEvent");
    for(int i = 0; i < sendersNumber; i++){
        char* dataForSender = new char[MAX_MESSAGE_SIZE];
        strcpy(dataForSender, "");
        strcat(dataForSender, "Sender.exe ");
        strcat(dataForSender, fileName);
        ZeroMemory(&startupInfo[i], sizeof(STARTUPINFO));
        ZeroMemory(&processInformation[i], sizeof(PROCESS_INFORMATION));
        startupInfo[i].cb = sizeof(STARTUPINFO);
        char* tmp = new char[20];
        char p[10];
        strcpy(tmp, "Event");
        strcat(tmp, itoa(i, p,10));
        strcat(dataForSender, " ");
        strcat(dataForSender, tmp);
        events[i] = CreateEvent(&securityAttributes, FALSE, FALSE, tmp);
        if(!CreateProcess(NULL, dataForSender,
                          NULL, NULL, TRUE,
                          CREATE_NEW_CONSOLE, NULL,
                          NULL, &startupInfo[i], &processInformation[i])){
                cout << "Error\nProcess is not created\n";
            return GetLastError();
        }
        senders[i] = processInformation[i].hProcess;
    }
    WaitForMultipleObjects(sendersNumber, events, TRUE, INFINITE);
    SetEvent(startEvent);

    while (WaitForMultipleObjects(sendersNumber, senders, TRUE, 0) == WAIT_TIMEOUT) {
        EnterCriticalSection(&criticalSection);
        cout << "Input 'E' for trying to read file: ";
        char* command;
        cin >> command;
        if (strcmp(command, "E")) {
            break;
        }
        ifstream in;
        in.open(fileName, fstream::binary);
        char* message;
        int i = 0;
        while (i < 2) {
            in.read(message, sizeof(message));
            cout << message;
            i++;
        }
        in.close();
        LeaveCriticalSection(&criticalSection);
    }

    for (int i = 0; i < sendersNumber; i++) {
        CloseHandle(processInformation[i].hThread);
        CloseHandle(processInformation[i].hProcess);
        CloseHandle(events[i]);
    }
    CloseHandle(startEvent);
    return 0;
}
