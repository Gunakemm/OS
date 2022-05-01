#include <iostream>
#include <windows.h>
#include <process.h>
#include <vector>
#include <ctime>

using namespace std;

struct ProcessData{
    int size;
    int * arr;
    int number;
    HANDLE events[2];
    ProcessData(int _size, int * _arr, int _numb): size(_size), arr(_arr), number(_numb){
        events[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
        events[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
};

HANDLE _begin;
CRITICAL_SECTION cs;
vector<HANDLE> threads_array;
vector<HANDLE> threads_events;
bool * is_processing;
vector<ProcessData> data;






UINT WINAPI marker(void *p){
    ProcessData * args = static_cast<ProcessData *>(p);
    cout << "Hello";
    WaitForSingleObject(_begin, INFINITE);
    vector<int> changed;
    srand(args->number);
    while(true){
        EnterCriticalSection(&cs);
        int i = rand() % args->number;
        if(args->arr[i] == 0){
            Sleep(5);
            args->arr[i] = args->number;
            changed.push_back(i);
            Sleep(5);
        } else {
            cout << "Thread " << args->number << " Marked " << changed.size() << " Unable to mark " << i;
            LeaveCriticalSection(&cs);
            SetEvent(threads_events[args->number - 1]);
            if(WaitForMultipleObjects(2, args->events, FALSE, INFINITE) - WAIT_OBJECT_0 == 1){
                for(int j : changed){
                    args->arr[j] = 0;
                }
                break;
            }
        }
        LeaveCriticalSection(&cs);
    }
    return 0;
}

void printArray(int * arr, int size){
    EnterCriticalSection(&cs);
    for(int i = 0; i < size; i++){
        cout << arr[i] << " ";
    }
    cout << "\n";
    LeaveCriticalSection(&cs);
}

int main() {

    srand(time(0));
    int size;
    int * arr;

    _begin = CreateEvent(NULL, TRUE, FALSE, NULL);



    cout << "Enter array size: \n";
    cin >> size;
    arr = new int[size];
    for(int i = 0; i < size; i++){
        arr[i] = 0;
    }
    int marker_size;
    cout << "Enter number of threads: \n";
    cin >> marker_size;

    
    is_processing = new bool[marker_size];


    HANDLE _Thread;

    for(int i = 0; i < marker_size; i++){
        ProcessData * currentData = new ProcessData(size, arr, i + 1);
        _Thread = (HANDLE)_beginthreadex(NULL, 0, marker, currentData, 0, NULL);
        if(_Thread == NULL){
            cout << "Process wasn't build;\n";
            return GetLastError();
        }
        threads_events.push_back(CreateEvent(NULL, TRUE, FALSE, NULL));
        if(threads_events[i] == NULL){
            return GetLastError();
        }
        is_processing[i] = true;
        data.push_back(*currentData);
    }



    InitializeCriticalSection(&cs);
    int finish;
    int processing_counter = 0;
    while(processing_counter != marker_size){
        WaitForMultipleObjects(marker_size, &threads_events[0], TRUE, INFINITE);
        printArray(arr, size);
        cout << "Which threed should finish work:\n";
        cin >> finish;
        if(is_processing[finish]){
            is_processing[finish - 1] = false;
            SetEvent(data[finish - 1].events[1]);
            WaitForSingleObject(threads_array[finish - 1], INFINITE);
            processing_counter++;
            printArray(arr, size);
            for(int i = 0; i < marker_size; i++){
                if(!is_processing[i]){
                    ResetEvent(threads_events[i]);
                    SetEvent(data[i].events[0]);
                }
            }
        }
    }


    DeleteCriticalSection(&cs);


    return 0;
}
