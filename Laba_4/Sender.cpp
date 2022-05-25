#include <iostream>
#include <windows.h>
#include <fstream>

using std::cin;
using std::cout;
using std::ofstream;
using std::ios;
CRITICAL_SECTION criticalSection;
const size_t MAX_MESSAGE_SIZE = 200;

int main(int argc, char** argv) {
    InitializeCriticalSection(&criticalSection);
    char* fileName = new char[MAX_MESSAGE_SIZE];
    strcpy(fileName, argv[1]);
    char* eventName = new char[MAX_MESSAGE_SIZE];
    strcpy(eventName, argv[2]);
    HANDLE event = OpenEvent(EVENT_MODIFY_STATE, FALSE, fileName);
    HANDLE synchronisedEvent = OpenEvent(SYNCHRONIZE, FALSE, eventName);
    ofstream out;
    out.open(fileName, ios::binary);
    if(!out.is_open()){
        cout << "Error\n File is not open\n";
        return -1;
    }
    SignalObjectAndWait(event, synchronisedEvent, INFINITE, FALSE);
    char* message;
    while(true){
        cout << "Enter message or '0' to stop\n";
        cin.getline(message, MAX_MESSAGE_SIZE);
        if(!strcmp(message, "0")){
            break;
        }
        EnterCriticalSection(&criticalSection);
        out.write(message, sizeof(message));
        SetEvent(synchronisedEvent);
        LeaveCriticalSection(&criticalSection);
    }
    out.close();
    CloseHandle(event);
    CloseHandle(synchronisedEvent);
    DeleteCriticalSection(&criticalSection);
    return 0;
}
