

#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <string>

#pragma warning(disable: 4996)

using namespace std;

struct Employee
{
	int num;
	char name[10];
	double hours;
};

char* getDataForCreator(char* bynaryName, int reportsAmount) {
	char data[200] = "Creator.exe ";
	strcat(data, bynaryName);
	strcat(data, " ");
	strcat(data, to_string(reportsAmount).c_str());
	return data;
}

void runCreatorProcess(char dataForCreator[]) {
	STARTUPINFO si;
	PROCESS_INFORMATION piApp;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);


	if (!CreateProcess(NULL, dataForCreator, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp)) {
		cout << "Process is not created\n";
		return;
	}
	WaitForSingleObject(piApp.hProcess, INFINITE);
	CloseHandle(piApp.hThread);
	CloseHandle(piApp.hProcess);

}

void readBinaryFile(char * fileName) {
	ifstream in;
	in.open(fileName, ios::binary);
	cout << "\n\nEmployees:\n";
	while (in.peek() != EOF) {
		Employee employee;
		in.read((char*)&employee, sizeof(Employee));
		cout << "Employee number:\n" << employee.num << "\nEmployee name:\n" << employee.name << "\nEmployee hours:\n" << employee.hours << "\n\n";
	}
	in.close();
}

char * getDataForReporter(char* binaryFile, char* fileName, int pay) {
	char data[100] = "Reporter.exe ";
	strcat(data, binaryFile);
	strcat(data, " ");
	strcat(data, fileName);
	strcat(data, " ");
	strcat(data, to_string(pay).c_str());
	return data;
}

void runReporterProcess(char dataForReporter[]) {
	STARTUPINFO si;
	PROCESS_INFORMATION piApp;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);


	if (!CreateProcess(NULL, dataForReporter, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp)) {
		cout << "Process is not created\n";
		return;
	}
	WaitForSingleObject(piApp.hProcess, INFINITE);
	CloseHandle(piApp.hThread);
	CloseHandle(piApp.hProcess);

}


void readFile(char* fileName) {
	ifstream in;

	char line[200];
	in.open(fileName);
	while (!in.eof()) {
		in.getline(line, 200);
		cout << line << "\n";
	}
}

int main() {

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	char* binaryFileName = new char[30];
	cout << "Enter binary file name:\n";
	cin >> binaryFileName;
	cout << "\n";

	int reportAmount;
	cout << "Enter amount of reports:\n";
	cin >> reportAmount;
	cout << "\n";

	char dataForCreator[100];
	strcpy(dataForCreator, getDataForCreator(binaryFileName, reportAmount));
	runCreatorProcess(dataForCreator);

	readBinaryFile(binaryFileName);

	char* reporFileName = new char[30];
	cout << "\nEnter report file name:\n";
	cin >> reporFileName;
	cout << "\n";

	double pay;
	cout << "Enter pay for hour:\n";
	cin >> pay;
	cout << "\n";

	char dataForReporter[100];

	strcpy(dataForReporter, getDataForReporter(binaryFileName ,reporFileName, pay));
	runReporterProcess(dataForReporter);

	readFile(reporFileName);


	return 0;
}