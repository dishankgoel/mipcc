int main() {
    float x = 1.0;
    {
        float x = 2.0;
        if(x == 2.0) {
            print("Hello\n");
        }
    }
    if(x == 1.0) {
        print("World\n");
    } else {
        print("oops\n");
    }
    return 0;
}

/*int f(int arr[]) {
    int i, j;
    int su = 0;
    for(i = 0; i < 5; i++) {
        for(j = 0; j < 5; j++) {
            su = su + arr[i];
        }
    }
    return su;
}

int main() {
    int arr[5];
    int i, j;
    for(i = 0; i < 5; i++) {
        for(j = 0; j < 5; j++) {
            arr[i] = i;
        }
    }
    print("Sum is: ", f(arr));
    return 0;
}*/