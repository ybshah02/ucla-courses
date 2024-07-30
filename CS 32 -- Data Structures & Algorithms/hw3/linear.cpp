bool allTrue(const string a[], int n){

    if (n <= 0){
        return true;
    }
    
    if (!somePredicate(a[n-1])){
        return false;
    }
    
    return allTrue(a, n-1);
}


int countFalse(const string a[], int n){
    int count = 0;
    
    if (n <= 0){
        return count;
    }
    
    if(!somePredicate(a[n-1])){
        count++;
    }
    
    count += countFalse(a, n-1);
    return count;
}

int firstFalse(const string a[], int n){
    
    if (n < 0){
        return -1;
    }
    
    if (!somePredicate(a[0])){
        return 0;
    }
    
    if(firstFalse(a+1, n-1) == -1){
        return -1;
    }else{
        return 1 + firstFalse(a+1, n-1);
    }
}

int indexOfLeast(const string a[], int n){
    if (n <= 0){
        return -1;
    }
    
    if (n == 1){
        return 0;
    }
    
    if (a[0] <= a[1+indexOfLeast(a+1, n-1)]){
        return 0;
    }else{
        return 1+indexOfLeast(a+1, n-1);
    }
}

bool includes(const string a1[], int n1, const string a2[], int n2){
    if (n2 <= 0){
        return true;
    }
    
    if (n1 <= 0 || n2 > n1){
        return false;
    }
    
    if(n1 == 1 && n2 == 1 && a1[0] == a2[0]){
        return true;
    }
    
    if(n1 >= 1 && n2 >= 1){
        if (a1[0] == a2[0]){
            return includes(a1+1, n1-1, a2+1, n2-1);
        }else{
            return includes(a1+1, n1-1, a2, n2);
        }
    }
    
    return false;
}
