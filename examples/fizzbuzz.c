int main() {
    int n;
    print("This is FizzBuzz program\n");
    print("Enter n: ");
    scan(n);
    print("\n");
    int i;
    for(i = 1; i <= n; i++) {
        if(i % 15 == 0) {
            print("FizzBuzz\n");
        } else if(i % 3 == 0) {
            print("Fizz\n");
        } else if(i % 5 == 0) {
            print("Buzz\n");
        } else {
            print(i, "\n");
        }
    }
    return 0;
}