#include <iostream>
#include <cmath>

short partition(float arr[], short start, short end)
{
    short pivot = arr[start];
    short count = 0;
    for (short i = start + 1; i <= end; i++)
    {
        if (arr[i] <= pivot)
            count++;
    }
    short pivotIndex = start + count;
    float tmp = arr[start];
    arr[start] = arr[pivotIndex];
    arr[pivotIndex] = tmp;
    short i = start, j = end;
    while (i < pivotIndex && j > pivotIndex)
    {
        while (arr[i] <= pivot)
        {
            i++;
        }
        while (arr[j] > pivot)
        {
            j--;
        }
        if (i < pivotIndex && j > pivotIndex)
        {
            float tmpval = arr[j];
            arr[j] = arr[i];
            arr[i] = tmpval;
            i++;
            j--;
        }
    }
    return pivotIndex;
}

void quickSort(float arr[], short start, short end)
{
    if (start >= end)
        return;
    short p = partition(arr, start, end);
    quickSort(arr, start, p - 1);
    quickSort(arr, p + 1, end);
}

/*float calculate(float arr[], short size)
{
    float total = 0;
    for (short i = 0; i < size; i++)
    {
        total += arr[i];
    }
    float mean = total / size;
    float summation = 0;
    for (short i = 0; i < size; i++)
    {
        total += pow(arr[i] - mean, 2);
    }
    float stddev = pow(summation / size, 0.5); // standard deviation
    float newtotal = 0;
    short newsize = 0;
    for (short i = 0; i < size; i++)
    {
        if (abs(arr[i] - mean) <= 2 * stddev)
        {
            newtotal += arr[i];
            newsize++;
        }
    }
    return newtotal / newsize; // new sample mean
}*/

float calculate(float arr[], short size)
{
    //assuming static arr size of 20
    //float median = (arr[9] + arr[10])/2;
    float q1 = (arr[4] + arr[5])/2;
    float q3 = (arr[14] + arr[15])/2;
    float iqrrng = (q3 - q1)*1.5;
    float sum = 0;
    short count = 0;
    for(short i = 0; i < size; i++) {
        if(arr[i] >= q1-iqrrng && arr[i] <= q3+iqrrng) {
            sum+=arr[i];
            count++;
        }
    }
    return sum/count;
}

int main()
{
    float values[20];
    std::cout << "Enter values:\n";
    for (int i = 0; i < 20; i++)
    {
        std::cin >> values[i];
    }
    quickSort(values, 0, 19);
    std::cout << "\n" << calculate(values, 20) << std::endl;
    return 0;
}
