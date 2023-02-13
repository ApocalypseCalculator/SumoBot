#include <iostream>
#include <cmath>

float calculate(float arr[], unsigned short size)
{
    float total = 0;
    for (unsigned short i = 0; i < size; i++)
    {
        total += arr[i];
    }
    float mean = total / size;
    float summation = 0;
    for (unsigned short i = 0; i < size; i++)
    {
        total += pow(arr[i] - mean, 2);
    }
    float stddev = pow(summation / size, 0.5); // standard deviation
    float newtotal = 0;
    unsigned short newsize = 0;
    for (unsigned short i = 0; i < size; i++)
    {
        if (abs(arr[i] - mean) <= 2 * stddev)
        {
            newtotal += arr[i];
            newsize++;
        }
    }
    return newtotal / newsize; // new sample mean
}

int main()
{
    float values[20];
    std::cout << "Enter values:\n";
    for(int i = 0; i < 20; i++) {
        std::cin >> values[i];
    }
    std::cout << "\n" << calculate(values, 20) << std::endl;
    return 0;
}
