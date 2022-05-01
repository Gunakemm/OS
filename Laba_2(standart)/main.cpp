#include <iostream>
#include <thread>
#include <vector>
#include <chrono>



int _size;
int *arr;

int _min;
int _max;
double avg;


void min_max() {
    std::this_thread::yield();
    _min = arr[0];
    _max = arr[0];
    for (int i = 0; i < _size; i++) {
        if (arr[i] > _max) {
            _max = arr[i];
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
        if (arr[i] < _min) {
            _min = arr[i];
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
    }
    std::cout << "Min: " << _min << "\n";
    std::cout << "Max: " << _max << "\n";

    return;
}

void average() {

    int sum = 0;
    for (int i = 0; i < _size; i++) {
        sum += arr[i];
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }
    avg = 1. * sum / _size;
    std::cout << "Average: " << avg << "\n";
    return;
}




int main() {
    std::cout << "Enter size of array:\n";
    std::cin >> _size;
    std::cout << "Enter elements of array:\n";

    arr = new int[_size];
    for (int i = 0; i < _size; i++) {
        std::cin >> arr[i];
    }

    std::thread thr(min_max);
    std::thread thr2(average);
    thr.join();
    thr2.join();

    for (int i = 0; i < _size; i++) {
        if (arr[i] == _min || arr[i] == _max) {
            arr[i] = avg;
        }
    }
    for (int i = 0; i < _size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";


    return 0;
}