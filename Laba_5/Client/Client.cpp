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

int main(int argc, char* argv[]){
    HANDLE EvReady = OpenEventA(EVENT_MODIFY_STATE, FALSE, argv[1]);
    HANDLE EvStart = OpenEventA(SYNCHRONIZE, FALSE, "START_ALL");
    SetEvent(EvReady);
    WaitForSingleObject(EvStart, INFINITE);
    HANDLE hPipe;
    while(true){
        hPipe = CreateFile(pipeName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if(hPipe != INVALID_HANDLE_VALUE) break;
    }
    while(true){
        int command;
        cout << "Choose an option:\n 1) Read \n 2) Write\n 3) Exit";
        cin >> command;
        if(command == 3) break;
        DWORD writeByte;
        bool isSend;
        char buf[10];
        isSend = WriteFile(hPipe, itoa(command, buf, 10), 10, &writeByte, NULL);
        if (!isSend) {
            cout << "Message can't be send" << endl;
            return 0;
        }
        employee emp;
        if (emp.id < 0) {
            cout << "Employee not found" << endl;
            continue;
        }
        else {
            cout << emp.id << " " << emp.name << " " << emp.hours << endl;
            if (command == 2) {
                cout << "Input ID, name and hours" << endl;
                cin >> emp.id >> emp.name >> emp.hours;
                isSend = WriteFile(hPipe, &emp, sizeof(emp), &writeByte, NULL);
                if (!isSend) {
                    cout << "Error in sending." << endl;
                }
            }
        }
    }
}