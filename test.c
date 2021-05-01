/* Comment */
/* int calc() {
    int a = 12e-4;
    int b = 12.043e-5;
    char* m = "t93248tywtwuon!@#@$#%$^%&^*&()_+=  ";
    return a;
} 
*/

int fib(int n) {
    if(n <= 1) {
        return n;
    } else {
        return fib(n - 1)  + fib(n - 2);
    }
}

int main() {
    /*int i, n, t1 = 0, t2 = 1;
    int nextTerm = t1 + t2;
    print("Enter the number of terms: ");
    scan(n);
    print("Fibonacci Series: ", t1, ", ", t2, ", ");
    for (i = 1; i <= n; ++i) {
        print(nextTerm, ", ");
        t1 = t2;
        t2 = nextTerm;
        nextTerm = t1 + t2;
    }
    return 0;*/

    int arr[10][10];
    int i;
    for(i = 0; i < 10; i++) {
        int j;
        for(j = 0; j < 10; j++) {
            arr[i][j] = i + j;
        }
    }
    for(i = 0; i < 10; i++) {
        for(j = 0; j < 10; j++) {
            print(arr[i][j], ", ");
        }
        print("\n");
    }
    /*int n;
    print("Enter a number: ");
    scan(n);
    print("\nOutput: ", fib(n), "\n");*/
}
