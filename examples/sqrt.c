float sqrt(float n) {
    float left = 0, right = n;
    float mid = (left + right)/2.0;
    int x;
    while(((mid*mid - n) >= 0.00001) || ((mid*mid - n) <= -0.00001)) {
        if((mid*mid - n > 0)) {
            right = mid;
        } else {
            left = mid;
        }
        mid = (left + right)/2.0;
    }
    return mid;
}

int main() {
    float n;
    print("Enter a number: ");
    scan(n);
    print("Square root: ", sqrt(n));
    return 0;
}