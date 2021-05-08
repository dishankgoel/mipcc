int add(int arr[], int size) {
    int i;
    int sum = 0;
    for(i = 0; i < size; i++) {
        sum = sum + arr[i]; 
    }
    return sum;
}


void main() {
    int n = 5;
    int arr[5];
    int i;
    for(i = 0; i < n; i++) {
        print("Enter index ", i, ": ");
        scan(arr[i]);
    }
    print("Sum is: ", add(arr, 5));
}
