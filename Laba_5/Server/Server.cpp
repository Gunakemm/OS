#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <vector>



using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::vector;

const int MAX_STRING_SIZE = 200;
const int PIPE_NAME_SIZE = 30;
const char pipeName[PIPE_NAME_SIZE] = "\\\\\\\\.\\\\pipe\\\\pipeName";

struct employee
{
    int id;
    char name[10];
    double hours;


    employee() {employee(0, "NULL", 0.);}
    employee(int id, char* name, double hours): id(id), hours(hours){strcat(this->name, name);}
};

CRITICAL_SECTION cs;
vector<employee> employersArray;
int entriesNumber;
vector<bool> processed;


DWORD WINAPI messaging(LPVOID p){
    HANDLE hPipe = (HANDLE)p;
    employee* errorEmployee = new employee(-1, "NULL", -1.);
    while (true){
        bool isRead = false;
        char message[10];
        DWORD readBytes;
        isRead = ReadFile(hPipe, message, 10, &readBytes, NULL);
        if (!isRead) {
            if (ERROR_BROKEN_PIPE == GetLastError()) {
                cout << "Client disconected." << endl;
                return 0;
            }
            else {
                cout << "Error in reading." << endl;
                return 0;
            }
        }
        if(!strlen(message)){
            char command = message[0];
            message[0] = ' ';
            int id = atoi(message);
            DWORD bytesWritten;
            EnterCriticalSection(&cs);
            employee* toSend = new employee;
            int index = -1;
            for(auto item : employersArray){
                if(id == item.id){
                    toSend = new employee(item.id, item.name, item.hours);
                    index++;
                    break;
                }
                index++;
            }
            LeaveCriticalSection(&cs);
            if (strcmp(toSend->name, "NULL")) {
                toSend = errorEmployee;
            } else {
                switch(command){
                    case '1':{
                        cout << "ID for reading: " << id << "\n";
                        break;
                    }
                    case '2': {
                        cout << "ID for modifinding: " << id << "\n";
                        processed[index] = true;
                        break;
                    }
                }
            }
            bool isSent = WriteFile(hPipe, toSend, sizeof(employee), &bytesWritten, NULL);
            if (!isSent) {
                cout << "Error in sending answer." << endl;
            }
            if ('2' == command && toSend != errorEmployee) {
                DWORD readBytes;
                isRead = ReadFile(hPipe, message, entriesNumber, &readBytes, NULL);
                if (!isRead) {
                    if (ERROR_BROKEN_PIPE == GetLastError()) {
                        cout << "Disconnected." << endl;
                        return 0;
                    }
                    else {
                        cout << "Error in reading." << endl;
                        return 0;
                    }
                }
                index = -1;
                for(auto item : employersArray){
                    if(id == item.id){
                        index++;
                        break;
                    }
                    index++;
                }
                processed[index] = false;
                EnterCriticalSection(&cs);
                LeaveCriticalSection(&cs);
                break;
            }
        }
    }
}

int main(){
    char fileName[50];
    cout << "Enter file name:\n";
    cin >> fileName;
    cout << "Enter entries number:\n";
    cin >> entriesNumber;
    cout << "Enter information about employers (ID, name, worked hours):\n";
    for(int i = 0; i < entriesNumber; i++){
        int id;
        char name[10];
        double hours;
        cin >> id >> name >> hours;
        employersArray.push_back(employee(id, name, hours));
    }
    ofstream out;
    out.open(fileName, ofstream::binary);
    out.write((const char*)employersArray.data(), sizeof(employee) * entriesNumber);
    out.close();

    int numberOfProcesses;
    cout << "Enter number of processes:\n";
    cin >> numberOfProcesses;
    processed.resize(numberOfProcesses);
    std::fill(processed.begin(), processed.end(), false);
    InitializeCriticalSection(&cs);
    HANDLE hStart = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
    vector <HANDLE> hEvents(numberOfProcesses);


    for(int i = 0; i < numberOfProcesses; i++){
        char processName[MAX_STRING_SIZE];
        char buf[10];
        strcpy(processName, "");
        strcat(processName, "Client.exe ");
        char eventName[20];
        strcpy(eventName, "Event-");
        strcat(eventName, itoa(i, buf, 10));
        strcat(processName, eventName);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        hEvents.push_back(CreateEvent(NULL, TRUE, FALSE, eventName));
        if (!CreateProcess(NULL, processName, NULL, NULL,
                           FALSE, CREATE_NEW_CONSOLE, NULL, NULL,
                           &si, &pi)) {
            GetLastError();
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    WaitForMultipleObjects(numberOfProcesses, hEvents.data(), TRUE, INFINITE);
    SetEvent(hStart);

    HANDLE hPipe;
    vector<HANDLE> hThreads(numberOfProcesses);
    for(int i = 0; i < numberOfProcesses; i++){
        hPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,PIPE_UNLIMITED_INSTANCES,
                                0, 0, INFINITE, NULL);
        if (INVALID_HANDLE_VALUE == hPipe) {
            cout << "Creating named pipe ended with error." << endl;
            break;
        }
        hThreads[i] = CreateThread(NULL, 0, messaging, (LPVOID)hPipe, 0, NULL);
    }
    WaitForMultipleObjects(numberOfProcesses, hThreads.data(), TRUE, INFINITE);
    cout << "File after changing:" << endl;
    for (int i = 0; i < numberOfProcesses; i++) {
        cout << employersArray[i].id << " " << employersArray[i].name << " " << employersArray[i].hours << endl;
    }
    DeleteCriticalSection(&cs);

}
